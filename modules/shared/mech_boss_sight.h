/* the mech bosses' sight: a spinning marker that follows a target. a ring lies
 * on the ground below it. Hot Shot and Flying Dog fire their beams at it */
#ifndef SHARED_MECH_BOSS_SIGHT_H
#define SHARED_MECH_BOSS_SIGHT_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* create a sight as parent's child at *target, and return its task. it
 * returns null for a null parent, or where task_create_child fails. it also
 * returns null where the heap can't give the sight its own record. that task
 * then ends on its next step.
 *
 * each step the sight moves toward where *target lay the step before, by at
 * most speed. while that point lies more than 80 away, it moves at 0.97 of
 * that. it stays where it is while its lock isn't 0. it holds still while
 * game_state.time_stopped isn't 0. target must stay valid while the sight
 * lives.
 *
 * the sight's record is a COLLISION_OBJECT sphere of radius 12. while a
 * player touches it, each step selects MECH_BOSS_HURT_LIGHT of
 * shared/mech_boss.h as player 0's light. otherwise each step selects light 0,
 * as the sight's end does */
struct task *mech_boss_sight_create(struct task *parent, NJS_POINT3 *target,
                                    Float speed);
/* arm the sight for its next step. where a player touches it then, player 0
 * takes WORK_HURT. player_launch_speed turns player 0 a half turn about y.
 * it launches player 0 at (-1, 1, 0) in the player's own axes. its stick locks
 * for 60 steps. the angles it takes leave ang.x and ang.z unset. no code calls
 * mech_boss_sight_arm */
void mech_boss_sight_arm(struct task *task);
/* the address of the sight's lock, a word that starts at 0. the sight stays
 * where it is while its lock isn't 0. mech_boss_beam_gun_fire can take it as
 * busy. the sight then stays from the shot's firing until its bomb ends */
Sint32 *mech_boss_sight_lock(struct task *task);
/* hide the sight and return TRUE, unless its lock is 1. a sight whose lock is
 * 1 stays shown, and the call returns FALSE. a hidden sight still steps, but
 * draws nothing. no call shows it again */
Bool mech_boss_sight_hide_unlocked(struct task *task);

#endif
