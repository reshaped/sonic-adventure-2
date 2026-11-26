/* the train's gate, GSGATE in Pumpkin Hill's object list. it puffs smoke while
 * a ghost train passes. this source also holds the stage's list of
 * sounds */
#ifndef STG05_TRAIN_GATE_H
#define STG05_TRAIN_GATE_H
#include <ninja.h>

#include "sound/lists.h"

struct task;

/* the stage's sound list, which pumpkin_hill_init hands to sound_load_stage
 * with se_ac_ph.mlt. sounds 0 to 4 are programs 0 to 4 of bank 5. sound 5 is
 * sequence 0 of bank 6 */
extern const struct sound_list pumpkin_hill_sounds;
/* the gate's texture lists for texture_load_shared: none, only the null that
 * ends them */
extern NJS_TEXLIST *train_gate_texlists[];

/* the object list's step for GSGATE. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the gate's step, draw and
 * destructor.
 *
 * task needs a work. its pos places the gate, and its ang.y turns it. each
 * later step tests the range as the first did. the gate draws nothing. it
 * puffs smoke while a ghost train's engine lies within scl.z + 30 of pos. each
 * step then gives a puff from a random point of its face. the face lies across
 * the gate's x and y, scl.x + 30 wide and scl.y + 30 high */
void train_gate_init(struct task *task);

#endif
