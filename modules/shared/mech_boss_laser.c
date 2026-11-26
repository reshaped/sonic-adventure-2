/* the mech bosses' laser: the spot where a boss's laser meets the ground. the
 * spot runs ahead of the boss and turns toward player 0. Big Foot, Hot Shot and
 * Flying Dog fire it */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "player/player.h"
#include "shared/gun_mech/effect_art.h"
#include "shared/boss_gunfire.h"
#include "shared/mech_boss_laser.h"
#include "shared/spark.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* the ground lies at y = 0, and the spot SPOT_HEIGHT above it */
#define SPOT_HEIGHT 1.0F
/* player 0 takes WORK_HURT within BEAM_RADIUS of the laser's line */
#define BEAM_RADIUS 10.0F
#define SPARKS 5
/* the sparks fly along (1, 0.5, 0) turned about y. the turn is rand() *
 * SPARK_TURN, up to a full turn */
#define SPARK_TURN 2.0F
/* the spot makes a muzzle flash each FLASH_STEPS + 1 steps */
#define FLASH_STEPS 5

/* the spot's own record, in task->motion */
struct laser_aim {
    Uint8 unk_00[0x18];
    struct angles upper;
    struct angles lower;
    Angle step;
    Float reach;
    NJS_POINT3 *player_pos; /* player 0's pos, which nothing reads */
    NJS_POINT3 *boss_pos; /* the parent's work's pos */
    NJS_POINT3 *origin;
    struct angles *boss_ang; /* the parent's work's ang */
};

/* clang-format off */
static struct collision_shape mech_boss_laser_shape[] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(3) | SHAPE_GUARD_LEVEL(3) | SHAPE_HURTS_PLAYERS |
         SHAPE_HURTS_OBJECTS,
     0, {0.0F, 0.0F, 0.0F}, 7.0F, 2.0F, 7.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void mech_boss_laser_destroy(struct task *task);
static Sint8 mech_boss_laser_hits_player(struct task *task);
static void mech_boss_laser_rest(struct task *task);
static void mech_boss_laser_aim(struct task *task);
static void mech_boss_laser_step(struct task *task);
static void mech_boss_laser_draw(struct task *task);

struct task *mech_boss_laser_create(struct task *parent,
                                    struct mech_boss_laser_spec *spec)
{
    struct task *task;
    struct task_work *work;
    struct laser_aim *aim;

    if (parent == NULL) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, mech_boss_laser_step, parent);
    if (task == NULL) {
        return NULL;
    }
    work = task->work;
    aim = syCalloc(1, sizeof(struct laser_aim));
    if (aim == NULL) {
        task_kill(task);
        return NULL;
    }
    task->motion.p = aim;
    aim->player_pos = &player_work[0]->pos;
    aim->boss_pos = &parent->work->pos;
    aim->boss_ang = &parent->work->ang;
    aim->reach = spec->reach;
    aim->step = spec->step;
    aim->upper = spec->upper;
    aim->lower = spec->lower;
    aim->origin = spec->origin;
    work->ang.y = 0;
    work->ang.z = 0;
    work->id = WORK_ID_ATTACK;
    collision_init(task, mech_boss_laser_shape, 1, COLLISION_ENEMY);
    /* @hack spelling: the work reached again through task, where work
     * would do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    task->draw = mech_boss_laser_draw;
    task->destructor = mech_boss_laser_destroy;
    return task;
}

static void mech_boss_laser_destroy(struct task *task)
{
}

/* @hack type: a Sint8 result, where a Bool would do */
static Sint8 mech_boss_laser_hits_player(struct task *task)
{
    struct task_work *work = task->work;
    struct laser_aim *aim = task->motion.p;
    NJS_LINE line;
    NJS_POINT3 nearest;
    /* @hack spelling: distance, a local for the call's result, where the
     * result compared in place would do */
    Float distance;

    if (aim->origin->y < player_work[0]->pos.y) {
        return FALSE;
    }
    line.px = work->pos.x;
    line.py = 0.0F;
    line.pz = work->pos.z;
    line.vx = aim->origin->x - work->pos.x;
    line.vy = aim->origin->y;
    line.vz = aim->origin->z - work->pos.z;
    distance = njDistanceP2L(&player_work[0]->pos, &line, &nearest);
    if (distance < BEAM_RADIUS) {
        return TRUE;
    }
    return FALSE;
}

static void mech_boss_laser_rest(struct task *task)
{
    struct task_work *work = task->work;
    struct laser_aim *aim = task->motion.p;
    Angle heading;
    Float ahead;

    if (work->ang.y) {
        work->ang.y = angle_approach(work->ang.y, 0, aim->step);
    }
    if (work->ang.z) {
        /* @bug logic: steps ang.z from ang.y's value, where the turn above
         * steps ang.y from its own (0x174 of the object's code) */
        work->ang.z = angle_approach(work->ang.y, 0, aim->step);
        ahead = njCos(work->ang.z) * aim->reach;
    } else {
        ahead = aim->reach;
    }
    /* @hack spelling: origin, front, offset and dir declared in a block of
     * their own, where the function's top would do */
    {
        NJS_POINT3 origin = {0.0F, 0.0F, 0.0F};
        NJS_VECTOR front = {-1.0F, 0.0F, 0.0F};
        NJS_VECTOR offset = {0.0F, 0.0F, 0.0F};
        NJS_VECTOR dir;

        offset.x = ahead;
        njPushMatrix(&_nj_unit_matrix_);
        njPushMatrixEx();
        njRotateY(NULL, aim->boss_ang->y);
        njCalcVector(NULL, &front, &dir);
        njRotateY(NULL, QUARTER_TURN);
        heading = atan2f(dir.x, dir.z) * ANGLE_PER_RADIAN;
        njPopMatrixEx();
        njTranslate(NULL, aim->boss_pos->x, 0.0F, aim->boss_pos->z);
        njRotateY(NULL, heading);
        njRotateY(NULL, work->ang.y);
        njTranslateV(NULL, &offset);
        njCalcPoint(NULL, &origin, &work->pos);
        njPopMatrix(1);
    }
    work->pos.y = SPOT_HEIGHT;
}

