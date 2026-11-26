/* the missile shutter LSHELTH of stg11/missile_shutter.h, whose door slides
 * along x. stg11/missile_shutter_v.c is its twin */
#include "stg11/missile_shutter_h/text.inc"
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/flash.h"
#include "shared/missile.h"
#include "sound/sound.h"
#include "stg11/missile_shutter.h"
#include "stg11/shutter.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define SHUTTER(task) ((struct missile_shutter *)(task)->motion.p)
/* in the work's submode: TRUE once a missile's landing takes effect */
#define HIT(work) ((work)->submode)
/* the steps until the landing takes effect, in the work's tick. each step
 * that finds hit set measures the delay anew, and then counts it down. so
 * while hit stays set, only a delay measured as 1 reaches 0 */
#define HIT_DELAY(work) ((work)->tick)
#define ID(work) ((Uint8)(work)->ang.x)

/* the landing's delay, in steps, is the missile's distance times
 * HIT_DELAY_SCALE plus under HIT_DELAY_RANGE at random. it is at most
 * HIT_DELAY_MAX */
#define HIT_DELAY_SCALE 0.1F
#define HIT_DELAY_RANGE 10.0F
#define HIT_DELAY_MAX 254.0F

/* the common sounds the burst plays */
#define BURST_SOUND_1 0x1022
#define BURST_SOUND_2 0x1011
#define BURST_VOLUME 60
#define BURST_POINTS 20

/* the shutting door's dust lies about the point SHUT_DUST_Y above the
 * shutter's pos */
#define SHUT_DUST_Y 25.0F
/* the door's flight as it bursts: its turn a step about x, and its speed */
#define BURST_SPIN_X (-9)
#define BURST_SPEED_Y 1.5F
#define BURST_SPEED_Z (-3.0F)
#define BURST_FLAGS                                                            \
    (DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_NO_SPIN_Y |                  \
     DEBRIS_NO_SPIN_Z | DEBRIS_BIG_PARTICLES | DEBRIS_THROUGH_FLOOR)

#define MISSILE_SHUTTER_SHAPES 5
#define DOOR_SHAPE 1

static NJS_TEXLIST missile_shutter_h_door_texlist;
static NJS_CNK_OBJECT missile_shutter_h_door_object;

static struct collision_shape missile_shutter_h_shapes[MISSILE_SHUTTER_SHAPES];

static void missile_shutter_h_destroy(struct task *task);
static void missile_shutter_h_step(struct task *task);
static void missile_shutter_h_draw(struct task *task);

void missile_shutter_h_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct missile_shutter));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = missile_shutter_h_draw;
    task->step = missile_shutter_h_step;
    task->destructor = missile_shutter_h_destroy;
    SHUTTER(task)->open = SHUTTER_OPEN_FULL;
    missile_shutters_shutting = FALSE;
    collision_init(task, missile_shutter_h_shapes, MISSILE_SHUTTER_SHAPES,
                   COLLISION_OBJECT);
    if (task->placement != NULL) {
        work->mode = placement_get_state(task);
        switch (work->mode) {
        case MODE_SHUTTING:
            SHUTTER(task)->open = SHUTTER_OPEN_HALF;
            break;
        case MODE_SHUT:
        case MODE_BURST:
            SHUTTER(task)->open = 0.0F;
            break;
        }
    }
    if (work->mode == MODE_SHUT) {
        missile_target_add(ID(work), &SHUTTER(task)->hit);
    } else {
        missile_target_add(ID(work) | TARGET_UNSHUT, &SHUTTER(task)->hit);
    }
}

