/* the second snake dish, SNAKEDISH2 in Pyramid Cave's object list:
 * snake_dish.c's dish, scaled by its placement. it has models of its own, and
 * lacks the two tall cylinders below that dish */
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
#include "stg28/snake_dish2.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#define SNAKE_DISH2_KINDS 1
#define KIND(task) ((Sint32)njAbs((task)->work->scl.z) % SNAKE_DISH2_KINDS)
#define SNAKE_DISH2_SHAPES 8
#define SCALE(task) ((task)->work->scl.x + 1.0F)

/* the fire draws FIRE_HEIGHT above the dish's pos, and its sound plays
 * SOUND_HEIGHT above that pos. the scale multiplies both */
#define FIRE_HEIGHT 108.2681F
#define SOUND_HEIGHT 110.0F
/* the fire's sound, an id in pyramid_cave_sound_list. a play of it lasts
 * FIRE_SOUND_STEPS + 1 updates, as sound_play_at_for says */
#define FIRE_SOUND 5
#define FIRE_SOUND_PRIORITY 1
#define FIRE_SOUND_STEPS 15

/* the fire hurts players */
#define FIRE_LEVELS (SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(2))

#include "stg28/snake_dish2/text.inc"
#include "stg28/models/snake_dish2.h"

static void snake_dish2_destroy(struct task *task);
static void snake_dish2_step(struct task *task);
static void snake_dish2_draw(struct task *task);

KEPT static NJS_CNK_MODEL *snake_dish2_models[SNAKE_DISH2_KINDS] = {
#include "stg28/snake_dish2/snake_dish2_models.inc"
};
KEPT static NJS_CNK_MODEL *snake_dish2_fire_models[SNAKE_DISH2_KINDS] = {
#include "stg28/snake_dish2/snake_dish2_fire_models.inc"
};
NJS_TEXLIST *cave_snake_dish2_texlists[SNAKE_DISH2_KINDS + 1] = {
#include "stg28/snake_dish2/cave_snake_dish2_texlists.inc"
};
KEPT static NJS_TEXLIST *snake_dish2_fire_texlists[SNAKE_DISH2_KINDS + 1] = {
#include "stg28/snake_dish2/snake_dish2_fire_texlists.inc"
};
static char *snake_dish2_kind_names[SNAKE_DISH2_KINDS] = {
#include "stg28/snake_dish2/snake_dish2_kind_names.inc"
};
/* the fire's shape, then the dish's, at a scale of 1 */
/* clang-format off */
static struct collision_shape snake_dish2_shapes[SNAKE_DISH2_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), FIRE_LEVELS,
     SHAPE_HURTABLE, {0.0F, 108.6638F, 0.0F}, 25.0F, 30.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, 56.34F, 0.0F}, 35.0F, 7.5F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {73.2643F, 90.7332F, 0.0F}, 12.0F, 31.5F, 0.0F, 0,
     {0, 0, -13288}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {-73.2643F, 90.7332F, 0.0F}, 12.0F, 31.5F, 0.0F, 0,
     {0, 0, 13288}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {129.7395F, 69.4049F, 0.0F}, 12.0F, 43.25F, 0.0F, 0,
     {0, 0, 8829}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {-129.7395F, 69.4049F, 0.0F}, 12.0F, 43.25F, 0.0F, 0,
     {0, 0, -8829}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {124.1292F, -0.3042F, 0.0F}, 12.0F, 53.0F, 0.0F, 0,
     {0, 0, -6863}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {-124.1292F, -0.3042F, 0.0F}, 12.0F, 53.0F, 0.0F, 0,
     {0, 0, 6863}},
};
/* clang-format on */
/* the fire's volume, and how near player 0 must lie for the step to play its
 * sound */
static Sint32 fire_volume = 25;
static Float fire_range = 120.0F;

void cave_snake_dish2_init(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_shape shapes[SNAKE_DISH2_SHAPES];
    int i;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y (   )";
        (void)"<- SCALE(1.0+x)";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = snake_dish2_draw;
    task->step = snake_dish2_step;
    task->destructor = snake_dish2_destroy;
    work->ang.x = work->ang.z = 0;
    for (i = 0; i < SNAKE_DISH2_SHAPES; i++) {
        shapes[i] = snake_dish2_shapes[i];
        shapes[i].center.x *= SCALE(task);
        shapes[i].center.y *= SCALE(task);
        shapes[i].center.z *= SCALE(task);
        shapes[i].radius *= SCALE(task);
        shapes[i].size_y *= SCALE(task);
    }
    collision_init(task, shapes, SNAKE_DISH2_SHAPES, COLLISION_OBJECT);
    FIRE_FRAME(task) = rand() * (FIRE_FRAMES * RAND_SCALE);
}

static void snake_dish2_destroy(struct task *task)
{
}

static void snake_dish2_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    /* @hack spelling: the step's body in the else of an empty then, where
     * the test negated, with no else, would do */
    if (placement_try_leave(task)) {
    } else {
        collision_register(task);
        pos = work->pos;
        pos.y += SCALE(task) * SOUND_HEIGHT;
        /* @hack redundant: a block no path reaches, a second copy of the
         * work's pos */
        if (0) {
            pos = work->pos;
        }
        if (fire_range > njDistanceP2P(&player_work[0]->pos, &pos)) {
            sound_play_at_for(FIRE_SOUND, task, FIRE_SOUND_PRIORITY,
                              fire_volume, FIRE_SOUND_STEPS, &pos);
        }
        if (++FIRE_FRAME(task) >= FIRE_FRAMES) {
            FIRE_FRAME(task) = 0;
        }
    }
}

static void snake_dish2_draw(struct task *task)
{
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused;
    struct task_work *work = task->work;
    Sint16 texture;

    FIRE_SET_FRAME(snake_dish2_fire_models[KIND(task)], FIRE_FRAME(task),
                   texture);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njSetTexture(cave_snake_dish2_texlists[KIND(task)]);
    cave_model_draw_scaled(snake_dish2_models[KIND(task)], SCALE(task));
    njPopMatrixEx();
    njScale(NULL, SCALE(task), SCALE(task), SCALE(task));
    njTranslate(NULL, 0.0F, FIRE_HEIGHT, 0.0F);
    /* the turn about y cancels the dish's and takes the camera's. so the fire
     * faces the camera from every side, but doesn't tilt toward it */
    njRotateY(NULL, -work->ang.y + camera_views[camera_current_index]->ang.y);
    njFogDisable();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(ADDITIVE_BLEND);
    njSetTexture(snake_dish2_fire_texlists[KIND(task)]);
    njCnkEasyDrawModel(snake_dish2_fire_models[KIND(task)]);
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_control_restore();
    njFogEnable();
    njPopMatrixEx();
}
