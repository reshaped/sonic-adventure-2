/* the draw of STEP and of STEPB, which engine_step.c includes once for each.
 * STEP_NAME names that platform's draw, texture list and model, and STEP_B is
 * defined for STEPB */
#include "shared/step_models.h"
#include "system/util.h"

static void STEP_NAME(draw)(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR guide;
    NJS_VECTOR position;
#if defined(COLONY_STEP) || defined(STEP_B)
    NJS_VECTOR size;
#endif
#ifdef COLONY_STEP
    Float marker_size;
#endif

    njSetTexture(&STEP_NAME(texlist));
    /* @hack redundant: a block no path reaches, of the set editor's preview of
     * the path */
    if (0) {
        switch (PATH_KIND(work)) {
        case PATH_STILL:
            break;
        case PATH_ARC:
            guide.x = work->scl.x;
            guide.y = 0.0F;
            guide.z = 0.0F;
            njPushMatrixEx();
            njTranslateEx(&TRACK(task)->pivot);
            njRotateY(NULL, work->ang.z - QUARTER_TURN);
            njTranslateEx(&guide);
            njRotateY(NULL, work->ang.y);
            njCnkEasyDrawModel(STEP_NAME(model));
            njPopMatrixEx();
#ifdef COLONY_STEP
            marker_size = PIVOT_SCALE;
            guide.x = marker_size;
            guide.y = marker_size;
            guide.z = marker_size;
#else
            guide.x = PIVOT_SCALE;
            guide.y = PIVOT_SCALE;
            guide.z = PIVOT_SCALE;
#endif
            debug_draw_model(&TRACK(task)->pivot, NULL, &guide,
                             DEBUG_MODEL_BOX);
            break;
        case PATH_RISE:
#if defined(COLONY_STEP) || defined(STEP_B)
            size.x = TRIGGER_RADIUS(work);
            size.y = TRIGGER_RADIUS(work);
            size.z = TRIGGER_RADIUS(work);
            debug_draw_model(&work->pos, NULL, &size, DEBUG_MODEL_SPHERE);
#endif
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njTranslate(NULL, 0.0F, work->scl.x, 0.0F);
            njCnkEasyDrawModel(STEP_NAME(model));
            njPopMatrixEx();
            break;
        case PATH_SINK:
#if defined(COLONY_STEP) || defined(STEP_B)
            size.x = TRIGGER_RADIUS(work);
            size.y = TRIGGER_RADIUS(work);
            size.z = TRIGGER_RADIUS(work);
            debug_draw_model(&work->pos, NULL, &size, DEBUG_MODEL_SPHERE);
#endif
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njTranslate(NULL, 0.0F, -work->scl.x, 0.0F);
            njCnkEasyDrawModel(STEP_NAME(model));
            njPopMatrixEx();
            break;
        case PATH_HORIZONTAL:
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.z);
            njTranslate(NULL, work->scl.x, 0.0F, 0.0F);
            njCnkEasyDrawModel(STEP_NAME(model));
            njPopMatrixEx();
            break;
        default:
            break;
        }
    }

#ifdef STEP_B
    draw_control_set(DRAW_CONTROL_SHADOW);
#endif
    njPushMatrixEx();
    position = work->pos;
    position.y += TRACK(task)->bob_height;
    njTranslateEx(&position);
#ifdef COLONY_STEP
    draw_control_set(DRAW_CONTROL_SHADOW);
#endif
    njRotateY(NULL, work->ang.y);
#if !defined(COLONY_STEP) && !defined(STEP_B)
    draw_control_set(DRAW_CONTROL_SHADOW);
#endif
    njCnkEasyDrawModel(STEP_NAME(model));
#ifdef COLONY_STEP
    njFogDisable();
    njCnkEasyDrawModel(&step_light_model);
    njFogEnable();
#endif
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}
