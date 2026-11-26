#ifndef CHAO_ACTIVITIES_H
#define CHAO_ACTIVITIES_H
#include <sg_xpt.h>
struct task;
int chao_sit_down_behavior(struct task *chao);
int chao_recline_behavior(struct task *chao);
int chao_rest_behavior(struct task *chao);
int chao_try_petting(struct task *chao);
int chao_try_take_headgear(struct task *chao);
void score_pain(struct task *chao, Float *best);
void score_pleasure(struct task *chao, Float *best);
int chao_try_player_call(struct task *chao);
int chao_try_ball(struct task *chao);
void chao_pick_wander_target(struct task *chao);
#endif
