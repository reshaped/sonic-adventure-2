/* TORCH's build, init and callbacks, which stg25/torch.c compiles for Death
 * Chamber and stg26/torch.c for Egg Quarters, under EGG_QUARTERS. the
 * includer defines TORCH(name), and the torch's art, builds, shapes and
 * sound */
#ifndef STG25_STANDING_TORCH_IMPL_H
#define STG25_STANDING_TORCH_IMPL_H
#include <stdlib.h>
#include "camera/camera.h"
#include "object/set.h"
#include "shared/object_builds.h"
#include "stg25/flame_prop.h"
#include "stg25/round_shadow.h"
#include "stg25/prop_stage.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the flame's sound plays at a point SOUND_HEIGHT plus scl.y above the
 * torch's pos. the init has raised that pos by scl.y already */
#define SOUND_HEIGHT 60.0F
/* the flame draws FLAME_HEIGHT above the torch's pos */
#define FLAME_HEIGHT 57.1593F

static void chamber_torch_destroy(struct task *task);
static void chamber_torch_step(struct task *task);
static void chamber_torch_draw(struct task *task);

Uint8 *TORCH(build)(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, chamber_torch_builds, CHAMBER_COLORS) ==
        CHAMBER_COLORS) {
        chamber_torch_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void TORCH(init)(struct task *task)
{
    /* @hack spelling: task->work read again for scl.y, where work would do */
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = chamber_torch_draw;
        task->step = chamber_torch_step;
        task->destructor = chamber_torch_destroy;
        work->pos.y += task->work->scl.y;
        collision_init(task, chamber_torch_shapes, TORCH_SHAPES,
                       COLLISION_OBJECT);
        work->ang.x = work->ang.z = 0;
        FLAME_FRAME(task) = (int)((Float)rand() * (FLAME_FRAMES * RAND_SCALE));
    }
}

static void chamber_torch_destroy(struct task *task)
{
}

static void chamber_torch_step(struct task *task)
{
    /* @hack spelling: task->work read again for scl.y, where work would do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    if (!placement_try_leave(task)) {
        pos = work->pos;
        pos.y += task->work->scl.y + SOUND_HEIGHT;
        PLAY_NEAR_FLAME(task, &pos, chamber_torch_sound_range,
                        chamber_torch_volume);
        collision_register(task);
        if (++FLAME_FRAME(task) >= FLAME_FRAMES) {
            FLAME_FRAME(task) = 0;
        }
    }
}

static void chamber_torch_draw(struct task *task)
{
    /* @hack spelling: task->work read again for scl.y and the color, where
     * work would do */
    struct task_work *work = task->work;
#ifdef EGG_QUARTERS
    NJS_POINT3 flame_pos;
#endif
    Sint16 tid;

    /* @hack spelling: tid, a local for the halfword's new value, built in
     * two statements, where one assignment to the halfword would do */
    tid =
        chamber_torch_flame_models[CHAMBER_COLOR(task->work)]->plist[FLAME_TID];
    tid = (tid & NJD_FFM_MASK) | FLAME_FRAME(task);
    chamber_torch_flame_models[CHAMBER_COLOR(task->work)]->plist[FLAME_TID] =
        tid;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, -task->work->scl.y, 0.0F);
    njSetTexture(&STAGE_PROP(round_shadow_small_texlist));
    njCnkEasyDrawModel(&STAGE_PROP(round_shadow_small_model));
    njPopMatrixEx();
    njSetTexture(TORCH(texlists)[CHAMBER_COLOR(task->work)]);
    if (chamber_torch_built) {
        njDirectDrawObject(
            chamber_torch_builds[CHAMBER_COLOR(task->work)].built);
    } else {
        njCnkEasyDrawModel(chamber_torch_models[CHAMBER_COLOR(task->work)]);
    }
    njTranslate(NULL, 0.0F, FLAME_HEIGHT, 0.0F);
#ifdef EGG_QUARTERS
    /* the flame draws in the camera's own axes, turned a half turn about x. so
     * it faces the camera however the torch and the camera turn */
    njGetTranslation(NULL, &flame_pos);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&flame_pos);
    njRotateX(NULL, HALF_TURN);
#else
    /* the turn about y cancels the torch's and takes the camera's. so the flame
     * faces the camera from every side, but doesn't tilt toward it */
    njRotateY(NULL, -work->ang.y + camera_views[camera_current_index]->ang.y);
#endif
    njFogDisable();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(ADDITIVE_BLEND);
    njSetTexture(chamber_torch_flame_texlists[CHAMBER_COLOR(task->work)]);
    njCnkEasyDrawModel(chamber_torch_flame_models[CHAMBER_COLOR(task->work)]);
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_control_restore();
    njFogEnable();
#ifdef EGG_QUARTERS
    njPopMatrixEx();
#endif
    njPopMatrixEx();
}

#endif
