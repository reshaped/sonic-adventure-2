/* helpers Wild Canyon's objects share */
#ifndef SHARED_CANYON_UTIL_H
#define SHARED_CANYON_UTIL_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the height that splits the canyon in two. a point lies in the high half at
 * HALF_LINE or above, and in the low half below it */
#define HALF_LINE 600.0F

/* whether task's player is a Knuckles or a Rouge that attacks, glides,
 * spirals or dives. player_slot_of must find a player for task, unchecked.
 * that player's work and record must not be null, unchecked */
Bool hunter_attacking(struct task *task);

/* turn (0, 0, radius) by ang about y, as njRotateY turns a point. *x and *z
 * take the result's x and z */
void canyon_polar_xz(Angle ang, Float radius, Float *x, Float *z);

/* make a puff with shared/dust.h's dust_create, at speed spd and size scale.
 * the puff starts at pos plus (0, 0, radius), turned three times. a random
 * turn about x of 0 up to 60 degrees comes first. a random turn about y of
 * 120 up to 240 degrees comes next. the current matrix's turn comes last, as
 * current_matrix_angles_xyz reads it.
 *
 * the call leaves the current matrix as it found it. Ninja's matrix stack must
 * have room for one more matrix, unchecked */
void canyon_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                        Float scale);

/* draw model scaled. canyon_model_draw_scaled and
 * canyon_model_draw_tinted_scaled scale by scale along each axis. the _v calls
 * scale by scale's x, y and z. canyon_model_draw_scaled and
 * canyon_model_draw_scaled_v draw with njCnkEasyDrawModel, and the tinted
 * calls with shared/common_object.h's model_draw_tinted.
 *
 * each call scales the current matrix, as njScale does, and leaves it so.
 * for the draw, model->r is scaled by the largest factor. any clip test in the
 * draw takes the scaled r. the call then puts r back */
void canyon_model_draw_scaled(NJS_CNK_MODEL *model, Float scale);
void canyon_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale);
void canyon_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                     Float scale);
void canyon_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                       NJS_VECTOR *scale);

/* whether point lies in a box of camera 0's view. let (x, y, z) be point
 * through the current matrix. let d be 320 over the tangent of half camera
 * 0's angle of view. the box holds z in (15, 450). it holds x * d / z in
 * (-280, 280). it holds -y * d / z in [-200, 160) */
Bool in_view_box(NJS_POINT3 *point);

/* leave as placement_try_leave does, by the canyon's halves in place of a
 * range. it returns TRUE where no player's work pos lies in the half of task's
 * work pos. task then destroys itself on its next step. it returns FALSE
 * otherwise. player 1 counts only in a two-player game. task's work and each
 * counted player's work must not be null, unchecked */
Bool half_try_leave(struct task *task);

/* pulse player's pad count times, interval steps apart. each pulse is
 * vibration_start(player, power, freq, 0). a task named VibSlightExec, at task
 * level 2, makes the pulses. it pulses on its first step, and ends interval - 1
 * steps after its last pulse. an interval of 0 counts as 1. where the heap
 * can't give the task, nothing pulses.
 *
 * count must be 1 or more, interval 0 or more, and power 0 or more. with a
 * count of 0 and an interval above 1, the task pulses once. with a count of 0
 * and an interval of 0 or 1, it pulses every step forever. the task keeps
 * power in a Uint16, so a negative power reaches vibration_start as 65536
 * plus power */
void canyon_vib_slight_create(int player, int power, int freq, int interval,
                              int count);

#endif
