/* the flag, FLAG in a stage's object list: a flag that waves, red or blue */
#ifndef SHARED_FLAG_H
#define SHARED_FLAG_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the flag's 24 frames, blue
 * and red, each ended by null */
extern NJS_TEXLIST *flag_blue_texlists[];
extern NJS_TEXLIST *flag_red_texlists[];

/* the object list's step for FLAG. unless placement_try_leave(task) returns
 * TRUE, it gives task the flag's own step, draw and destructor. it gives the
 * flag a random frame to start its wave from too. the flag's step only
 * leaves as placement_try_leave decides.
 *
 * task needs a work. pos places the flag, and ang turns it by njRotateY, then
 * njRotateX. the flag is blue where ang.z is odd, and red where it is even.
 * the wave moves on a frame with each count of game_state.steps */
void flag_init(struct task *task);

#endif
