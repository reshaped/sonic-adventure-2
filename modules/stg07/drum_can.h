/* the drum can, DRMCN and drmcun in Aquatic Mine's object list. it is the can
 * of shared/drum_can.h, with textures of its own. it scores and sounds only at
 * the hit */
#ifndef STG07_DRUM_CAN_H
#define STG07_DRUM_CAN_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the can and of its pieces,
 * ended by null */
extern NJS_TEXLIST *mine_drum_can_texlists[];

/* the object list's step for DRMCN and drmcun. it does what drum_can_init
 * does, for this can. once the hit has bounced player 0, it scores 20 points
 * and plays the can's sound. the break's steps neither score nor sound */
void mine_drum_can_init(struct task *task);

#endif