static void mech_boss_laser_aim(struct task *task)
{
    struct task_work *work = task->work;
    struct laser_aim *aim = task->motion.p;
    Angle heading;
    Float ahead;

    /* @hack spelling: target, goal, turn and distance declared in a block
     * of their own, where the function's top would do */
    {
        NJS_POINT3 target;
        Angle goal;
        Angle turn;
        Float distance;

        target = player_work[0]->pos;
        goal =
            atan2f(target.x - aim->boss_pos->x, target.z - aim->boss_pos->z) *
            ANGLE_PER_RADIAN;
        turn = angle_difference(aim->boss_ang->y + work->ang.y, goal);
        if (aim->step) {
            if (turn > aim->step) {
                turn = aim->step;
            } else if (turn < -aim->step) {
                turn = -aim->step;
            }
        }
        work->ang.y += turn;
        if (work->ang.y > aim->upper.y) {
            work->ang.y = aim->upper.y;
        } else if (work->ang.y < aim->lower.y) {
            work->ang.y = aim->lower.y;
        }
        distance = njSqrt(
            ((target.x - aim->boss_pos->x) * (target.x - aim->boss_pos->x)) +
            ((target.z - aim->boss_pos->z) * (target.z - aim->boss_pos->z)));
        if (distance < aim->reach) {
            goal = atan2f(aim->boss_pos->y, distance) * ANGLE_PER_RADIAN;
            if (goal > aim->upper.z) {
                /* @bug logic: puts the spot the full reach ahead where its
                 * tilt holds at upper.z. the path beside it, and the rest
                 * mode, put it njCos of the tilt times reach ahead. the BRA
                 * at 0x38e of the object's code passes over that njCos call,
                 * at 0x394 */
                work->ang.z = aim->upper.z;
                ahead = aim->reach;
            } else {
                work->ang.z = goal;
                ahead = njCos(work->ang.z) * aim->reach;
            }
        } else {
            work->ang.z = 0;
            ahead = aim->reach;
        }
    }
    /* @hack spelling: origin, front, offset and dir declared in a block of
     * their own, where the function's top would do */
    {
        NJS_POINT3 origin = {0.0F, 0.0F, 0.0F};
        NJS_VECTOR front = {-1.0F, 0.0F, 0.0F};
        NJS_VECTOR offset = {0.0F, 0.0F, 0.0F};
        NJS_VECTOR dir;

        offset.x = ahead;
        njPushMatrix(&_nj_unit_matrix_);
        njPushMatrixEx();
        njRotateY(NULL, aim->boss_ang->y);
        njCalcVector(NULL, &front, &dir);
        heading = atan2f(dir.x, dir.z) * ANGLE_PER_RADIAN;
        njRotateY(NULL, QUARTER_TURN);
        njPopMatrixEx();
        njTranslate(NULL, aim->boss_pos->x, 0.0F, aim->boss_pos->z);
        njRotateY(NULL, heading);
        njRotateY(NULL, work->ang.y);
        njTranslateV(NULL, &offset);
        njCalcPoint(NULL, &origin, &work->pos);
        njPopMatrix(1);
    }
    work->pos.y = SPOT_HEIGHT;
}

void mech_boss_laser_set_mode(struct task *task, Sint8 mode)
{
    struct task_work *work = task->work;

    work->mode = mode;
    work->submode = 0;
}

struct angles *mech_boss_laser_angles(struct task *task)
{
    return &task->work->ang;
}

static void mech_boss_laser_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *object;

    if (game_state.time_stopped) {
        return;
    }
    switch (work->mode) {
    case MECH_BOSS_LASER_REST:
        mech_boss_laser_rest(task);
        break;
    case MECH_BOSS_LASER_AIM:
        mech_boss_laser_aim(task);
        break;
    }
    object = collision_next_object(task);
    if (object != NULL) {
        object->work->collision->flags |= COLLISION_HURT;
    }
    collision_register(task);
    if (mech_boss_laser_hits_player(task)) {
        player_work[0]->flags |= WORK_HURT;
    }
    /* @hack spelling: dir and spd declared in a block of their own, where
     * the function's top would do */
    {
        NJS_VECTOR dir = {1.0F, 0.5F, 0.0F};
        NJS_VECTOR spd;

        njPushMatrix(&_nj_unit_matrix_);
        njRotateY(NULL, rand() * SPARK_TURN);
        njCalcVector(NULL, &dir, &spd);
        njPopMatrix(1);
        spark_throw(SPARKS, &work->pos, &spd);
    }
    if (work->timer++ >= FLASH_STEPS) {
        boss_gunfire_muzzle_create(task, &work->pos, &work->ang);
        work->timer = 0;
    }
}

static void mech_boss_laser_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njCnkSimpleDrawObject(&mech_boss_laser_object);
    njPopMatrixEx();
}
