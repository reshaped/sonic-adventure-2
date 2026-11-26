/* the crying behaviors of a garden Chao */
#ifndef CHAO_CRY_H
#define CHAO_CRY_H
#include <sg_xpt.h>

struct task;

/* chao_choose_intention's bid from the Chao's sorrow, made while sorrow
 * passes chao_tuning.sorrow_threshold. the more aggression the Chao has, the
 * lower its bid. a bid above *best sets one of the three cries, clears the
 * intention and becomes *best */
void score_sorrow(struct task *chao, Float *best);
/* weep 100 to 200 frames at a time. each weep takes 10 from the Chao's
 * sorrow, and the behavior finishes once sorrow falls below 10 */
int chao_cry_behavior(struct task *chao);
/* sigh for 180 to 300 frames, and finish. the Chao's sorrow stays */
int chao_sigh_behavior(struct task *chao);
/* chao_cry_behavior's rounds, bawling and in tears. a held motion also ends
 * a round */
int chao_sob_behavior(struct task *chao);
#endif
