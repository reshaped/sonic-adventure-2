/* the rising current, WIND in Dry Lagoon's and Cannon's Core's object lists,
 * which stg18/updraft.c and stg38/updraft.c include. current_stage.h gives
 * each stage's calls. the current is an upright column from pos up scl.y. its
 * radius is scl.x at its foot and scl.z at its top. while player 0 lies
 * inside, the current pushes it up. where the radii differ, the push also has
 * parts across x and z. they are APEX_PULL times those of a push along the
 * line through the player and the column's apex. the current's sound plays
 * where bit 0 of ang.x is set. in Cannon's Core, it plays only while time
 * doesn't stand still. in Dry Lagoon, that bit also gives the current its
 * specks */
#ifndef SHARED_RISING_CURRENT_H
#define SHARED_RISING_CURRENT_H
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/current_stage.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* whether player 0 lies within the column's height, with HEIGHT_MARGIN to
 * spare at each end */
#define HEIGHT_MARGIN 2.0F
#define PLAYER_IN_HEIGHT(record, owner_task)                                   \
    ((record)->pos.y - HEIGHT_MARGIN <= player_work[0]->pos.y &&               \
     player_work[0]->pos.y <=                                                  \
         (record)->pos.y + (owner_task)->work->scl.y + HEIGHT_MARGIN)

/* in the work's ang.z: whether player 0 lay inside the current at the last
 * step. the step starts the pad's vibration as the player comes in */
#define PLAYER_INSIDE(task) ((task)->work->ang.z)
/* a column holds speck_density * SPECKS_SCALE specks for each unit of its top
 * radius squared times its height */
#define SPECKS_SCALE 0.001F
/* at the column's foot, the push up on player 0 is lift times START_GAIN. it
 * loses lift for each scl.y above the foot. WAVER() then scales it */
#define START_GAIN 1.5F
/* WAVER() runs from WAVER_LOW to WAVER_LOW + 2 * WAVER_DEPTH and back. its
 * angle turns 1 << WAVER_SHIFT a step, a whole turn each 64 steps */
#define WAVER_SHIFT 10
#define WAVER_LOW 0.5F
#define WAVER_DEPTH 0.25F
#define WAVER()                                                                \
    (((njCos(game_state.steps << WAVER_SHIFT) + 1.0F) * WAVER_DEPTH) +         \
     WAVER_LOW)
/* a tapered column's push has the push up as its y, along the line to the
 * apex. its parts across x and z are then APEX_PULL times as large */
#define APEX_PULL 2.5F
/* in Dry Lagoon, bit 0 of ang.x, which UPDRAFT_SOUND_ON reads, gives the
 * current its specks */
#define HAS_SPECKS(task) ((task)->work->ang.x & 1)

static Float lift = 0.175F;
static int near_volume = LOUDEST;
static int far_volume = -75;
static Float sound_range = 75.0F;
/* the sound's scale while the camera is above the water, which current_sound.h
 * reads in Dry Lagoon alone */
static Float above_water_gain = 0.75F;
static Float speck_density = 0.5F;
static Float pulse_height = 2.0F;
static int pulse_power = 2;
static int pulse_frequency = 20;
static int pulse_interval = 30;
static int pulse_count = 3;
static int entry_power = ENTRY_POWER;
static int entry_frequency = ENTRY_FREQUENCY;
static int entry_duration = 0;

#include "shared/current_sound.h"

static void updraft_destroy(struct task *task);
static void updraft_step(struct task *task);
static void updraft_draw(struct task *task);

void UPDRAFT_INIT(struct task *task)
{
#ifndef LAGOON_CURRENT
    int count;
#endif

    if (!placement_try_leave(task)) {
        task->draw = updraft_draw;
        task->step = updraft_step;
        task->destructor = updraft_destroy;
#ifdef LAGOON_CURRENT
        if (HAS_SPECKS(task)) {
            specks_column_create(task, 0,
                                 (int)(task->work->scl.z * task->work->scl.z *
                                       task->work->scl.y * speck_density *
                                       SPECKS_SCALE),
                                 &task->extra.task);
        }
#else
        count = task->work->scl.z * task->work->scl.z * task->work->scl.y *
                speck_density * SPECKS_SCALE;
        if (count <= 0) {
            count = 1;
        }
        specks_column_create(task, 0, count, &task->extra.task);
#endif
        PLAYER_INSIDE(task) = 0;
        PLAYER_HEIGHT_MARK(task) = player_work[0]->pos.y;
    }
}

static void updraft_destroy(struct task *task)
{
    if (task->extra.task) {
        task_kill(task->extra.task);
    }
    task->extra.task = NULL;
}

static Float updraft_strength(struct task *task)
{
    Float strength;

    strength =
        lift * (START_GAIN - ((player_work[0]->pos.y - task->work->pos.y) /
                              task->work->scl.y));
    return strength * WAVER();
}

