/* the port's end of each frame, and the split frame its 60 fps shows. port.h
 * gives the split's rule. make dc links this into 1ST_READ.BIN after the
 * image's sources */
#include <ninja.h>
#include <nj_global.h>
#include <rnext.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>
#ifdef DREAMCAST
#include <machine.h>
#endif

#include "engine/dc/port.h"
#include "system/frame.h"

/* syTmrGetCount's counts in a vertical blank, at 1.28 microseconds a count:
 * at 59.94 blanks a second, and at DISPLAY_PAL's 50. every display but
 * DISPLAY_PAL takes the shorter blank, which lets no frame fit that
 * doesn't */
#define BLANK_COUNTS 13034
#define PAL_BLANK_COUNTS 15625
/* the counts a frame must spare to count toward engaging the split: a
 * fourteenth of blank, a blank's counts. a smaller room would let the split
 * engage and end by turns */
#define ENGAGE_ROOM(blank) ((blank) / 14)

/* the frame running. start is when its first step began. asked is TRUE once
 * a loop offered it to the split, and count the steps it takes. first is how
 * long its first step ran, and steps how long all of them did. shown is TRUE
 * where the split's picture showed. a frame that shows it holds that
 * picture's time in the times after it, which nothing reads while the split
 * is engaged */
static struct {
    Uint32 start;
    int asked;
    int count;
    Uint32 first;
    Uint32 steps;
    int shown;
} frame;
/* TRUE while frames split. fit_run counts the held frames in a row that fit,
 * and calm the split frames since one ran late, up to SETTLE_FRAMES */
static int engaged;
static int fit_run;
static int calm;
/* the frames the split stays off for yet, those it stays off for when it
 * next ends, and the frames it has lasted */
static int wait_left;
static int wait_next = SETTLE_FRAMES;
static int lasted;

/* what a camera's draw leaves in Ninja that a step may read: the screen, the
 * clips, the matrix stack's top and its base, the view matrix, and the
 * matrix loaded in XMTRX. each matrix stands alone, since make dc's
 * NJS_MATRIX aligns to 8 and a structure's members to 4 */
static struct {
    NJS_SCREEN screen;
    NJS_POINT2 clip[2];
    Float near_clip;
    Float far_clip;
    NJS_MATRIX *top;
} kept;
static NJS_MATRIX kept_top;
static NJS_MATRIX kept_base;
static NJS_MATRIX kept_view;
#ifdef DREAMCAST
static NJS_MATRIX kept_loaded;
#endif

static void view_get(void)
{
    kept.screen = _nj_screen_;
    kept.clip[0] = _nj_2d_clip_[0];
    kept.clip[1] = _nj_2d_clip_[1];
    kept.near_clip = _nj_near_clip_;
    kept.far_clip = _nj_far_clip_;
    kept.top = _nj_current_matrix_ptr_;
    memcpy(kept_top, *kept.top, sizeof(NJS_MATRIX));
    memcpy(kept_base, *_nj_base_matrix_ptr_, sizeof(NJS_MATRIX));
    memcpy(kept_view, _nj_view_matrix_, sizeof(NJS_MATRIX));
#ifdef DREAMCAST
    st_ext((float (*)[4])kept_loaded);
#endif
}

static void view_put(void)
{
    _nj_screen_ = kept.screen;
    _nj_2d_clip_[0] = kept.clip[0];
    _nj_2d_clip_[1] = kept.clip[1];
    _nj_near_clip_ = kept.near_clip;
    _nj_far_clip_ = kept.far_clip;
    _nj_current_matrix_ptr_ = kept.top;
    memcpy(*kept.top, kept_top, sizeof(NJS_MATRIX));
    memcpy(*_nj_base_matrix_ptr_, kept_base, sizeof(NJS_MATRIX));
    memcpy(_nj_view_matrix_, kept_view, sizeof(NJS_MATRIX));
#ifdef DREAMCAST
    ld_ext((float (*)[4])kept_loaded);
#endif
}

void frame_begin(void)
{
    frame.start = syTmrGetCount();
    port_pad();
}

void present_step(int done, void (*draw)(void))
{
    Uint32 spent;
    unsigned long seed;

    if (draw == NULL || frame_interval != HELD_BLANKS) {
        return;
    }
    spent = syTmrDiffCount(frame.start, syTmrGetCount());
    frame.asked = TRUE;
    frame.count = frame_step_count;
    if (done == frame_step_count) {
        frame.steps = spent;
    }
    if (done != 1 || frame_step_count != SPLIT_STEPS) {
        return;
    }
    frame.first = spent;
    if (!engaged || !port_toggles[TOGGLE_FAST] || frame_render_off == TRUE) {
        return;
    }
    syTmrGenStop();
    seed = _rnext;
    view_get();
    draw();
    readout_draw();
    view_put();
    _rnext = seed;
    njRenderStart();
    njFlipFrameBuffer();
    syTmrGenStart();
    frame.shown = TRUE;
}

/* engage or end the split by the frame ending, as port.h says. busy is how
 * long the frame ran up to its wait. a frame no loop offered the split
 * starts the count again. a held frame of another count of steps changes
 * nothing */
static void split_plan(Uint32 busy, int late)
{
    Uint32 blank =
        port_blank_rate() == PAL_BLANKS ? PAL_BLANK_COUNTS : BLANK_COUNTS;
    Uint32 room = ENGAGE_ROOM(blank);
    Uint32 draw;

    if (!port_toggles[TOGGLE_FAST] || !frame.asked) {
        engaged = FALSE;
        fit_run = 0;
        return;
    }
    if (frame.count != SPLIT_STEPS) {
        return;
    }
    if (engaged) {
        if (late != 0) {
            if (calm < SETTLE_FRAMES) {
                engaged = FALSE;
                wait_left = wait_next;
                wait_next =
                    wait_next * 2 < WAIT_MOST ? wait_next * 2 : WAIT_MOST;
            }
            calm = 0;
        } else if (calm < SETTLE_FRAMES) {
            calm++;
        }
        if (engaged && ++lasted >= WAIT_MOST) {
            lasted = 0;
            wait_next = SETTLE_FRAMES;
        }
        return;
    }
    if (wait_left > 0) {
        wait_left--;
        fit_run = 0;
        return;
    }
    /* each half holds a step and a draw the length of the frame's own */
    draw = busy > frame.steps ? busy - frame.steps : 0;
    if (late == 0 && frame.first + draw + room <= blank &&
        frame.steps - frame.first + draw + room <= blank) {
        if (++fit_run >= ENGAGE_FRAMES) {
            engaged = TRUE;
            fit_run = 0;
            calm = SETTLE_FRAMES;
            lasted = 0;
        }
    } else {
        fit_run = 0;
    }
}

void frame_end(Uint32 wait, int late)
{
    split_plan(syTmrDiffCount(frame.start, wait), late);
    if (frame_render_off != TRUE) {
        readout_frame(late, 1 + frame.shown);
        wide_end();
        readout_draw();
    }
    frame.asked = frame.shown = FALSE;
    wide_apply();
}
