/* the treasure box, BOX in Wild Canyon's object list: a chest. a Knuckles or
 * a Rouge breaks it with an attack, and it stays broken */
#ifndef SHARED_TREASURE_BOX_H
#define SHARED_TREASURE_BOX_H
#include <ninja.h>

struct task;

/* the texture lists of the whole box, the broken box and its five pieces,
 * ended by null. a stage's texture groups hold the array */
extern NJS_TEXLIST *treasure_box_texlists[];

/* the object list's step for BOX. it does nothing where half_try_leave or
 * placement_try_leave returns TRUE for task. otherwise it gives task the box's
 * own step, draw and destructor. task needs a work, whose pos and ang.y place
 * the box.
 *
 * a box whose placement's state isn't 0 starts broken. any other box starts
 * whole and gets a collision record, as does a box without a placement. it
 * also puts a copy of the box's collision model in the collision world.
 * world_pool_alloc lends the copy, which task->extra keeps. the pool must have
 * a copy to lend, and the heap room for the record, each unchecked.
 *
 * a player who touches a whole box while hunter_attacking holds breaks it.
 * the break throws the box's pieces and takes its copy out of the world. it
 * sets the placement's state to 1, plays a sound and vibrates the player's pad.
 * a broken box only draws, until it leaves range */
void treasure_box_init(struct task *task);

#endif
