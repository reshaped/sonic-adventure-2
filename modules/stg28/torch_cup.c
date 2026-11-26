/* the torch cup, TORCHCUP in Pyramid Cave's object list: a fire burning in a
 * cup on a post */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg28/cave_util.h"
#include "stg28/pyramid_cave.h"
#include "stg28/torch_cup.h"
#include "system/task.h"
#include "system/util.h"

#include "stg28/torch_cup/text.inc"
#include "stg28/models/torch_cup.h"

#define SCALE(task) ((task)->work->scl.x + 1.0F)

/* the fire draws FIRE_HEIGHT above the cup's pos, and its sound plays
 * SOUND_HEIGHT above that pos. the scale multiplies both */
#define FIRE_HEIGHT 69.2788F
#define SOUND_HEIGHT 75.0F
/* the fire's sound, an id in pyramid_cave_sound_list. a play of it lasts
 * FIRE_SOUND_STEPS + 1 updates, as sound_play_at_for says */
#define FIRE_SOUND 5
#define FIRE_SOUND_PRIORITY 1
#define FIRE_SOUND_STEPS 15

#define TORCH_CUP_SHAPES 2
enum torch_cup_shape { FIRE_SHAPE, CUP_SHAPE };
/* the fire hurts players */
#define FIRE_LEVELS (SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(2))

NJS_TEXLIST *cave_torch_cup_texlists[2] = {
#include "stg28/torch_cup/cave_torch_cup_texlists.inc"
};
/* the fire's shape and the cup's, at a scale of 1 */
/* clang-format off */
static struct collision_shape torch_cup_shapes[TORCH_CUP_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), FIRE_LEVELS,
     SHAPE_HURTABLE, {0.0F, 65.0F, 0.0F}, 25.0F, 35.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, 15.0F, 0.0F}, 30.0F, 15.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the fire's volume, and how near player 0 must lie for the step to play its
 * sound */
static Sint32 fire_volume = 25;
static Float fire_range = 100.0F;

static void torch_cup_destroy(struct task *task);
static void torch_cup_step(struct task *task);
static void torch_cup_draw(struct task *task);

void cave_torch_cup_init(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    struct collision_shape shapes[TORCH_CUP_SHAPES];

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = torch_cup_draw;
    task->step = torch_cup_step;
    task->destructor = torch_cup_destroy;
    shapes[FIRE_SHAPE] = torch_cup_shapes[FIRE_SHAPE];
    shapes[CUP_SHAPE] = torch_cup_shapes[CUP_SHAPE];
    /* @bug logic: the fire's shape keeps its length at every scale. the
     * init scales the fire's radius (FMUL at 0x8c509282) and the cup's
     * size_y (FMUL at 0x8c5092b6), but not the fire's size_y. the draw
     * scales the fire's model */
    shapes[FIRE_SHAPE].radius *= SCALE(task);
    shapes[FIRE_SHAPE].center.y *= SCALE(task);
    shapes[CUP_SHAPE].radius *= SCALE(task);
    shapes[CUP_SHAPE].size_y *= SCALE(task);
    shapes[CUP_SHAPE].center.y *= SCALE(task);
    work->ang.x = work->ang.z = 0;
    collision_init(task, shapes, TORCH_CUP_SHAPES, COLLISION_OBJECT);
    FIRE_FRAME(task) = rand() * (FIRE_FRAMES * RAND_SCALE);
}

static void torch_cup_destroy(struct task *task)
{
}

static void torch_cup_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    if (placement_try_leave(task)) {
        return;
    }
    pos = work->pos;
    pos.y += SCALE(task) * SOUND_HEIGHT;
    if (fire_range > njDistanceP2P(&player_work[0]->pos, &pos)) {
        sound_play_at_for(FIRE_SOUND, task, FIRE_SOUND_PRIORITY, fire_volume,
                          FIRE_SOUND_STEPS, &pos);
    }
    collision_register(task);
    if (++FIRE_FRAME(task) >= FIRE_FRAMES) {
        FIRE_FRAME(task) = 0;
    }
}

static void torch_cup_draw(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    Sint16 texture;

    FIRE_SET_FRAME(cave_torch_cup_flame_model, FIRE_FRAME(task), texture);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njSetTexture(&cave_torch_cup_texlist);
    cave_model_draw_scaled(&cave_torch_cup_model, SCALE(task));
    njPopMatrixEx();
    njTranslate(NULL, 0.0F, SCALE(task) * FIRE_HEIGHT, 0.0F);
    /* the turn about y cancels the cup's and takes the camera's. so the fire
     * faces the camera from every side, but doesn't tilt toward it */
    njRotateY(NULL, -work->ang.y + camera_views[camera_current_index]->ang.y);
    njFogDisable();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(ADDITIVE_BLEND);
    njSetTexture(&pyramid_cave_fire_texlist);
    cave_model_draw_scaled(cave_torch_cup_flame_model, SCALE(task));
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_control_restore();
    njFogEnable();
    njPopMatrixEx();
}
