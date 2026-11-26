/* the sound point, SE in Prison Lane's object list. it plays a gas leak's
 * sound or a fan's. the stage's sound list lies in the same source */
#ifndef STG09_SOUND_POINT_H
#define STG09_SOUND_POINT_H

#include "sound/lists.h"

struct task;

/* the stage's sound list, which prison_lane_init hands to sound_load_stage.
 * sounds 0 to 9 are programs 0 to 9 of bank 5. sounds 10 and 11 are its
 * sequences 51 and 52. sounds 12 and 13 are sequences 0 and 1 of bank 6 */
extern const struct sound_list prison_lane_sounds;

/* the object list's step for SE. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the point's step, draw and
 * destructor.
 *
 * task needs a work, whose pos places the sound. bit 0 of ang.x picks the gas
 * leak's sound over the fan's. each later step tests the range as the first
 * did. it then plays the sound at its pos for 11 sound updates. so the sound
 * stops 11 updates after the point's last step */
void sound_point_init(struct task *task);

#endif
