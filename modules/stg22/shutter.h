/* Crazy Gadget's SHUTTER, a door whose side panels slide apart and whose top
 * rises */
#ifndef STG22_SHUTTER_H
#define STG22_SHUTTER_H
#include <ninja.h>
struct task;

extern NJS_TEXLIST *gadget_shutter_texlists[];
/* the object list's init for SHUTTER. it does nothing where task leaves at its
 * placement's range. where the heap can't give the shutter's motion, it does
 * nothing, and the next step tries again. otherwise it gives task the
 * shutter's step, draw and destructor, and a collision record of kind
 * COLLISION_OBJECT.
 *
 * task needs a work. pos places the shutter, and ang.y turns it. the low two
 * bits of ang.z pick its trigger, BY_SWITCH to BY_PROXIMITY of
 * shared/shutter_motion.h. the low byte of ang.x is the id of its switch, its
 * enemy group or its dynamite. a switch's id must be below SWITCH_IDS of
 * object/switch_state.h, unchecked.
 *
 * a switch shutter opens while its switch is on. a dynamite shutter opens
 * while no charge of its id is left. an enemy shutter opens while its group
 * holds no enemy. it opens only with a player within scl.x + 100 of pos.
 * where its trigger stops holding, each of the three shuts at once.
 *
 * a proximity shutter opens once either player lies in its box. once it is
 * open and neither player does, it closes. the box lies in the shutter's own
 * frame. it spans -100 to 100 in x, -10 to 80 in y and -80 to 150 in z */
void gadget_shutter_init(struct task *task);
#endif
