/* the treasure chest, TREASURE in Death Chamber's object list: a chest that a
 * Knuckles or a Rouge breaks */
#ifndef STG25_TREASURE_H
#define STG25_TREASURE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the texture lists for texture_load_shared of the whole chest and of its
 * pieces. a color's lies at its index, and each array ends in null */
extern NJS_TEXLIST *chamber_treasure_texlists[];
extern NJS_TEXLIST *chamber_treasure_pieces_texlists[];

/* compile each color's chest at buf, as model_builds_compile does, and return
 * buf past the builds. the chests draw the builds once one call has compiled
 * all three. while objects_fog_off isn't 0, the call turns the fog off for
 * the builds and on after them */
Uint8 *chamber_treasure_build(Uint8 *buf);

/* the object list's step for TREASURE. task needs a work and a placement. the
 * work's pos places the chest, and its ang.y turns it. CHAMBER_COLOR of
 * stg25/chamber_color.h picks its color. a chest whose placement's state isn't
 * 0 is broken, and draws nothing.
 *
 * the players collide with a copy of an unbroken chest's shape in the collision
 * world, which task->extra holds. world_pool_alloc lends the copy, and the call
 * writes through null where the pool has none. the copy collides only while a
 * player lies within 36 of its pos.
 *
 * the call also gives an unbroken chest a collision record of one box. the heap
 * must have room for the record, unchecked. the box pushes no shape of a
 * player, an enemy, an object or a pickup. it resists the push of players,
 * enemies and objects.
 *
 * the chest breaks where chamber_hunter_attacking holds for the first player
 * its record touched. it breaks into eight pieces, which vanish at the floor
 * under it. the break plays a sound at the chest's pos. it sets the placement's
 * state to 1, and kills the placement with placement_kill */
void chamber_treasure_init(struct task *task);

#endif
