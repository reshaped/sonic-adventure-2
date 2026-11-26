/* the mech bosses' missile: it rides its launcher, then flies a curve to a
 * target. it leaves a trail of smoke. it bursts where it lands or hits. Big
 * Foot, Hot Shot and Flying Dog fire it */
#ifndef SHARED_MECH_BOSS_MISSILE_H
#define SHARED_MECH_BOSS_MISSILE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;

/* a missile's modes, as mech_boss_missile_mode gives them */
enum mech_boss_missile_mode {
    MECH_BOSS_MISSILE_RIDING, /* on its launcher */
    MECH_BOSS_MISSILE_FLYING
};
/* the mode mech_boss_missile_mode gives a missile that is gone */
#define MECH_BOSS_MISSILE_GONE (-1)
/* each boss launches a missile at a yaw within MECH_BOSS_MISSILE_YAW_SPREAD of
 * its turn toward player 0, at random */
#define MECH_BOSS_MISSILE_YAW_SPREAD 2048.0F

/* create a missile as parent's child, riding at *pos and turned by *ang, and
 * return its task. it returns null for a null parent, or where
 * task_create_child fails. it also returns null where the heap can't give the
 * missile its own record. that task then ends on its next step. where
 * collision_init can't give the missile its record, the call writes through
 * null. while the missile rides, each step puts it at *pos and turns it by
 * *ang. both must stay valid until it flies.
 *
 * the missile's record, of COLLISION_ENEMY, hurts players and objects. in
 * flight, the missile bursts where it touches a player or an object. it bursts
 * where it reaches the ground, at y = 0. it bursts where it passes radius + 40
 * from the world's y axis. a step bursts it once for each of these it meets. a
 * burst is a BLAST_SMALL_RAISED of shared/blast.h, with a sound. it ends the
 * missile's task on its next step. a missile that reaches the ground also
 * shakes player 0's pad. the missile holds still while game_state.time_stopped
 * or game_state.unk_09 isn't 0 */
struct task *mech_boss_missile_create(struct task *parent, NJS_POINT3 *pos,
                                      struct angles *ang, Float radius);
/* the square of the distance from the missile to its target in x and z */
Float mech_boss_missile_target_distance2_flat(struct task *task);
/* turn the missile's ang.y toward its target by at most step. it returns the
 * turn left to make, -0x8000 to 0x7fff */
Angle mech_boss_missile_turn(struct task *task, Angle step);
/* launch the missile toward target at speed. it takes its launcher's angles as
 * it leaves, with a sound and a flash of shared/flash.h. it flies on along its
 * ang.y for 6 steps, at 0.8 of speed. it then flies a Bezier curve to target,
 * at about speed a step. yaw turns the curve's middle about y. past the
 * curve's end, it flies straight on by its last step's move until it bursts */
void mech_boss_missile_launch(struct task *task, Angle yaw, Float speed,
                              NJS_POINT3 target);
/* task's mode, an enum mech_boss_missile_mode. it returns
 * MECH_BOSS_MISSILE_GONE for a null task, or a task without a work */
Sint8 mech_boss_missile_mode(struct task *task);

#endif
