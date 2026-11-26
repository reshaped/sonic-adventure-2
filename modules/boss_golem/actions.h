/* the Egg Golem's actions in BOSS_GOLEM.PRS, by enum golem_action of
 * shared/golem.h. both calls read golem, which must not be null */
#ifndef BOSS_GOLEM_ACTIONS_H
#define BOSS_GOLEM_ACTIONS_H

struct task;

/* make action golem's current one, and drop any action requested. the action
 * starts at action_mode 0, with action_steps 0. task isn't read */
void golem_action_start(struct task *task, int action);
/* run golem's current action once, for task, the golem's, and count the run in
 * action_steps. an action requested since the last run first becomes current,
 * as golem_action_start makes it */
void golem_actions_step(struct task *task);

#endif
