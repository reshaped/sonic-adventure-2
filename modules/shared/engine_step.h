/* the moving platforms of Eternal Engine, STEP and STEPB in a stage's object
 * list */
#ifndef SHARED_ENGINE_STEP_H
#define SHARED_ENGINE_STEP_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST step_texlist;
extern NJS_TEXLIST step_b_texlist;

/* the object list's steps for STEP and STEPB. each does nothing where
 * placement_try_leave(task) returns TRUE. task needs a work, and the platform
 * keeps a record of its own in task->motion. its node goes into the world as a
 * moving solid. where task has a force, its two records carry the riders of
 * players 0 and 1.
 *
 * the work's fields place the platform and shape its path as core_step.h says
 * of Cannon's Core's platforms. a rising or sinking platform comes and goes as
 * core_step.h says too. STEP has a child with a rail along each side, a box
 * that pushes all but pickups. STEPB has none */
void engine_step_init(struct task *task);
void engine_step_b_init(struct task *task);

#endif
