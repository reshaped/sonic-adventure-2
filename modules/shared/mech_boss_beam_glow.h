/* the mech bosses' beam glows: sprites that fly into a charge of
 * shared/mech_boss_beam.h and fade. the charge throws them */
#ifndef SHARED_MECH_BOSS_BEAM_GLOW_H
#define SHARED_MECH_BOSS_BEAM_GLOW_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* what mech_boss_beam_glow_create builds a glow from */
struct mech_boss_beam_glow_spec {
    NJS_POINT3 *target; /* the point the glow flies at */
    Float *radius; /* the glow fades once it comes within 0.8 times *radius */
    Float accel; /* what the glow's speed gains each step, from 0 */
    NJS_VECTOR offset; /* where the glow starts, from *target */
};

/* create a glow as parent's child, and return its task. it returns null where
 * task_create_child fails. parent must not be null: for a null parent the
 * result is undefined.
 *
 * each step the glow flies straight at *spec->target, and grows. it starts to
 * fade once it comes as near the target as spec->radius sets. it starts at the
 * latest after its 181st step. its task ends once it has faded. it holds still
 * while game_state.time_stopped isn't 0. spec->target and spec->radius must
 * stay valid while the task lives */
struct task *mech_boss_beam_glow_create(struct task *parent,
                                        struct mech_boss_beam_glow_spec *spec);

#endif
