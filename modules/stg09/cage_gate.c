/* the cell gate, CAGE in Prison Lane's object list. its door rises once a
 * group of enemies is beaten */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg09/cage_gate.h"
#include "stg09/door.h"
#include "system/angle.h"
#include "system/task.h"

#include "stg09/cage_gate/text.inc"

/* the gate's modes, in the work's mode. in MODE_OPEN the door has risen, and
 * bounces at its end */
enum cage_gate_mode { MODE_SHUT, MODE_RISING, MODE_OPEN };

/* how far the door rises to open */
#define DOOR_RISE 48.0F
#define DUST_HALF_WIDTH 30.0F
#define GROUP(work) ((Uint8)(work)->ang.x)

#define CAGE_GATE_SHAPES 1
#define DOOR_SHAPE 0

/* the gate's node, whose child is the door */
static NJS_TEXLIST cage_gate_texlist;
static NJS_CNK_OBJECT cage_gate_object;

static struct collision_shape cage_gate_shapes[CAGE_GATE_SHAPES];

static void cage_gate_destroy(struct task *task);
static void cage_gate_step(struct task *task);
static void cage_gate_draw(struct task *task);

void cage_gate_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->motion.p = syCalloc(1, sizeof(struct door));
        if (task->motion.p != NULL) {
            /* @hack spelling: the draw set before the step, where struct task's
             * order would set the step first */
            task->draw = cage_gate_draw;
            task->step = cage_gate_step;
            task->destructor = cage_gate_destroy;
            DOOR(task)->travel = 0.0F;
            collision_init(task, cage_gate_shapes, CAGE_GATE_SHAPES,
                           COLLISION_OBJECT);
            if (task->placement != NULL && placement_get_state(task)) {
                work->mode = MODE_OPEN;
                DOOR(task)->travel = DOOR_RISE;
            }
        }
    }
}

static void cage_gate_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement != NULL && work->mode == MODE_OPEN) {
        placement_set_state(task, DOOR_STATE_DONE);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void cage_gate_step(struct task *task)
{
    struct task_work *work = task->work;
    Float past;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_SHUT:
        if (enemy_group_empty(GROUP(work))) {
            work->mode = MODE_RISING;
            work->timer = 0;
        }
        break;
    case MODE_RISING:
        DOOR(task)->travel += work->scl.x;
        if (DOOR(task)->travel >= DOOR_RISE) {
            NJS_POINT3 top;
            struct angles ang;

            top.x = 0.0F;
            top.y = DOOR(task)->travel;
            top.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &top, &top);
            njPopMatrix(1);
            ang.x = 0;
            ang.y = work->ang.y;
            ang.z = 0;
            dust_short_rect(&top, DUST_HALF_WIDTH, DOOR_DUST_HALF_DEPTH, &ang,
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
        past = DOOR(task)->travel - DOOR_RISE;
        DOOR(task)->speed =
            -(past * DOOR_SPRING) + (DOOR(task)->speed * DOOR_DAMPING);
        if (DOOR(task)->travel >= DOOR_RISE) {
            DOOR(task)->travel = DOOR_RISE;
            DOOR(task)->speed = -njAbs(DOOR(task)->speed);
        }
    }
    if (work->mode != MODE_SHUT && !enemy_group_empty(GROUP(work))) {
        work->mode = MODE_SHUT;
        DOOR(task)->speed = 0.0F;
        DOOR(task)->travel = 0.0F;
        work->timer = 0;
    }
    if (work->collision != NULL) {
        work->collision->shapes[DOOR_SHAPE].center.y =
            cage_gate_shapes[DOOR_SHAPE].center.y + DOOR(task)->travel;
        collision_fit_radius(task);
        collision_register(task);
    }
}

static void cage_gate_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)CAGE_GATE_EDITOR_ENE_ID;
        (void)CAGE_GATE_EDITOR_ANGLE_Y;
        (void)CAGE_GATE_EDITOR_SHUT_SPEED;
    }
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&cage_gate_texlist);
    object = &cage_gate_object;
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njTranslate(NULL, 0.0F, DOOR(task)->travel, 0.0F);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
}

#include "stg09/fixtures/cage_gate.h"

NJS_TEXLIST *cage_gate_texlists[3] = {
#include "stg09/cage_gate/cage_gate_texlists.inc"
};
/* the door's box. cage_gate_step raises the record's copy with the door */
static struct collision_shape cage_gate_shapes[CAGE_GATE_SHAPES] = {
#include "stg09/cage_gate/cage_gate_shapes.inc"
};
