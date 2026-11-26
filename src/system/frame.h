/* frame pacing: the vertical blanks and steps of a frame, its render, and the
 * vertical blank callback */
#ifndef SYSTEM_FRAME_H
#define SYSTEM_FRAME_H
#include <sg_xpt.h>

#include "system/pad.h"
#include "system/task.h"

/* the vertical blanks to a frame, and those left in the running frame */
extern int frame_interval;
#ifdef NONMATCHING
/* volatile, as frame.c defines it on this side, so a wait reads it at each
 * pass */
extern volatile int frame_countdown;
#else
extern int frame_countdown;
#endif
/* the steps to a frame, and the running frame's step, from 0 */
extern int frame_step_count;
extern int frame_step;
/* while it is TRUE, frame_wait drops what the frame drew. it then calls
 * kmResetPointer in place of njRenderStart and njFlipFrameBuffer */
extern Bool frame_render_off;

/* make each frame last interval vertical blanks and take steps steps. the
 * running frame starts again at step 0, with interval blanks left, and
 * frame_render_off goes FALSE. the vertical blank callback it installs counts
 * frame_countdown down by 1. where that leaves it at 0 or below, the callback
 * serves the sound driver. at every blank it then serves the memory cards and
 * calls loader_poll_drive */
void frame_pacing_init(int interval, int steps);
/* wait out the running frame's vertical blanks, and start counting the next
 * frame's. then render what the frame drew, flip to it and serve the pads */
void frame_wait(void);
/* wait until left or fewer vertical blanks of the running frame remain, then
 * serve the pads and call sound_update */
void frame_wait_step(int left);
/* step the tasks of level 6, which task_step_all passes over */
void task_step_level_6(void);

/* under DISPLAY_PAL, one frame in every PAL_FRAMES takes a step more, so 50
 * frames take 60 steps */
#define PAL_FRAMES 5
/* RUN_TASKS steps the tasks twice in the frame that brings pal_frames to
 * PAL_EXTRA_FRAME */
#define PAL_EXTRA_FRAME 2
/* scan the pads, step the tasks for a frame and draw them. under DISPLAY_PAL,
 * it counts frames in the caller's pal_frames, which it wraps to 0 at
 * PAL_FRAMES */
#define RUN_TASKS()                                                            \
    pad_scan();                                                                \
    task_step_all();                                                           \
    if (game_state.display == DISPLAY_PAL) {                                   \
        pal_frames++;                                                          \
        if (pal_frames == PAL_EXTRA_FRAME) {                                   \
            pdExecPeripheralServer();                                          \
            pad_scan();                                                        \
            task_step_all();                                                   \
        }                                                                      \
        if (pal_frames >= PAL_FRAMES) {                                        \
            pal_frames = 0;                                                    \
        }                                                                      \
    }                                                                          \
    task_draw_all()

#endif
