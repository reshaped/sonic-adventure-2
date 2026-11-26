/* the mech bosses' sight: a spinning marker that follows a target. a ring lies
 * on the ground below it. Hot Shot and Flying Dog fire their beams at it */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "player/player.h"
#include "shared/gun_mech/effect_art.h"
#include "shared/mech_boss.h"
#include "shared/mech_boss_sight.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* bits of the work's flags */
#define SIGHT_ARMED 0x100
#define SIGHT_HIDDEN 0x200

/* the light the bosses' stages give player 0 */
#define PLAYER_LIGHT 0
/* an armed sight launches player 0 at (LAUNCH_X, LAUNCH_Y, 0), as
 * player_launch_speed takes it */
#define LAUNCH_X (-1.0F)
#define LAUNCH_Y 1.0F

/* both motions play from frame 0 up to FRAMES, over and over. a following
 * sight plays FRAME_SPEED frames a step at first. that speed gains
 * FRAME_SPEED_GAIN once every GAIN_STEPS + 1 steps. a locked sight plays
 * LOCKED_FRAME_SPEED, and its gains start over */
#define FRAMES 3.0F
#define FRAME_SPEED 0.05F
#define FRAME_SPEED_GAIN 0.01F
#define GAIN_STEPS 60
#define LOCKED_FRAME_SPEED 0.6F
/* a sight whose goal lies more than FAR_DISTANCE away moves at FAR_SHARE of
 * its speed */
#define FAR_DISTANCE 80.0F
#define FAR_SHARE 0.97F
/* each step turns the spin about each axis by a random share of the most
 * sight_turns.spin_most gives. the share runs from SPIN_LEAST up to SPIN_LEAST
 * + SPIN_SPREAD */
#define SPIN_LEAST 0.5F
#define SPIN_SPREAD 0.5F
/* the draw turns the marker about the sight's pos, raised to LOWEST_Y where it
 * lies lower. the marker lies at MARKER_Y along its turned y axis. the ring
 * lies below the sight at RING_Y, just above the ground */
#define LOWEST_Y 7.0F
#define MARKER_Y (-13.0F)
#define RING_Y 0.05F

/* the sight's record, in task->motion */
struct sight {
    Uint8 unk_00[0x18];
    /* each step adds sight_turns.tilt. nothing reads it */
    Angle turn[3];
    Angle spin[3]; /* the marker's spin, as njRotateEx takes it */
    Sint32 lock;
    Uint16 gain_count; /* the steps since frame_speed last gained */
    NJS_POINT3 *target;
    Float speed; /* the most it moves in a step */
    Float frame; /* the frame both motions play */
    Float frame_speed;
    NJS_POINT3 goal; /* where *target lay the step before */
};

/* the marker's tilt, as njRotateEx takes it. spin_most is the most the spin
 * turns about each axis in a step */
static struct {
    Angle tilt[3];
    Angle spin_most[3];
} sight_turns = {{8192, 2048, -12800}, {-576, 32, 1616}};
/* clang-format off */
static struct collision_shape sight_shape = {
    0, SHAPE_SPHERE, PUSH_FIRM(PUSH_PLAYERS),
    SHAPE_GUARD_LEVEL(3) | SHAPE_HURTS_PLAYERS | SHAPE_HURTS_OBJECTS, 0,
    {0.0F, 0.0F, 0.0F}, 12.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */

static void sight_destroy(struct task *task);
static void sight_step(struct task *task);
static void sight_draw(struct task *task);

struct task *mech_boss_sight_create(struct task *parent, NJS_POINT3 *target,
                                    Float speed)
{
    struct task *task;
    struct task_work *work;
    struct sight *sight;

    if (parent == NULL) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, sight_step, parent);
    if (task == NULL) {
        return NULL;
    }
    work = task->work;
    sight = syCalloc(1, sizeof(struct sight));
    if (sight == NULL) {
        task_kill(task);
        return NULL;
    }
    task->motion.p = sight;
    sight->target = target;
    sight->speed = speed;
    /* @hack spelling: the target read back from the record, where target
     * would do */
    work->pos = *sight->target;
    sight->goal = work->pos;
    work->scl.x = work->scl.y = work->scl.z = 1.0F;
    sight->frame_speed = FRAME_SPEED;
    collision_init(task, &sight_shape, 1, COLLISION_OBJECT);
    /* @hack spelling: task->work read again, where work would do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    task->draw = sight_draw;
    task->destructor = sight_destroy;
    return task;
}

static void sight_destroy(struct task *task)
{
    light_select(PLAYER_LIGHT, 0);
}

static void sight_follow(struct task *task)
{
    struct task_work *work = task->work;
    struct sight *sight = task->motion.p;
    NJS_VECTOR move;
    Float length;
    Float speed;

    move.x = sight->goal.x - work->pos.x;
    move.y = sight->goal.y - work->pos.y;
    move.z = sight->goal.z - work->pos.z;
    length = njScalor(&move);
    if (length > sight->speed) {
        /* @hack spelling: the length taken again by njSqrt, where length would
         * do */
        if (njSqrt((move.x * move.x) + (move.y * move.y) + (move.z * move.z)) >
            FAR_DISTANCE) {
            speed = sight->speed * FAR_SHARE;
        } else {
            speed = sight->speed;
        }
        njUnitVector(&move);
        move.x *= speed;
        move.y *= speed;
        move.z *= speed;
    }
    work->pos.x += move.x;
    work->pos.y += move.y;
    work->pos.z += move.z;
    sight->goal = *sight->target;
}

