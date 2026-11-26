/* frame pacing: the vertical blanks and steps of a frame, its render, and the
 * vertical blank callback */
#include <kamui.h>
#include <ninja.h>
#include <sg_bup.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "sound/sound.h"
#include "system/frame.h"
#include "system/loader.h"
#include "system/task.h"

#ifdef NONMATCHING
/* the port's calls at a frame's end, a serve of the pads and a blank */
#include "engine/dc/port.h"
#endif

int frame_interval;
/* @hack type: frame_countdown left without volatile, where a volatile int
 * would do: frame_vsync counts it down while frame_wait and frame_wait_step
 * spin on it */
#ifdef NONMATCHING
/* without volatile, GCC reads frame_countdown once for a wait, which then
 * spins for good or not at all. SHC's code reads it at each pass */
volatile int frame_countdown;
#else
int frame_countdown;
#endif
int frame_step_count;
int frame_step;
Bool frame_render_off;

static void frame_vsync(void);

/* make each frame last interval vertical blanks, and start the running frame
 * again with interval blanks left. nothing calls it, and no header declares
 * it. it stays external, since SHC leaves out a static function that nothing
 * calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void frame_set_interval(int interval);
/* NOLINTEND(misc-use-internal-linkage) */

void frame_pacing_init(int interval, int steps)
{
    frame_interval = interval;
    /* @hack spelling: frame_interval read back, where interval would do */
    frame_countdown = frame_interval;
    frame_step_count = steps;
    frame_step = 0;
    frame_render_off = FALSE;
    njSetVSyncFunction(frame_vsync);
}

void frame_set_interval(int interval)
{
    frame_interval = interval;
    frame_countdown = interval;
}

void frame_wait(void)
{
#ifdef NONMATCHING
    /* this side notes when the wait began and how late the frame ran, for
     * the port. the flip waits out the first blank of the frame it starts.
     * so a frame that ends in time finds 0 blanks left or more, and one
     * that ran n blanks late finds -n */
    Uint32 wait = syTmrGetCount();
    int left = frame_countdown;

#endif
    while (frame_countdown > 0) {
    }
    frame_countdown = frame_interval;
#ifdef NONMATCHING
    /* this side ends the frame for the port, which draws its readout over
     * a frame that renders (engine/dc/port.h) */
    frame_end(wait, left < 0 ? -left : 0);
#endif
    /* @hack spelling: frame_render_off compared with TRUE, where
     * frame_render_off alone would do while it holds TRUE or FALSE */
    if (frame_render_off == TRUE) {
        kmResetPointer();
    } else {
        njRenderStart();
        njFlipFrameBuffer();
    }
    pdExecPeripheralServer();
#ifdef NONMATCHING
    /* this side starts the next frame for the port, which reads its chord
     * before the game reads the pads */
    frame_begin();
#endif
}

void frame_wait_step(int left)
{
    while (frame_countdown > left) {
    }
    pdExecPeripheralServer();
#ifdef NONMATCHING
    /* this side reads the port's chord, as frame_wait does */
    port_pad();
#endif
    sound_update();
}

static void frame_vsync(void)
{
#ifdef NONMATCHING
    /* this side counts the blank for the port's readout */
    port_blank();
#endif
    frame_countdown--;
    if (frame_countdown <= 0) {
        sdSysServer();
    }
    buExecServer();
    loader_poll_drive();
}

void task_step_level_6(void)
{
    task_step_level(6);
}
