/* ADXT's AHX player. its calls and counts follow those of ADXSJD's decoders.
 * the format, cutoff and loop calls return 0 */
#include <adxahx.h>
#include <ahxbsr.h>
#include <mpeg_audio.h>
#include <mpeg_crs.h>
#include <mpeg_timer.h>
#include <sg_xpt.h>
#include <sj.h>
#include <string.h>

#define PLAYER_COUNT 2
#define OUTPUT_JOINTS 8
#define PCM_CHANNELS 2
#define HEADER_MIN_BYTES 36
#define NO_LIMIT 0x7fffffff
#define NO_TRAP (-1)
#define BYTE_BITS 8

struct adxahx_player {
    MPEG_DEC *decoder;
    Sint8 used;
    Sint8 state;
    Sint8 max_channels;
    Sint8 header_read;
    AHXBSR reader;
    SJ input;
    SJ outputs[OUTPUT_JOINTS];
    Sint32 header_bytes;
    Sint32 decoded_samples;
    Sint32 input_bytes;
    Sint32 position;
    Sint32 sample_limit;
    Sint32 trap_sample;
    Sint32 trap_count;
    Sint32 trap_bytes;
    void (*trap)(void *object);
    void *trap_object;
    /* the filter adxahx_set_filter sets. no step calls it */
    void (*filter)(void *object, Sint32 channel, void *data, Sint32 bytes);
    void *filter_object;
};

static struct adxahx_player players[PLAYER_COUNT];
/* the timer's counts at the start and end of the last decode and the last
 * frame header's read */
enum tick { DECODE_START, DECODE_END, HEADER_START, HEADER_END, TICKS };
static Uint32 ticks[TICKS];

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 adxahx_max_channels(ADXAHX player);
void adxahx_set_output(ADXAHX player, Sint32 channel, SJ output);
Sint32 adxahx_block_samples(ADXAHX player);
void adxahx_step_all(void);
Sint32 adxahx_input_bytes(ADXAHX player);
Sint32 adxahx_decoded_samples(ADXAHX player);
void adxahx_set_position(ADXAHX player, Sint32 position);
Sint32 adxahx_position(ADXAHX player);
void adxahx_set_filter(ADXAHX player,
                       void (*filter)(void *, Sint32, void *, Sint32),
                       void *object);
void adxahx_set_trap(ADXAHX player, void (*trap)(void *), void *object);
void adxahx_set_trap_sample(ADXAHX player, Sint32 sample);
Sint32 adxahx_trap_sample(ADXAHX player);
void adxahx_set_trap_count(ADXAHX player, Sint32 count);
Sint32 adxahx_trap_count(ADXAHX player);
void adxahx_set_trap_bytes(ADXAHX player, Sint32 bytes);
Sint32 adxahx_trap_bytes(ADXAHX player);
Sint32 adxahx_format(ADXAHX player);
Sint32 adxahx_cutoff(ADXAHX player);
Sint32 adxahx_loop_count(ADXAHX player);
Sint32 adxahx_loop_insert_samples(ADXAHX player);
Sint32 adxahx_loop_start_sample(ADXAHX player);
Sint32 adxahx_loop_start_offset(ADXAHX player);
Sint32 adxahx_loop_end_sample(ADXAHX player);
Sint32 adxahx_loop_end_offset(ADXAHX player);
/* NOLINTEND(misc-use-internal-linkage) */

void adxahx_init(void)
{
    mpeg_init();
    memset(players, 0, sizeof(players));
}

void adxahx_finish(void)
{
    memset(players, 0, sizeof(players));
    mpeg_finish();
}

static void clear_counts(ADXAHX player)
{
    player->header_bytes = 0;
    player->decoded_samples = 0;
    player->input_bytes = 0;
    player->position = 0;
    player->sample_limit = NO_LIMIT;
    player->trap_sample = NO_TRAP;
    player->trap_count = 0;
    player->trap_bytes = 0;
    player->header_read = FALSE;
}

