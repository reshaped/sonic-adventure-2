/* the cell door's init and callbacks. the door sinks into the floor once its
 * group of enemies is beaten. stg09/cage_door.c compiles them for Prison
 * Lane's CAGEB, and stg09/wide_cage_door.c for CAGEC. the includer defines the
 * CAGE_DOOR_EDITOR_ captions */
#ifndef STG09_CAGE_DOOR_IMPL_H
#define STG09_CAGE_DOOR_IMPL_H

#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg09/cage_door.h"
#include "stg09/door.h"
#include "system/angle.h"
#include "system/task.h"

/* the door's modes, in the work's mode. in MODE_OPEN the door has sunk, and
 * bounces at the end of its travel */
enum cage_door_mode { MODE_SHUT, MODE_SINKING, MODE_OPEN };

/* how far the door sinks to open */
#define DOOR_SINK 39.0F

/* the door's half width, which wide_cage_door.c defines for its wider door */
#ifndef CAGE_DOOR_HALF_WIDTH
#define CAGE_DOOR_HALF_WIDTH 44.0F
#endif
/* the dust the door raises as it stops sinking ends DUST_INSET inside its
 * edges */
#define DUST_INSET 5.0F
#define DUST_HALF_WIDTH (CAGE_DOOR_HALF_WIDTH - DUST_INSET)
/* the posts' centers lie POST_OFFSET outside the door's edges */
#define POST_OFFSET 2.0F
#define POST_X (CAGE_DOOR_HALF_WIDTH + POST_OFFSET)
/* the low byte of ang.x: the enemy group the door waits on. the door stays
 * shut while any of the group is left */
#define GROUP(work) ((Uint8)(work)->ang.x)
/* how far the door sinks a step */
#define SINK_SPEED(work) ((work)->scl.x)

#define CAGE_DOOR_SHAPES 4
#define DOOR_SHAPE 1

/* the cell front's texture list and node, which the art defines below this
 * body. the node's child is the door */
static NJS_TEXLIST cage_door_texlist;
static NJS_CNK_OBJECT cage_door_object;

/* the shapes of the sill, the door and the two posts, which the includer
 * defines below this body. the step lowers each door's own copy of
 * DOOR_SHAPE */
static struct collision_shape cage_door_shapes[CAGE_DOOR_SHAPES];

static void cage_door_destroy(struct task *task);
static void cage_door_step(struct task *task);
static void cage_door_draw(struct task *task);

void cage_door_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct door));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = cage_door_draw;
    task->step = cage_door_step;
    task->destructor = cage_door_destroy;
    DOOR(task)->travel = 0.0F;
    collision_init(task, cage_door_shapes, CAGE_DOOR_SHAPES, COLLISION_OBJECT);
    if (task->placement != NULL) {
        if (placement_get_state(task)) {
            work->mode = MODE_OPEN;
            DOOR(task)->travel = DOOR_SINK;
        }
    }
}

static void cage_door_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement != NULL && work->mode == MODE_OPEN) {
        placement_set_state(task, DOOR_STATE_DONE);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void cage_door_step(struct task *task)
{
    struct task_work *work = task->work;
    Float past;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_SHUT:
        if (enemy_group_empty(GROUP(work))) {
            work->mode = MODE_SINKING;
            work->timer = 0;
        }
        break;
    case MODE_SINKING:
        DOOR(task)->travel += SINK_SPEED(work);
        if (DOOR(task)->travel >= DOOR_SINK) {
            NJS_POINT3 sill;
            struct angles ang;

            sill.x = 0.0F;
            sill.y = 0.0F;
            sill.z = 0.0F;
            /* @hack redundant: a block no path reaches, which clears sill.x
             * and sill.y as the lines above do */
            if (0) {
                sill.x = 0.0F;
                sill.y = 0.0F;
            }
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &sill, &sill);
            njPopMatrix(1);
            ang.x = 0;
            ang.y = work->ang.y;
            ang.z = 0;
            dust_short_rect(&sill, DUST_HALF_WIDTH, DOOR_DUST_HALF_DEPTH, &ang,
                            DOOR_DUST_SCALE, DOOR_DUST_DENSITY,
                            DOOR_DUST_SPEED);
            sound_play_at_for(DOOR_STOP_SOUND, work, DOOR_SOUND_PRIORITY,
                              DOOR_SOUND_VOLUME, DOOR_STOP_UPDATES, &work->pos);
            work->mode = MODE_OPEN;
            DOOR(task)->speed += DOOR_KICK;
            work->timer = 0;
        } else {
            sound_play_at_for(DOOR_MOVE_SOUND, work, DOOR_SOUND_PRIORITY,
                              DOOR_SOUND_VOLUME, DOOR_MOVE_UPDATES, &work->pos);
        }
        break;
    case MODE_OPEN:
        DOOR(task)->travel += DOOR(task)->speed;
        past = DOOR(task)->travel - DOOR_SINK;
        DOOR(task)->speed =
            -(past * DOOR_SPRING) + (DOOR(task)->speed * DOOR_DAMPING);
        if (DOOR(task)->travel >= DOOR_SINK) {
            DOOR(task)->travel = DOOR_SINK;
            DOOR(task)->speed = -njAbs(DOOR(task)->speed);
        }
    }
    if (work->mode != MODE_SHUT) {
        if (!enemy_group_empty(GROUP(work))) {
            work->mode = MODE_SHUT;
            DOOR(task)->speed = 0.0F;
            DOOR(task)->travel = 0.0F;
            work->timer = 0;
        }
    }
    if (work->collision != NULL && work->mode != MODE_OPEN) {
        work->collision->shapes[DOOR_SHAPE].center.y =
            cage_door_shapes[DOOR_SHAPE].center.y - DOOR(task)->travel;
        collision_register(task);
    }
}

static void cage_door_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *door;

    /* @hack redundant: a block no path reaches, kept for the SET editor's
     * captions, which SHC still places in this source's constants */
    if (0) {
        (void)CAGE_DOOR_EDITOR_ENE_ID;
        (void)CAGE_DOOR_EDITOR_ANGLE_Y;
        (void)CAGE_DOOR_EDITOR_SHUT_SPEED;
    }
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&cage_door_texlist);
    object = &cage_door_object;
    njCnkEasyDrawModel(object->model);
    door = object->child;
    njTranslate(NULL, 0.0F, -DOOR(task)->travel, 0.0F);
    njCnkEasyDrawModel(door->model);
    njPopMatrix(1);
}

#endif
