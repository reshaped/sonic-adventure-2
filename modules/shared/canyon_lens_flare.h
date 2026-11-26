/* Wild Canyon's lens flare, EFLENSF0 in its object list: the lens flare of
 * shared/lens_flare.h. it shows only from the canyon's high half, and from near
 * one spot below it */
#ifndef SHARED_CANYON_LENS_FLARE_H
#define SHARED_CANYON_LENS_FLARE_H

struct task;

/* the object list's step for EFLENSF0. it sets the flare up as lens_flare_init
 * does, with a list of rivals of its own. the flare draws while player 0 stands
 * at HALF_LINE or above. below it, the flare draws while player 0 lies within
 * 70 of x 200, z 0. player_work[0] must not be null while the flare draws,
 * unchecked */
void canyon_lens_flare_init(struct task *task);

#endif
