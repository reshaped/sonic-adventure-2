/* the fire dish, FIRE4 in a stage's object list: a dish with a flame that
 * hurts the players. its source is the wall fire's, but for the flame's
 * place, its shape and its sound. the flame draws the wall fire's model */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/ocean_util.h"
#include "shared/fire_dish.h"
#include "shared/wall_fire.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the frame of the flame's textures the task shows. the init picks one at
 * random. each step moves it on */
#define FRAME(task) ((task)->unk_3c.l)
#define SCALE(work) ((work)->scl.x + 1.0F)
/* the flame's texture word in its polygon list */
#define FLAME_TID 7
/* the flame stands FLAME_Y above the dish, times the scale. it draws
 * FLAME_SCALE times the scale */
#define FLAME_Y 7.0F
#define FLAME_SCALE 0.5F
/* the sound plays SOUND_HEIGHT times the scale above the pos. each play lasts
 * SOUND_DURATION + 1 steps */
#define SOUND_HEIGHT 12.0F
#define FIRE_SOUND 2
#define SOUND_PRIORITY 1
#define SOUND_DURATION 15

NJS_TEXLIST *fire_dish_texlists[] = {&fire_dish_texlist, NULL};
/* the flame's sphere, before the scale: it hurts players */
/* clang-format off */
static struct collision_shape fire_dish_shape = {
    0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
    {0.0F, 6.0F, 0.0F}, 7.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
/* the sound's volume, and how near player 0 must be for it */
static Sint32 fire_dish_volume = 25;
static Float fire_dish_range = 100.0F;

static void fire_dish_destroy(struct task *task);
static void fire_dish_step(struct task *task);
static void fire_dish_draw(struct task *task);

void fire_dish_init(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack type: shape as an array of one, where the shape alone would do */
    struct collision_shape shape[1];
    NJS_POINT3 pos;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = fire_dish_draw;
    task->step = fire_dish_step;
    task->destructor = fire_dish_destroy;
    shape[0] = fire_dish_shape;
    /* @hack spelling: task->work read again, where work would do */
    shape[0].radius *= SCALE(task->work);
    shape[0].center.y *= SCALE(task->work);
    shape[0].center.z *= SCALE(task->work);
    collision_init(task, shape, 1, COLLISION_OBJECT);
    pos = work->pos;
    pos.y += SCALE(task->work) * SOUND_HEIGHT;
    if (njDistanceP2P(&player_work[0]->pos, &pos) < fire_dish_range) {
        sound_play_at_for(FIRE_SOUND, task, SOUND_PRIORITY, fire_dish_volume,
                          SOUND_DURATION, &pos);
    }
    work->ang.x = work->ang.z = 0;
    FRAME(task) = rand() * (OCEAN_FIRE_FRAMES * RAND_SCALE);
}

static void fire_dish_destroy(struct task *task)
{
}

static void fire_dish_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    collision_register(task);
    if (++FRAME(task) >= OCEAN_FIRE_FRAMES) {
        FRAME(task) = 0;
    }
}

static void fire_dish_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    Sint16 texture;

    texture = wall_fire_flame_model->plist[FLAME_TID];
    texture = (texture & NJD_FFM_MASK) | FRAME(task);
    wall_fire_flame_model->plist[FLAME_TID] = texture;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njSetTexture(&fire_dish_texlist);
    /* @hack spelling: task->work read again, where work would do */
    ocean_model_draw_scaled(&fire_dish_model, SCALE(task->work));
    njPopMatrixEx();
    njScale(NULL, SCALE(task->work), SCALE(task->work), SCALE(task->work));
    njTranslate(NULL, 0.0F, FLAME_Y, 0.0F);
    njGetTranslation(NULL, &pos);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&pos);
    njRotateX(NULL, HALF_TURN);
    njFogDisable();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ONE);
    njSetTexture(&ocean_fire_texlist);
    ocean_model_draw_scaled(wall_fire_flame_model,
                            SCALE(task->work) * FLAME_SCALE);
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_control_restore();
    njFogEnable();
    njPopMatrixEx();
    njPopMatrixEx();
}