ADXAHX adxahx_create(SJ input, Sint32 channels, SJ *outputs)
{
    ADXAHX player;
    Sint32 i;

    for (i = 0; i < PLAYER_COUNT; i++) {
        if (!players[i].used) {
            break;
        }
    }
    if (i == PLAYER_COUNT) {
        return NULL;
    }
    player = &players[i];
    player->reader = ahxbsr_create(input);
    if (!player->reader) {
        return NULL;
    }
    player->decoder = mpeg_create();
    /* @bug memory: a failed mpeg_create leaks the player's bit reader. the
     * reader is stored at 0x8c157fa0. the return at 0x8c157fb0 leaves used
     * clear, so the next create takes the player and a second reader */
    if (!player->decoder) {
        return NULL;
    }
    player->max_channels = channels;
    player->input = input;
    for (i = 0; i < channels; i++) {
        player->outputs[i] = outputs[i];
    }
    player->state = ADXAHX_STOP;
    clear_counts(player);
    player->trap = NULL;
    player->trap_object = NULL;
    player->filter = NULL;
    player->filter_object = NULL;
    player->used = TRUE;
    return player;
}

void adxahx_destroy(ADXAHX player)
{
    if (player) {
        if (player->reader) {
            ahxbsr_destroy(player->reader);
        }
        if (player->decoder) {
            mpeg_destroy(player->decoder);
        }
        mpeg_crs_lock();
        memset(player, 0, sizeof(*player));
        mpeg_crs_unlock();
    }
}

Sint32 adxahx_state(ADXAHX player)
{
    return player->state;
}

Sint32 adxahx_max_channels(ADXAHX player)
{
    return player->max_channels;
}

void adxahx_set_input(ADXAHX player, SJ input)
{
    player->input = input;
}

void adxahx_set_output(ADXAHX player, Sint32 channel, SJ output)
{
    player->outputs[channel] = output;
}

void adxahx_set_sample_limit(ADXAHX player, Sint32 samples)
{
    player->sample_limit = samples;
}

void adxahx_start(ADXAHX player)
{
    clear_counts(player);
    if (player->reader) {
        ahxbsr_destroy(player->reader);
    }
    player->reader = ahxbsr_create(player->input);
    mpeg_set_stream(player->decoder, player->reader);
    player->state = ADXAHX_HEADER;
}

void adxahx_stop(ADXAHX player)
{
    mpeg_reset(player->decoder);
    player->state = ADXAHX_STOP;
}

static void read_header(ADXAHX player)
{
    if (SJ_GetNumData(player->input, SJ_LIN_DATA) < HEADER_MIN_BYTES) {
        return;
    }
    mpeg_read_stream_header(player->decoder);
    mpeg_read_frame_header(player->decoder);
    player->header_read = TRUE;
    player->state = ADXAHX_DECODE;
}

static void decode_block(ADXAHX player)
{
    MPEG_DEC *decoder;
    SJ input;
    SJ *outputs;
    Sint32 free_samples;
    /* @hack spelling: sample_bytes declared before block_samples, where the
     * order they are set in would do */
    Sint32 sample_bytes;
    Sint32 block_samples;
    Sint32 channels;
    Sint32 decoded;
    Sint32 channel;
    Sint16 *left;
    Sint16 *right;
    SJCK block[PCM_CHANNELS];
    SJCK remainder[PCM_CHANNELS];

    decoder = player->decoder;
    input = player->input;
    outputs = player->outputs;
    if (player->header_read == TRUE && SJ_GetNumData(input, SJ_LIN_DATA) == 0 &&
        ahxbsr_empty(player->reader) == TRUE) {
        player->state = ADXAHX_END;
        return;
    }

    block_samples = adxahx_block_samples(player);
    sample_bytes = adxahx_sample_bits(player) / BYTE_BITS;
    free_samples =
        SJ_GetNumData(player->outputs[0], SJ_LIN_FREE) / sample_bytes;
    if (free_samples < block_samples) {
        return;
    }
    if (mpeg_frame_done(decoder)) {
        ticks[HEADER_START] = mpeg_timer_count();
        if (mpeg_read_frame_header(decoder) == 1) {
            player->state = ADXAHX_END;
            return;
        }
        ticks[HEADER_END] = mpeg_timer_count();
    }

    channels = adxahx_channels(player);
    /* @hack spelling: &block passed to memset, where block would do */
    memset(&block, 0, sizeof(block));
    for (channel = 0; channel < channels; channel++) {
        SJ_GetChunk(outputs[channel], SJ_LIN_FREE, block_samples * sample_bytes,
                    &block[channel]);
    }
    left = (Sint16 *)block[0].data;
    right = (Sint16 *)block[1].data;
    if (block[0].len / sizeof(Sint16) != block_samples) {
        for (;;) {
        }
    }
    ticks[DECODE_START] = mpeg_timer_count();
    decoded = mpeg_decode_granule(decoder, left, right, block_samples);
    ticks[DECODE_END] = mpeg_timer_count();

    for (channel = 0; channel < channels; channel++) {
        SJ_SplitChunk(&block[channel], decoded * sample_bytes, &block[channel],
                      &remainder[channel]);
        SJ_PutChunk(outputs[channel], SJ_LIN_DATA, &block[channel]);
        SJ_UngetChunk(outputs[channel], SJ_LIN_FREE, &remainder[channel]);
    }
    player->decoded_samples += decoded;
    player->input_bytes =
        (ahxbsr_position(player->reader) + BYTE_BITS - 1) / BYTE_BITS;
    player->position += decoded;
    player->trap_count += decoded;
    /* @hack spelling: a nested if for trap_sample, where one test of all
     * three would do */
    if (player->trap_sample >= 0) {
        if (player->trap_count >= player->trap_sample && player->trap) {
            player->trap(player->trap_object);
        }
    }
}

