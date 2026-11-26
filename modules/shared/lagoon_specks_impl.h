/* the specks of stg18/specks.h and shared/lagoon_specks.h: specks_create's
 * view cloud, and the column and side clouds. stg18/specks.c includes this,
 * and stg38/specks.c includes it under CORE_WATER_SPECKS */
#ifndef SHARED_LAGOON_SPECKS_IMPL_H
#define SHARED_LAGOON_SPECKS_IMPL_H
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "player/player.h"
#ifdef CORE_WATER_SPECKS
#include "shared/stage_util.h"
#include "shared/lagoon_specks.h"
#define SPECKS_POLAR_XZ polar_xz
#define SPECKS_POINT_TURN_Y point_turn_y
#else
#include "stg18/lagoon_util.h"
#include "stg18/specks.h"
#define SPECKS_POLAR_XZ lagoon_polar_xz
#define SPECKS_POINT_TURN_Y lagoon_point_turn_y
#endif
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

static NJS_TEXNAME speck_texture = {
#include "shared/lagoon_specks_impl/speck_texture.inc"
};
NJS_TEXLIST specks_texlist = {&speck_texture, 1};

#define SPECKS_LEVEL 1
/* the texture of specks_texlist the sprites take, and njPtclSpriteStart's flag
 * for the translucent list */
#define SPRITE_TEXTURE 0
#define TRANSLUCENT 1

/* a cloud's record, at its task's motion. distances is a side cloud's alone,
 * and null in the others. it holds each speck's place along the run. a place
 * is at most 0, and falls as its speck drifts */
struct specks_cloud {
    NJS_POINT3 *points;
    Sint32 count;
    struct task **owner;
    Float *distances;
};
/* @hack type: motion read as a void * through its address, where
 * (task)->motion.p would do */
#define CLOUD(task) ((struct specks_cloud *)*(void **)&(task)->motion)

/* a sine wave of the view cloud's drift: height times the sine of rate times
 * game_state.steps. rate is in Angle units a step */
struct specks_wave {
    Angle rate;
    Float height;
};
#define WAVES 3

/* Dry Lagoon's column and side clouds hide while player 0 stands above
 * HIDE_HEIGHT in either of two boxes. a box holds each x and z from its MIN up
 * to, but not including, its MAX */
#define HIDE_HEIGHT 100.0F
#define HIDE_1_MIN_X (-80.0F)
#define HIDE_1_MAX_X 270.0F
#define HIDE_1_MIN_Z 860.0F
#define HIDE_1_MAX_Z 1210.0F
#define HIDE_2_MIN_X 140.0F
#define HIDE_2_MAX_X 270.0F
#define HIDE_2_MIN_Z 1210.0F
#define HIDE_2_MAX_Z 1450.0F

static void specks_step(struct task *task);
static void specks_draw(struct task *task);
static void specks_destroy(struct task *task);
static void specks_column_step(struct task *task);
static void specks_cloud_draw(struct task *task);
static void specks_side_step(struct task *task);
#ifndef CORE_WATER_SPECKS
static Bool specks_visible(void);
#endif

/* the view cloud's radius */
static Float specks_radius = 100.0F;
/* the half sizes of a view cloud's sprites, and of a column's or side's */
static Float specks_size = 0.075F;
static Float specks_cloud_size = 0.15F;

static Float dat_lagoon_specks_0 = 13.0F;

/* a column speck rises specks_rise a step, and up to specks_rise_spread more
 * at random. a side speck drifts along its run specks_drift a step, and up to
 * specks_drift_spread more */
static Float specks_rise_spread = 0.6F;
static Float specks_rise = 0.6F;
static Float specks_drift_spread = 1.0F;
static Float specks_drift = 1.0F;
/* a column's or side's run ends parent's scl.y from its start. a speck past
 * the end starts over with odds of specks_restart_odds a step. once
 * specks_restart_margin past it, the speck always starts over */
