/* the imitations of the animals a Chao has taken in */
#ifndef CHAO_ANIMAL_ACTIONS_H
#define CHAO_ANIMAL_ACTIONS_H
#include "chao/behavior.h"
#include "object/animal.h"
/* each animal's imitation, by animal_kind, or null for an animal with none */
extern chao_behavior_fn animal_actions[ANIMAL_KINDS];
/* set the imitation of a random animal the Chao has taken in, of those that
 * have one. FALSE where there is none, which sets nothing */
Bool chao_choose_animal_action(struct task *actor);
/* sing CHAO_ANIM_UTAU_LEFT 4 to 6 times over. no code runs it */
int chao_repeat_song_pose(struct task *chao);
#endif
