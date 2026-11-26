/* the Egg Golem's actions in BOSS_GOLEM_E.PRS, by enum golem_action of
 * shared/golem.h. both calls read golem_e, which must not be null */
#ifndef BOSS_GOLEM_E_ACTIONS_H
#define BOSS_GOLEM_E_ACTIONS_H

struct task;

/* make action golem_e's current one, and drop any action requested. the action
 * starts at action_mode 0, with action_steps 0. task isn't read */
void golem_e_action_start(struct task *task, int action);
/* run golem_e's current action once, for task, the golem's, and count the run
 * in action_steps. an action requested since the last run first becomes
 * current, as golem_e_action_start makes it */
void golem_e_actions_step(struct task *task);

#endif