/* pulse player 0's pad as the frame of its fall's animation passes
 * threshold. the pulse needs the player less than pulse_height above
 * PLAYER_HEIGHT_MARK, the player's height as the current started, or in Dry
 * Lagoon at the step before */
#define PULSE_AT_ENTRY(task, threshold)                                        \
    if (player_work[0]->mode == PLAYER_MODE_FALL) {                            \
        Float pulse_frame = (threshold);                                       \
        struct player *player = player_records[0];                             \
        if (player->anim.last_frame <= pulse_frame &&                          \
            player->anim.frame > pulse_frame &&                                \
            player_work[0]->pos.y - PLAYER_HEIGHT_MARK(task) < pulse_height) { \
            CURRENT_RUMBLE(0, pulse_power, pulse_frequency, pulse_interval,    \
                           pulse_count);                                       \
        }                                                                      \
    }

static void updraft_step(struct task *task)
{
    /* @hack spelling: scl and ang.z read through task->work, where work
     * would do */
    struct task_work *work = task->work;
    NJS_POINT3 center;
    Float x;
    Float z;
    Float distance;
    Float taper;
    Float apex_height;
    Float strength;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: center, half way up the column, which nothing reads */
    center = work->pos;
    center.y += task->work->scl.y * 0.5F;
    distance = njSqrt(((player_work[0]->pos.x - work->pos.x) *
                       (player_work[0]->pos.x - work->pos.x)) +
                      ((player_work[0]->pos.z - work->pos.z) *
                       (player_work[0]->pos.z - work->pos.z)));
    taper = task->work->scl.x - task->work->scl.z;
    if (taper != 0.0F) {
        /* the apex, where the column's radius would reach 0, lies apex_height
         * above pos */
        apex_height = task->work->scl.x * (task->work->scl.y / taper);
        /* @bug logic: tests a tapered column as a cone whose radius at player
         * 0's height divides by the apex's y, apex_height + pos.y, where
         * apex_height belongs. the test agrees with the column only where
         * pos.y is 0 (the FDIV at 0x8c5054c2 in Dry Lagoon's image and at
         * 0x8c50935e in Cannon's Core's) */
        if (PLAYER_IN_HEIGHT(work, task) &&
            (task->work->scl.x / (apex_height + work->pos.y)) *
                    (apex_height + work->pos.y - player_work[0]->pos.y) >
                distance) {
            NJS_VECTOR direction;
            if (!PLAYER_INSIDE(task)) {
                vibration_start(0, entry_power, entry_frequency,
                                entry_duration);
            }
            PLAYER_INSIDE(task) = 1;
            strength = updraft_strength(task);
            direction.x = work->pos.x - player_work[0]->pos.x;
            direction.y = work->pos.y + apex_height - player_work[0]->pos.y;
            direction.z = work->pos.z - player_work[0]->pos.z;
            njUnitVector(&direction);
            x = (strength / direction.y) * direction.x * APEX_PULL;
            z = (strength / direction.y) * direction.z * APEX_PULL;
            player_updraft(0, x, strength, z);
            PULSE_AT_ENTRY(task, TAPERED_PULSE_FRAME);
        } else {
            PLAYER_INSIDE(task) = 0;
        }
    } else {
        if (PLAYER_IN_HEIGHT(work, task)) {
            if (task->work->scl.x > distance) {
                if (!PLAYER_INSIDE(task)) {
                    vibration_start(0, entry_power, entry_frequency,
                                    entry_duration);
                }
                PLAYER_INSIDE(task) = 1;
                strength = updraft_strength(task);
                player_updraft(0, 0.0F, strength, 0.0F);
                PULSE_AT_ENTRY(task, 0.0F);
            } else {
                PLAYER_INSIDE(task) = 0;
            }
        } else {
            PLAYER_INSIDE(task) = 0;
        }
    }
#ifdef LAGOON_CURRENT
    if (PLAYER_HEIGHT_MARK(task) != player_work[0]->pos.y) {
        PLAYER_HEIGHT_MARK(task) = player_work[0]->pos.y;
    }
#endif
    if (UPDRAFT_SOUND_ON(task)) {
        NJS_LINE axis;
        axis.px = work->pos.x;
        axis.py = work->pos.y;
        axis.pz = work->pos.z;
        axis.vx = 0.0F;
        axis.vy = task->work->scl.y;
        axis.vz = 0.0F;
        distance = njSqrt(point_segment_distance2(
                       &camera_views[camera_current_index]->pos, &axis, NULL)) -
                   task->work->scl.z;
        PLAY_CURRENT_SOUND(task, distance);
    }
}

static void updraft_draw(struct task *task)
{
}

#endif
