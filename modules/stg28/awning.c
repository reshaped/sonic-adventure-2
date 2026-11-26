/* the awning, AWNING in Pyramid Cave's object list: a solid awning with three
 * fires burning on it */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg28/awning.h"
#include "stg28/cave_util.h"
#include "stg28/pyramid_cave.h"
#include "system/task.h"
#include "system/util.h"

#include "stg28/awning/text.inc"
#include "stg28/models/awning.h"

/* the front fire shows FIRE_FRAME, and each side fire the frame SIDE_FRAMES
 * past the fire before it */
#define SIDE_FRAMES 5

/* where the fires burn, from the awning's pos in its own axes. the side
 * fires lie at -SIDE_X and SIDE_X */
#define FRONT_Y 16.2601F
#define FRONT_Z 20.4154F
#define SIDE_X 20.3932F
#define SIDE_Y 17.2136F
#define SIDE_Z 13.0681F

/* the fires' sound plays SOUND_Y above the awning's pos. SOUND_AHEAD lies
 * near FRONT_Z, but the step hands it to cave_polar_xz as the angle */
#define SOUND_Y 20.0F
#define SOUND_AHEAD 20
/* the fires' sound, an id in pyramid_cave_sound_list. a play of it lasts
 * FIRE_SOUND_STEPS + 1 updates, as sound_play_at_for says */
#define FIRE_SOUND 5
#define FIRE_SOUND_PRIORITY 1
#define FIRE_SOUND_STEPS 15
/* the awning's shape in the collision world collides only while a player
 * lies within REACH of its pos */
#define REACH 310.0F

#define AWNING_SHAPES 3
/* the fires hurt a player whose guard lies below level 2 */
#define FIRE_LEVELS (SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(2))

NJS_TEXLIST *cave_awning_texlists[2] = {
#include "stg28/awning/cave_awning_texlists.inc"
};
/* the front fire, then the side fires */
/* clang-format off */
static struct collision_shape awning_shapes[AWNING_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), FIRE_LEVELS,
     SHAPE_HURTABLE, {0.0F, FRONT_Y, FRONT_Z}, 3.5F, 5.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CAPSULE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), FIRE_LEVELS,
     SHAPE_HURTABLE, {-SIDE_X, SIDE_Y, SIDE_Z}, 3.0F, 3.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CAPSULE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), FIRE_LEVELS,
     SHAPE_HURTABLE, {SIDE_X, SIDE_Y, SIDE_Z}, 3.0F, 3.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the fires' volume, and how near player 0 must lie for the step to play their
 * sound */
static Sint32 fire_volume = 25;
static Float fire_range = 120.0F;

static void awning_destroy(struct task *task);
static void awning_step(struct task *task);
static void awning_draw(struct task *task);

void cave_awning_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = awning_draw;
    task->step = awning_step;
    task->destructor = awning_destroy;
    collision_init(task, awning_shapes, AWNING_SHAPES, COLLISION_OBJECT);
    work->ang.x = work->ang.z = 0;
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies the collision object through null where
     * world_pool_alloc has none to lend (MOV.L at 0x8c509f20). awning_destroy
     * tests task->extra for null (TST at 0x8c509fba). awning_step hands
     * player_near the copy's pos untested (ADD at 0x8c509ffa) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the awning, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = cave_awning_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
    object->ang[0] = 0;
    /* @hack spelling: task->work read again for ang.y, where work would do */
    object->ang[1] = task->work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SOLID, task, object);
    FIRE_FRAME(task) = rand() * (FIRE_FRAMES * RAND_SCALE);
}

static void awning_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void awning_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 pos;
    NJS_VECTOR offset;

    if (placement_try_leave(task)) {
        return;
    }
    object = task->extra.p;
    if (player_near((NJS_POINT3 *)object->pos, REACH)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    collision_register(task);
    /* @bug logic: passes SOUND_AHEAD as the angle and ang.y as the length
     * (MOV at 0x8c50a032, FLOAT at 0x8c50a02c). the sound then lies nearly
     * ang.y along z from the awning. so it sounds only where ang.y is small
     * enough, or player 0 stands near that place */
    cave_polar_xz(SOUND_AHEAD, work->ang.y, &offset.x, &offset.z);
    pos = work->pos;
    pos.y += SOUND_Y;
    pos.x += offset.x;
    pos.z += offset.z;
    if (fire_range > njDistanceP2P(&player_work[0]->pos, &pos)) {
        sound_play_at_for(FIRE_SOUND, task, FIRE_SOUND_PRIORITY, fire_volume,
                          FIRE_SOUND_STEPS, &pos);
    }
    if (++FIRE_FRAME(task) >= FIRE_FRAMES) {
        FIRE_FRAME(task) = 0;
    }
}

static void awning_draw(struct task *task)
{
    struct task_work *work = task->work;
    Sint16 texture;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njSetTexture(&cave_awning_texlist);
    njCnkEasyDrawModel(&cave_awning_model);
    njPopMatrixEx();
    njFogDisable();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(ADDITIVE_BLEND);
    njSetTexture(&pyramid_cave_fire_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, FRONT_Y, FRONT_Z);
    /* each fire's turn about y cancels the awning's and takes the camera's. so
     * the fire faces the camera from every side, but doesn't tilt toward it */
    njRotateY(NULL, -work->ang.y + camera_views[camera_current_index]->ang.y);
    FIRE_SET_FRAME(cave_awning_front_flame_model, FIRE_FRAME(task), texture);
    njCnkEasyDrawModel(cave_awning_front_flame_model);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslate(NULL, -SIDE_X, SIDE_Y, SIDE_Z);
    njRotateY(NULL, -work->ang.y + camera_views[camera_current_index]->ang.y);
    FIRE_SET_FRAME(cave_awning_side_flame_model,
                   (FIRE_FRAME(task) + SIDE_FRAMES) % FIRE_FRAMES, texture);
    njCnkEasyDrawModel(cave_awning_side_flame_model);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslate(NULL, SIDE_X, SIDE_Y, SIDE_Z);
    njRotateY(NULL, -work->ang.y + camera_views[camera_current_index]->ang.y);
    FIRE_SET_FRAME(cave_awning_side_flame_model,
                   (FIRE_FRAME(task) + (2 * SIDE_FRAMES)) % FIRE_FRAMES,
                   texture);
    njCnkEasyDrawModel(cave_awning_side_flame_model);
    njPopMatrixEx();
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_control_restore();
    njFogEnable();
    njPopMatrixEx();
}
