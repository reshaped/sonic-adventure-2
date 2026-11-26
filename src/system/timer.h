/* the frame timer, on Shinobi's general-purpose timer, and the task profiler
 * that reads it. every time here is in thousandths of a vertical blank: 1000
 * is 1/60 s */
#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H
#include <sg_xpt.h>

struct task;

/* the time since the mark, which moves to now */
Float timer_lap(void);
/* move the mark to now */
void timer_mark(void);
/* start a frame's times: restart the timer at 0, and move the mark to now.
 * it also sets the callback that Ninja calls as each render ends. the
 * callback notes the time for timer_frame_end, then steps the tasks of level
 * 6 */
void timer_frame_start(void);
/* end a frame's times. game_state.frame_time takes the time since
 * timer_frame_start, and the mark moves to now. game_state.render_time takes
 * the time from timer_frame_start to the end of the last render */
void timer_frame_end(void);

/* the profiler's hooks, which the task walks call around each step and each
 * draw. a task without a step or a draw passes null. the profiler is
 * switched off: each hook returns at once, and none moves the mark */
void profile_step_start(void (*step)(struct task *task), const char *name);
void profile_step_end(void (*step)(struct task *task));
void profile_draw_start(void (*draw)(struct task *task));
void profile_draw_end(void (*draw)(struct task *task));

#endif
