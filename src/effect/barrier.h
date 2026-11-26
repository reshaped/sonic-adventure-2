/* the effects a player wears: the barrier, the magnetic barrier and
 * invincibility. prominences and D sparks flicker as a magnetic barrier's
 * bolts do */
#ifndef EFFECT_BARRIER_H
#define EFFECT_BARRIER_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* Resident geometry for the player's two barriers and the invincibility
 * effect. The kart race draws the same invincibility ball and spark. */
extern NJS_CNK_MODEL barrier_spark_model;
extern NJS_CNK_MODEL magnetic_spark_model;
extern NJS_CNK_MODEL invincibility_ball_model;
extern NJS_CNK_MODEL invincibility_spark_model;

/* make the effect the player in slot player wears, a task in level 3. each
 * returns the task, or null where the heap can't give it. the effect stands at
 * unk_218 of the player's record for a mech, and unk_230 for any other. the
 * original draws read the slot's work and record even while the task is ending.
 * the portable drawers skip the effect and its sparks once the slot empties.
 *
 * barrier_create's NBarrier and magnetic_barrier_create's TBarrier play sound
 * 0x801a as they start. they throw off sparks at random over their shells. each
 * lasts while the slot holds a work and the record's flags hold its bit,
 * PLAYER_BARRIER or PLAYER_MAGNETIC_BARRIER. the caller sets the bit.
 *
 * invincibility_create's Muteki draws mutekiball's ball at the player, with
 * sparks of muteki01 to 08 about it. it sets PLAYER_INVINCIBLE at each of its
 * first 1261 steps. the step after them clears the bit and ends the effect. so
 * does a step that finds the slot's work null or game_state.cleared at 1 */
struct task *barrier_create(int player);
struct task *magnetic_barrier_create(int player);
struct task *invincibility_create(int player);

/* start a prominence at a random point radius from pos. the prominence is
 * parent's child, or where parent is null, a task in level 3 named
 * TProminence. it returns the task, or null where the heap can't give it. a
 * prominence flickers through halves of barria_t01 to t08 for 11 steps, added
 * to what lies behind it. it then dims from half bright by 0.025 a step, and
 * ends once dark */
struct task *prominence_create(struct task *parent, NJS_POINT3 *pos,
                               Float radius);

#endif
