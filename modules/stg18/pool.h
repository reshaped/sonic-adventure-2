/* POOL in Dry Lagoon's object list: a well of water that the players can
 * stand in */
#ifndef STG18_POOL_H
#define STG18_POOL_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the pool's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *pool_texlists[];

/* build the pool's model at buf, if it isn't built yet and its texture list
 * is loaded. it returns buf past the build. the pool draws from its build
 * once built, and from its model before. where objects_fog_off isn't 0, the
 * call builds with the fog off, then turns the fog on */
Uint8 *pool_build(Uint8 *buf);

/* the object list's step for POOL. unless placement_try_leave(task) returns
 * TRUE, it gives task a child. where task_create_child can't give one, the
 * call does nothing more. otherwise it gives task the pool's own step, draw
 * and destructor. the well's four walls take a collision record of kind
 * COLLISION_OBJECT. the heap must have room for the record, unchecked.
 *
 * the child puts a copy of the water's shape in the collision world, under
 * WORLD_SOLID and WORLD_FLAT_THIN. world_pool_alloc lends the copy, which the
 * child's extra keeps. where world_pool_alloc has none to lend, the child
 * writes through null.
 *
 * task needs a work. its pos places the pool and the copy, and scl.x + 1
 * scales both in x and z. its ang.y turns the pool, and its ang the copy. the
 * copy collides only while a player lies within 50 * (scl.x + 1) + 10 of pos.
 * player 0 must also lie less than 20 above or below it. player 0's work must
 * not be null, unchecked */
void pool_init(struct task *task);

#endif
