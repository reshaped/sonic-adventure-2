/* the mech bosses' beam gun: it holds a charge of shared/mech_boss_beam.h at a
 * boss's muzzle, and fires it. Hot Shot and Flying Dog each carry one */
#ifndef SHARED_MECH_BOSS_BEAM_GUN_H
#define SHARED_MECH_BOSS_BEAM_GUN_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;

/* create a gun as parent's child at *pos, turned by *ang, and return its task.
 * it returns null for a null parent, or where task_create_child fails. it also
 * returns null where the heap can't give the gun its own record. that task
 * then ends on its next step. the gun starts out holding a charge, as
 * mech_boss_beam_gun_charge gives it one.
 *
 * each step the gun takes *pos and the angles of parent's work. *ang turns it
 * only until its first step. it holds still while game_state.time_stopped
 * or game_state.unk_09 isn't 0. pos and parent's work must stay valid while
 * the gun lives */
struct task *mech_boss_beam_gun_create(struct task *parent, NJS_POINT3 *pos,
                                       struct angles *ang);
/* fire a shot of mech_boss_beam_fire at *target from the gun's charge, with
 * MECH_BOSS_SOUND_BEAM_SHOT, and end the charge. the gun must hold a charge,
 * unchecked. it then holds none until mech_boss_beam_gun_charge. the shot is
 * the gun's child. *busy becomes 1, and the shot sets it back to 0 as
 * mech_boss_beam_fire says. target must stay valid while the shot lives */
void mech_boss_beam_gun_fire(struct task *task, NJS_POINT3 *target,
                             Sint32 *busy);
/* give the gun a new charge of mech_boss_beam_charge_create, as its child. the
 * charge follows the gun's pos and angles, at an offset of (-8, -6, 0). a
 * charge the gun held before goes on, but the gun no longer holds it */
void mech_boss_beam_gun_charge(struct task *task);

#endif
