/* the water's specks: a cloud of specks that drift in the water about the
 * camera */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "player/player.h"
#include "shared/water_specks.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the task level of a cloud */
#define SPECKS_LEVEL 1

struct water_specks {
    NJS_POINT3 *points; /* count specks, each a point from the cloud's center */
    Sint32 count;
    struct task **handle; /* water_specks_create's handle, or null */
};

#define SPECKS(task) ((struct water_specks *)(task)->motion.p)
/* water_specks_create's player, kept in the work's tick. the task ends once
 * that player's slot is empty */
#define SLOT(work) ((work)->tick)
/* any mode but MODE_SPECKS_SHOWN hides the specks */
#define HIDDEN(work) ((work)->mode)

/* one wave of the water's speed: amplitude times the sine of frequency times
 * the game's steps */
struct water_wave {
    Sint32 frequency; /* in angle units a step */
    Float amplitude;
};
#define WAVES 3

/* a speck placed near the edge lies no more than EDGE_DEPTH inside the radius
 * water_specks_place is given. a speck that leaves the cloud's sphere comes
 * back placed with the cloud's radius less up to RETURN_INSET. it then lies up
 * to EDGE_DEPTH + RETURN_INSET inside the cloud's edge */
#define EDGE_DEPTH 5.0F
#define RETURN_INSET 1.0F
/* the flow moves the specks by its x and z, and by FLOW_LIFT of its y */
#define FLOW_LIFT 0.5F
#define X_TURNS 2
/* the texture of water_specks_texlist the sprites take, and
 * njPtclSpriteStart's flag for the translucent list */
#define SPRITE_TEXTURE 0
#define TRANSLUCENT 1

#include "shared/water_specks/text.inc"

static NJS_TEXNAME water_specks_texname[1] = {
#include "shared/water_specks/water_specks_texname.inc"
};
NJS_TEXLIST water_specks_texlist = {water_specks_texname, 1};
/* the specks lie within water_specks_radius of the center */
static Float water_specks_radius = 100.0F;
/* half the side of a speck's sprite, at the screen's distance */
static Float water_specks_size = 0.075F;
static Float dat_water_specks_0 = 13.0F;
/* the water's speed is the absolute value of the three waves' sum */
static struct water_wave water_specks_waves[WAVES] = {
    {0x170, 0.06F},
    {0xb0, 0.08F},
    {0x15, 0.3F},
};
/* the water flows along y turned about z, then about x by X_TURNS times as
 * much. the turn about z is water_specks_turn_rate angle units for each of the
 * game's steps */
static Float water_specks_turn_rate = 8.0F;
static Uint32 water_specks_color = 0xff809050;

static void water_specks_step(struct task *task);
static void water_specks_draw(struct task *task);
static void water_specks_destroy(struct task *task);

/* set *pos to the point halfway from camera 0 to player 0. while camera 0
 * isn't in use, set it to player 0's pos */
static void water_specks_center(NJS_POINT3 *pos)
{
    if (camera_views[0] != NULL) {
        pos->x = (player_work[0]->pos.x + camera_views[0]->pos.x) * 0.5F;
        pos->y = (player_work[0]->pos.y + camera_views[0]->pos.y) * 0.5F;
        pos->z = (player_work[0]->pos.z + camera_views[0]->pos.z) * 0.5F;
    } else {
        *pos = player_work[0]->pos;
    }
}

/* the sum of count waves at step */
static Float water_specks_wave(struct water_wave *wave, int count, Uint16 step)
{
    Float sum = 0.0F;
    /* @hack spelling: at, a copy of step, where step would do */
    Sint32 at = step;

    while (count--) {
        sum += njSin(wave->frequency * at) * wave->amplitude;
        wave++;
    }
    return sum;
}

/* set *pos to a random point within radius of the cloud's center. where
 * near_edge isn't 0, the point lies no more than EDGE_DEPTH inside radius */
/* @hack type: near_edge a Uint8, where a Bool would do */
static void water_specks_place(NJS_POINT3 *pos, Float radius, Uint8 near_edge)
{
    if (near_edge) {
        pos->x = 0.0F;
        pos->y = radius - ((Float)rand() * (EDGE_DEPTH * RAND_SCALE));
    } else {
        pos->x = 0.0F;
        pos->y = (Float)rand() * radius * RAND_SCALE;
    }
    pos->z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, (Float)rand() * (FULL_TURN * RAND_SCALE));
    njRotateX(NULL, (Float)rand() * (FULL_TURN * RAND_SCALE));
    njCalcPoint(NULL, pos, pos);
    njPopMatrix(1);
}

struct task *water_specks_create(int player, int count, struct task **handle)
{
    struct task *task;
    struct task_work *work;
    NJS_POINT3 *point;
    int i;