static Float specks_restart_odds = 0.05F;
static Float specks_restart_margin = 25.0F;
static struct specks_wave specks_waves[WAVES] = {
    {0x170, 0.06F},
    {0xb0, 0.08F},
    {0x15, 0.3F},
};
/* the view cloud's drift starts as the absolute value of the waves' sum, along
 * y. it turns specks_turn_rate Angle units a step about z, then DRIFT_X_TURNS
 * times that about x. a speck moves by the turned drift's x and z, and by
 * DRIFT_Y_SCALE times its y */
#define DRIFT_X_TURNS 2
#define DRIFT_Y_SCALE 0.5F
static Float specks_turn_rate = 8.0F;
static Uint32 specks_color = 0xff809050;
#ifdef CORE_WATER_SPECKS
/* the view cloud of Knuckles's route, or null. its destructor nulls it */
struct task *core_knuckles_specks = NULL;
#endif

/* set *pos halfway between player 0's pos and camera 0's. while
 * camera_views[0] is null, *pos takes player 0's */
static void specks_center(NJS_POINT3 *pos)
{
    if (camera_views[0] != NULL) {
        pos->x = (player_work[0]->pos.x + camera_views[0]->pos.x) * 0.5F;
        pos->y = (player_work[0]->pos.y + camera_views[0]->pos.y) * 0.5F;
        pos->z = (player_work[0]->pos.z + camera_views[0]->pos.z) * 0.5F;
    } else {
        *pos = player_work[0]->pos;
    }
}

static Float specks_wave_sum(struct specks_wave *wave, int count, Uint16 time)
{
    Float sum = 0.0F;
    /* @hack spelling: ticks, an int copy of time, where time read in place
     * would do */
    int ticks = time;

    while (count--) {
        sum += njSin(wave->rate * ticks) * wave->height;
        wave++;
    }
    return sum;
}

#define SHELL_DEPTH 5.0F

/* put pos at a random point within radius of the origin. with on_shell, it
 * lies within SHELL_DEPTH of the ball's edge */
/* @hack type: on_shell a Uint8, where a Bool would do */
static void specks_place(NJS_POINT3 *pos, Float radius, Uint8 on_shell)
{
    if (on_shell) {
        pos->x = 0.0F;
        pos->y = radius - ((Float)rand() * (SHELL_DEPTH * RAND_SCALE));
    } else {
        pos->x = 0.0F;
        pos->y = (Float)rand() * radius * RAND_SCALE;
    }
    pos->z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, RANDOM_TURN());
    njRotateX(NULL, RANDOM_TURN());
    njCalcPoint(NULL, pos, pos);
    njPopMatrix(1);
}

#ifdef CORE_WATER_SPECKS
struct task *specks_create(int player, int count, struct task **owner)
{
    struct task *task;
    struct task_work *work;
    int i;

    if (!player_work[player]) {
        return NULL;
    }
    task = task_create(TASK_WORK, SPECKS_LEVEL, specks_step, "MizugomiExec");
    if (task) {
        work = task->work;
        /* @hack spelling: &task->work->pos, where &work->pos would do */
        specks_center(&task->work->pos);
        work->tick = player;
        task->motion.p = syCalloc(1, sizeof(struct specks_cloud));
        if (!task->motion.p) {
            task_destroy(task);
            return NULL;
        }
        CLOUD(task)->points = syCalloc(count, sizeof(NJS_POINT3));
        if (!CLOUD(task)->points) {
            syFree(task->motion.p);
            task->motion.p = NULL;
            task_destroy(task);
            return NULL;
        }
        task->draw = specks_draw;
        task->step = specks_step;
        task->destructor = specks_destroy;
        CLOUD(task)->count = count;
        for (i = 0; i < count; i++) {
            specks_place(&CLOUD(task)->points[i], specks_radius, FALSE);
        }
        CLOUD(task)->owner = owner;
        if (owner) {
            *owner = task;
        }
        return task;
    }
    return NULL;
}
#else
struct task *specks_create(int player, int count, struct task **owner)
{
    struct task *task;
    struct task_work *work;
    NJS_POINT3 *pos;
    int i;

