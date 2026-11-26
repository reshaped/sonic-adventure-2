/* the ramp, TJUMPDAI in City Escape's object list: a solid ramp */
#ifndef STG13_RAMP_H
#define STG13_RAMP_H

#include <ninja.h>

struct task;

/* the ramp's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *ramp_texlists[];
/* the ramp's chunk object, which no code reads. the draw takes ramp_model */
extern NJS_CNK_OBJECT ramp_object;

/* the object list's step for TJUMPDAI. unless placement_try_leave(task)
 * returns TRUE, it gives task a record in task->motion. it also gives task the
 * ramp's own step, draw and destructor. where the heap can't give the record,
 * this stays task's step and tries again on the next.
 *
 * task needs a work. pos places the ramp, and ang turns it. the init puts a
 * copy of the ramp's collision object in the collision world. the copy lies as
 * the ramp does. the world collects the copy only while a player is near.
 * player_near tests for one within the collision model's radius plus 5 of its
 * center. the init sets the work's id to WORK_ID_RAMP, which sonic_test_jump's
 * ramp jump needs.
 *
 * once player 0 steps off the ramp, the step sets the placement's state to 1.
 * a trick off a ramp in that state takes SONIC_TRICK_GRADE_PASSED */
void ramp_init(struct task *task);

#endif
