/* O HAZ BOMB, the bomb Meteor Herd and Mad Space place, which a touch sends up
 * to drop */
#ifndef SHARED_HAZARD_BOMB_H
#define SHARED_HAZARD_BOMB_H

struct task;

/* the object list's step for O HAZ BOMB. it does nothing where
 * placement_try_leave(task) returns TRUE. task needs a work. a player's touch
 * sends the bomb up at 2.5 units a step, which slows by 0.06 a step. at the top
 * of its rise, it drops a BOMB_NORMAL of shared/enemy_bomb.h at its pos and
 * ends */
void hazard_bomb_init(struct task *task);

#endif