    /* @hack redundant: a jump to the statement that follows it */
    goto check_player;
check_player:
    /* @hack spelling: the rest nested in the tests of the player and the
     * task, where a return after each failed test would do */
    if (player_work[player] != NULL) {
        task = task_create(TASK_WORK, SPECKS_LEVEL, water_specks_step,
                           "MizugomiExec");
        if (task != NULL) {
            /* @hack redundant: work's address taken and dropped */
            (void)&work;
            /* @hack redundant: player's address taken and dropped */
            (void)&player;
            work = task->work;
            water_specks_center(&work->pos);
            SLOT(work) = player;
            task->motion.p = syCalloc(1, sizeof(struct water_specks));
            if (task->motion.p == NULL) {
                task_destroy(task);
                return NULL;
            }
            SPECKS(task)->points = syCalloc(count, sizeof(NJS_POINT3));
            if (SPECKS(task)->points == NULL) {
                syFree(SPECKS(task)->points);
                syFree(task->motion.p);
                task->motion.p = NULL;
                task_destroy(task);
                return NULL;
            }
            task->draw = water_specks_draw;
            task->step = water_specks_step;
            task->destructor = water_specks_destroy;
            SPECKS(task)->count = count;
            /* @hack spelling: i set ahead of point, where the for's own
             * start would do */
            i = 0;
            point = SPECKS(task)->points;
            for (; i < count; i++) {
                water_specks_place(point, water_specks_radius, FALSE);
                point++;
            }
            SPECKS(task)->handle = handle;
            if (handle != NULL) {
                *handle = task;
            }
            return task;
        }
    }
    return NULL;
}

/* @hack redundant: task declared register, where a plain parameter would do */
static void water_specks_step(register struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 center;
    NJS_VECTOR flow;
    Float dx;
    Float dy;
    /* @hack redundant: dz, turn, point, count and i declared register, where
     * plain locals would do */
    register Float dz;
    register Angle turn;
    struct water_specks *specks;
    register NJS_POINT3 *point;
    register int count;
    register int i;
    Float x;
    Float y;
    Float z;
    /* @hack spelling: distance, a local for the sum its test could compute */
    Float distance;

    if (player_work[SLOT(work)] == NULL) {
        task_destroy(task);
        return;
    }
    water_specks_center(&center);
    dx = -center.x + work->pos.x;
    dy = -center.y + work->pos.y;
    dz = -center.z + work->pos.z;
    work->pos = center;
    /* @hack spelling: the work read again through task, where work would
     * do */
    if (HIDDEN(task->work)) {
        return;
    }
    turn = game_state.steps * water_specks_turn_rate;
    flow.x = 0.0F;
    flow.y = _builtin_fabsf(
        water_specks_wave(water_specks_waves, WAVES, game_state.steps));
    flow.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateX(NULL, turn * X_TURNS);
    njRotateZ(NULL, turn);
    njCalcVector(NULL, &flow, &flow);
    njPopMatrix(1);
    dx += flow.x;
    dy += flow.y * FLOW_LIFT;
    dz += flow.z;
    specks = task->motion.p;
    count = specks->count;
    point = specks->points;
    for (i = 0; i < count; i++, point++) {
        x = point->x + dx;
        y = point->y + dy;
        z = point->z + dz;
        distance = (x * x) + (y * y) + (z * z);
        if (distance > water_specks_radius * water_specks_radius) {
            water_specks_place(
                point,
                water_specks_radius -
                    ((Float)rand() * (RETURN_INSET * RAND_SCALE)),
                TRUE);
        } else {
            point->x = x;
            point->y = y;
            point->z = z;
        }
    }
}

static void water_specks_draw(struct task *task)
{
    if (HIDDEN(task->work)) {
        return;
    }
    njFogDisable();
    njPushMatrixEx();
    njTranslateV(NULL, &task->work->pos);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(&water_specks_texlist);
    njPtclSpriteStart(SPRITE_TEXTURE, water_specks_color, TRANSLUCENT);
    njPtclDrawSprite(SPECKS(task)->points, SPECKS(task)->count,
                     water_specks_size, water_specks_size);
    njPtclSpriteEnd();
    njPopMatrix(1);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static void water_specks_destroy(struct task *task)
{
    struct water_specks *specks;
    struct task **handle;

    /* @hack redundant: specks's address taken and dropped */
    (void)&specks;
    /* @hack spelling: the assignment to specks inside the read of its
     * handle, where a statement of its own would do */
    handle = (specks = task->motion.p)->handle;
    if (handle != NULL && *handle == task) {
        *SPECKS(task)->handle = NULL;
    }
    syFree(SPECKS(task)->points);
    syFree(task->motion.p);
    task->motion.p = NULL;
}
