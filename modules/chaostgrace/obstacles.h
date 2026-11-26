/* a racer's behaviors at the course's boxes, trees and scavenger hunt, which
 * racer.c sets and queues. each is a chao_behavior_fn */
#ifndef CHAOSTGRACE_OBSTACLES_H
#define CHAOSTGRACE_OBSTACLES_H
struct task;

int race_wait_at_box(struct task *chao);
int race_notice_obstacle(struct task *chao);
int race_understand_box(struct task *chao);
int race_think_at_obstacle(struct task *chao);
int race_open_box(struct task *chao);
int race_startled(struct task *chao);
int race_approach_tree_item(struct task *chao);
int race_shake_tree(struct task *chao);
int race_eat_fruit(struct task *chao);
/* set work->race_choice to the toy kind the racer looks for, and finish. the
 * more intelligent the racer, the likelier the kind is race_scavenger_target,
 * and else it is any kind */
int race_choose_answer(struct task *chao);
/* target a toy of the kind the racer chose, or any toy once enough searches
 * have failed. the toy leaves race_scavenger_items, and the behavior
 * finishes */
int race_find_answer_item(struct task *chao);
int race_bring_answer(struct task *chao);
int race_submit_answer(struct task *chao);

#endif
