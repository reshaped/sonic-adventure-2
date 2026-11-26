/* the side current, SIDEWIND in Dry Lagoon's and Cannon's Core's object
 * lists, which stg18/current.c and stg38/current.c include. current_stage.h
 * gives each stage's calls. the current is a cylinder of radius scl.x from
 * pos, scl.y long along -z turned by ang.y. while player 0 lies inside, the
 * current pushes it along the cylinder, away from pos */
#ifndef SHARED_SIDE_CURRENT_H
#define SHARED_SIDE_CURRENT_H
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

/* the specks a cylinder holds for each unit of its radius squared times its
 * length. Dry Lagoon's are speck_density * SPECKS_SCALE, and Cannon's Core's
 * PARTICLE_DENSITY */
#define SPECKS_SCALE 0.001F
#define PARTICLE_DENSITY 0.00090000004F
/* at pos, the push on player 0 is force times START_GAIN. it loses force for
 * each scl.y of the player's distance from pos across x and z. WAVER() then
 * scales it */
#define START_GAIN 1.5F
/* WAVER() runs from WAVER_LOW to WAVER_LOW + 2 * WAVER_DEPTH and back. its
 * angle turns 1 << WAVER_SHIFT a step, a whole turn each 64 steps */
#define WAVER_SHIFT 10
#define WAVER_LOW 0.75F
#define WAVER_DEPTH 0.125F
#define WAVER()                                                                \
    (((njCos(game_state.steps << WAVER_SHIFT) + 1.0F) * WAVER_DEPTH) +         \
     WAVER_LOW)

static int pulse_power = 2;
static int pulse_frequency = 20;
static int pulse_interval = 30;
static int pulse_count = 3;
static Float force = 0.3F;
static Float dat_side_current_0 = 0.075F;
static Float dat_side_current_1 = 0.015F;
static int near_volume = LOUDEST;
static int far_volume = -75;
static Float sound_range = 75.0F;
/* the sound's scale while the camera is above the water, which current_sound.h
 * reads in Dry Lagoon alone */
static Float above_water_gain = 0.75F;
#ifdef LAGOON_CURRENT
static Float speck_density = 0.5F;
#endif
/* the push up on player 0 each step, which Cannon's Core alone gives */
static Float lift = 0.067F;

#include "shared/current_sound.h"

static void current_destroy(struct task *task);
static void current_step(struct task *task);
static void current_draw(struct task *task);
static Bool current_contains_player(struct task *task);

void CURRENT_INIT(struct task *task)
{
#ifdef LAGOON_CURRENT
    /* @hack spelling: owner and radius, locals for the call's arguments, where
     * the arguments alone would do */
    struct task **owner;
    Float radius;
#else
    int count;
#endif

    if (!placement_try_leave(task)) {
        task->draw = current_draw;
        task->step = current_step;
        task->destructor = current_destroy;
#ifdef LAGOON_CURRENT
        owner = &task->extra.task;
        radius = task->work->scl.x;
        specks_side_create(task, 0,
                           (int)(radius * radius * task->work->scl.y *
                                 speck_density * SPECKS_SCALE),
                           owner);
#else
        count = task->work->scl.x * task->work->scl.x * task->work->scl.y *
                PARTICLE_DENSITY;
        if (count <= 0) {
            count = 1;
        }
        specks_side_create(task, 0, count, &task->extra.task);
#endif
    }
}

static void current_destroy(struct task *task)
{
    if (task->extra.task) {
        task_kill(task->extra.task);
    }
    task->extra.task = NULL;
}

static Float current_strength(struct task *task)
{
    Float x;
    Float z;
    Float base;
    Float distance;
    Float waver;

    x = player_work[0]->pos.x - task->work->pos.x;
    z = player_work[0]->pos.z - task->work->pos.z;
    distance = njSqrt((x * x) + (z * z));
    base = force * (START_GAIN - (distance / task->work->scl.y));
    waver = WAVER();
    return base * waver;
}

static void current_step(struct task *task)
{
    /* @hack spelling: scl read through task->work, where work would do */
    struct task_work *work = task->work;
    Float x;
    Float z;
    Float strength;
    Float distance;

    if (placement_try_leave(task)) {
        return;
    }
    if (CAN_PUSH) {
        /* @hack redundant: center, pos raised by half scl.y, which nothing
         * reads */
        NJS_POINT3 center;
        center = work->pos;
        center.y += task->work->scl.y * 0.5F;
        if (current_contains_player(task)) {
            strength = current_strength(task);
            CURRENT_POLAR(work->ang.y, -strength, &x, &z);
            player_work[0]->flags |= PLAYER_WORK_REQUEST;
            player_work[0]->submode = PLAYER_REQUEST_UPDRAFT;
            player_motion[0]->acc.x += x;
#ifndef LAGOON_CURRENT
            player_motion[0]->acc.y += lift;
#endif
            player_motion[0]->acc.z += z;
            player_records[0]->spd.y = 0.0F;
            if (player_work[0]->mode == PLAYER_MODE_FALL &&
                player_records[0]->anim.last_frame <= 0.0F &&
                player_records[0]->anim.frame > 0.0F) {
                CURRENT_RUMBLE(0, pulse_power, pulse_frequency, pulse_interval,
                               pulse_count);
            }
        }
    }
    if (SOUND_ACTIVE) {
        NJS_LINE axis;
        axis.px = work->pos.x;
        axis.py = work->pos.y;
        axis.pz = work->pos.z;
        CURRENT_POLAR(work->ang.y, -task->work->scl.y, &axis.vx, &axis.vz);
        axis.vy = 0.0F;
        distance = njSqrt(point_segment_distance2(
                       &camera_views[camera_current_index]->pos, &axis, NULL)) -
                   task->work->scl.x;
        PLAY_CURRENT_SOUND(task, distance);
    }
}

static void current_draw(struct task *task)
{
}

static Bool current_contains_player(struct task *task)
{
    /* @hack spelling: scl read through task->work, where work would do */
    struct task_work *work = task->work;
    struct task_work *player = player_work[0];
    NJS_VECTOR offset;
    NJS_VECTOR axis;
    NJS_VECTOR radial;
    NJS_POINT3 start;
    NJS_POINT3 end;
    Float x;
    Float z;
    Float length2;
    Float dot;
    Float along;

    if (work->pos.y - task->work->scl.x > player->pos.y ||
        player->pos.y > work->pos.y + task->work->scl.x) {
        return FALSE;
    }
    CURRENT_POLAR(work->ang.y, -task->work->scl.y, &x, &z);
    start = work->pos;
    /* @hack redundant: end, the axis's far end, which nothing reads */
    end.x = work->pos.x + x;
    end.y = work->pos.y;
    end.z = work->pos.z + z;
    axis.x = x;
    axis.y = 0.0F;
    axis.z = z;
    length2 = (axis.x * axis.x) + (axis.y * axis.y) + (axis.z * axis.z);
    offset.x = player->pos.x - start.x;
    offset.y = player->pos.y - start.y;
    offset.z = player->pos.z - start.z;
    dot = (offset.x * axis.x) + (offset.y * axis.y) + (offset.z * axis.z);
    if (dot > 0.0F) {
        if (dot < length2) {
            along = dot / length2;
            radial.x = offset.x - (along * axis.x);
            radial.y = offset.y - (along * axis.y);
            radial.z = offset.z - (along * axis.z);
            return (radial.x * radial.x) + (radial.y * radial.y) +
                       (radial.z * radial.z) <
                   task->work->scl.x * task->work->scl.x;
        }
    }
    return FALSE;
}

#endif
