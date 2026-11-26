/* the drum can, DRMCN in Sky Rail's object list: the can of shared/drum_can.h
 * with textures of its own. it scores and sounds only at the hit */
#ifndef STG06_DRUM_CAN_H
#define STG06_DRUM_CAN_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the can and of its pieces,
 * ended by null */
extern NJS_TEXLIST *sky_rail_drum_can_texlists[];

/* the object list's step for DRMCN. it does what drum_can_init does, for this
 * can. the hit scores 20 points before it bounces player 0, and plays the
 * can's sound after. the break's steps neither score nor sound. the can's
 * collision stays in the checks through the break */
void sky_rail_drum_can_init(struct task *task);

#endif
