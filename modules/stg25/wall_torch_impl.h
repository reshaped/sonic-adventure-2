/* TORCH2's build, init and callbacks, which stg25/torch2.c compiles for Death
 * Chamber and stg26/wall_torch.c for Egg Quarters, under EGG_QUARTERS. the
 * includer defines TORCH2(name), and the torch's art, builds, shapes and
 * sound */
#ifndef STG25_WALL_TORCH_IMPL_H
#define STG25_WALL_TORCH_IMPL_H
#include <stdlib.h>
#include "camera/camera.h"
#include "object/set.h"
#include "shared/object_builds.h"
#include "stg25/prop_stage.h"
#include "stg25/flame_prop.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

enum chamber_torch2_shape { FLAME_SHAPE, STAND_SHAPE };

#define SCALE(work) ((work)->scl.x + 1.0F)
/* the flame's sound plays at a point SOUND_HEIGHT above the torch's pos,
 * scaled as the torch is */
#define SOUND_HEIGHT 8.0F
/* the flame draws FLAME_HEIGHT above the torch's pos and FLAME_AHEAD ahead of
 * it, scaled as the torch is */
#define FLAME_HEIGHT 6.8318F
#define FLAME_AHEAD 5.1163F

static void chamber_torch2_destroy(struct task *task);
static void chamber_torch2_step(struct task *task);
static void chamber_torch2_draw(struct task *task);

Uint8 *TORCH2(build)(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, chamber_torch2_builds, CHAMBER_COLORS) ==
        CHAMBER_COLORS) {
        chamber_torch2_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void TORCH2(init)(struct task *task)
{
    /* @hack spelling: task->work read again for the scale, where work would
     * do */
    struct task_work *work = task->work;
    struct collision_shape shapes[TORCH2_SHAPES];

    if (!placement_try_leave(task)) {
        task->draw = chamber_torch2_draw;
        task->step = chamber_torch2_step;
        task->destructor = chamber_torch2_destroy;
        shapes[FLAME_SHAPE] = chamber_torch2_shapes[FLAME_SHAPE];
        shapes[STAND_SHAPE] = chamber_torch2_shapes[STAND_SHAPE];
        shapes[FLAME_SHAPE].radius *= SCALE(task->work);
        shapes[FLAME_SHAPE].size_y *= SCALE(task->work);
        shapes[FLAME_SHAPE].center.y *= SCALE(task->work);
        shapes[FLAME_SHAPE].center.z *= SCALE(task->work);
        shapes[STAND_SHAPE].radius *= SCALE(task->work);
        shapes[STAND_SHAPE].size_y *= SCALE(task->work);
        shapes[STAND_SHAPE].center.y *= SCALE(task->work);
        collision_init(task, shapes, TORCH2_SHAPES, COLLISION_OBJECT);
        work->ang.x = work->ang.z = 0;
        FLAME_FRAME(task) = (int)((Float)rand() * (FLAME_FRAMES * RAND_SCALE));
    }
}

static void chamber_torch2_destroy(struct task *task)
{
}

static void chamber_torch2_step(struct task *task)
{
    /* @hack spelling: task->work read again for the scale, where work would
     * do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    if (!placement_try_leave(task)) {
        pos = work->pos;
        pos.y += SCALE(task->work) * SOUND_HEIGHT;
        PLAY_NEAR_FLAME(task, &pos, chamber_torch2_sound_range,
                        chamber_torch2_volume);
        collision_register(task);
        if (++FLAME_FRAME(task) >= FLAME_FRAMES) {
            FLAME_FRAME(task) = 0;
        }
    }
}

static void chamber_torch2_draw(struct task *task)
{
    /* @hack spelling: task->work read again for the scale and the color,
     * where work would do */
    struct task_work *work = task->work;
    /* @hack spelling: tid, a local for the halfword's new value, set as it
     * is declared and then ORed with the frame, where one assignment to the
     * halfword would do */
    Sint16 tid = chamber_torch2_flame_models[CHAMBER_COLOR(task->work)]
                     ->plist[FLAME_TID] &
                 NJD_FFM_MASK;

    tid |= FLAME_FRAME(task);
    chamber_torch2_flame_models[CHAMBER_COLOR(task->work)]->plist[FLAME_TID] =
        tid;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(TORCH2(texlists)[CHAMBER_COLOR(task->work)]);
    if (chamber_torch2_built) {
        STAGE_PROP(direct_draw_scaled)(
            chamber_torch2_builds[CHAMBER_COLOR(task->work)].built,
            SCALE(task->work));
    } else {
        STAGE_PROP(model_draw_scaled)(
            chamber_torch2_models[CHAMBER_COLOR(task->work)],
            SCALE(task->work));
    }
    njTranslate(NULL, 0.0F, FLAME_HEIGHT, FLAME_AHEAD);
    /* the turn about y cancels the torch's and takes the camera's. so the flame
     * faces the camera from every side, but doesn't tilt toward it */
    njRotateY(NULL, -work->ang.y + camera_views[camera_current_index]->ang.y);
    njFogDisable();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(ADDITIVE_BLEND);
    njSetTexture(chamber_torch2_flame_texlists[CHAMBER_COLOR(task->work)]);
    njCnkEasyDrawModel(chamber_torch2_flame_models[CHAMBER_COLOR(task->work)]);
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_control_restore();
    njFogEnable();
    njPopMatrixEx();
}

#endif
