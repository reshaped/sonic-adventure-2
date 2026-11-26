/* the gravestone, GRVSTONE in Pumpkin Hill's object list: a stone the players
 * stand on. a part of it swings to and fro */
#ifndef STG05_GRAVESTONE_H
#define STG05_GRAVESTONE_H

#include <ninja.h>

struct task;

/* the stones' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *gravestone_texlists[];

/* the object list's step for GRVSTONE. it does nothing where task leaves at its
 * placement's range. otherwise the call gives task the stone's step, draw and
 * destructor.
 *
 * task needs a work. its pos places the stone, and its ang.y turns it. bit 0 of
 * ang.x makes it the A stone, of another model. each later step tests the
 * range as the first did.
 *
 * the players stand on a copy of the stone's collision model in the collision
 * world, which task->motion holds. the copy comes from syCalloc. where syCalloc
 * fails, the call does nothing, and the next step tries again. the copy
 * collides only while a player lies within its model's radius plus 5 of its
 * pos. the call also gives task a collision record of one box, which only
 * players meet */
void gravestone_init(struct task *task);

#endif
