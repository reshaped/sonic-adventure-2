/* the shaft, SHAFT in Death Chamber and Egg Quarters: a model that plays its
 * motion over and over */
#ifndef SHARED_SHAFT_H
#define SHARED_SHAFT_H
#include <ninja.h>

struct task;

/* each stage that links the shaft defines its texture list, model and
 * motion in its shaft_textures.c */
extern NJS_TEXLIST shaft_texlist;

/* the shaft's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *shaft_texlists[];

/* the object list's step for SHAFT. task needs a work. its pos places the
 * shaft, ang.y turns it, and scl.x + 1 scales it along each axis.
 *
 * the call does nothing where placement_try_leave(task) is TRUE. otherwise it
 * gives task the shaft's step, draw and destructor. it sets scl.z, the
 * motion's frame, to 0. each step where placement_try_leave(task) is FALSE
 * adds scl.y + 1 to the frame. the frame wraps at the motion's nbFrame by
 * losing nbFrame once. where it is still nbFrame or more, it goes to 0 */
void shaft_init(struct task *task);

extern NJS_CNK_OBJECT shaft_object;
extern NJS_MOTION shaft_motion[1];

#endif
