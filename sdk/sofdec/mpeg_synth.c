/* the AHX decoder's synthesis filters (mpeg_synth.h). a call runs the matrix
 * into its channel's ring, then the window of the ring's phase */
#include <mpeg_alloc.h>
#include <mpeg_synth.h>
#include <mpeg_synth_tables.h>
#include <mpeg_timer.h>
#include <sg_xpt.h>
#include <string.h>

#define FILTERS 2
#define CHANNELS 2
/* a channel's ring holds the matrix's outputs of its last MPEG_SYNTH_PHASES
 * calls. each call writes its outputs MPEG_SYNTH_OUTPUTS below the last's,
 * round the ring */
#define RING_SAMPLES (MPEG_SYNTH_PHASES * MPEG_SYNTH_OUTPUTS)
/* MPEG_SYNTH_OUTPUTS is 1 << PHASE_SHIFT. outputs at position p take
 * windows[p >> PHASE_SHIFT] */
#define PHASE_SHIFT 6
#define ALIGN32(p) ((Uint8 *)(((Uint32)(p) + 31) & ~31UL))
/* the start counts the member keeps, though mpeg_synth_pcm writes only the
 * first */
#define TICK_COUNTS 4

struct mpeg_synth {
    Sint8 used;
    Sint32 position[CHANNELS];
    Float (*matrix)[SBLIMIT];
    Float ring[CHANNELS][RING_SAMPLES];
    Float *window;
};

void mpeg_matrix(const Float *samples, const Float *matrix, Float *ring);
typedef void MPEG_WINDOW(const Float *ring, const Float *window, Sint16 *pcm);
extern MPEG_WINDOW mpeg_window_0;
extern MPEG_WINDOW mpeg_window_1;
extern MPEG_WINDOW mpeg_window_2;
extern MPEG_WINDOW mpeg_window_3;
extern MPEG_WINDOW mpeg_window_4;
extern MPEG_WINDOW mpeg_window_5;
extern MPEG_WINDOW mpeg_window_6;
extern MPEG_WINDOW mpeg_window_7;
extern MPEG_WINDOW mpeg_window_8;
extern MPEG_WINDOW mpeg_window_9;
extern MPEG_WINDOW mpeg_window_10;
extern MPEG_WINDOW mpeg_window_11;
extern MPEG_WINDOW mpeg_window_12;
extern MPEG_WINDOW mpeg_window_13;
extern MPEG_WINDOW mpeg_window_14;
extern MPEG_WINDOW mpeg_window_15;

static Sint32 open_count = 0;
static MPEG_WINDOW *windows[MPEG_SYNTH_PHASES] = {
    mpeg_window_0,  mpeg_window_1,  mpeg_window_2,  mpeg_window_3,
    mpeg_window_4,  mpeg_window_5,  mpeg_window_6,  mpeg_window_7,
    mpeg_window_8,  mpeg_window_9,  mpeg_window_10, mpeg_window_11,
    mpeg_window_12, mpeg_window_13, mpeg_window_14, mpeg_window_15,
};

static MPEG_SYNTH filters[FILTERS];
static Uint8 *aligned_matrix;
static Uint8 *aligned_window;
/* the member exports these data, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* mpeg_synth_pcm's timer counts before and after the matrix, then before and
 * after the window, each in element 0. the window's replace the matrix's */
Uint32 mpeg_synth_start_ticks[TICK_COUNTS];
/* @hack type: the end count in a one-element array, where a scalar would do */
Uint32 mpeg_synth_end_ticks[1];
/* NOLINTEND(misc-use-internal-linkage) */

void mpeg_synth_init(void)
{
    Uint8 *source;
    Uint8 *aligned;
    Sint32 i;
    /* @hack spelling: each copy's bound through size, where its sizeof in
     * the loop would do */
    Sint32 size;

    if (open_count == 0) {
        memset(filters, 0, sizeof(filters));
    }
    /* @bug logic: a second call copies tables the first already moved, which
     * shifts their coefficients 16 bytes further. mpeg_init calls it again
     * once mpeg_finish has ended the last use. mpeg_window_store at
     * 0x8c186b10 and mpeg_matrix_store at 0x8c184a90 each lie 16 bytes past a
     * 32-byte boundary. mpeg_synth_init, at 0x8c177584, copies each up to the
     * boundary after it */
    source = (Uint8 *)&mpeg_window_store;
    aligned_window = ALIGN32(source);
    size = sizeof(mpeg_window_store.coefficients);
    aligned = aligned_window;
    for (i = size; i >= 0; i--) {
        aligned[i] = source[i];
    }
    /* @hack spelling: source assigned inside ALIGN32's argument, where a
     * statement of its own would do */
    aligned_matrix = ALIGN32(source = (Uint8 *)&mpeg_matrix_store);
    size = sizeof(mpeg_matrix_store.coefficients);
    aligned = aligned_matrix;
    for (i = size; i >= 0; i--) {
        aligned[i] = source[i];
    }
    open_count++;
}

void mpeg_synth_finish(void)
{
    open_count--;
    if (open_count == 0) {
        memset(filters, 0, sizeof(filters));
    }
}

MPEG_SYNTH *mpeg_synth_create(void)
{
    Sint32 i;
    MPEG_SYNTH *filter;

    for (i = 0; i < FILTERS; i++) {
        filter = &filters[i];
        if (filter->used == FALSE) {
            break;
        }
    }
    if (i == FILTERS) {
        return NULL;
    }
    filter->position[0] = filter->position[1] = MPEG_SYNTH_OUTPUTS;
    filter->window = (Float *)aligned_window;
    filter->matrix = (Float(*)[SBLIMIT])aligned_matrix;
    filter->used = TRUE;
    return filter;
}

void mpeg_synth_destroy(MPEG_SYNTH *filter)
{
    memset(filter, 0, sizeof(*filter));
    /* @hack redundant: used cleared, which the memset has cleared */
    filter->used = FALSE;
}

void mpeg_synth_pcm(MPEG_SYNTH *filter, const Float *samples, Sint32 channel,
                    Sint16 *pcm)
{
    Float *destination;
    /* @hack redundant: matrix declared register, where a plain local would
     * do */
    register const Float *matrix;
    Float *ring;
    Sint32 position;
    Sint32 phase;

    filter->position[channel] =
        (filter->position[channel] - MPEG_SYNTH_OUTPUTS) & (RING_SAMPLES - 1);
    destination = &filter->ring[channel][filter->position[channel]];
    /* @hack spelling: &filter->matrix[0][0], where filter->matrix[0] would
     * do */
    matrix = &filter->matrix[0][0];
    mpeg_synth_start_ticks[0] = mpeg_timer_count();
    mpeg_matrix(samples, matrix, destination);
    mpeg_synth_end_ticks[0] = mpeg_timer_count();
    ring = filter->ring[channel];
    position = filter->position[channel];
    phase = position >> PHASE_SHIFT;
    mpeg_synth_start_ticks[0] = mpeg_timer_count();
    windows[phase](&ring[position], filter->window, pcm);
    mpeg_synth_end_ticks[0] = mpeg_timer_count();
}
