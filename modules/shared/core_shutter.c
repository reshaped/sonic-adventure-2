#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/core_shutter.h"
#include "shared/shutter_art.h"
#include "shared/shutter_motion.h"
#ifdef SHUTTER_FAR_FACE
#include "shared/colony_shutter_textures.h"
#endif
#include "shared/dynamite.h"
#include "shared/switch.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"

enum shutter_part {
    TOP,
    RIGHT,
    LATCH,
    LEFT,
#ifdef SHUTTER_FAR_FACE
    FAR_FACE,
#endif
    PARTS
};
/* the switch, enemy group or charges' id that the trigger reads */
#define TRIGGER_ID(work) ((Uint8)(work)->ang.x)
/* the shutter's trigger, by shared/shutter_motion.h's enum shutter_trigger */
#ifdef SHUTTER_FAR_FACE
#define TRIGGER(work) ((work)->ang.z & 3)
#define FAR_FACE_DEPTH 100.0F
#else
/* @hack spelling: ang.z cut to a byte and taken mod 4, where ang.z & 3 would
 * do */
#define TRIGGER(work) ((Uint8)(work)->ang.z % 4)
#endif
/* the half sizes of the lock-on box a closed shutter carries, which stops a
 * mech's lock-on */
#define BOX_HALF_WIDTH 200.0F
#define BOX_HALF_HEIGHT 120.0F
#ifdef SHUTTER_FAR_FACE
#define BOX_HALF_DEPTH 1.0F
#else
#define BOX_HALF_DEPTH 5.0F
#endif
NJS_TEXLIST *core_shutter_texlists[] = {
    &core_shutter_texlist,
#ifdef SHUTTER_FAR_FACE
    &core_shutter_far_texlist,
#endif
    NULL,
};
static Bool core_shutter_built = FALSE;
static struct model_build core_shutter_builds[PARTS] = {
    {MODEL_BUILD_ALONE, &core_shutter_texlist, core_shutter_top, NULL},
    {MODEL_BUILD_ALONE, &core_shutter_texlist, core_shutter_right, NULL},
    {MODEL_BUILD_ALONE, &core_shutter_texlist, core_shutter_latch, NULL},
    {MODEL_BUILD_ALONE, &core_shutter_texlist, core_shutter_left, NULL},
#ifdef SHUTTER_FAR_FACE
    {MODEL_BUILD_ALONE, &core_shutter_far_texlist, core_shutter_far_face, NULL},
#endif
};
#ifndef SHUTTER_FAR_FACE
static Sint32 core_shutter_start_volume = 75;
static Sint32 core_shutter_stop_volume = 125;
static Float core_shutter_sound_range = 500.0F;
#endif
/* the set editor's names for the four triggers, which no code reads */
static char *trigger_names[] = {"SWITCH", "ENEMY", "DYNAMITE", "NEAR"};
static Float slide_speed = 0.015F;
static Float lift_speed = 0.05F;
/* the boxes of the right and left panels, which slide apart as they open */
/* clang-format off */
static struct collision_shape doors[2] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {20.0F, 30.0F, 0.0F}, 60.0F, 30.0F, 18.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {-55.0F, 30.0F, 0.0F}, 25.0F, 30.0F, 18.0F, 0, {0, 0, 0}}};
/* clang-format on */
/* each track's velocity as the shutter comes fully open, and as it comes
 * shut */
static Float opening_kick[TRACKS] = {1.0F, 1.5F, 2.0F};
static Float closing_kick[TRACKS] = {-1.0F, -1.5F, -2.0F};

static void core_shutter_destroy(struct task *task);
static void core_shutter_step(struct task *task);
static void core_shutter_draw(struct task *task);
static void core_shutter_draw_direct(struct task *task);

Uint8 *core_shutter_build(Uint8 *buffer)
{
    /* @hack spelling: cursor, a copy of buffer, where buffer would do */
    Uint8 *cursor = buffer;
    /* @hack redundant: a jump to the next statement */
    goto build;
build:
    if (model_builds_compile(&cursor, core_shutter_builds, PARTS) == PARTS) {
        core_shutter_built = TRUE;
    }
    return cursor;
}

void core_shutter_init(struct task *task)
{
    struct task_work *work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct shutter_motion));
    if (!task->motion.p) {
        return;
    }
    if (core_shutter_built) {
        task->draw = core_shutter_draw_direct;
    } else {
        task->draw = core_shutter_draw;
    }
    task->step = core_shutter_step;
    task->destructor = core_shutter_destroy;
    collision_init(task, doors, 2, COLLISION_OBJECT);
    work->mode = CLOSED;
}

static void core_shutter_destroy(struct task *task)
{
    /* @hack spelling: owner, a copy of task, where task would do */
    struct task *owner = task;

    syFree(owner->motion.p);
    owner->motion.p = NULL;
}

/* whether point, in the shutter's frame, lies in the box where a player
 * opens a BY_PROXIMITY shutter. the box spans NEAR_HALF_WIDTH either way
 * across, from NEAR_BOTTOM to NEAR_TOP, and from NEAR_BACK to NEAR_FRONT
 * along z */