    /* @hack spelling: a jump to the statement after it, where falling through
     * would do */
    goto check;
check:
    if (player_work[player] != NULL) {
        task =
            task_create(TASK_WORK, SPECKS_LEVEL, specks_step, "MizugomiExec");
        if (task != NULL) {
            /* @hack redundant: work's address taken, which nothing uses */
            (void)&work;
            /* @hack redundant: player's address taken, which nothing uses */
            (void)&player;
            work = task->work;
            specks_center(&work->pos);
            work->tick = player;
            task->motion.p = syCalloc(1, sizeof(struct specks_cloud));
            if (task->motion.p == NULL) {
                task_destroy(task);
                return NULL;
            }
            CLOUD(task)->points = syCalloc(count, sizeof(NJS_POINT3));
            if (CLOUD(task)->points == NULL) {
                syFree(task->motion.p);
                task->motion.p = NULL;
                task_destroy(task);
                return NULL;
            }
            task->draw = specks_draw;
            task->step = specks_step;
            task->destructor = specks_destroy;
            CLOUD(task)->count = count;
            /* @hack spelling: i set to 0 before pos, outside the for, where the
             * for's start would do */
            i = 0;
            pos = CLOUD(task)->points;
            for (; i < count; i++) {
                specks_place(pos, specks_radius, FALSE);
                pos++;
            }
            CLOUD(task)->owner = owner;
            if (owner != NULL) {
                *owner = task;
            }
            return task;
        }
    }
    return NULL;
}

#endif

/* @hack redundant: task declared register, where a plain parameter would do */
static void specks_step(register struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 center;
    NJS_VECTOR rise;
    Float dx;
#ifdef CORE_WATER_SPECKS
    /* @hack redundant: dy declared register, where a plain local would do */
    register Float dy;
#else
    Float dy;
#endif
    /* @hack redundant: dz declared register, where a plain local would do */
    register Float dz;
    Float x;
    Float y;
    Float z;
    Float distance;
#ifdef CORE_WATER_SPECKS
    Angle ang;
#else
    /* @hack redundant: ang declared register, where a plain local would do */
    register Angle ang;
#endif
    struct specks_cloud *cloud;
    /* @hack redundant: pos, count and i declared register, where plain locals
     * would do */
    register NJS_POINT3 *pos;
    register int count;
    register int i;

    if (player_work[work->tick] == NULL) {
        task_destroy(task);
        return;
    }
#ifdef CORE_WATER_SPECKS
    if (game_state.time_stopped) {
        return;
    }
#endif
    specks_center(&center);
    /* @hack spelling: center's part negated and added, where work's less
     * center's would do */
    dx = -center.x + work->pos.x;
    dy = -center.y + work->pos.y;
    dz = -center.z + work->pos.z;
    work->pos = center;
    if (work->mode == MODE_LAGOON_SPECKS_HIDDEN) {
        return;
    }
#ifdef NONMATCHING
    /* the mask keeps the product within int's range. it changes the product
     * by whole turns alone, since specks_turn_rate is a whole number */
    ang = (Angle)((game_state.steps & ANGLE_MASK) * specks_turn_rate);
#else
    ang = (int)(game_state.steps * specks_turn_rate);
#endif
    rise.x = 0.0F;
    rise.y = njAbs(specks_wave_sum(specks_waves, WAVES, game_state.steps));
    rise.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateX(NULL, ang * DRIFT_X_TURNS);
    njRotateZ(NULL, ang);
    njCalcVector(NULL, &rise, &rise);
    njPopMatrix(1);
    dx += rise.x;
    dy += rise.y * DRIFT_Y_SCALE;
#ifdef CORE_WATER_SPECKS
    /* @hack spelling: dz = dz + rise.z, where dz += rise.z would do */
    dz = dz + rise.z;
#else
    dz += rise.z;
#endif
    cloud = CLOUD(task);
    count = cloud->count;
    pos = cloud->points;
    for (i = 0; i < count; i++, pos++) {
        x = pos->x + dx;
#ifdef CORE_WATER_SPECKS
        /* @hack spelling: y set to pos->y, then increased by dy, where
         * y = pos->y + dy would do */
        y = pos->y;
        y += dy;
#else
        y = pos->y + dy;
#endif
        z = pos->z + dz;
        distance = (x * x) + (y * y) + (z * z);
        if (distance > specks_radius * specks_radius) {
            specks_place(pos, specks_radius - ((Float)rand() * RAND_SCALE),
                         TRUE);
        } else {
            pos->x = x;
            pos->y = y;
            pos->z = z;
        }
    }
}

