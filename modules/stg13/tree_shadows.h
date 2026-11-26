/* the trees' shadows, TREESHADOWS in City Escape's object list. a player
 * inside one of its six cylinders takes the shadows' light */
#ifndef STG13_TREE_SHADOWS_H
#define STG13_TREE_SHADOWS_H

struct task;

/* the object list's step for TREESHADOWS. unless placement_try_leave(task)
 * returns TRUE, it gives task a record in task->extra and a collision record
 * of the cylinders. it also gives task the shadows' own step, draw and
 * destructor. where the heap can't give the record, tree_shadows_init stays
 * task's step and tries again on the next step.
 *
 * task needs a work, whose pos places the cylinders and whose ang.y turns
 * them. the low byte of ang.x is the index of the shadows' light. the call
 * clears ang.x, ang.z and the low byte of ang.y.
 *
 * a player whose first shape comes to touch a cylinder takes the shadows'
 * light. a player whose first shape then touches none takes light_camera,
 * which draw/light.h declares. the stage selects that light for the players
 * it places. a player inside as the task ends keeps the shadows' light */
void tree_shadows_init(struct task *task);

#endif
