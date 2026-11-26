/* parked cars, CARKAZ in a stage's object list. a car stands and pushes the
 * players out of its box. a crash throws it as a wreck, which tumbles and
 * bounces, then fades out */
#ifndef SHARED_PARKED_CAR_H
#define SHARED_PARKED_CAR_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the texture lists of the cars, their dust and their crash's flash, for
 * texture_load_shared, ended by null */
extern NJS_TEXLIST *parked_car_texlists[];

/* tint the models of the five cars without beacons. then compile each kind's
 * model at buf, under its texture list. it returns buf past what it compiled.
 * each call tints the five again, so the tint compounds. once a call has
 * compiled every kind, later calls compile nothing. a call in which a build
 * fails leaves the next call to compile every kind again */
Uint8 *parked_car_build(Uint8 *buf);

/* the object list's step for CARKAZ. it gives task the car's setup as its
 * step. the setup does nothing where placement_try_leave(task) returns TRUE.
 * where the heap can't give the car's record, it runs again on the next step.
 *
 * task needs a work. pos places the car, and ang turns it about z, then x, then
 * y. scl.y + 0.1, truncated and taken modulo 16, is the car's kind. kinds 2 to
 * 8 are the seven cars, and kinds 9 to 15 repeat them. kind 1 is a taxi that
 * pushes no player. for kind 0, the setup picks one of kinds 2 to 8 at random.
 * it stores that kind in scl.y. a car whose scl.x + 0.1 truncates to other than
 * 0 pushes no player either.
 *
 * a car pushes players 0 and 1 out of its box. after its first 20 steps, a
 * touch may crash it. a player's touch crashes it only while some player lies
 * in its box. Sonic or Shadow crashes it in PLAYER_MODE_ROLL, or in a mode from
 * PLAYER_MODE_SONIC_COMBO_1 to PLAYER_MODE_SONIC_COMBO_RUNNING_3. a mech's body
 * crashes it, and so does any touch but a player's. any player's touch crashes
 * a car that pushes no player.
 *
 * a crash a player makes scores 20 points. a crash kills the car's placement,
 * or its task where it has none. it does so even where the heap can't give the
 * wreck. where the heap can't give the car or its wreck a collision record, the
 * code reads through null */
void parked_car_init(struct task *task);

#endif
