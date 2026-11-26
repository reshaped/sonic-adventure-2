/* the draw of the shutters, METEOSHUT in Meteor Herd and SHUT in Mad Space.
 * stg32/meteor_shutter.c and stg44/shutter.c include it. the includer defines
 * SHUTTER_TEXLIST, and SHUTTER_HOUSING, SHUTTER_FIRST_LIGHT and
 * SHUTTER_SECOND_LIGHT, the objects of the housing and its two lamps. Meteor
 * Herd's also defines SHUTTER_BROKEN and SHUTTER_BROKEN_TEXLIST, the broken
 * shutter's object and textures. a shutter whose work's mode is 0 draws its
 * housing and two blinking lamps. once the mode isn't 0, Meteor Herd's draws
 * SHUTTER_BROKEN, and Mad Space's draws nothing */
#ifndef SHARED_METEOR_SHUTTER_LIGHTS_H
#define SHARED_METEOR_SHUTTER_LIGHTS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"
#include "shared/common_object.h"
#include "system/cache.h"
#include "system/task.h"

/* a lamp's tint runs from LAMP_DIM to LAMP_BRIGHT. the first lamp blinks by
 * blink_level of FIRST_PERIOD, FIRST_REST and FIRST_FADE. the second blinks
 * by blink_level of SECOND_PERIOD, SECOND_REST and SECOND_FADE */
#define LAMP_DIM 0xff101010
#define LAMP_BRIGHT 0xffffffff
#define FIRST_PERIOD 38
#define FIRST_REST 20
#define FIRST_FADE 6
#define SECOND_PERIOD 30
#define SECOND_REST 15
#define SECOND_FADE 5

static void shutter_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 first;
    Uint32 second;

    /* @hack redundant: the set editor's label, in a block no path reaches */
    if (0) {
        (void)"<- ANGLE Y";
    }
    if (work->mode) {
#ifdef SHUTTER_BROKEN_TEXLIST
        njSetTexture(&SHUTTER_BROKEN_TEXLIST);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCnkEasyDrawModel(SHUTTER_BROKEN->model);
        njPopMatrixEx();
#else
        return;
#endif
    } else {
        first = color_blend(LAMP_DIM, LAMP_BRIGHT,
                            blink_level(FIRST_PERIOD, FIRST_REST, FIRST_FADE));
        second =
            color_blend(LAMP_DIM, LAMP_BRIGHT,
                        blink_level(SECOND_PERIOD, SECOND_REST, SECOND_FADE));
        njSetTexture(&SHUTTER_TEXLIST);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njPushMatrixEx();
        njCnkEasyDrawModel(SHUTTER_HOUSING->model);
        njTranslateEx((NJS_VECTOR *)SHUTTER_FIRST_LIGHT->pos);
        model_draw_tinted(SHUTTER_FIRST_LIGHT->model, first);
        matrix_load_parent();
        njTranslateEx((NJS_VECTOR *)SHUTTER_SECOND_LIGHT->pos);
        model_draw_tinted(SHUTTER_SECOND_LIGHT->model, second);
        matrix_load_parent();
        njPopMatrix(2);
    }
}

#endif
