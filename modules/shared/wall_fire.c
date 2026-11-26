/* the wall fire, WALL FIRE in Sand Ocean's object list: a dish on a wall.
 * its flame hurts the players. Sand Ocean's chain and post fires, FIRE2 and
 * FIRE3, include this source with names of their own */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/ocean_util.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the frame of ocean_fire_texlist the flame shows */
#define FRAME(task) ((task)->unk_3c.l)
/* the flame's texture word in its polygon list */
#define FLAME_TID 7
/* fixture_shapes' flame, and with FIRE_HAS_POST its post */
#define FLAME_SHAPE 0
#define POST_SHAPE 1
#define FIRE_SOUND 2
#define SOUND_PRIORITY 1
/* each play of the sound lasts SOUND_DURATION + 1 steps */
#define SOUND_DURATION 15

/* the wall fire's names. a fire that includes this source defines its own.
 * it leaves out FIRE_WALL_MOUNTED, which scales the z of the flame's center */
#ifndef FIRE_INIT
#include "shared/wall_fire.h"

#define FIRE_INIT wall_fire_init
#define FIRE_HOLDER wall_fire_model
#define FIRE_TEXLIST wall_fire_texlist
#define FIRE_FLAME wall_fire_flame_model
#define FIRE_WALL_MOUNTED
/* the flame stands FLAME_Y above the dish and FLAME_Z before it, times the
 * scale. it draws FLAME_SCALE times the scale */
#define FLAME_Y 5.0F
#define FLAME_Z 5.0F
#define FLAME_SCALE 0.5F
/* the sound plays SOUND_HEIGHT times the scale above the work's pos */
#define SOUND_HEIGHT 10.0F

NJS_TEXLIST *wall_fire_texlists[] = {&wall_fire_texlist, NULL};
/* the flame's sphere, before the scale: it hurts players */
/* clang-format off */
static struct collision_shape fixture_shapes[] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
     {0.0F, 5.0F, 3.0F}, 6.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
#endif

#define FIRE_SHAPES ARRAY_COUNT(fixture_shapes)
/* @hack spelling: SCALE reads the work through task, where each function's
 * own work would do */
#define SCALE(task) ((task)->work->scl.x + 1.0F)

/* the sound's volume, and how near player 0 must be for it */
static Sint32 fire_volume = 25;
static Float fire_range = 100.0F;

static void fire_destroy(struct task *task);
static void fire_step(struct task *task);
static void fire_draw(struct task *task);

void FIRE_INIT(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: an array even for a fire of one sphere, where a struct
     * and its address would do */
    struct collision_shape scaled[FIRE_SHAPES];

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = fire_draw;
    task->step = fire_step;
    task->destructor = fire_destroy;
    scaled[FLAME_SHAPE] = fixture_shapes[FLAME_SHAPE];
#ifdef FIRE_HAS_POST
    scaled[POST_SHAPE] = fixture_shapes[POST_SHAPE];
#endif
    scaled[FLAME_SHAPE].radius *= SCALE(task);
    scaled[FLAME_SHAPE].center.y *= SCALE(task);
#ifdef FIRE_WALL_MOUNTED
    scaled[FLAME_SHAPE].center.z *= SCALE(task);
#endif
#ifdef FIRE_HAS_POST
    scaled[POST_SHAPE].radius *= SCALE(task);
    scaled[POST_SHAPE].size_y *= SCALE(task);
    scaled[POST_SHAPE].center.y *= SCALE(task);
#endif
    collision_init(task, scaled, FIRE_SHAPES, COLLISION_OBJECT);
    work->ang.x = work->ang.z = 0;
    FRAME(task) = rand() * (OCEAN_FIRE_FRAMES * RAND_SCALE);
}

static void fire_destroy(struct task *task)
{
}

static void fire_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    if (placement_try_leave(task)) {
        return;
    }
    collision_register(task);
    pos = work->pos;
    pos.y += SCALE(task) * SOUND_HEIGHT;
    if (njDistanceP2P(&player_work[0]->pos, &pos) < fire_range) {
        sound_play_at_for(FIRE_SOUND, task, SOUND_PRIORITY, fire_volume,
                          SOUND_DURATION, &pos);
    }
    if (++FRAME(task) >= OCEAN_FIRE_FRAMES) {
        FRAME(task) = 0;
    }
}

static void fire_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    Sint16 texture;

    texture = FIRE_FLAME->plist[FLAME_TID];
    texture = (texture & NJD_FFM_MASK) | FRAME(task);
    FIRE_FLAME->plist[FLAME_TID] = texture;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njSetTexture(&FIRE_TEXLIST);
    ocean_model_draw_scaled(&FIRE_HOLDER, SCALE(task));
    njPopMatrixEx();
    njScale(NULL, SCALE(task), SCALE(task), SCALE(task));
    njTranslate(NULL, 0.0F, FLAME_Y, FLAME_Z);
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
    ocean_model_draw_scaled(FIRE_FLAME, SCALE(task) * FLAME_SCALE);
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_control_restore();
    njFogEnable();
    njPopMatrixEx();
    njPopMatrixEx();
}
