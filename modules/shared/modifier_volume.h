/* the modifier volumes, MODMOD in a stage's object list. each is a box or a
 * cylinder, drawn as a modifier volume */
#ifndef SHARED_MODIFIER_VOLUME_H
#define SHARED_MODIFIER_VOLUME_H
#include <ninja.h>

struct task;

/* the object list's step for MODMOD. unless placement_try_leave(task)
 * returns TRUE, it gives task its own step, draw and destructor. task needs a
 * work. the step does nothing but end task once placement_try_leave finds it
 * out of range.
 *
 * bit 0 of ang.z picks the shape: 0 a cylinder along y, 1 a box. each shape
 * is 30 across, scaled by scl + 1 along its own axes. ang.y then turns it
 * about y, and ang.x after that about x. pos is its center. the draw draws
 * it with njCnkModDrawModel */
void modifier_volume_init(struct task *task);

/* the box's volume model, a cube 30 on a side, centered on its origin. it is
 * a volume chunk of 12 triangles */
extern NJS_CNK_MODEL modifier_volume_box_model;
/* the cylinder's volume model: a hexagonal prism 30 high along y and 15 in
 * radius. it is centered on its origin */
extern NJS_CNK_MODEL modifier_volume_cylinder_model;
/* modifier_volume_box_model under a second name, as an array.
 * shared/parked_car.c copies the model through this name: through the
 * model's own, SHC builds other code there */
#ifdef NONMATCHING
/* the model's address under the name parked_car.c copies through. this side's
 * objects don't define the array, which only symbols.txt places */
#define modifier_volume_box_model_array (&modifier_volume_box_model)
#else
extern NJS_CNK_MODEL modifier_volume_box_model_array[];
#endif

#endif
