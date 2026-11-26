/* the rocket, ROCKET in a stage's object list: a launcher that raises a
 * rocket from its base. a player who takes hold of the rocket rides it to a
 * place */
#ifndef SHARED_ROCKET_H
#define SHARED_ROCKET_H
#include <ninja.h>

struct task;

/* the object list's step for ROCKET. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the
 * launcher's record in task->motion, and its own step, draw and destructor.
 * where the heap has no room for it, task keeps this step and tries again
 * on its next. each rocket the launcher makes is a child task, and the heap
 * must have room for it, unchecked.
 *
 * task needs a work. pos places the launcher, and scl is the place the
 * rocket flies to. the low 5 bits of ang.z are a switch's id. with bit 0 of
 * ang.x set, the launcher makes a rocket only while that switch is on.
 * rocket_init clears the other bits of both, and turns ang.y to face the
 * place.
 *
 * the rocket stands upright where the place lies below the launcher, or more
 * than an eighth turn above */
void rocket_init(struct task *task);

/* the launcher's and its rockets' texture lists for texture_load_shared,
 * ended by null */
extern NJS_TEXLIST *rocket_texlists[];

/* the rocket's and the base's textures, models and motions, which Final
 * Rush's STROCKET draws with too */
extern NJS_TEXLIST rocket_texlist;
extern NJS_CNK_OBJECT rocket_object;
extern NJS_TEXLIST rocket_base_texlist;
extern NJS_CNK_OBJECT rocket_base_object;
extern NJS_OBJECT rocket_base_collision;
extern NJS_MOTION rocket_motion;
extern NJS_MOTION rocket_base_motion;

#endif
