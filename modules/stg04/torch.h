/* the torch, TAIMATU in White Jungle's object list: a post with a flame at its
 * top. both push players, and the flame hurts them. the flame lights each
 * player near it */
#ifndef STG04_TORCH_H
#define STG04_TORCH_H
#include <ninja.h>

struct task;

/* the torch's texture lists for texture_load_shared, its model's and its
 * flame's, ended by null */
extern NJS_TEXLIST *torch_texlists[];

/* the object list's step for TAIMATU. it does nothing where task leaves at
 * its placement's range. otherwise it gives task the torch's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. it holds the
 * flame's sound at the work's pos, keyed by the work, until the destructor
 * stops it.
 *
 * task needs a work, whose pos places the torch and whose ang.y turns it. each
 * later step tests the range as the first did. it lights players 0 and 1 near
 * the flame by object_light_add_near */
void torch_init(struct task *task);

#endif