static void specks_draw(struct task *task)
{
    struct task_work *work = task->work;

    if (work->mode == MODE_LAGOON_SPECKS_SHOWN) {
        njFogDisable();
        njPushMatrixEx();
        /* @hack spelling: pos read through task->work, where work would do */
        njTranslateV(NULL, &task->work->pos);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(&specks_texlist);
        njPtclSpriteStart(SPRITE_TEXTURE, specks_color, TRANSLUCENT);
        /* @bug memory: the call reads the point after the cloud's last: the
         * 12 bytes just past the block syCalloc gave for count points. the
         * call is at 0x474 of the object's code for stg18/specks.c, and 0x482
         * for stg38/specks.c */
        njPtclDrawSprite(CLOUD(task)->points, CLOUD(task)->count, specks_size,
                         specks_size);
        njPtclSpriteEnd();
        njPopMatrix(1);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njFogEnable();
    }
}

static void specks_destroy(struct task *task)
{
    if (CLOUD(task)->owner != NULL && *CLOUD(task)->owner == task) {
        *CLOUD(task)->owner = NULL;
    }
    syFree(CLOUD(task)->points);
    syFree(CLOUD(task)->distances);
    syFree(task->motion.p);
    task->motion.p = NULL;
}

/* a column's or side's specks lie within RADIUS_SCALE times its radius,
 * parent's scl.x, of its axis */
#define RADIUS_SCALE 0.8F

/* put pos at a random point of a column of that height and radius */
static void specks_column_place(NJS_POINT3 *pos, Float height, Float radius)
{
    SPECKS_POLAR_XZ(RANDOM_TURN(),
                    (Float)rand() * radius * (RADIUS_SCALE * RAND_SCALE),
                    &pos->x, &pos->z);
    pos->y = (Float)rand() * height * RAND_SCALE;
}

struct task *specks_column_create(struct task *parent, int player, int count,
                                  struct task **owner)
{
    struct task *task;
    struct task_work *work;
    NJS_POINT3 *pos;
    int i;

    if (player_work[player] == NULL) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, specks_column_step, parent);
    if (task != NULL) {
        work = task->work;
        task->motion.p = syCalloc(1, sizeof(struct specks_cloud));
        if (task->motion.p == NULL) {
            task_destroy(task);
            return NULL;
        }
        CLOUD(task)->points = syCalloc(count, sizeof(NJS_POINT3));
        if (CLOUD(task)->points == NULL) {
            syFree(task->motion.p);
            task->motion.p = NULL;
            task_destroy(task);
            return NULL;
        }
        task->draw = specks_cloud_draw;
        task->step = specks_column_step;
        task->destructor = specks_destroy;
        work->pos = parent->work->pos;
        CLOUD(task)->count = count;
        /* @hack spelling: i set to 0 before pos, outside the for, where the
         * for's start would do */
        i = 0;
        pos = CLOUD(task)->points;
        for (; i < count; i++) {
            specks_column_place(pos, parent->work->scl.y, parent->work->scl.x);
            /* @hack spelling: pos stepped as pos = pos + 1, where pos++ would
             * do */
            pos = pos + 1;
        }
        CLOUD(task)->owner = owner;
        if (owner != NULL) {
            *owner = task;
        }
        /* @hack spelling: task->work for work, where work would do */
        task->work->mode = MODE_LAGOON_SPECKS_SHOWN;
        return task;
    }
    return NULL;
}

