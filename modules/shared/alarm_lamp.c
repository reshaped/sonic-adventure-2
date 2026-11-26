/* the alarm lamp, SIREN in Prison Lane's object list, whose head turns */
#include <ninja.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/alarm_lamp.h"
#include "system/task.h"
#include "system/util.h"

#include "shared/alarm_lamp/text.inc"

#define ALARM_LAMP_SHAPES 1

/* stg11/alarm_lamp.c compiles this source again for Iron Gate's lamp, and
 * defines these two first. there, ALARM_LAMP_BEFORE_HEAD() turns the fog off
 * for the head's draw, and ALARM_LAMP_AFTER_HEAD() turns it on again. in
 * Prison Lane's lamp, the two do nothing */
#ifndef ALARM_LAMP_BEFORE_HEAD
#define ALARM_LAMP_BEFORE_HEAD() ((void)0)
#endif
#ifndef ALARM_LAMP_AFTER_HEAD
#define ALARM_LAMP_AFTER_HEAD() ((void)0)
#endif

/* the lamp's texture list and object, which alarm_lamp.inc defines at the end
 * of this source. the object's child is the head */
static NJS_TEXLIST alarm_lamp_texlist;
static NJS_CNK_OBJECT alarm_lamp_object;

static Angle head_spin;
static struct collision_shape alarm_lamp_shapes[ALARM_LAMP_SHAPES];

static void alarm_lamp_destroy(struct task *task);
static void alarm_lamp_step(struct task *task);
static void alarm_lamp_draw(struct task *task);

void alarm_lamp_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = alarm_lamp_draw;
    task->step = alarm_lamp_step;
    task->destructor = alarm_lamp_destroy;
    /* the head's heading starts at random */
    work->timer = RANDOM_TURN();
    collision_init(task, alarm_lamp_shapes, ALARM_LAMP_SHAPES,
                   COLLISION_OBJECT);
}

static void alarm_lamp_destroy(struct task *task)
{
}

static void alarm_lamp_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: the turn made inside the test, with the return in an
     * else, where a return first would do */
    if (!placement_try_leave(task)) {
        work->timer += head_spin;
    } else {
        return;
    }
    collision_register(task);
}

static void alarm_lamp_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *head;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)ALARM_LAMP_EDITOR_ANGLE_X;
        (void)ALARM_LAMP_EDITOR_ANGLE_Y;
        (void)ALARM_LAMP_EDITOR_ANGLE_Z;
    }
    njSetTexture(&alarm_lamp_texlist);
    object = &alarm_lamp_object;
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkSimpleDrawModel(object->model);
    head = object->child;
    ALARM_LAMP_BEFORE_HEAD();
    njRotateY(NULL, work->timer);
    njCnkEasyDrawModel(head->model);
    ALARM_LAMP_AFTER_HEAD();
    njPopMatrix(1);
}

#include "shared/prison_fixtures/alarm_lamp.h"

NJS_TEXLIST *alarm_lamp_texlists[] = {&alarm_lamp_texlist, NULL};
/* the head's turn in a step */
static Angle head_spin = {
#include "shared/alarm_lamp/head_spin.inc"
};
static struct collision_shape alarm_lamp_shapes[ALARM_LAMP_SHAPES] = {
#include "shared/alarm_lamp/alarm_lamp_shapes.inc"
};
