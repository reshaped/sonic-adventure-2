/* a garden Chao's drowsiness, sleep and waking */
#ifndef CHAO_SLEEP_H
#define CHAO_SLEEP_H
#include <sg_xpt.h>

struct task;

void score_drowsiness(struct task *chao, Float *best);
void score_waking(struct task *chao, Float *best);
void score_sleep(struct task *chao, Float *best);
int chao_sleep_behavior(struct task *chao);
int chao_seated_sleep_behavior(struct task *chao);
int chao_wake_behavior(struct task *chao);
#endif
