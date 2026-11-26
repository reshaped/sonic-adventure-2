/* the mech bosses' laser: the spot where a boss's laser meets the ground. the
 * spot runs ahead of the boss and turns toward player 0. Big Foot, Hot Shot and
 * Flying Dog fire it */
#ifndef SHARED_MECH_BOSS_LASER_H
#define SHARED_MECH_BOSS_LASER_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;

/* what mech_boss_laser_create builds a spot from */
struct mech_boss_laser_spec {
    Float reach; /* how far ahead of the boss the spot runs */
    /* the most the spot turns about y in a step. while the spot aims, 0 sets
     * no bound. at rest, 0 holds its turn about y still */
    Angle step;
    /* upper.y and lower.y bound the spot's turn about y. upper.z bounds its
     * tilt, which grows as the laser points further down. no call reads the
     * others */
    struct angles upper;
    struct angles lower;
    NJS_POINT3 *origin; /* the point the laser fires from */
};

/* the spec each boss gives its spot. the spot runs MECH_BOSS_LASER_REACH
 * ahead, and turns by at most MECH_BOSS_LASER_STEP in a step. its bounds are
 * MECH_BOSS_LASER_TURN either way about y, and MECH_BOSS_LASER_LOWER_TILT to
 * MECH_BOSS_LASER_UPPER_TILT about z. the boss's turret takes the same
 * bounds */
#define MECH_BOSS_LASER_REACH 40.0F
#define MECH_BOSS_LASER_STEP 32
#define MECH_BOSS_LASER_TURN 0x1400
#define MECH_BOSS_LASER_UPPER_TILT 0x1800
#define MECH_BOSS_LASER_LOWER_TILT (-0xc00)
/* the turret's aim takes the spot's angles, tilted MECH_BOSS_TURRET_TILT
 * further about z */
#define MECH_BOSS_TURRET_TILT 0x1000

/* what the spot's step does. an aiming spot tilts the laser down toward a
 * player 0 nearer than reach across the ground. the tilt brings the spot
 * nearer. once the tilt reaches upper.z it holds there, and the spot lies the
 * full reach ahead */
enum mech_boss_laser_mode {
    MECH_BOSS_LASER_REST, /* turn back to the boss's heading, reach ahead */
    MECH_BOSS_LASER_AIM /* turn toward player 0, within the spec's bounds */
};

/* create a spot as parent's child, in MECH_BOSS_LASER_REST, and return its
 * task. it returns null for a null parent, or where task_create_child fails.
 * it also returns null where the heap can't give the spot's own record. that
 * task then ends on its next step.
 *
 * each step places the spot at y = 1, just above the ground. the spot lies up
 * to reach ahead of the parent's pos. its heading is the parent's ang.y turned
 * by the spot's own ang.y. the step does nothing while game_state.time_stopped
 * isn't 0.
 *
 * the spot's record, of COLLISION_ENEMY, hurts the objects it touches. the
 * laser runs from *spec->origin to the spot, at the ground. player 0 takes
 * WORK_HURT while it lies within 10 of that line and no higher than the
 * origin. the spot throws sparks each step, and a muzzle flash of
 * shared/boss_gunfire.h each sixth step.
 *
 * parent's work, spec->origin and player_work[0] must stay valid while the
 * task lives. where collision_init can't give the spot its record, the call
 * writes through null */
struct task *mech_boss_laser_create(struct task *parent,
                                    struct mech_boss_laser_spec *spec);
/* set the spot's step to mode */
void mech_boss_laser_set_mode(struct task *task, Sint8 mode);
/* the spot's ang: its turn about y from the parent's heading in ang.y, and its
 * tilt in ang.z */
struct angles *mech_boss_laser_angles(struct task *task);

#endif
