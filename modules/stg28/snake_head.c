/* the snake head, SNEAKHEAD in Pyramid Cave's object list: a solid fire pot
 * shaped as a snake's head. a fire burns at either side of it */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg28/cave_util.h"
#include "stg28/pyramid_cave.h"
#include "stg28/snake_head.h"
#include "system/task.h"
#include "system/util.h"

#include "stg28/snake_head/text.inc"
#include "stg28/models/snake_head.h"

/* the fires burn FIRE_X to either side of the head's pos and FIRE_Y above it.
 * each fire's sound plays SOUND_X to its side and SOUND_Y above */
#define FIRE_X 68.5973F
#define FIRE_Y 12.7058F
#define SOUND_X 70.0F
#define SOUND_Y 15.0F
/* the fires' sound, an id in pyramid_cave_sound_list. a play of it lasts
 * FIRE_SOUND_STEPS + 1 updates, as sound_play_at_for says. the fire on +x
 * plays it for the task, and the fire on -x for SOUND_SECOND_KEY. with one
 * key, the second play would move the first's sound */
#define FIRE_SOUND 5
#define FIRE_SOUND_PRIORITY 1
#define FIRE_SOUND_STEPS 15

/* the head's shape in the collision world collides only while a player lies
 * within REACH of its pos */
#define REACH 310.0F

#define SNAKE_HEAD_SHAPES 2

/* the fires hurt players */
#define FIRE_LEVELS (SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(2))

NJS_TEXLIST *cave_snake_head_texlists[2] = {
#include "stg28/snake_head/cave_snake_head_texlists.inc"
};
/* clang-format off */
static struct collision_shape snake_head_shapes[SNAKE_HEAD_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), FIRE_LEVELS,
     SHAPE_HURTABLE, {-FIRE_X, FIRE_Y, 0.0F}, 4.0F, 13.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CAPSULE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), FIRE_LEVELS,
     SHAPE_HURTABLE, {FIRE_X, FIRE_Y, 0.0F}, 4.0F, 13.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the fires' volume. the step plays a fire's sound only while player 0 lies
 * within fire_range of it */
static Sint32 fire_volume = 25;
static Float fire_range = 120.0F;

static void snake_head_destroy(struct task *task);
static void snake_head_step(struct task *task);
static void snake_head_draw(struct task *task);

void cave_snake_head_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y (   )";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = snake_head_draw;
    task->step = snake_head_step;
    task->destructor = snake_head_destroy;
    collision_init(task, snake_head_shapes, SNAKE_HEAD_SHAPES,
                   COLLISION_OBJECT);
    work->ang.z = 0;
    work->ang.x = 0;
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies the collision object through null where
     * world_pool_alloc has none to lend (MOV.L at 0x8c509580).
     * snake_head_destroy tests task->extra for null (TST at 0x8c50961a).
     * snake_head_step hands player_near the copy's pos untested (ADD at
     * 0x8c50965e) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the snake head, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = cave_snake_head_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
    object->ang[0] = 0;
    /* @hack spelling: the work read again through task, where work
     * would do */
    object->ang[1] = task->work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SOLID, task, object);
    FIRE_FRAME(task) = rand() * (FIRE_FRAMES * RAND_SCALE);
}

static void snake_head_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void snake_head_step(struct task *task)
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
    cave_point_turn_y(SOUND_X, 0.0F, work->ang.y, &offset);
    pos = work->pos;
    pos.x += offset.x;
    pos.z += offset.z;
    pos.y += SOUND_Y;
    if (fire_range > njDistanceP2P(&player_work[0]->pos, &pos)) {
        sound_play_at_for(FIRE_SOUND, task, FIRE_SOUND_PRIORITY, fire_volume,
                          FIRE_SOUND_STEPS, &pos);
    }
    pos = work->pos;
    pos.x -= offset.x;
    pos.z -= offset.z;
    pos.y += SOUND_Y;
    if (fire_range > njDistanceP2P(&player_work[0]->pos, &pos)) {
        sound_play_at_for(FIRE_SOUND, SOUND_SECOND_KEY(task),
                          FIRE_SOUND_PRIORITY, fire_volume, FIRE_SOUND_STEPS,
                          &pos);
    }
    if (++FIRE_FRAME(task) >= FIRE_FRAMES) {
        FIRE_FRAME(task) = 0;
    }
}

static void snake_head_draw(struct task *task)
{
    /* @hack spelling: model, a local for the fire's model, where the model in
     * place would do */
    NJS_CNK_MODEL *model = &cave_snake_head_flame_model;
    struct task_work *work = task->work;
    Sint16 texture;
    /* @hack spelling: word, a pointer to the fire's texture halfword, where
     * the halfword in place would do */
    Sint16 *word;

    word = &model->plist[FIRE_TID];
    texture = *word;
    texture = (texture & NJD_FFM_MASK) | FIRE_FRAME(task);
    *word = texture;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njSetTexture(&cave_snake_head_texlist);
    njCnkEasyDrawModel(&cave_snake_head_model);
    njPopMatrixEx();
    njFogDisable();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(ADDITIVE_BLEND);
    njSetTexture(&pyramid_cave_fire_texlist);
    njPushMatrixEx();
    njTranslate(NULL, -FIRE_X, FIRE_Y, 0.0F);
    /* the turn about y cancels the head's and takes the camera's. so each
     * fire faces the camera from every side, but doesn't tilt toward it */
    njRotateY(NULL, -work->ang.y + camera_views[camera_current_index]->ang.y);
    njCnkEasyDrawModel(model);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslate(NULL, FIRE_X, FIRE_Y, 0.0F);
    njRotateY(NULL, -work->ang.y + camera_views[camera_current_index]->ang.y);
    njCnkEasyDrawModel(model);
    njPopMatrixEx();
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_control_restore();
    njFogEnable();
    njPopMatrixEx();
}
