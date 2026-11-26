/* FLUO's init and callbacks. shared/fluorescent_lamp.c compiles them for
 * Prison Lane, and stg11/fluorescent_lamp.c for Iron Gate. the includer
 * defines the FLUORESCENT_LAMP_EDITOR_ captions */
#ifndef SHARED_PRISON_MACHINERY_FLUORESCENT_LAMP_TASK_H
#define SHARED_PRISON_MACHINERY_FLUORESCENT_LAMP_TASK_H

#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "draw/color.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/fluorescent_lamp.h"
#include "system/task.h"
#include "system/util.h"

/* the work's mode. the work's timer counts the mode's steps down. the step
 * that finds the timer at 0 switches to the other mode */
enum fluorescent_lamp_mode { MODE_LIT, MODE_DARK };

/* the lamp's brightness, from 0 to 1, in the work's scl.z. a step moves it
 * BRIGHTNESS_STEP toward 1 while the lamp is lit, and toward 0 while dark */
#define BRIGHTNESS(work) ((work)->scl.z)
#define BRIGHTNESS_STEP 0.25F
/* the first lit spell's timer takes FIRST_LIT_SHARE of lit_range. each later
 * spell's takes a random share of its range */
#define FIRST_LIT_SHARE 0.5F

/* the lamp and its tube draw in grays that reach white at brightness 1.
 * at 0 the lamp's gray is SHADE_HIGH of white, and its tube's SHADE_LOW */
#define SHADE_LOW 0.2F
#define SHADE_HIGH 0.8F
#define SHADE_FULL 255.0F
#define SHADE_MAX 255
#define SHADE_ALPHA 0xff000000

#define FLUORESCENT_LAMP_SHAPES 1

/* FLUORESCENT_LAMP_VIEW_TEST(work) opens the draw, and may return from it.
 * Iron Gate's culls the lamp, and Prison Lane's does nothing */
#ifndef FLUORESCENT_LAMP_VIEW_TEST
#define FLUORESCENT_LAMP_VIEW_TEST(work) ((void)0)
#endif

/* the lamp's texture list and object, which the art defines below this body.
 * the object's child is the tube */
static NJS_TEXLIST fluorescent_lamp_texlist;
static NJS_CNK_OBJECT fluorescent_lamp_object;

/* a spell's timer starts at its mode's least plus a share of its range, in
 * steps. the includer defines these four, and the lamp's shape, below this
 * body */
static Float lit_least;
static Float lit_range;
static Float dark_least;
static Float dark_range;
static struct collision_shape fluorescent_lamp_shapes[FLUORESCENT_LAMP_SHAPES];

static void fluorescent_lamp_destroy(struct task *task);
static void fluorescent_lamp_step(struct task *task);
static void fluorescent_lamp_draw(struct task *task);

void fluorescent_lamp_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = fluorescent_lamp_draw;
    task->step = fluorescent_lamp_step;
    task->destructor = fluorescent_lamp_destroy;
    collision_init(task, fluorescent_lamp_shapes, FLUORESCENT_LAMP_SHAPES,
                   COLLISION_OBJECT);
    BRIGHTNESS(work) = 1.0F;
    work->timer = lit_least + (lit_range * FIRST_LIT_SHARE);
}

static void fluorescent_lamp_destroy(struct task *task)
{
}

static void fluorescent_lamp_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->mode == MODE_LIT) {
        BRIGHTNESS(work) += BRIGHTNESS_STEP;
        if (BRIGHTNESS(work) > 1.0F) {
            BRIGHTNESS(work) = 1.0F;
        }
        if (work->timer-- == 0) {
            work->timer = dark_least + (dark_range * rand() * RAND_SCALE);
            work->mode = MODE_DARK;
        }
    } else {
        BRIGHTNESS(work) -= BRIGHTNESS_STEP;
        if (BRIGHTNESS(work) < 0.0F) {
            BRIGHTNESS(work) = 0.0F;
        }
        if (work->timer-- == 0) {
            work->timer = lit_least + (lit_range * rand() * RAND_SCALE);
            work->mode = MODE_LIT;
        }
    }
    /* @hack redundant: a block no path reaches, kept for the SET editor's
     * captions, which SHC still places in this source's constants. SHC also
     * still sizes the step's frame for the block's local and call */
    if (0) {
        int mode = work->mode;

        njPopMatrix(mode);
        work->mode = mode;
        work->mode = mode;
        (void)FLUORESCENT_LAMP_EDITOR_ANGLE_X;
        (void)FLUORESCENT_LAMP_EDITOR_ANGLE_Y;
        (void)FLUORESCENT_LAMP_EDITOR_ANGLE_Z;
    }
}

static void fluorescent_lamp_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *tube;
    /* @hack spelling: level, a local for each gray's share of white, where
     * the share times SHADE_FULL in place would do */
    Float level;
    Uint32 shade;

    FLUORESCENT_LAMP_VIEW_TEST(work);
    njSetTexture(&fluorescent_lamp_texlist);
    object = &fluorescent_lamp_object;
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    level = SHADE_HIGH + (BRIGHTNESS(work) * SHADE_LOW);
    shade = level * SHADE_FULL;
    if (shade > SHADE_MAX) {
        shade = SHADE_MAX;
    }
    shade = (shade << RED_SHIFT) + (shade << GREEN_SHIFT) + shade + SHADE_ALPHA;
    model_draw_tinted(object->model, shade);
    tube = object->child;
    level = SHADE_LOW + (BRIGHTNESS(work) * SHADE_HIGH);
    shade = level * SHADE_FULL;
    if (shade > SHADE_MAX) {
        shade = SHADE_MAX;
    }
    shade = (shade << RED_SHIFT) + (shade << GREEN_SHIFT) + shade + SHADE_ALPHA;
    model_draw_tinted(tube->model, shade);
    njPopMatrix(1);
}

#endif
