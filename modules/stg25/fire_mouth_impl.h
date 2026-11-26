/* MOUTH's build, init and callbacks, which stg25/fire_mouth.c compiles for
 * Death Chamber and stg26/fire_mouth.c for Egg Quarters, under EGG_QUARTERS.
 * the includer defines FIRE_MOUTH(name), and the mouth's art and texture
 * lists */
#ifndef STG25_FIRE_MOUTH_IMPL_H
#define STG25_FIRE_MOUTH_IMPL_H
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg25/prop_stage.h"
#include "stg25/fire_mouth.h"
#include "stg25/chamber_color.h"
#include "stg25/flame_prop.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define FIRE_MOUTH_SHAPES 1
#define SCALE_ACROSS(work) ((work)->scl.x + 1.0F)
#define SCALE_UP(work) ((work)->scl.y + 1.0F)
/* the init puts a copy of fire_mouth_collision in the collision world, and
 * keeps it in task->extra. the copy is WORLD_SOLID, and its WORLD_SIZE_CLASS
 * is 2 */
#define FACE_WORLD_FLAGS (WORLD_SIZE_CLASS(2) | WORLD_SOLID)
/* the eval flags the copy clears, so that the world reads its pos, ang and
 * scl */
#define FACE_EVAL_CLEARED                                                      \
    (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL)
/* the flame's sound plays SOUND_DEPTH times SCALE_UP below the face's pos.
 * Death Chamber's face plays it at its init, and Egg Quarters' at each step */
#define SOUND_DEPTH 5.0F
/* the copy collides only while a player lies near its pos. near is within
 * FACE_REACH times the larger of SCALE_ACROSS and SCALE_UP, plus
 * REACH_MARGIN */
#define FACE_REACH 31.1F
#define REACH_MARGIN 10.0F
/* the flame draws FLAME_DEPTH below the face's pos and FLAME_AHEAD ahead of
 * it, scaled as the face is */
#ifdef EGG_QUARTERS
#define FLAME_DEPTH 13.5014F
#else
#define FLAME_DEPTH 11.6729F
#endif
#define FLAME_AHEAD 12.6816F

static void chamber_fire_mouth_destroy(struct task *task);
static void chamber_fire_mouth_step(struct task *task);
static void chamber_fire_mouth_draw(struct task *task);

static NJS_TEXLIST *chamber_fire_mouth_flame_texlists[CHAMBER_COLORS] = {
#include "stg25/fire_mouth_impl/chamber_fire_mouth_flame_texlists.inc"
};
static char *chamber_fire_mouth_color_names[CHAMBER_COLORS] = {
#include "stg25/fire_mouth_impl/chamber_fire_mouth_color_names.inc"
};
/* the flame's shape at a scale of 1 */
static struct collision_shape chamber_fire_mouth_shape = {
#include "stg25/fire_mouth_impl/chamber_fire_mouth_shape.inc"
};
static Sint32 chamber_fire_mouth_volume = {
#include "stg25/fire_mouth_impl/chamber_fire_mouth_volume.inc"
};
static Float chamber_fire_mouth_sound_range = {
#include "stg25/fire_mouth_impl/chamber_fire_mouth_sound_range.inc"
};
/* TRUE once one call of FIRE_MOUTH(build) has compiled all CHAMBER_COLORS
 * colors' faces */
static Bool chamber_fire_mouth_built = {
#include "stg25/fire_mouth_impl/chamber_fire_mouth_built.inc"
};
static struct model_build chamber_fire_mouth_builds[CHAMBER_COLORS] = {
#include "stg25/fire_mouth_impl/chamber_fire_mouth_builds.inc"
};
/* the radius of the face's collision record, which replaces the one
 * collision_init gives it */
static Float chamber_fire_mouth_radius = {
#include "stg25/fire_mouth_impl/chamber_fire_mouth_radius.inc"
};

