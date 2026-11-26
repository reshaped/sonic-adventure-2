/* the Dark Garden's props: eight kinds of object that stand still and draw a
 * model each. the graves draw a shadow too */
#ifndef CHAOSTGDARK_PROPS_H
#define CHAOSTGDARK_PROPS_H
#include "chao/garden_scene.h"

struct task;

/* the steps chaostgdark_kinds holds for the props. each gives task the
 * prop's step, draw and destructor. dark_long_fence_init,
 * dark_medium_fence_init, dark_short_fence_init, dark_grave_a_init and
 * dark_grave_b_init also give it a collision record. the task must hold a
 * work. it destroys itself once placement_try_leave finds it has left range */
void dark_long_fence_init(struct task *task);
void dark_medium_fence_init(struct task *task);
void dark_short_fence_init(struct task *task);
void dark_grave_a_init(struct task *task);
void dark_grave_b_init(struct task *task);
void dark_stump_init(struct task *task);
void dark_kuwa_a_init(struct task *task);
void dark_grass_b_init(struct task *task);

#endif
