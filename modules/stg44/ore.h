/* the ore, ORE in Mad Space's object list: a rock that a player's attacks
 * crack */
#ifndef STG44_ORE_H
#define STG44_ORE_H

#include <ninja.h>
struct task;

/* the object list's step for ORE. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the rock's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. where the heap
 * can't give the record, the init reads through null. each later step tests
 * the range as the first did.
 *
 * task needs a work, whose pos places the rock and whose three angles turn it.
 * a scl.x of 1 or more makes a small rock, and less a large one. the init sets
 * scl.y and scl.z to 0, and the rock keeps its sway there. it keeps its hits in
 * submode and its cooldown in tick.
 *
 * a player's attack hits the rock, but not within 15 steps of the last attack
 * to touch it. a hit sways the rock. the small rock cracks at its first hit,
 * and the large one at its second. the crack scores 20 through hud.h's
 * score_add and throws chips. a cracked rock takes no more hits. a rock that
 * leaves its range and comes back keeps its hits, through the placement's
 * state */
void mad_space_ore_init(struct task *task);
/* the rocks' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *mad_space_ore_textures[];

#endif
