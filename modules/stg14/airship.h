/* the airship, AIRSHIP in Radical Highway's object list: a model whose child
 * swings about z */
#ifndef STG14_AIRSHIP_H
#define STG14_AIRSHIP_H
#include <ninja.h>

struct task;

/* the models' texture lists for texture_load_shared, model 0's first, ended
 * by null */
extern NJS_TEXLIST *airship_texlists[];

/* the object list's step for AIRSHIP. unless placement_try_leave(task) returns
 * TRUE, it gives task the airship's own step, draw and destructor.
 *
 * task needs a work. pos places the airship, and ang.y turns it. a scl.x of 1
 * or more draws model 1, and less draws model 0. the model's child swings
 * about z, 30 degrees each way, by the sine of the work's timer. each step adds
 * ang.x to the timer while game_state.time_stopped is 0 */
void airship_init(struct task *task);

#endif