void adxahx_step(ADXAHX player)
{
    if (player->state == ADXAHX_DECODE) {
        decode_block(player);
    } else if (player->state == ADXAHX_HEADER) {
        read_header(player);
    }
}

void adxahx_step_all(void)
{
    ADXAHX player;
    Sint32 i;

    for (i = 0; i < PLAYER_COUNT; i++) {
        player = &players[i];
        if (player->used == TRUE) {
            adxahx_step(player);
        }
    }
}

Sint32 adxahx_input_bytes(ADXAHX player)
{
    return player->input_bytes;
}

Sint32 adxahx_decoded_samples(ADXAHX player)
{
    return player->decoded_samples;
}

void adxahx_set_position(ADXAHX player, Sint32 position)
{
    player->position = position;
}

Sint32 adxahx_position(ADXAHX player)
{
    return player->position;
}

void adxahx_set_filter(ADXAHX player,
                       void (*filter)(void *, Sint32, void *, Sint32),
                       void *object)
{
    player->filter = filter;
    player->filter_object = object;
}

void adxahx_set_trap(ADXAHX player, void (*trap)(void *), void *object)
{
    player->trap = trap;
    player->trap_object = object;
}

void adxahx_set_trap_sample(ADXAHX player, Sint32 sample)
{
    player->trap_sample = sample;
}

Sint32 adxahx_trap_sample(ADXAHX player)
{
    return player->trap_sample;
}

void adxahx_set_trap_count(ADXAHX player, Sint32 count)
{
    player->trap_count = count;
}

Sint32 adxahx_trap_count(ADXAHX player)
{
    return player->trap_count;
}

void adxahx_set_trap_bytes(ADXAHX player, Sint32 bytes)
{
    player->trap_bytes = bytes;
}

Sint32 adxahx_trap_bytes(ADXAHX player)
{
    return player->trap_bytes;
}

Sint32 adxahx_format(ADXAHX player)
{
    return 0;
}

Sint32 adxahx_sample_rate(ADXAHX player)
{
    return mpeg_sample_rate(player->decoder);
}

Sint32 adxahx_channels(ADXAHX player)
{
    return mpeg_channels(player->decoder);
}

Sint32 adxahx_sample_bits(ADXAHX player)
{
    return mpeg_sample_bits(player->decoder);
}

Sint32 adxahx_block_samples(ADXAHX player)
{
    return mpeg_block_samples(player->decoder);
}

Sint32 adxahx_total_samples(ADXAHX player)
{
    return mpeg_total_samples(player->decoder);
}

Sint32 adxahx_cutoff(ADXAHX player)
{
    return 0;
}

Sint32 adxahx_loop_count(ADXAHX player)
{
    return 0;
}

Sint32 adxahx_loop_insert_samples(ADXAHX player)
{
    return 0;
}

Sint32 adxahx_loop_start_sample(ADXAHX player)
{
    return 0;
}

Sint32 adxahx_loop_start_offset(ADXAHX player)
{
    return 0;
}

Sint32 adxahx_loop_end_sample(ADXAHX player)
{
    return 0;
}

Sint32 adxahx_loop_end_offset(ADXAHX player)
{
    return 0;
}

Sint32 adxahx_output_volume(ADXAHX player)
{
    return mpeg_out_vol(player->decoder);
}

Sint32 adxahx_direct_level(ADXAHX player)
{
    return mpeg_drct_lvl(player->decoder);
}

void adxahx_effect(ADXAHX player, Sint32 *channel, Sint32 *level)
{
    mpeg_fx(player->decoder, channel, level);
    /* @hack redundant: the return at the function's end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

Sint32 adxahx_pan(ADXAHX player, Sint32 channel)
{
    return mpeg_channel_pan(player->decoder, channel);
}
