/* the Finalhazard's lasers, the beams its cannons fire */
#ifndef BOSS_LAST2_LASER_H
#define BOSS_LAST2_LASER_H
#include <sg_xpt.h>
struct task;
struct finalhazard_attack;

/* a pointer may be null only where a comment says so */

/* fire attack's beams as children of parent, as finalhazard_attack says. a
 * holder, another child of parent, kills them as it dies. each beam starts
 * where attack's first or second point lies now. attack's target must have a
 * work, and outlive the beams, which read its pos. returns the holder, or
 * null without parent or where the heap can't give the holder. a beam the
 * heap can't give is left out */
struct task *finalhazard_fire_lasers(struct task *parent,
                                     const struct finalhazard_attack *attack);
/* have each of task's beams draw back once it has reached out, and fire no
 * more. without task it does nothing */
void finalhazard_lasers_retract(struct task *task);
/* 1 once task's last beam has drawn back and stopped, else 0. -1 without task
 * or that beam */
Sint8 finalhazard_lasers_finished(struct task *task);
/* show the lasers' setting under the test menu: the beams' top tracking
 * speed. the active player's L and R lower or raise it */
void finalhazard_debug_lasers(void);
#endif
