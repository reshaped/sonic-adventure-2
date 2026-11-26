/* the gas cylinder, BOMBE in Prison Lane's object list. it bursts into pieces
 * at an attack's touch */
#ifndef STG09_GAS_CYLINDER_H
#define STG09_GAS_CYLINDER_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the cylinder, its pieces and
 * their puffs and bursts, ended by null */
extern NJS_TEXLIST *gas_cylinder_texlists[];

/* the object list's step for BOMBE. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the cylinder's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. the record's
 * one shape is an upright cylinder. where the heap has no room for it, the
 * cylinder goes on without one, and never bursts.
 *
 * task needs a work, whose pos places the cylinder and whose ang.y turns it.
 * each later step tests the range as the first did. the work's submode carries
 * the placement's state, unchanged, to the next task. the first attack that
 * touches the cylinder bursts it into six pieces, with object_sound_play's
 * kind 9. the burst scores 20 points and ends the task */
void gas_cylinder_init(struct task *task);

#endif