#define NEAR_HALF_WIDTH 100.0F
#define NEAR_BOTTOM (-10.0F)
#define NEAR_TOP 80.0F
#define NEAR_BACK (-80.0F)
#define NEAR_FRONT 150.0F
#define PLAYER_INSIDE(point)                                                   \
    ((point).y < NEAR_TOP && (point).y > NEAR_BOTTOM &&                        \
     njAbs((point).x) < NEAR_HALF_WIDTH && (point).z < NEAR_FRONT &&           \
     (point).z > NEAR_BACK)
static Bool core_shutter_triggered(struct task *task)
{
    struct task_work *work = task->work;
    Bool inside;
    NJS_POINT3 point;
    switch (TRIGGER(work)) {
    case BY_SWITCH:
        return switch_is_on(TRIGGER_ID(work));
    case BY_ENEMIES:
        return enemy_group_empty(TRIGGER_ID(work));
    case BY_DYNAMITE:
        return !dynamite_count(TRIGGER_ID(work));
    case BY_PROXIMITY:
        inside = FALSE;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, -work->ang.y);
        njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
        if (player_work[0]) {
            njCalcPoint(NULL, &player_work[0]->pos, &point);
            if (PLAYER_INSIDE(point)) {
                inside = TRUE;
            }
        }
        if (player_work[1]) {
            njCalcPoint(NULL, &player_work[1]->pos, &point);
            if (PLAYER_INSIDE(point)) {
                inside = TRUE;
            }
        }
        njPopMatrixEx();
        return inside;
    }
    return TRUE;
}

