/* the Biolizard's life support, where a player hurts it, and its lifted
 * wave's barrier */
#ifndef BOSS_LAST1_WEAK_POINT_H
#define BOSS_LAST1_WEAK_POINT_H
#include <ninja.h>
#include <sg_xpt.h>
#include "collision/collision.h"
struct task;

/* create the life support as a child of parent, the Biolizard's task. it
 * moves to *anchor each step, so anchor must stay valid while it lives. its
 * draw turns it to the Biolizard's ang.y. while the Biolizard's attack is
 * BIOLIZARD_ATTACK_LIFTED_WAVE, it hurts no player. returns the task, or
 * null for a null parent or where the heap can't give it. the heap must give
 * its collision record, unchecked */
struct task *biolizard_life_support_create(struct task *parent,
                                           const NJS_POINT3 *anchor);
/* 1 while the life support is hurt, else 0, or -1 for a null task. a
 * player's hit hurts it until it rearms. biolizard_life_support_reset rearms
 * it. so does a hurt life support's step while the Biolizard is in neither
 * BIOLIZARD_DAMAGE nor BIOLIZARD_DEFEAT */
Sint8 biolizard_life_support_hit(struct task *task);
/* rearm the life support, so that a player can hit it again. a null task
 * does nothing */
void biolizard_life_support_reset(struct task *task);
/* create the barrier as a child of parent: biolizard_upper_cylinder, 147
 * above the origin. returns the task, or null for a null parent or where the
 * heap can't give it. the heap must give its collision record, unchecked */
struct task *biolizard_barrier_create(struct task *parent);
/* the barrier's shape, which upper_barrier.c defines */
extern struct collision_shape biolizard_upper_cylinder;
#endif
