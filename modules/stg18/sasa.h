/* SASA in Dry Lagoon's object list: a plant that sways once as player 0 runs
 * into it */
#ifndef STG18_SASA_H
#define STG18_SASA_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the plant's texture lists for texture_load_shared, a kind's at its index,
 * ended by null */
extern NJS_TEXLIST *sasa_texlists[];

/* build each kind's model at buf, if it isn't built yet and its texture list
 * is loaded. it returns buf past the builds. the plant draws from the builds
 * once one call has built both, and from its models before. where
 * objects_fog_off isn't 0, the call builds with the fog off, then turns the
 * fog on */
Uint8 *sasa_build(Uint8 *buf);

/* the object list's step for SASA. unless placement_try_leave(task) returns
 * TRUE, it gives task the plant's own step, draw and destructor.
 *
 * task needs a work. its pos places the plant, and scl.x + 1 scales it. its
 * ang turns it about z, then x, then y. (int)|scl.z| picks the kind: BRIGHT
 * where it is even, and DARK where it is odd.
 *
 * the plant's inside reaches from 15 below its pos to 14 times the scale
 * above it. across x and z, it lies within 4 times the scale of pos. player 0
 * starts a sway by running on the ground inside it faster than 0.2. the sway
 * runs once through the kind's shape motion, and plays sound 8 with task as
 * its key. task's unk_3c keeps the sway's frame. it is 0 while the plant stands
 * still, as task_create leaves it. slot 0 must hold a player, unchecked */
void sasa_init(struct task *task);

#endif