Uint8 *FIRE_MOUTH(build)(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, chamber_fire_mouth_builds, CHAMBER_COLORS) ==
        CHAMBER_COLORS) {
        chamber_fire_mouth_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void FIRE_MOUTH(init)(struct task *task)
{
    /* @hack spelling: task->work read again for scl, where work would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = chamber_fire_mouth_draw;
    task->step = chamber_fire_mouth_step;
    task->destructor = chamber_fire_mouth_destroy;
    object = task->extra.p = world_pool_alloc();
    /* @bug memory: writes fire_mouth_collision through null where
     * world_pool_alloc has none to lend. the step then reads pos through
     * null, though the destructor tests task->extra for it. Death Chamber
     * writes at 0x8c505e94, tests at 0x8c50602a and reads at 0x8c506124. Egg
     * Quarters writes at 0x8c502dd4, tests at 0x8c502f10 and reads at
     * 0x8c502ff2 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the face, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = fire_mouth_collision;
    object->evalflags &= ~FACE_EVAL_CLEARED;
    object->scl[0] = object->scl[2] = SCALE_ACROSS(task->work);
    object->scl[1] = SCALE_UP(task->work);
    object->ang[0] = 0;
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(FACE_WORLD_FLAGS, task, object);
    /* @hack spelling: the shapes and pos each in a block of its own, where
     * declarations at the function's top would do */
    {
        /* @hack type: the copy an array of one shape, where a shape would
         * do */
        struct collision_shape shapes[FIRE_MOUTH_SHAPES];

        shapes[0] = chamber_fire_mouth_shape;
        shapes[0].radius *= SCALE_ACROSS(task->work);
        shapes[0].size_y *= SCALE_UP(task->work);
        shapes[0].center.y *= SCALE_UP(task->work);
        shapes[0].center.z *= SCALE_ACROSS(task->work);
        collision_init(task, shapes, FIRE_MOUTH_SHAPES, COLLISION_OBJECT);
    }
    work->collision->radius = chamber_fire_mouth_radius;
#ifndef EGG_QUARTERS
    {
        NJS_POINT3 pos;

        pos = work->pos;
        /* @hack spelling: the depth added as its negative, where -= would
         * do */
        pos.y += -(SCALE_UP(task->work) * SOUND_DEPTH);
        PLAY_NEAR_FLAME(task, &pos, chamber_fire_mouth_sound_range,
                        chamber_fire_mouth_volume);
    }
#endif
    FLAME_FRAME(task) = (int)((Float)rand() * (FLAME_FRAMES * RAND_SCALE));
}

static void chamber_fire_mouth_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void chamber_fire_mouth_step(struct task *task)
{
    /* @hack spelling: task->work read again for scl, where work would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float scale;

    if (!placement_try_leave(task)) {
        object = task->extra.p;
        scale = SCALE_ACROSS(task->work) > SCALE_UP(task->work)
                    ? SCALE_ACROSS(task->work)
                    : SCALE_UP(task->work);
        if (player_near((NJS_POINT3 *)object->pos,
                        (scale * FACE_REACH) + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
        collision_register(task);
#ifdef EGG_QUARTERS
        {
            NJS_POINT3 pos;

            pos = work->pos;
            pos.y += -(SCALE_UP(task->work) * SOUND_DEPTH);
            PLAY_NEAR_FLAME(task, &pos, chamber_fire_mouth_sound_range,
                            chamber_fire_mouth_volume);
        }
#endif
        if (++FLAME_FRAME(task) >= FLAME_FRAMES) {
            FLAME_FRAME(task) = 0;
        }
    }
}

static void chamber_fire_mouth_draw(struct task *task)
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
    tid = chamber_fire_mouth_flame_models[CHAMBER_COLOR(task->work)]
              ->plist[FLAME_TID];
    tid &= ~FLAME_TID_BITS;
    tid |= FLAME_FRAME(task);
    chamber_fire_mouth_flame_models[CHAMBER_COLOR(task->work)]
        ->plist[FLAME_TID] = tid;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(FIRE_MOUTH(texlists)[CHAMBER_COLOR(task->work)]);
    if (chamber_fire_mouth_built) {
        STAGE_PROP(direct_draw_scaled_v)(
            chamber_fire_mouth_builds[CHAMBER_COLOR(task->work)].built, &scale);
    } else {
        STAGE_PROP(model_draw_scaled_v)(
            chamber_fire_mouth_models[CHAMBER_COLOR(task->work)], &scale);
    }
    njTranslate(NULL, 0.0F, -FLAME_DEPTH, FLAME_AHEAD);
    /* the flame draws at its place in the camera's own axes, turned a half
     * turn about x. it then faces the camera however the face and the camera
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
    njSetTexture(chamber_fire_mouth_flame_texlists[CHAMBER_COLOR(task->work)]);
    STAGE_PROP(model_draw_scaled_v)(
        chamber_fire_mouth_flame_models[CHAMBER_COLOR(task->work)], &scale);
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
