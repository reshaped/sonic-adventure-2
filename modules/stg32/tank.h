/* the tank, TANK in Meteor Herd's object list: a tank that a player's attack
 * breaks, leaving its base */
#ifndef STG32_TANK_H
#define STG32_TANK_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the tank, its base and its
 * pieces, ended by null */
extern NJS_TEXLIST *meteor_tank_textures[];
/* the object list's step for TANK. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the tank's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. task needs a
 * work, whose pos places the tank and whose ang.y turns it.
 *
 * a player's attack on the record's upright cylinder breaks the tank.
 * breakable_player_hit of shared/breakable_hit.h judges the attack. the break
 * throws the tank's ten pieces, and scores 20 points. the cylinder then
 * collides no more, but the record's capsule still does. the work's submode is
 * 1 once the tank is broken. the destructor keeps it in the placement's state.
 * the placement's next task starts with it, so a broken tank stays broken.
 * where the heap has no room for the record, the tank never breaks */
void meteor_tank_init(struct task *task);

#endif