static void missile_shutter_h_destroy(struct task *task)
{
    struct task_work *work = task->work;

    missile_target_remove(&SHUTTER(task)->hit);
    if (task->placement != NULL) {
        placement_set_state(task, work->mode);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void missile_shutter_h_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a loop that never runs, which sets HIT */
    while (0) {
        HIT(work) = TRUE;
    }
    /* @hack spelling: a goto to the next statement, where the statement
     * alone would do */
    goto check;
check:
    if (SHUTTER(task)->hit != NULL) {
        Float delay;

        delay = njDistanceP2P(SHUTTER(task)->hit, &work->pos) * HIT_DELAY_SCALE;
        delay += (Float)rand() * (HIT_DELAY_RANGE * RAND_SCALE);
        if (delay > HIT_DELAY_MAX) {
            delay = HIT_DELAY_MAX;
        }
        HIT_DELAY(work) = delay;
    }
    if (HIT_DELAY(work) != 0) {
        HIT_DELAY(work)--;
        if (HIT_DELAY(work) == 0) {
            HIT(work) = TRUE;
        }
    }
    switch (work->mode) {
    case MODE_OPEN:
        /* @hack redundant: a loop that never runs, which clears the timer */
        while (0) {
            work->timer = 0;
        }
        if (HIT(work) ||
            player_near(&work->pos, work->scl.z + SHUTTER_NEAR_MARGIN) ||
            missile_shutters_shutting) {
            missile_shutters_shutting = TRUE;
            work->mode = MODE_HALF;
            work->timer = 0;
            if (!HIT(work)) {
                sound_play(SHUTTER_START_SOUND, NULL, SHUTTER_SOUND_PRIORITY,
                           SHUTTER_SOUND_VOLUME);
            }
        }
        break;
    case MODE_HALF:
        SHUTTER(task)->open -= work->scl.x + SHUTTER_MIN_SPEED;
        if (HIT(work)) {
            SHUTTER(task)->open = 0.0F;
        }
        if (!(SHUTTER(task)->open > SHUTTER_OPEN_HALF)) {
            SHUTTER(task)->open = SHUTTER_OPEN_HALF;
            work->timer++;
            if (work->timer > SHUTTER_WAIT_STEPS(work)) {
                sound_play(SHUTTER_HALF_SOUND, NULL, SHUTTER_SOUND_PRIORITY,
                           SHUTTER_SOUND_VOLUME);
                work->mode = MODE_SHUTTING;
            }
        }
        break;
    case MODE_SHUTTING: {
        NJS_POINT3 edge;
        struct angles ang;

        sound_play_for(SHUTTER_SHUTTING_SOUND, work, SHUTTER_SHUTTING_PRIORITY,
                       SHUTTER_SOUND_VOLUME, SHUTTER_SHUTTING_UPDATES);
        SHUTTER(task)->open -= work->scl.y + SHUTTER_MIN_SPEED;
        if (HIT(work)) {
            SHUTTER(task)->open = 0.0F;
        }
        if (!(SHUTTER(task)->open > 0.0F)) {
            SHUTTER(task)->open = 0.0F;
            work->mode = MODE_SHUT;
            missile_target_remove(&SHUTTER(task)->hit);
            missile_target_add(ID(work), &SHUTTER(task)->hit);
            work->timer = 0;
            /* @hack spelling: the edge's x picked by a test of SHUTTER_REVERSED
             * plus SHUT_DUST_Y, which always holds, where -open would do */
            edge.x = (SHUTTER_REVERSED(work) + SHUT_DUST_Y)
                         ? -SHUTTER(task)->open
                         : SHUTTER(task)->open;
            edge.y = SHUT_DUST_Y;
            edge.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &edge, &edge);
            njPopMatrixEx();
            /* dust_short_rect ignores ang.x, so this quarter turn tilts
             * nothing */
            ang.x = QUARTER_TURN;
            ang.y = work->ang.y;
            ang.z = 0;
            dust_short_rect(&edge, SHUTTER_DUST_HALF_X, SHUTTER_DUST_HALF_Z,
                            &ang, SHUTTER_DUST_SCALE, SHUTTER_DUST_DENSITY,
                            SHUTTER_DUST_SPEED);
            SHUTTER(task)->speed += SHUTTER_SHUT_KICK;
        }
        break;
    }
    case MODE_SHUT: {
        struct angles ang;
        struct debris_spin spin;
        NJS_VECTOR spd;

        SHUTTER(task)->open += SHUTTER(task)->speed;
        SHUTTER(task)->speed =
            -(SHUTTER(task)->open * SHUTTER_BOUNCE_STIFFNESS) +
            (SHUTTER(task)->speed * SHUTTER_BOUNCE_DAMPING);
        if (SHUTTER(task)->open < 0.0F) {
            SHUTTER(task)->open = 0.0F;
            SHUTTER(task)->speed = njAbs(SHUTTER(task)->speed);
        }
        if (HIT(work)) {
            ang.y = work->ang.y;
            ang.x = 0;
            ang.z = 0;
            spin.x = BURST_SPIN_X;
            spin.y = 0;
            spin.z = 0;
            spd.x = 0.0F;
            spd.y = BURST_SPEED_Y;
            spd.z = BURST_SPEED_Z;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, work->ang.y);
            njCalcVector(NULL, &spd, &spd);
            njPopMatrixEx();
            /* @bug logic: the burst throws LSHELTV's door (0x8c505620),
             * where the draw draws LSHELTH's own (0x8c5057ba) */
            debris_launch_spin(&work->pos, &ang, work->pos.y, &spd, &spin,
                               missile_shutter_v_door_object->model,
                               &missile_shutter_v_door_texlist, BURST_FLAGS);
            score_add(BURST_POINTS);
            work->mode = MODE_BURST;
            sound_play_at(BURST_SOUND_1, NULL, SHUTTER_SOUND_PRIORITY,
                          BURST_VOLUME, &work->pos);
            sound_play_at(BURST_SOUND_2, NULL, SHUTTER_SOUND_PRIORITY,
                          BURST_VOLUME, &work->pos);
        }
        break;
    }
    case MODE_BURST:
        break;
    default:
        break;
    }
    if (work->collision != NULL) {
        /* @hack spelling: the door's x picked by a test of its shape's
         * center.x plus SHUTTER_REVERSED, where center.x plus a pick by
         * SHUTTER_REVERSED would do. center.x is 0, so the two agree */
        work->collision->shapes[DOOR_SHAPE].center.x =
            (missile_shutter_h_shapes[DOOR_SHAPE].center.x +
             SHUTTER_REVERSED(work))
                ? SHUTTER(task)->open
                : -SHUTTER(task)->open;
        if (work->mode != MODE_BURST) {
            collision_register(task);
        }
    }
}

