/* the mech bosses' beam. a charge grows at a boss's muzzle. a shot homes in on
 * a target, leaves a trail, and sets off a bomb where it hits. Hot Shot and
 * Flying Dog fire it through shared/mech_boss_beam_gun.h */
#ifndef SHARED_MECH_BOSS_BEAM_H
#define SHARED_MECH_BOSS_BEAM_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;

/* the texture list of grobo_eff_tex: jet1, e102newbeam and c6_ht16. each
 * mech boss's stage loads it with its table of struct texture_pvm */
extern NJS_TEXLIST grobo_eff_texlist;

/* where mech_boss_beam_fire fires a shot from, and at */
struct mech_boss_beam_aim {
    NJS_POINT3 *pos; /* where the shot starts */
    NJS_POINT3 *target; /* what it homes in on */
};

/* create a charge as parent's child, and return its task. it returns null
 * where task_create_child fails, or where the heap can't give the charge its
 * own record. that task then ends on its next step. parent must not be null:
 * for a null parent the result is undefined.
 *
 * each step puts the charge at a copy of *offset from *pos. the offset lies in
 * the frame *ang turns, turned a further quarter turn about y. while the
 * charge grows, its offset drifts along -x and -y. until
 * mech_boss_beam_charge_end, it throws a glow of shared/mech_boss_beam_glow.h
 * each step. it plays MECH_BOSS_SOUND_BEAM_CHARGE, whose pitch and volume
 * rise as it goes on. it holds still while game_state.time_stopped or
 * game_state.unk_09 isn't 0. pos and ang must stay valid while it lives */
struct task *mech_boss_beam_charge_create(struct task *parent, NJS_POINT3 *pos,
                                          NJS_POINT3 *offset,
                                          struct angles *ang);
/* end the charge: it throws no more glows, and its scale shrinks by 0.1 a
 * step. its task ends once the scale falls below 1 */
void mech_boss_beam_charge_end(struct task *task);
/* fire a shot as parent's child, from *aim->pos at *aim->target. where it
 * makes the shot, the call returns no value, and no caller reads one. it
 * returns null where task_create_child fails, or where the heap can't give
 * the shot its own record. that task then ends on its next step. parent must
 * not be null: for a null parent the result is undefined.
 *
 * the shot starts along -x, turned about y by parent's ang.y and a quarter
 * turn. each step it moves at most 10. it heads along a blend of its last move
 * and the way to the target. the target's share of the blend grows from 0 to 1.
 * within 10 of the target, the shot moves the rest of the way. it leaves a
 * trail that fades.
 *
 * where the shot's record touches a player's or an object's, the shot ends,
 * and shakes player 0's pad. it sets off a bomb there, a task with no parent,
 * with MECH_BOSS_SOUND_BEAM_BOMB. the bomb's record hurts players and
 * objects. at its full size the bomb throws a pillar, and a torus that shakes
 * the pad again. the bomb sets *busy to 0 as it ends. *busy keeps its value
 * where no bomb is made, as where the shot ends with its parent. the shot and
 * the bomb hold still while game_state.time_stopped or game_state.unk_09 isn't
 * 0. aim->target must stay valid while the shot lives, and busy while the bomb
 * lives */
struct task *mech_boss_beam_fire(struct task *parent,
                                 struct mech_boss_beam_aim *aim, Sint32 *busy);

#endif
