/* FIREPOT's build, init and callbacks, which stg25/fire_pot.c compiles for
 * Death Chamber and stg26/fire_pot.c for Egg Quarters, under EGG_QUARTERS. the
 * includer defines FIRE_POT(name), and the pot's art and texture lists */
#ifndef STG25_FIRE_POT_IMPL_H
#define STG25_FIRE_POT_IMPL_H
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg25/prop_stage.h"
#include "stg25/fire_pot.h"
#include "stg25/chamber_color.h"
#include "stg25/flame_prop.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define FIRE_POT_SHAPES 2
enum chamber_fire_pot_shape { FLAME_SHAPE, POT_SHAPE };
#define SCALE_ACROSS(work) ((work)->scl.x + 1.0F)
#define SCALE_UP(work) ((work)->scl.y + 1.0F)

/* the flame's sound plays SOUND_HEIGHT times SCALE_UP above the pot's pos, at
 * each step */
#define SOUND_HEIGHT 8.0F
/* the flame draws FLAME_HEIGHT above the pot's pos, scaled as the pot is */
#define FLAME_HEIGHT 17.9449F

static void chamber_fire_pot_destroy(struct task *task);
static void chamber_fire_pot_step(struct task *task);
static void chamber_fire_pot_draw(struct task *task);

static NJS_TEXLIST *chamber_fire_pot_flame_texlists[CHAMBER_COLORS] = {
#include "stg25/fire_pot_impl/chamber_fire_pot_flame_texlists.inc"
};
static char *chamber_fire_pot_color_names[CHAMBER_COLORS] = {
#include "stg25/fire_pot_impl/chamber_fire_pot_color_names.inc"
};
/* the flame's shape and the pot's, at a scale of 1 */
static struct collision_shape chamber_fire_pot_shapes[FIRE_POT_SHAPES] = {
#include "stg25/fire_pot_impl/chamber_fire_pot_shapes.inc"
};
static Sint32 chamber_fire_pot_volume = {
#include "stg25/fire_pot_impl/chamber_fire_pot_volume.inc"
};
static Float chamber_fire_pot_sound_range = {
#include "stg25/fire_pot_impl/chamber_fire_pot_sound_range.inc"
};
/* TRUE once one call of FIRE_POT(build) has compiled all CHAMBER_COLORS
 * colors' pots */
static Bool chamber_fire_pot_built = {
#include "stg25/fire_pot_impl/chamber_fire_pot_built.inc"
};
static struct model_build chamber_fire_pot_builds[CHAMBER_COLORS] = {
#include "stg25/fire_pot_impl/chamber_fire_pot_builds.inc"
};

Uint8 *FIRE_POT(build)(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, chamber_fire_pot_builds, CHAMBER_COLORS) ==
        CHAMBER_COLORS) {
        chamber_fire_pot_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void FIRE_POT(init)(struct task *task)
{
    /* @hack spelling: task->work read again for scl, where work would do */
    struct task_work *work = task->work;
    struct collision_shape shapes[FIRE_POT_SHAPES];

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = chamber_fire_pot_draw;
    task->step = chamber_fire_pot_step;
    task->destructor = chamber_fire_pot_destroy;
    shapes[FLAME_SHAPE] = chamber_fire_pot_shapes[FLAME_SHAPE];
    shapes[POT_SHAPE] = chamber_fire_pot_shapes[POT_SHAPE];
    shapes[FLAME_SHAPE].radius *= SCALE_ACROSS(task->work);
    shapes[FLAME_SHAPE].size_y *= SCALE_UP(task->work);
    shapes[FLAME_SHAPE].center.y *= SCALE_UP(task->work);
    shapes[POT_SHAPE].radius *= SCALE_ACROSS(task->work);
    shapes[POT_SHAPE].size_y *= SCALE_UP(task->work);
    shapes[POT_SHAPE].center.y *= SCALE_UP(task->work);
    collision_init(task, shapes, FIRE_POT_SHAPES, COLLISION_OBJECT);
    work->ang.x = work->ang.z = 0;
    FLAME_FRAME(task) = (int)((Float)rand() * (FLAME_FRAMES * RAND_SCALE));
}

static void chamber_fire_pot_destroy(struct task *task)
{
}

static void chamber_fire_pot_step(struct task *task)
{
    /* @hack spelling: task->work read again for scl.y, where work would do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    if (!placement_try_leave(task)) {
        pos = work->pos;
        pos.y += SCALE_UP(task->work) * SOUND_HEIGHT;
        PLAY_NEAR_FLAME(task, &pos, chamber_fire_pot_sound_range,
                        chamber_fire_pot_volume);
        collision_register(task);
        if (++FLAME_FRAME(task) >= FLAME_FRAMES) {
            FLAME_FRAME(task) = 0;
        }
    }
}

static void chamber_fire_pot_draw(struct task *task)
{
    /* @hack spelling: task->work read again for scl.y and the color, where
     * work would do */
    struct task_work *work = task->work;
    NJS_VECTOR scale;
    NJS_POINT3 pos;
    Sint16 tid;

    scale.x = scale.z = SCALE_ACROSS(work);
    scale.y = SCALE_UP(task->work);
    /* @hack spelling: the texture id built in tid by three statements, where
     * one assignment to plist[FLAME_TID] would do */
    tid = chamber_fire_pot_flame_models[CHAMBER_COLOR(task->work)]
              ->plist[FLAME_TID];
    tid &= ~FLAME_TID_BITS;
    tid |= FLAME_FRAME(task);
    chamber_fire_pot_flame_models[CHAMBER_COLOR(task->work)]->plist[FLAME_TID] =
        tid;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(FIRE_POT(texlists)[CHAMBER_COLOR(task->work)]);
    if (chamber_fire_pot_built) {
        STAGE_PROP(direct_draw_scaled_v)(
            chamber_fire_pot_builds[CHAMBER_COLOR(task->work)].built, &scale);
    } else {
        STAGE_PROP(model_draw_scaled_v)(
            chamber_fire_pot_models[CHAMBER_COLOR(task->work)], &scale);
    }
    njTranslate(NULL, 0.0F, FLAME_HEIGHT, 0.0F);
    /* the flame draws at its place in the camera's own axes, turned a half
     * turn about x. it then faces the camera however the pot and the camera
     * turn */
    njGetTranslation(NULL, &pos);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&pos);
    njRotateX(NULL, HALF_TURN);
#ifdef EGG_QUARTERS
    njFogDisable();
#endif
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(ADDITIVE_BLEND);
    njSetTexture(chamber_fire_pot_flame_texlists[CHAMBER_COLOR(task->work)]);
    STAGE_PROP(model_draw_scaled_v)(
        chamber_fire_pot_flame_models[CHAMBER_COLOR(task->work)], &scale);
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_control_restore();
#ifdef EGG_QUARTERS
    njFogEnable();
#endif
    njPopMatrixEx();
    njPopMatrixEx();
}

#endif
