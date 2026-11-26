/* the wall fire, WALL FIRE in Sand Ocean's object list: a dish on a wall.
 * its flame hurts the players. Sand Ocean's two-player stage holds it too,
 * though its object list doesn't name it */
#ifndef SHARED_WALL_FIRE_H
#define SHARED_WALL_FIRE_H
#include <ninja.h>

struct task;

/* the dish's texture list, and the array of it for texture_load_shared,
 * ended by null */
extern NJS_TEXLIST wall_fire_texlist;
extern NJS_TEXLIST *wall_fire_texlists[];
/* the flame model's own list of one texture. the draws bind
 * ocean_fire_texlist of shared/ocean_util.h instead */
extern NJS_TEXLIST wall_fire_flame_texlist;
/* the flame's model, a square that faces the screen. each draw writes the
 * flame's frame into its polygon list, as a texture of ocean_fire_texlist */
/* @hack type: the model an array of one, where the model would do */
extern NJS_CNK_MODEL wall_fire_flame_model[1];

/* the object list's step for WALL FIRE. unless placement_try_leave(task)
 * returns TRUE, it gives task the wall fire's step, draw and destructor. it
 * gives task a collision record of kind COLLISION_OBJECT too, whose sphere
 * about the flame hurts players. where the heap has no room for the record, the
 * task goes on without one.
 *
 * task needs a work. its pos places the dish and its ang.y turns it. 1 more
 * than scl.x scales the whole fire. the call sets ang.x and ang.z to 0. the
 * fire plays its sound while player 0 is near */
void wall_fire_init(struct task *task);
extern NJS_CNK_MODEL wall_fire_model;

#endif
