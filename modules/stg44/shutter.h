/* the shutter, SHUT in Mad Space's object list: a gate that vanishes once
 * broken */
#ifndef STG44_SHUTTER_H
#define STG44_SHUTTER_H

#include <ninja.h>

struct task;
/* the object list's step for SHUT. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the shutter's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. the record's one
 * shape is a box. it puts a copy of mad_space_shutter_collision into the world
 * as WORLD_SOLID and WORLD_MOVING. the work's pos and ang.y place the copy.
 * where the heap can't give the copy, it tries again at task's next step. each
 * later step tests the range as the first did.
 *
 * task needs a work. the touch of a shape of id 17 breaks the shutter. the
 * break scores 20 through hud.h's score_add. a broken shutter draws nothing,
 * and its work keeps no WORLD_ADDED. an unbroken one keeps WORLD_ADDED while a
 * player lies within 120 of pos. the placement's state keeps the break from one
 * task to the next, so no break scores twice */
void mad_space_shutter_init(struct task *task);
/* the shutter's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *mad_space_shutter_textures[];

#endif
