/* the sound point, SE in Iron Gate's object list. it plays one sound. the
 * stage's sound list lies in the same source */
#ifndef STG11_SOUND_POINT_H
#define STG11_SOUND_POINT_H

#include "sound/lists.h"

struct task;

/* the stage's sound list, which iron_gate_init hands to sound_load_stage.
 * sounds 0 to 10 are programs 0 to 10 of bank 5. sounds 11 to 13 are its
 * sequences 51 to 53. sound 14 is sequence 0 of bank 6 */
extern const struct sound_list iron_gate_sounds;

/* the object list's step for SE. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the point's step, draw and
 * destructor.
 *
 * task needs a work, whose pos places the sound. each later step tests the
 * range as the first did. it then plays the stage's sound 14 at its pos for 11
 * sound updates. so the sound stops 11 updates after the point's last step */
void iron_gate_sound_point_init(struct task *task);

#endif
