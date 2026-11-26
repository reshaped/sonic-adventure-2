/* the stake, STAKE in Dry Lagoon's object list: a post of two kinds */
#ifndef STG18_STAKE_H
#define STG18_STAKE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the stake's texture lists for texture_load_shared, a kind's at its index,
 * ended by null */
extern NJS_TEXLIST *stake_texlists[];

/* build each kind's model at buf, if it isn't built yet and its texture list
 * is loaded. it returns buf past the builds. the stake draws from the builds
 * once one call has built both, and from its models before. where
 * objects_fog_off isn't 0, the call builds with the fog off, then turns the
 * fog on */
Uint8 *stake_build(Uint8 *buf);

/* the object list's step for STAKE. unless placement_try_leave(task) returns
 * TRUE, it gives task the stake's own step, draw and destructor. it gives
 * task a collision record of one capsule, of kind COLLISION_OBJECT. the
 * capsule pushes players, enemies and objects, and resists their push. the
 * heap must have room for the record, unchecked.
 *
 * task needs a work. its pos places the stake, and scl.x + 1 scales it and
 * its capsule. its ang turns both about z, then x, then y. (int)|scl.z|
 * picks the kind: BRIGHT where it is even, and DARK where it is odd. each
 * step registers the record, until placement_try_leave(task) returns TRUE */
void stake_init(struct task *task);

#endif