void mech_boss_sight_arm(struct task *task)
{
    struct task_work *work = task->work;

    work->flags |= SIGHT_ARMED;
}

static void sight_step(struct task *task)
{
    struct task_work *work = task->work;
    struct sight *sight = task->motion.p;
    NJS_VECTOR spd;
    struct angles ang;
    Angle *turn;
    Angle *spin;

    if (game_state.time_stopped) {
        return;
    }
    if (sight->lock == 0) {
        sight_follow(task);
        sight->frame += sight->frame_speed;
        if (sight->gain_count++ == GAIN_STEPS) {
            sight->gain_count = 0;
            sight->frame_speed += FRAME_SPEED_GAIN;
        }
    } else {
        sight->frame += LOCKED_FRAME_SPEED;
        sight->gain_count = 0;
        sight->frame_speed = FRAME_SPEED;
    }
    if (sight->frame >= FRAMES) {
        sight->frame -= FRAMES;
    }
    /* @hack spelling: turn and spin, pointers into sight_turns, spin set after
     * the turn's adds, where its fields read in place would do */
    turn = sight_turns.tilt;
    sight->turn[0] += turn[0];
    sight->turn[1] += turn[1];
    sight->turn[2] += turn[2];
    spin = sight_turns.spin_most;
    sight->spin[0] += spin[0] * RANDOM_FROM(SPIN_LEAST, SPIN_SPREAD);
    sight->spin[1] += spin[1] * RANDOM_FROM(SPIN_LEAST, SPIN_SPREAD);
    sight->spin[2] += spin[2] * RANDOM_FROM(SPIN_LEAST, SPIN_SPREAD);
    light_apply_player(0);
    if (collision_next_player(task)) {
        light_select(MECH_BOSS_HURT_LIGHT, 0);
        if (work->flags & SIGHT_ARMED) {
            player_work[0]->flags |= WORK_HURT;
            spd.x = LAUNCH_X;
            spd.y = LAUNCH_Y;
            spd.z = 0.0F;
            /* ang.x and ang.z stay unset, though player_launch_speed reads
             * them. only mech_boss_sight_arm sets SIGHT_ARMED, and no code
             * calls it */
            ang.y = player_work[0]->ang.y + HALF_TURN;
            player_launch_speed(0, &spd, &ang, MECH_BOSS_LAUNCH_LOCK_STEPS);
        }
    } else {
        light_select(PLAYER_LIGHT, 0);
    }
    /* @hack redundant: the test of SIGHT_ARMED before it is cleared */
    if (work->flags & SIGHT_ARMED) {
        work->flags &= ~SIGHT_ARMED;
    }
    collision_register(task);
}

static void sight_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct sight *sight = task->motion.p;
    Float y;

    if (work->flags & SIGHT_HIDDEN) {
        return;
    }
    draw_attr_save();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR |
                     NJD_CONTROL_3D_OFFSET_MATERIAL);
    draw_attr_set(0, NJD_FST_UA);
    njSetTexture(&grobo_eff2_texlist);
    njPushMatrixEx();
    njPushMatrixEx();
    if (work->pos.y < LOWEST_Y) {
        y = LOWEST_Y;
    } else {
        y = work->pos.y;
    }
    njTranslate(NULL, work->pos.x, y, work->pos.z);
    njRotateEx(sight_turns.tilt, 1);
    njRotateEx(sight->spin, 1);
    njScaleV(NULL, &work->scl);
    njTranslate(NULL, 0.0F, MARKER_Y, 0.0F);
    njCnkSimpleDrawMotion(&mech_boss_sight_object, &mech_boss_sight_motion,
                          sight->frame);
    matrix_load_parent();
    njTranslate(NULL, work->pos.x, RING_Y, work->pos.z);
    njCnkSimpleDrawMotion(&mech_boss_sight_ring_object,
                          &mech_boss_sight_ring_motion, sight->frame);
    njPopMatrixEx();
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}

/* @hack type: visible, a Sint8, where a Bool would do */
static void sight_show(struct task *task, Sint8 visible)
{
    struct task_work *work = task->work;

    if (!visible) {
        work->flags |= SIGHT_HIDDEN;
    } else {
        work->flags &= ~SIGHT_HIDDEN;
    }
}

Sint32 *mech_boss_sight_lock(struct task *task)
{
    return &((struct sight *)task->motion.p)->lock;
}

Bool mech_boss_sight_hide_unlocked(struct task *task)
{
    struct sight *sight = task->motion.p;

    if (sight->lock == 1) {
        return FALSE;
    }
    sight_show(task, FALSE);
    return TRUE;
}
