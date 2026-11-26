/* a hatch's draw, which hatch.c includes for each hatch and each way to draw
 * it. DRAW_LID and DRAW_SPRING draw the lid and the spring, and DRAW_NAME
 * names the draw */
#include "effect/particle.h"
#include "system/cache.h"

static void DRAW_NAME(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 color;
    Float drop;

    color = color_blend(LAMP_DARK, COLOR_NONE,
                        blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE));
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njSetTexture(&PART(texlist));
    if (work->mode == OPEN) {
#ifdef WALL_HATCH
        njTranslateEx((NJS_VECTOR *)wall_hatch_open_lamp->pos);
        njRotateEx(wall_hatch_open_lamp->ang, 0);
#endif
        model_draw_tinted(&PART(open_lamp_model), color);
        matrix_load_parent();
    }
    if (work->mode == OPEN && HATCH(task)->depth > 0.0F) {
        /* @hack redundant: a read of depth, discarded, before the test reads
         * it again */
        if ((void)HATCH(task)->depth, HATCH(task)->depth < CLEARANCE) {
            drop = HATCH(task)->depth * SHALLOW_SHARE;
        } else {
            drop = HATCH(task)->depth - DEEP_LAG;
        }
#ifdef WALL_HATCH
        njTranslate(NULL, -drop, -(drop * WALL_DROP_Y), drop * WALL_DROP_Z);
#else
        njTranslate(NULL, 0.0F, -drop, 0.0F);
#endif
    }
    njPushMatrixEx();
    DRAW_LID();
#ifdef WALL_HATCH
    if (work->mode != OPEN) {
        njTranslateEx((NJS_VECTOR *)wall_hatch_closed_lamp->pos);
        njRotateEx(wall_hatch_closed_lamp->ang, 0);
        model_draw_tinted(&wall_hatch_closed_lamp_model, color);
        matrix_load_parent();
    }
    njTranslateEx((NJS_VECTOR *)wall_hatch_lamp->pos);
    njRotateEx(wall_hatch_lamp->ang, 0);
#endif
    model_draw_tinted(&PART(lamp_model), color);
#ifdef WALL_HATCH
    matrix_load_parent();
    njTranslateEx((NJS_VECTOR *)wall_hatch_spring->pos);
    njRotateEx(wall_hatch_spring->ang, 0);
#endif
    njTranslate(NULL, 0.0F, HATCH(task)->bounce, 0.0F);
    DRAW_SPRING();
    njPopMatrix(3);
}
