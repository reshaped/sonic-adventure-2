/* HASHIRA in Hidden Base's object list: a tower whose supports blow up */
#ifndef STG23_TOWER_H
#define STG23_TOWER_H
#include <ninja.h>

struct task;
extern NJS_TEXLIST *base_tower_texlists[];

/* the object list's step for HASHIRA. task needs a work. the tower stands in
 * four runs of tiers: from the bottom up, supports, platforms, supports and
 * platforms. the nibbles of ang.z, low first, give the runs' lengths. ang.y
 * turns the tiers. ang.x turns each support's three charges about it, in
 * quarter turns. blowing up all three breaks the support, and the tiers above
 * it fall by its height.
 *
 * the placement's state keeps which of the lowest eight tiers have broken. the
 * tower starts again with those broken. once the eighth has broken, every tier
 * above it starts again broken too.
 *
 * without room for its table of tiers, the tower makes no tiers. its step then
 * reads the table through null. without room for a tier's task, the tower
 * destroys itself */
void base_tower_init(struct task *task);

#endif
