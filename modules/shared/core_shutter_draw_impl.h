/* the shutter's draw, which core_shutter.c includes twice. without
 * SHUTTER_DIRECT it defines core_shutter_draw, which draws the rigid parts'
 * models. with it, core_shutter_draw_direct draws their compiled builds instead
 */
#include "camera/camera.h"
#include "draw/chunk.h"
#include "effect/particle.h"
#include "shared/shutter_motion.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/util.h"

#ifdef SHUTTER_DIRECT
#define DRAW_RIGID(node, part)                                                 \
    njDirectDrawObject(core_shutter_builds[part].built)
static void core_shutter_draw_direct(struct task *task)
#else
#define DRAW_RIGID(node, part) njCnkEasyDrawModel((node)->model)
static void core_shutter_draw(struct task *task)
#endif
{
    struct task_work *work = task->work;
    struct shutter_motion *motion;
    NJS_POINT3 player;
    NJS_POINT3 position;
    NJS_VECTOR normal;
    Uint32 closed_light;
    Uint32 open_light;
    Uint32 main_light;
    Float left;
    Float right;
    Float top;
#ifdef SHUTTER_FAR_FACE
    Float pulse;
#endif
    njCalcPoint(NULL, &player_work[camera_current_index]->pos, &player);
    njCalcPoint(NULL, &work->pos, &position);
    if (player.z > position.z && !game_state.demo &&
        TRIGGER(work) == BY_PROXIMITY) {
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        njSetTexture(&core_shutter_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njPushMatrixEx();
        motion = MOTION(task);
        left = -(motion->opening * LEFT_TRAVEL) + motion->offset[LEFT_TRACK];
        right = (motion->opening * RIGHT_TRAVEL) - motion->offset[RIGHT_TRACK];
        /* @hack redundant: the top's offset computed in the translucent pass,
         * which draws only the side panels */
        top = (motion->opening * TOP_TRAVEL) - motion->offset[TOP_TRACK];
        MOVE_X(core_shutter_left, left);
        model_draw_tinted(core_shutter_left->model, TRANSLUCENT);
        matrix_load_parent();
        MOVE_X(core_shutter_right, right);
        model_draw_tinted(core_shutter_right->model, TRANSLUCENT);
        matrix_load_parent();
        njPopMatrix(2);
        draw_attr_restore();
        draw_control_restore();
    } else {
        closed_light = color_blend(
            SHUTTER_DARK, SHUTTER_LIT,
            blink_level(STOP_BLINK_PERIOD, STOP_BLINK_REST, STOP_BLINK_FADE) *
                MOTION(task)->closed_light);
#ifdef SHUTTER_FAR_FACE
        pulse = blink_level(GO_BLINK_PERIOD, GO_BLINK_REST, GO_BLINK_FADE);
        pulse *= MOTION(task)->open_light;
        open_light = color_blend(SHUTTER_DARK, SHUTTER_LIT, pulse);
#else
        open_light = color_blend(
            SHUTTER_DARK, SHUTTER_LIT,
            blink_level(GO_BLINK_PERIOD, GO_BLINK_REST, GO_BLINK_FADE) *
                MOTION(task)->open_light);
#endif
        main_light =
            work->mode == CLOSED
                ? color_blend(SHUTTER_DARK, SHUTTER_LIT,
                              blink_level(MAIN_BLINK_PERIOD, MAIN_BLINK_REST,
                                          MAIN_BLINK_FADE))
                : SHUTTER_LIT;
        njSetTexture(&core_shutter_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njPushMatrixEx();
        motion = MOTION(task);
        /* @hack spelling: the direct draw reads the offsets through a pointer
         * of its own, where motion->offset would do */
#if defined(SHUTTER_DIRECT) && !defined(SHUTTER_FAR_FACE)
        {
            Float *offset = motion->offset;
            left = -(motion->opening * LEFT_TRAVEL) + offset[LEFT_TRACK];
            right = (motion->opening * RIGHT_TRAVEL) - offset[RIGHT_TRACK];
            top = (motion->opening * TOP_TRAVEL) - offset[TOP_TRACK];
        }
#else
        left = -(motion->opening * LEFT_TRAVEL) + motion->offset[LEFT_TRACK];
        right = (motion->opening * RIGHT_TRAVEL) - motion->offset[RIGHT_TRACK];
        top = (motion->opening * TOP_TRAVEL) - motion->offset[TOP_TRACK];
#endif
        normal.x = 0.0F;
        normal.z = 1.0F;
        normal.y = 0.0F;
        njCalcVector(NULL, &normal, &normal);
#ifdef SHUTTER_FAR_FACE
        if (work->mode == CLOSED && position.z > FAR_FACE_DEPTH &&
            normal.z < 0.0F) {
            njSetTexture(&core_shutter_far_texlist);
            DRAW_RIGID(core_shutter_far_face, FAR_FACE);
            njFogDisable();
            if (TRIGGER(work) != BY_PROXIMITY) {
                MOVE_LATCH(core_shutter_far_closed_lamp, right,
                           MOTION(task)->lift);
                model_draw_tinted(core_shutter_far_closed_lamp->model,
                                  closed_light);
                matrix_load_parent();
            }
            MOVE_X(core_shutter_far_lamp, right);
            model_draw_tinted(core_shutter_far_lamp->model, main_light);
            njFogEnable();
        } else
#endif
        {
#ifdef SHUTTER_FAR_FACE
            /* @hack spelling: the left panel's z read through a pointer of its
             * own, where core_shutter_left->pos[2] would do */
            Float *left_pos = core_shutter_left->pos;
            njTranslate(NULL, core_shutter_left->pos[0] + left,
                        core_shutter_left->pos[1], left_pos[2]);
#else
            MOVE_X(core_shutter_left, left);
#endif
            DRAW_RIGID(core_shutter_left, LEFT);
            matrix_load_parent();
            MOVE_LATCH(core_shutter_latch, right, MOTION(task)->lift);
            DRAW_RIGID(core_shutter_latch, LATCH);
            matrix_load_parent();
            njFogDisable();
            if (TRIGGER(work) != BY_PROXIMITY) {
                MOVE_LATCH(core_shutter_closed_lamp, right, MOTION(task)->lift);
                model_draw_tinted(core_shutter_closed_lamp->model,
                                  closed_light);
                matrix_load_parent();
            }
            MOVE_LATCH(core_shutter_open_lamp, right, MOTION(task)->lift);
            model_draw_tinted(core_shutter_open_lamp->model, open_light);
            matrix_load_parent();
            MOVE_X(core_shutter_lamp, right);
            model_draw_tinted(core_shutter_lamp->model, main_light);
            matrix_load_parent();
            njFogEnable();
            MOVE_X(core_shutter_right, right);
            DRAW_RIGID(core_shutter_right, RIGHT);
            matrix_load_parent();
            njTranslate(NULL, core_shutter_top->pos[0],
                        core_shutter_top->pos[1] + top,
                        core_shutter_top->pos[2]);
            DRAW_RIGID(core_shutter_top, TOP);
            matrix_load_parent();
        }
        njPopMatrix(2);
    }
}
#undef DRAW_RIGID
