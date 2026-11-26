/* a garden Chao's intention, and the bids that choose its next behavior */
#ifndef CHAO_INTENTION_H
#define CHAO_INTENTION_H
#include <sg_xpt.h>

struct task;

/* the shares a bid takes of a need or a feeling. _above is value's share of
 * the way from threshold to a top. chao_need_above's top is
 * chao_tuning.need_max, and chao_feeling_above's CHAO_FEELING_MAX.
 * _below is the share of threshold that value falls short by. each is 0
 * where value hasn't crossed threshold */
Float chao_need_above(int value, int threshold);
Float chao_need_below(int value, int threshold);
Float chao_feeling_above(int value, int threshold);
Float chao_feeling_below(int value, int threshold);
/* keep the share chao_tuning.score_retained of *score. scale the rest by a
 * random share, from 0 up to 1 */
void chao_vary_score(Float *score);
/* a Chao's cooldowns, by index, hold the frames a scorer waits before it
 * bids again. chao_tick_intentions counts the first 16 down to 0 */
Uint32 chao_cooldown(struct task *chao, Uint16 index);
void chao_set_cooldown(struct task *chao, Uint16 index, Uint32 frames);
/* a Chao's intention, which chao_run_intention carries out when a behavior
 * asks for a choice. 0 is none, for which chao_choose_intention chooses
 * first. setting an intention restarts its age */
void chao_clear_intention(struct task *chao);
int chao_intention(struct task *chao);
void chao_set_intention(struct task *chao, int intention);
/* choose a Chao's next behavior by bids. *best, the best bid so far, starts
 * at chao_tuning.score_floor. each scorer bids against *best. a scorer whose
 * bid is higher sets its behavior in place of the last one's. its bid
 * becomes *best. a child skips the last six scorers. where no bid passes the
 * floor, the Chao walks to a new spot */
void chao_choose_intention(struct task *chao);
void chao_tick_intentions(struct task *chao);
/* carry out intention. intention 0, none, leaves the chosen behavior alone.
 * any other sets the idle behavior and clears the intention */
void chao_run_intention(struct task *chao, int intention);
void fun_0c50ee3a(struct task *chao);
int fun_0c50ee3e(struct task *chao);
#endif
