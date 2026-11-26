/* the background's draw for Final Chase and Cosmic Wall. each stage's source
 * includes this after its background_ models, builds and tuning. the source
 * defines SPACE_SKY_LAND, its sky land, which draws last. the four models draw
 * relative to the current camera. models 2 and 3 turn about their own
 * centers */
#ifndef SHARED_SPACE_SKY_DRAW_H
#define SHARED_SPACE_SKY_DRAW_H
#include <ninja.h>

#include "camera/camera.h"
#include "shared/land_draw.h"
#include "shared/space_sky_textures.h"
#include "system/task.h"
#include "system/cache.h"
#include "system/game_state.h"

/* models 2 and 3's angle about x at step 0 */
#define TURNED_START_X 0x3000

static void background_draw(struct task *task)
{
    /* @hack spelling: model, a local for background_2_object.model, where
     * the field read in place would do */
    NJS_CNK_MODEL *model;

    njFogDisable();
    njPushMatrixEx();
    njTranslateEx(&camera_views[camera_current_index]->pos);
    njTranslate(NULL, 0.0F, background_height, 0.0F);
    njSetTexture(&background_0_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, background_0_height, 0.0F);
    njScale(NULL, background_0_scale, 1.0F, background_0_scale);
    if (background_built) {
        njDirectDrawObject(background_builds[0].built);
    } else {
        njCnkEasyDrawObject(&background_0_object);
    }
    njPopMatrixEx();
    njSetTexture(&background_1_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, background_1_height, 0.0F);
    njScale(NULL, background_1_scale, 1.0F, background_1_scale);
    if (background_built) {
        njDirectDrawObject(background_builds[1].built);
    } else {
        njCnkEasyDrawObject(&background_1_object);
    }
    njPopMatrixEx();
    njPushMatrixEx();
    njSetTexture(&background_2_texlist);
    njTranslateEx((NJS_VECTOR *)background_2_object.pos);
    model = background_2_object.model;
    njTranslate(NULL, model->center.x, model->center.y, model->center.z);
    njRotateX(NULL, (background_2_turn_x * game_state.steps) + TURNED_START_X);
    njRotateY(NULL, background_2_turn_y * game_state.steps);
    njTranslate(NULL, -background_2_object.model->center.x,
                -background_2_object.model->center.y,
                -background_2_object.model->center.z);
    njScale(NULL, background_turned_scale, background_turned_scale,
            background_turned_scale);
    if (background_built) {
        njDirectDrawObject(background_builds[2].built);
    } else {
        njCnkEasyDrawModel(background_2_object.model);
    }
    matrix_load_parent();
    njSetTexture(&background_3_texlist);
    njTranslateEx((NJS_VECTOR *)background_3_object.pos);
    njTranslate(NULL, background_3_object.model->center.x,
                background_3_object.model->center.y,
                background_3_object.model->center.z);
    njRotateX(NULL, (background_3_turn_x * game_state.steps) + TURNED_START_X);
    njRotateY(NULL, background_3_turn_y * game_state.steps);
    njScale(NULL, background_turned_scale, background_turned_scale,
            background_turned_scale);
    njTranslate(NULL, -background_3_object.model->center.x,
                -background_3_object.model->center.y,
                -background_3_object.model->center.z);
    if (background_built) {
        njDirectDrawObject(background_builds[3].built);
    } else {
        njCnkEasyDrawModel(background_3_object.model);
    }
    matrix_load_parent();
    njPopMatrix(2);
    njFogEnable();
    land_table_draw(&SPACE_SKY_LAND);
}
#endif