static void missile_shutter_h_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)MISSILE_SHUTTER_EDIT_MODE_REV_ID;
        (void)MISSILE_SHUTTER_EDIT_MODE_DOORNUM;
        (void)MISSILE_SHUTTER_EDIT_ANGLE_Y;
        (void)MISSILE_SHUTTER_EDIT_WAITTIME;
        (void)MISSILE_SHUTTER_EDIT_SHUT_SPEED1;
        (void)MISSILE_SHUTTER_EDIT_SHUT_SPEED2;
        (void)MISSILE_SHUTTER_EDIT_START_RANGE;
    }
    njPushMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    object = &shutter_frame_object;
    njSetTexture(&shutter_frame_texlist);
    njCnkEasyDrawObject(object);
    njTranslate(NULL,
                SHUTTER_REVERSED(work) ? SHUTTER(task)->open
                                       : -SHUTTER(task)->open,
                0.0F, 0.0F);
    njSetTexture(&missile_shutter_h_door_texlist);
    if (work->mode != MODE_BURST) {
        object = &missile_shutter_h_door_object;
        njCnkEasyDrawModel(object->model);
    }
    njPopMatrixEx();
}

#include "stg11/fixtures/missile_shutter_names.h"

static NJS_TEXLIST missile_shutter_h_door_texlist = {
#include "stg11/missile_shutter_h/missile_shutter_h_door_texlist.inc"
};

static Sint16 missile_shutter_h_door_polygons[741] = {
#include "stg11/missile_shutter_h/missile_shutter_h_door_polygons.inc"
};

static Sint32 missile_shutter_h_door_vertices[627] = {
#include "stg11/missile_shutter_h/missile_shutter_h_door_vertices.inc"
};

static NJS_CNK_MODEL missile_shutter_h_door_model = {
#include "stg11/missile_shutter_h/missile_shutter_h_door_model.inc"
};

static NJS_CNK_OBJECT missile_shutter_h_door_object = {
#include "stg11/missile_shutter_h/missile_shutter_h_door_object.inc"
};

NJS_TEXLIST *missile_shutter_h_texlists[5] = {
#include "stg11/missile_shutter_h/missile_shutter_h_texlists.inc"
};
Bool missile_shutters_shutting = FALSE;
/* the sill, the door, the two posts and the lintel. the step moves the
 * record's copy of the door's box with the door */
/* clang-format off */
static struct collision_shape
    missile_shutter_h_shapes[MISSILE_SHUTTER_SHAPES] = {
    {SHAPE_ID_NO_STRIKE, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000, {0.0F, -7.0F, 0.0F}, 35.0F, 5.5F, 10.0F, 0, {0, 0, 0}},
    {SHAPE_ID_NO_STRIKE, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000, {0.0F, 25.0F, 0.0F}, 35.0F, 30.0F, 5.0F, 0, {0, 0, 0}},
    {SHAPE_ID_NO_STRIKE, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000, {37.0F, 25.0F, 0.0F}, 5.0F, 30.0F, 10.0F, 0, {0, 0, 0}},
    {SHAPE_ID_NO_STRIKE, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000, {-37.0F, 25.0F, 0.0F}, 5.0F, 30.0F, 10.0F, 0, {0, 0, 0}},
    {SHAPE_ID_NO_STRIKE, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000, {0.0F, 57.0F, 0.0F}, 35.0F, 7.5F, 10.0F, 0, {0, 0, 0}},
};
/* clang-format on */
