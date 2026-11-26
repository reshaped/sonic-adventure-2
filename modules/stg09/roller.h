/* the roller, ROLL in Prison Lane's object list: a drum that turns on its
 * frame */
#ifndef STG09_ROLLER_H
#define STG09_ROLLER_H
#include <ninja.h>

struct task;

/* the roller's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *roller_texlists[];

/* the object list's step for ROLL. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the roller's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. the record's
 * one shape is the drum's cylinder. the heap must have room for that record.
 * without it, each step reads through null.
 *
 * task needs a work, whose pos places the roller and whose ang.y and ang.z
 * turn it. each later step tests the range as the first did. it then turns
 * the drum by scl.x degrees, and plays the roller's sound at its pos. the
 * work's timer keeps the drum's angle */
void roller_init(struct task *task);

#endif