static void core_shutter_step(struct task *task)
{
    struct task_work *work = task->work;
    int track;
    int closing_track;
#ifdef SHUTTER_FAR_FACE
    struct shutter_motion *motion;
#endif
#ifdef NONMATCHING
    /* the plain form of the hack tagged below: an array of three Angles */
    Angle turn[3];
#else
    struct angles turn;
#endif
    NJS_VECTOR size;
    if (placement_try_leave(task)) {
        return;
    }
#ifdef SHUTTER_FAR_FACE
    /* @hack redundant: a store of OPENING that no path reaches */
    if (0) {
        work->mode = OPENING;
    }
#endif
    if (core_shutter_triggered(task)) {
        if (work->mode == CLOSED) {
            /* @bug logic: a proximity shutter that has closed unlocks for one
             * step, not UNLOCK_STEPS + 1. SETTLING leaves the timer past
             * UNLOCK_STEPS. this store doesn't reset it, so the test of the
             * timer ends UNLOCKING on its first step. the store and the test
             * lie at 0x264 and 0x3d2 of the object's code. colony_shutter.c's
             * object holds them at 0x236 and 0x34e */
            work->mode = UNLOCKING;
#ifdef SHUTTER_FAR_FACE
            sound_play(START_SOUND, NULL, 0, 0);
#else
            if (njDistanceP2P(&work->pos, &player_work[0]->pos) <
                core_shutter_sound_range) {
                sound_play_at(START_SOUND, NULL, 0, core_shutter_start_volume,
                              &work->pos);
            }
#endif
        }
    } else if (TRIGGER(work) == BY_PROXIMITY) {
        if (work->mode == OPEN) {
            work->mode = CLOSING;
        }
    } else {
        work->mode = CLOSED;
    }
    switch (work->mode) {
    case CLOSED:
        MOTION(task)->closed_light = 1.0F;
        MOTION(task)->open_light = 0.0F;
        MOTION(task)->opening = 0.0F;
        MOTION(task)->lift = 0.0F;
        if (!task->child) {
            size.x = BOX_HALF_WIDTH;
            size.y = BOX_HALF_HEIGHT;
            size.z = BOX_HALF_DEPTH;
#ifdef NONMATCHING
            /* the plain form the hack tag names */
            turn[0] = 0;
            turn[1] = work->ang.y;
            turn[2] = 0;
            lockon_box_create(task, turn, &size);
#else
            /* @hack type: turn a struct angles passed as an Angle *, where an
             * array of three Angles would do */
            turn.x = 0;
            turn.y = work->ang.y;
            turn.z = 0;
            lockon_box_create(task, (Angle *)&turn, &size);
#endif
        }
        break;
    case UNLOCKING:
        MOTION(task)->closed_light = 1.0F;
        MOTION(task)->open_light = 1.0F;
        work->timer++;
        if (work->timer > UNLOCK_STEPS) {
            work->mode = LIFTING;
            work->timer = 0;
        }
        break;
    case LIFTING:
        MOTION(task)->closed_light = 0.0F;
        MOTION(task)->open_light = 1.0F;
#ifdef SHUTTER_FAR_FACE
        /* @hack redundant: a store into lift that no path reaches */
        if (0) {
            motion->lift = lift_speed * 0.2F;
        }
#endif
        MOTION(task)->lift += lift_speed;
        if (MOTION(task)->lift > 1.0F) {
            MOTION(task)->lift = 1.0F;
            work->mode = OPENING;
        }
        break;
    case OPENING:
        MOTION(task)->closed_light = 0.0F;
        MOTION(task)->open_light = 1.0F;
        MOTION(task)->opening += slide_speed;
        if (MOTION(task)->opening > 1.0F) {
            MOTION(task)->opening = 1.0F;
            work->mode = OPEN;
#ifdef SHUTTER_FAR_FACE
            sound_play(STOP_SOUND, NULL, 0, 0);
#else
            if (njDistanceP2P(&work->pos, &player_work[0]->pos) <
                core_shutter_sound_range) {
                sound_play_at(STOP_SOUND, NULL, 0, core_shutter_stop_volume,
                              &work->pos);
            }
#endif
#ifdef SHUTTER_FAR_FACE
            {
                /* @hack spelling: opening_kick read through kick, where
                 * opening_kick[track] would do */
                Float *kick = &opening_kick[0];
                for (track = 0; track < TRACKS; track++) {
                    MOTION(task)->velocity[track] = kick[track];
                }
            }
#else
            for (track = 0; track < TRACKS; track++) {
                MOTION(task)->velocity[track] = opening_kick[track];
            }
#endif
        }
        if (task->child) {
            task_kill(task->child);
        }
        break;
    case OPEN:
        MOTION(task)->open_light = 1.0F;
        MOTION(task)->closed_light = 0.0F;
        for (track = 0; track < TRACKS; track++) {
            MOTION(task)->offset[track] += MOTION(task)->velocity[track];
#ifdef SHUTTER_FAR_FACE
            /* @hack spelling: the record reached through motion, where
             * MOTION(task) would do */
            motion = MOTION(task);
            motion->velocity[track] =
                SPRING_VELOCITY(motion, track, SPRING_INCREMENT);
#else
            MOTION(task)->velocity[track] =
                SPRING_VELOCITY(MOTION(task), track, SPRING_INCREMENT);
#endif
            if (MOTION(task)->offset[track] < 0.0F) {
                MOTION(task)->offset[track] = 0.0F;
                MOTION(task)->velocity[track] =
                    njAbs(MOTION(task)->velocity[track]) * BOUNCE;
            }
        }
        if (task->child) {
            task_kill(task->child);
        }
        break;
    case CLOSING:
        MOTION(task)->closed_light = 1.0F;
        MOTION(task)->open_light = 1.0F;
        MOTION(task)->lift -= lift_speed;
        if (MOTION(task)->lift <= 0.0F) {
            MOTION(task)->lift = 0.0F;
            MOTION(task)->opening -= slide_speed;
            if (MOTION(task)->opening < 0.0F) {
#ifndef SHUTTER_FAR_FACE
                Float *kick;
#endif
                MOTION(task)->opening = 0.0F;
                work->mode = SETTLING;
#ifndef SHUTTER_FAR_FACE
                sound_play_at(STOP_SOUND, NULL, 0, core_shutter_stop_volume,
                              &work->pos);
#endif
                work->timer = 0;
#ifdef SHUTTER_FAR_FACE
                for (track = 0; track < TRACKS; track++) {
                    MOTION(task)->velocity[track] = closing_kick[track];
                }
#else
                /* @hack spelling: closing_kick read through kick, where
                 * closing_kick[track] would do */
                kick = &closing_kick[0];
                for (track = 0; track < TRACKS; track++) {
                    MOTION(task)->velocity[track] = kick[track];
                }
#endif
            }
        }
        break;
    case SETTLING: {
#ifndef SHUTTER_FAR_FACE
        /* @hack spelling: the increment held in spring_step, where
         * SPRING_INCREMENT would do */
        Float spring_step = SPRING_INCREMENT;
#endif
        MOTION(task)->open_light = 0.0F;
        MOTION(task)->closed_light = 1.0F;
        for (closing_track = 0; closing_track < TRACKS; closing_track++) {
            MOTION(task)->offset[closing_track] +=
                MOTION(task)->velocity[closing_track];
#ifdef SHUTTER_FAR_FACE
            /* @hack spelling: the record reached through motion, where
             * MOTION(task) would do */
            motion = MOTION(task);
            motion->velocity[closing_track] =
                SPRING_VELOCITY(motion, closing_track, SPRING_INCREMENT);
#else
            MOTION(task)->velocity[closing_track] =
                SPRING_VELOCITY(MOTION(task), closing_track, spring_step);
#endif
            if (MOTION(task)->offset[closing_track] > 0.0F) {
                MOTION(task)->offset[closing_track] = 0.0F;
                MOTION(task)->velocity[closing_track] =
                    -(njAbs(MOTION(task)->velocity[closing_track]) * BOUNCE);
            }
        }
        work->timer++;
        if (work->timer > SETTLE_STEPS) {
            work->mode = CLOSED;
        }
    }
    }
    if (work->collision) {
        work->collision->shapes[0].center.x =
            doors[0].center.x + (MOTION(task)->opening * RIGHT_TRAVEL);
        work->collision->shapes[1].center.x =
            doors[1].center.x - (MOTION(task)->opening * LEFT_TRAVEL);
        collision_fit_radius(task);
        collision_register(task);
    }
}

#include "shared/core_shutter_draw_impl.h"
#define SHUTTER_DIRECT
#include "shared/core_shutter_draw_impl.h"
