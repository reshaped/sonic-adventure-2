/* the machine's init and callbacks. shared/machine.c compiles them for
 * Prison Lane's MECHA, and stg11/machine.c for Iron Gate's. stg10/signal.c
 * compiles them for Metal Harbor's SIGNAL and SIGNAL_F. the includer defines
 * MACHINE_EDITOR_ROT_Y */
#ifndef SHARED_PRISON_MACHINERY_MACHINE_TASK_H
#define SHARED_PRISON_MACHINERY_MACHINE_TASK_H

#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/machine.h"
#include "shared/vertex_colors.h"
#include "system/game_state.h"
#include "system/task.h"

/* the lamp, the model of the machine's child, blinks with a period of
 * BLINK_STEPS of game_state.steps. it is LAMP_ON for the first half and
 * LAMP_OFF for the second */
#define BLINK_STEPS 40
#define LAMP_ON 0xffffffff
#define LAMP_OFF 0xff808080

#define MACHINE_SHAPES 1

/* the machine's texture list, object and shape, which the includer defines
 * below this body */
static NJS_TEXLIST machine_texlist;
static NJS_CNK_OBJECT machine_object;
static struct collision_shape machine_shapes[MACHINE_SHAPES];

static void machine_destroy(struct task *task);
static void machine_step(struct task *task);
static void machine_draw(struct task *task);

void machine_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = machine_draw;
    task->step = machine_step;
    task->destructor = machine_destroy;
    collision_init(task, machine_shapes, MACHINE_SHAPES, COLLISION_OBJECT);
}

static void machine_destroy(struct task *task)
{
}

static void machine_step(struct task *task)
{
    /* @hack redundant: the work read into a local nothing uses */
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    collision_register(task);
}

static void machine_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for the SET editor's
     * caption, which SHC still places in this source's constants */
    if (0) {
        (void)MACHINE_EDITOR_ROT_Y;
    }
    njSetTexture(&machine_texlist);
    object = &machine_object;
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    vlist_set_color(
        object->model->vlist,
        game_state.steps % BLINK_STEPS < BLINK_STEPS / 2 ? LAMP_ON : LAMP_OFF);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
}

#endif
