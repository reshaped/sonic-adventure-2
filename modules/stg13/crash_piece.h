/* crash pieces, the debris a crash throws. each is a task,
 * ObjectCrashObjExec. a piece falls and spins, bounces off walls, settles on
 * the floor and fades */
#ifndef STG13_CRASH_PIECE_H
#define STG13_CRASH_PIECE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "sound/lists.h"
#include "system/angle.h"

struct task;

/* the stage's sound list, which city_escape_init hands to sound_load_stage.
 * crash_piece.c defines it because the image places it among that source's
 * constants */
extern const struct sound_list city_escape_sound_list;

/* start a crash piece of object, drawn with texlist, and return its task. it
 * returns null where the heap can't give the task or its record. built is
 * object's
 * build for njDirectDrawObject, or null. a fading piece draws object's model,
 * whatever built holds.
 *
 * object stands at pos, turned by ang about z, then x, then y. the piece
 * starts at its model's center there, flies at spd and spins at random. size
 * turns with the piece, whose center stays at least the turned size's |y|
 * above its floor. its collision sphere, 0.7 times size's length in radius,
 * hurts enemies.
 *
 * the piece starts to fade delay steps on, and ends 15 steps into its fade. a
 * piece of delay 0 never fades. landing cuts a delay above 22 to 22, once the
 * piece has lived more than 5 steps. a piece also ends once its draw finds it
 * unseen 20 times in a row */
struct task *crash_piece_create(NJS_TEXLIST *texlist, NJS_CNK_OBJECT *object,
                                NJS_DIRECT_HEAD *built, NJS_VECTOR *size,
                                NJS_POINT3 *pos, struct angles *ang,
                                NJS_VECTOR *spd, Sint32 delay);

#endif
