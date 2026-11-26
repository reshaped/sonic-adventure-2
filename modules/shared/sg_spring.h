/* the SG springs, SG_SPRA and SG_SPRB in a stage's object list, or SGSPRING
 * and SGSPRINGB. they are the springs of shared/spring.h. only the players
 * whose bits are set in game_state.sg_players see them and touch them */
#ifndef SHARED_SG_SPRING_H
#define SHARED_SG_SPRING_H

struct task;

/* the object list's steps for SG_SPRA and SG_SPRB. the work and the task's
 * pointers mean what they mean for spring_a_step and spring_b_step. an SG
 * spring draws and launches as a spring does. it has no collision record, and
 * draws its coil at the model's full width. its launch never tests
 * PLAYER_MISSED, and gives no messenger event.
 *
 * a player touches the spring where its first collision shape's center lies
 * less than 14 from pos. its collision record must hold a shape, unchecked.
 * the scan takes players 0 and 1 in order. it stops at the first without a
 * work or a collision record */
void sg_spring_a_step(struct task *task);
void sg_spring_b_step(struct task *task);

#endif
