/* the stopper, STOPPER in a stage's object list: a solid block whose lamp
 * blinks. stg10's list calls it MHSLIDER */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/stopper.h"
#include "shared/stopper_model.h"
#include "shared/vertex_colors.h"
#include "system/game_state.h"
#include "system/task.h"

/* the lamp's blink, in steps of game_state.steps. it is gray through step
 * LAMP_GRAY_LAST of each BLINK_STEPS, and white after */
#define BLINK_STEPS 48
#define LAMP_GRAY_LAST 20
#define LAMP_ON 0xffffffff
#define LAMP_OFF 0xff808080

/* the copy's size class is that of its model's radius plus REACH_MARGIN. the
 * world collects the copy only while a player is within that sum of its
 * center */
#define REACH_MARGIN 15.0F

/* a stopper's record, in task->extra */
struct stopper {
    /* the copy of stopper_collision, placed as the work is, that the
     * collision world takes */
    NJS_OBJECT object;
    NJS_POINT3 center; /* the center of object's model, in world space */
};

#define STOPPER(task) ((struct stopper *)(task)->extra.p)

NJS_TEXLIST *stopper_texlists[] = {&stopper_texlist, NULL};

static void stopper_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 color;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njSetTexture(&stopper_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    if (game_state.steps % BLINK_STEPS > LAMP_GRAY_LAST) {
        color = LAMP_ON;
    } else {
        color = LAMP_OFF;
    }
    vlist_set_color(stopper_object->child->model->vlist, color);
    njCnkEasyDrawObject(stopper_object);
    njPopMatrix(1);
}

static void stopper_destroy(struct task *task)
{
    if (task->extra.p != NULL) {
        if (STOPPER(task)->object.model != NULL) {
            world_remove(task, &STOPPER(task)->object);
        }
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void stopper_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (task->extra.p != NULL && STOPPER(task)->object.model != NULL) {
        if (player_near(&STOPPER(task)->center,
                        STOPPER(task)->object.model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

void stopper_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;

    if (placement_try_leave(task)) {
        return;
    }
    task->extra.p = syCalloc(1, sizeof(struct stopper));
    if (task->extra.p == NULL) {
        return;
    }
    object = &STOPPER(task)->object;
    *object = stopper_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    /* @hack spelling: the angles set y, z, x, where x, y, z would do */
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID;
    radius = object->model->r + REACH_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    /* @hack spelling: the record reached through task->extra again, where
     * object would do */
    njTranslateV(NULL, (NJS_VECTOR *)STOPPER(task)->object.pos);
    njRotateY(NULL, STOPPER(task)->object.ang[1]);
    njCalcPoint(NULL, &STOPPER(task)->object.model->center,
                &STOPPER(task)->center);
    njPopMatrix(1);
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = stopper_draw;
    task->step = stopper_step;
    task->destructor = stopper_destroy;
}
