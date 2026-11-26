/* Pyramid Cave's sandglass, SANDGLASS in the stage's object list:
 * shared/sandglass.h's sandglass. the stage's sounds are written in where that
 * one calls hooks */
#ifndef STG28_SANDGLASS_H
#define STG28_SANDGLASS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the texture lists for texture_load_shared, ended by null: the frame's, the
 * ends', the sand's and the stream's */
extern NJS_TEXLIST *cave_sandglass_texlists[];

/* the object list's step for SANDGLASS. task needs a work and a motion
 * record. the work's pos is where the sandglass stands and lands. its ang.y
 * turns it, until a landing sets ang.y to 0. the low byte of ang.z is its id,
 * and its sand runs for scl.x steps.
 *
 * the call does nothing where placement_try_leave(task) is TRUE. otherwise it
 * gives task its record and two collision children, the button's and the
 * body's. it registers task in cave_sandglasses where the table has room. where
 * the heap can't give the record, its matrix or a child, the call frees what it
 * took. it then runs again at task's next step.
 *
 * a standing sandglass turns over at a touch on its button, or at a blow to
 * its body. a blow counts only from a player for whom
 * cave_sonic_combo_or_bounce is TRUE. a touch by player 0 bounces player 0
 * up. a blow turns the sandglass over away from the player who struck. where
 * cave_sonic_in_combo is TRUE too, the blow stops player 0, whichever player
 * struck. a strike plays sound 3 of the stage's list, and each tick of the run
 * sound 4 */
void cave_sandglass_init(struct task *task);
/* make a sandglass at *pos, turned to ang, with id and a run of time steps.
 * it returns the task, or null where the heap can't give the task. it returns
 * null too where the heap can't give what cave_sandglass_init gives. the task
 * then stays with no record, and its step reads through null.
 *
 * the sandglass stays in SANDGLASS_MODE_HELD until cave_sandglass_throw. it
 * lands at a height of -40. its frame and ends draw translucent while it
 * stands or runs. nothing can strike it. nothing in the stage calls
 * cave_sandglass_create */
struct task *cave_sandglass_create(NJS_POINT3 *pos, Angle ang, int id,
                                   int time);

#endif