static void specks_column_step(struct task *task)
{
    struct specks_cloud *cloud;
    /* @hack redundant: speck, i and count declared register, where plain locals
     * would do */
    register NJS_POINT3 *speck;
    register int i;
    register int count;

#ifdef CORE_WATER_SPECKS
    if (game_state.time_stopped) {
        return;
    }
#else
    if (specks_visible()) {
        task->work->mode = MODE_LAGOON_SPECKS_SHOWN;
    } else {
        task->work->mode = MODE_LAGOON_SPECKS_HIDDEN;
        return;
    }
#endif
    cloud = CLOUD(task);
    count = cloud->count;
    speck = cloud->points;
    for (i = 0; i < count; i++, speck++) {
        speck->y +=
            ((Float)rand() * specks_rise_spread * RAND_SCALE) + specks_rise;
        if ((speck->y > task->parent->work->scl.y &&
             (Float)rand() * RAND_SCALE < specks_restart_odds) ||
            speck->y > task->parent->work->scl.y + specks_restart_margin) {
            speck->y = 0.0F;
        }
    }
}

static void specks_cloud_draw(struct task *task)
{
    struct task_work *work = task->work;

    if (work->mode == MODE_LAGOON_SPECKS_SHOWN) {
        njFogDisable();
        njPushMatrixEx();
        /* @hack spelling: pos read through task->work, where work would do */
        njTranslateV(NULL, &task->work->pos);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(&specks_texlist);
        njPtclSpriteStart(SPRITE_TEXTURE, specks_color, TRANSLUCENT);
        /* @bug memory: the call reads the point after the cloud's last, as
         * specks_draw's does. the call is at 0x836 of the object's code for
         * stg18/specks.c, and 0x828 for stg38/specks.c */
        njPtclDrawSprite(CLOUD(task)->points, CLOUD(task)->count,
                         specks_cloud_size, specks_cloud_size);
        njPtclSpriteEnd();
#ifdef CORE_WATER_SPECKS
        njPopMatrixEx();
#else
        njPopMatrix(1);
#endif
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njFogEnable();
    }
}

/* put speck at a random point of a side cloud's run of that length and
 * radius. the run leads along -z turned by ang. *distance takes the speck's
 * place along it, from -length up to 0 */
static void specks_side_place(NJS_POINT3 *speck, Float length, Float radius,
                              Angle ang, Float *distance)
{
    Angle turn;
    Float x;
    Float z;
    Float px;

    turn = RANDOM_TURN();
    speck->x =
        njCos(turn) * radius * (Float)rand() * (RADIUS_SCALE * RAND_SCALE);
    px = speck->x;
    speck->y =
        njSin(turn) * radius * (Float)rand() * (RADIUS_SCALE * RAND_SCALE);
    SPECKS_POINT_TURN_Y(px, 0.0F, ang, speck);
    *distance = -(length * (Float)rand() * RAND_SCALE);
    SPECKS_POLAR_XZ(ang, *distance, &x, &z);
    speck->x += x;
    speck->z += z;
}

struct task *specks_side_create(struct task *parent, int player, int count,
                                struct task **owner)
{
    struct task *task;
    struct task_work *work;
    NJS_POINT3 *speck;
    Float *distance;
    int i;

