/* the mech bosses' laser beam: pulses that run from the laser's origin to its
 * spot on the ground. Big Foot, Hot Shot and Flying Dog fire it */
#ifndef SHARED_MECH_BOSS_LASER_BEAM_H
#define SHARED_MECH_BOSS_LASER_BEAM_H
#include <ninja.h>
#include <sg_xpt.h>

struct mover;
struct task;

/* what mech_boss_laser_beam_create builds a beam from */
struct mech_boss_laser_beam_spec {
    Sint8 unk_00; /* no call reads it */
    NJS_POINT3 *origin;
    NJS_POINT3 *target; /* the spot of shared/mech_boss_laser.h */
    struct mover *mover; /* the boss's, whose speed the sparks take */
};

/* create a beam as parent's child, and return its task. parent must not be
 * null: for a null parent the result is undefined. where task_create_child
 * fails, the call writes through a pointer it reads from null. spec's
 * pointers must stay valid while the task lives.
 *
 * every third step the beam fires a pulse, a child task of its own, from
 * *spec->origin toward *spec->target. a pulse ends once it passes below the
 * ground, at y = 0. each step, each pulse moves *spec->origin and
 * *spec->target, and puts the target on the ground.
 *
 * with one pulse in five, at random, the beam throws sparks from the origin.
 * with one in three, it makes a fire of shared/boss_gunfire.h there, with
 * MECH_BOSS_SOUND_LASER_FIRE. MECH_BOSS_SOUND_LASER_BEAM plays at the target
 * while the beam lives. the beam holds still while game_state.time_stopped
 * isn't 0. its pulses hold still while that or game_state.unk_09 isn't 0 */
struct task *
mech_boss_laser_beam_create(struct task *parent,
                            struct mech_boss_laser_beam_spec *spec);

#endif
