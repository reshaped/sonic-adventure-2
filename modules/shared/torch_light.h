/* the torch light, TAIMATSU in a stage's object list: a flickering light on
 * the players near it. it draws nothing */
#ifndef SHARED_TORCH_LIGHT_H
#define SHARED_TORCH_LIGHT_H

struct task;

/* the object list's step for TAIMATSU. task needs a work, whose pos places
 * the light and whose scl.x is its range. unless placement_try_leave_by(task,
 * scl.x squared) returns TRUE, it gives task the light's own step, draw and
 * destructor. each step makes the same test. it then lights the players in
 * slots 0 and 1 by object_light_add_near, and jitters each light it adds */
void torch_light_init(struct task *task);

#endif
