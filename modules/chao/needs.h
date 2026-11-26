/* the scoring of the behaviors that fear of the player, boredom, hunger and
 * illness drive. illness_actions holds a behavior for each of the six
 * illnesses, in their order in a Chao's emotions */
#ifndef CHAO_NEEDS_H
#define CHAO_NEEDS_H
#include "chao/behavior.h"

struct task;
extern chao_behavior_fn illness_actions[6];
void score_player_fear(struct task *chao, Float *best);
void score_play(struct task *chao, Float *best);
void score_hunger(struct task *chao, Float *best);
void score_condition(struct task *chao, Float *best);

int fun_0c52e4f6(void);
#endif
