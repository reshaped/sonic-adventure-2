/* CAM's init and callbacks, and its head's.
 * shared/security_camera.c compiles them for Prison Lane, and
 * stg11/security_camera.c for Iron Gate. the includer defines the camera's art,
 * security_camera_most_turn, and the shapes of the camera and its head */
#ifndef SHARED_SECURITY_CAMERA_TASK_H
#define SHARED_SECURITY_CAMERA_TASK_H
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/flash.h"
#include "shared/security_camera.h"
#include "system/angle.h"
#include "system/task.h"

/* a camera's modes, in its work's mode */
enum security_camera_mode { MODE_WATCHING, MODE_BROKEN };

/* the head's heading about y, which the camera keeps in its work's timer */
#define HEADING(work) ((work)->timer)
/* the head turns toward a player within WATCH_RANGE of the camera. where both
 * are, it turns toward slot 0's */
#define WATCH_RANGE 300.0F
/* a hurt head sets bit HEAD_SHIFT of its camera's submode. the shift is the
 * head's tick, which task_create_child leaves 0 */
#define HEAD_SHIFT(head_work) ((head_work)->tick)

/* the knocked-off head falls as debris. it starts FALL_DROP below its place, at
 * a speed of FALL_RISE up. it ends in a burst on the floor below */
#define FALL_DROP 10.0F
#define FALL_RISE 0.2F
#define FALL_FLAGS (DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_VANISH)
/* the points the camera scores, and object_sound_play's kind, as its head
 * falls */
#define BREAK_POINTS 20
#define BREAK_SOUND_KIND 1
/* the burst a hurt head leaves at its place: a puff of dust and a flash */
#define HURT_DUST_SCALE 6.0F
#define HURT_FLASH_SCALE 20.0F
#define HURT_SPARKS 3
#define HURT_FLASH_DEPTH_BIAS 30.0F

/* the state of the camera's placement once it has lost its head. a camera whose
 * placement's state isn't 0 starts without its head */
#define STATE_BROKEN 1
#define SECURITY_CAMERA_SHAPES 1
#define HEAD_SHAPES 1

/* the statement the camera's step runs as the head falls. Prison Lane's
 * returns, so that step skips collision_register. Iron Gate's source defines
 * it as ((void)0), so its camera registers its record on that step too */
#ifndef SECURITY_CAMERA_BROKEN_END
#define SECURITY_CAMERA_BROKEN_END return
#endif

static void security_camera_destroy(struct task *task);
static void security_camera_step(struct task *task);
static void security_camera_draw(struct task *task);
static void security_camera_head_init(struct task *task);
static void security_camera_head_destroy(struct task *task);
static void security_camera_head_step(struct task *task);

void security_camera_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *head;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = security_camera_draw;
    task->step = security_camera_step;
    task->destructor = security_camera_destroy;
    /* @bug logic: a camera that starts without its head still makes the head's
     * child. an attack can hurt that unseen head, which then bursts in dust
     * and sparks where nothing is drawn. the JSR is at 0x26 of the object's
     * code. Prison Lane's tank_init makes no child for a valve knocked off
     * before (0x8c502d94) */
    head = task_create_child(TASK_WORK, security_camera_head_init, task);
    if (head != NULL) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, (NJS_POINT3 *)security_camera_object->child->pos,
                    &head->work->pos);
        njPopMatrix(1);
    }
    collision_init(task, security_camera_shapes, SECURITY_CAMERA_SHAPES,
                   COLLISION_OBJECT);
    if (task->placement != NULL && placement_get_state(task)) {
        work->mode = MODE_BROKEN;
    }
    HEADING(work) = work->ang.y;
}

static void security_camera_destroy(struct task *task)
{
    struct task_work *work = task->work;

    task_destroy_children(task);
    if (task->placement != NULL && work->mode == MODE_BROKEN) {
        placement_set_state(task, STATE_BROKEN);
    }
}

static void security_camera_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 point;
    struct angles ang;
    struct angles floor_ang;
    NJS_VECTOR spd;
    /* @hack spelling: to, a pointer to point, where point written and read in
     * place would do */
    NJS_POINT3 *to;
    int player;
    int facing;
    Sint16 turn;
    Float floor;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: a switch on the mode with an empty case, where a test
     * of the mode would do */
    switch (work->mode) {
    case MODE_BROKEN:
        break;
    default:
        player = player_near(&work->pos, WATCH_RANGE);
        if (player) {
            /* @hack spelling: player stepped down by an assignment, where
             * player-- would do */
            player = player - 1;
            to = &point;
            to->x = player_work[player]->pos.x - work->pos.x;
            to->y = player_work[player]->pos.y - work->pos.y;
            to->z = player_work[player]->pos.z - work->pos.z;
            /* @hack spelling: facing, a local for the heading toward the
             * player, where its expression inside turn's would do */
            facing = atan2f(to->x, to->z) * ANGLE_PER_RADIAN;
            turn = facing - HEADING(work);
            if (turn > 0) {
                if (turn > security_camera_most_turn) {
                    turn = security_camera_most_turn;
                }
            } else if (turn < -security_camera_most_turn) {
                turn = -security_camera_most_turn;
            }
            HEADING(work) += turn;
        }
        if (work->submode) {
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, (NJS_POINT3 *)security_camera_object->child->pos,
                        &point);
            njPopMatrix(1);
            point.y -= FALL_DROP;
            ang.x = 0;
            ang.y = HEADING(work);
            ang.z = 0;
            spd.x = 0.0F;
            spd.y = FALL_RISE;
            spd.z = 0.0F;
            work->mode = MODE_BROKEN;
            floor = surface_probe_floor(point.x, point.y, point.z, &floor_ang);
            score_add(BREAK_POINTS);
            debris_launch_flags(&point, &ang, floor, &spd,
                                security_camera_object->child->model,
                                &security_camera_texlist, FALL_FLAGS);
            object_sound_play(BREAK_SOUND_KIND, &point);
            SECURITY_CAMERA_BROKEN_END;
        }
        break;
    }
    collision_register(task);
}

static void security_camera_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    njSetTexture(&security_camera_texlist);
    object = security_camera_object;
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    if (work->mode != MODE_BROKEN) {
        object = object->child;
        njTranslateV(NULL, (NJS_VECTOR *)object->pos);
        njRotateY(NULL, HEADING(work) - work->ang.y);
        njCnkEasyDrawModel(object->model);
    }
    njPopMatrix(1);
}

static void security_camera_head_init(struct task *task)
{
    task->step = security_camera_head_step;
    task->destructor = security_camera_head_destroy;
    collision_init(task, security_camera_head_shapes, HEAD_SHAPES,
                   COLLISION_ITEM);
    enemy_work_alloc(task);
}

static void security_camera_head_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void security_camera_head_step(struct task *task)
{
    struct task *camera = task->parent;
    struct task_work *camera_work = camera->work;
    struct task_work *work = task->work;
    NJS_VECTOR spd;
    NJS_POINT3 pos;

    if (work->flags & WORK_HURT) {
        camera_work->submode |= 1 << HEAD_SHIFT(work);
        spd.x = 0.0F;
        spd.y = 0.0F;
        spd.z = 0.0F;
        pos = work->pos;
        dust_create(&pos, &spd, HURT_DUST_SCALE);
        flash_manager_create_sparks_depth(pos.x, pos.y, pos.z, HURT_FLASH_SCALE,
                                          HURT_SPARKS, HURT_FLASH_DEPTH_BIAS);
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    } else {
        collision_register(task);
    }
}

#endif
