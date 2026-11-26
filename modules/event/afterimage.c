#include <ninja.h>
#include <sg_xpt.h>

#include "event/afterimage.h"
#include "event/mirror.h"
#include "event/scene.h"
#include "system/task.h"

/* the task level of an afterimage */
#define EFFECT_LEVEL 2

static void afterimage_step(struct task *task);
static void afterimage_draw(struct task *task);
static void afterimage_destroy(struct task *task);

void event_afterimage_start(struct event_afterimage *pose)
{
    struct task *task;
    struct event_afterimage *work;

    task = task_create(TASK_WORK, EFFECT_LEVEL, afterimage_step, "blareExec");
    task->draw = afterimage_draw;
    task->destructor = afterimage_destroy;
    work = (struct event_afterimage *)task->work;
    *work = *pose;
    work->age = 0;
}

static void afterimage_step(struct task *task)
{
    struct event_afterimage *work = (struct event_afterimage *)task->work;

    work->age++;
    work->alpha = 1.0F - ((Float)work->age / (Float)work->lifetime);
    if (work->age >= work->lifetime) {
        task_destroy(task);
    }
}

static void afterimage_draw(struct task *task)
{
    struct event_afterimage *work = (struct event_afterimage *)task->work;

    if (work->alpha >= 1.0F) {
        return;
    }
    if (work->alpha <= 0.0F) {
        return;
    }
    event_blend_start(work->alpha * 100.0F);
    njPushMatrix(NULL);
    switch (work->kind) {
    case EVENT_OBJECT:
        if (work->flags & EVENT_SIMPLE_LIGHT) {
            njCnkSimpleDrawObject(work->object);
        } else {
            njCnkEasyDrawObject(work->object);
        }
        break;
    case EVENT_MULTI_OBJECT:
        event_select_lights(work->flags);
        if (work->flags & EVENT_SIMPLE_LIGHT) {
            njCnkSimpleMultiDrawObject(work->object);
        } else {
            njCnkEasyMultiDrawObject(work->object);
        }
        break;
    case EVENT_MOTION:
        if (work->flags & EVENT_SIMPLE_LIGHT) {
            njCnkSimpleDrawMotion(work->object, work->motion, work->frame);
        } else {
            njCnkEasyDrawMotion(work->object, work->motion, work->frame);
        }
        break;
    case EVENT_MULTI_MOTION:
        event_select_lights(work->flags);
        if (work->flags & EVENT_SIMPLE_LIGHT) {
            njCnkSimpleMultiDrawMotion(work->object, work->motion, work->frame);
        } else {
            njCnkEasyMultiDrawMotion(work->object, work->motion, work->frame);
        }
        break;
    case EVENT_SHAPE:
        /* @bug logic: EVENT_SHAPE selects the entity's lights at 0x8c507270,
         * though its drawers take none. EVENT_MULTI_SHAPE, whose drawers do,
         * draws under the lights the last selection left */
        event_select_lights(work->flags);
        if (work->flags & EVENT_SIMPLE_LIGHT) {
            njCnkSimpleDrawShapeMotion(work->object, work->motion, work->shape,
                                       event_scene_frame);
        } else {
            njCnkEasyDrawShapeMotion(work->object, work->motion, work->shape,
                                     event_scene_frame);
        }
        break;
    case EVENT_MULTI_SHAPE:
        if (work->flags & EVENT_SIMPLE_LIGHT) {
            njCnkSimpleMultiDrawShapeMotion(work->object, work->motion,
                                            work->shape, event_scene_frame);
        } else {
            njCnkEasyMultiDrawShapeMotion(work->object, work->motion,
                                          work->shape, event_scene_frame);
        }
        break;
    default:
        break;
    }
    njPopMatrix(1);
    event_blend_stop();
}

static void afterimage_destroy(struct task *task)
{
}