    if (player_work[player] != NULL) {
        task = task_create_child(TASK_WORK, specks_side_step, parent);
        if (task != NULL) {
            work = task->work;
            task->motion.p = syCalloc(1, sizeof(struct specks_cloud));
            if (task->motion.p == NULL) {
                task_destroy(task);
                return NULL;
            }
            CLOUD(task)->points = syCalloc(count, sizeof(NJS_POINT3));
            if (CLOUD(task)->points == NULL) {
                syFree(task->motion.p);
                task->motion.p = NULL;
                task_destroy(task);
                return NULL;
            }
            CLOUD(task)->distances = syCalloc(count, sizeof(Float));
            if (CLOUD(task)->distances == NULL) {
                syFree(CLOUD(task)->points);
                syFree(task->motion.p);
                task->motion.p = NULL;
                task_destroy(task);
                return NULL;
            }
            task->draw = specks_cloud_draw;
            task->step = specks_side_step;
            task->destructor = specks_destroy;
            work->pos = parent->work->pos;
#ifdef CORE_WATER_SPECKS
            work->ang.x = work->ang.z = 0;
#endif
            work->ang.y = parent->work->ang.y;
            CLOUD(task)->count = count;
            /* @hack spelling: speck and distance stepped as speck = speck + 1
             * and distance = distance + 1, where speck++ and distance++ would
             * do */
            for (i = 0, speck = CLOUD(task)->points,
                distance = CLOUD(task)->distances;
                 i < count; i++, speck = speck + 1, distance = distance + 1) {
                specks_side_place(speck, parent->work->scl.y,
                                  parent->work->scl.x, parent->work->ang.y,
                                  distance);
            }
            CLOUD(task)->owner = owner;
            if (owner != NULL) {
                *owner = task;
            }
            /* @hack spelling: task->work for work, where work would do */
            task->work->mode = MODE_LAGOON_SPECKS_SHOWN;
            return task;
        }
    }
    return NULL;
}

static void specks_side_step(struct task *task)
{
    struct task_work *work = task->work;
    struct specks_cloud *cloud;
    /* @hack redundant: speck, i and count declared register, where plain locals
     * would do */
    register NJS_POINT3 *speck;
    register int i;
    register int count;
    Float step;
    Float x;
    Float z;

#ifdef CORE_WATER_SPECKS
    if (game_state.time_stopped) {
        return;
    }
#else
    if (specks_visible()) {
        task->work->mode = MODE_LAGOON_SPECKS_SHOWN;
    } else {
        task->work->mode = MODE_LAGOON_SPECKS_HIDDEN;
        return;
    }
#endif
    cloud = CLOUD(task);
    count = cloud->count;
    speck = cloud->points;
    for (i = 0; i < count; i++, speck++) {
        step =
            -((Float)rand() * specks_drift_spread * RAND_SCALE) - specks_drift;
        CLOUD(task)->distances[i] += step;
        SPECKS_POLAR_XZ(work->ang.y, step, &x, &z);
        speck->x += x;
        speck->z += z;
        if ((njAbs(CLOUD(task)->distances[i]) > task->parent->work->scl.y &&
             (Float)rand() * RAND_SCALE < specks_restart_odds) ||
            njAbs(CLOUD(task)->distances[i]) >
                task->parent->work->scl.y + specks_restart_margin) {
            SPECKS_POLAR_XZ(work->ang.y, -CLOUD(task)->distances[i], &x, &z);
            speck->x += x;
            speck->z += z;
            CLOUD(task)->distances[i] = 0.0F;
        }
    }
}

#ifndef CORE_WATER_SPECKS
static Bool specks_visible(void)
{
    NJS_POINT3 pos;

    pos = player_work[0]->pos;
    /* @hack type: camera 0's land_block_mask read as
     * game_state_land_block_mask_0, where game_state.land_block_mask[0] would
     * do */
    return (game_state_land_block_mask_0 & LOW_BLOCKS) &&
           camera_views[camera_current_index]->pos.y < lagoon_water_level() &&
           !(HIDE_1_MIN_X <= pos.x && pos.x < HIDE_1_MAX_X &&
             HIDE_1_MIN_Z <= pos.z && pos.z < HIDE_1_MAX_Z &&
             HIDE_HEIGHT < pos.y) &&
           !(HIDE_2_MIN_X <= pos.x && pos.x < HIDE_2_MAX_X &&
             HIDE_2_MIN_Z <= pos.z && pos.z < HIDE_2_MAX_Z &&
             HIDE_HEIGHT < pos.y);
}
#endif

#endif
