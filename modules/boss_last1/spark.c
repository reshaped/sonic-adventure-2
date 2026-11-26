#include <ninja.h>
#include "boss_last1/combat_art.h"
#include "boss_last1/arena.h"
#include "boss_last1/contacts.h"
#include "object/set.h"
#include "system/port.h"
#include "system/task.h"

#define SPARK_MODEL_COUNT 2
KEPT static NJS_CNK_OBJECT *spark_models[SPARK_MODEL_COUNT] = {
#include "boss_last1/spark/spark_models.inc"
};
static void energy_spark_draw(struct task *task);
static void energy_spark_step(struct task *task);
static void energy_spark_destroy(struct task *task);

void biolizard_spark_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = energy_spark_draw;
        task->step = energy_spark_step;
        task->destructor = energy_spark_destroy;
    }
}

static void energy_spark_destroy(struct task *task)
{
}

static void energy_spark_step(struct task *task)
{
    struct task_work *work = task->work;
    if (!placement_try_leave(task)) {
        /* @hack redundant: a scale by work's scl plus 1 that never runs */
        if (0) {
            njScale(NULL, work->scl.x + 1.0F, work->scl.y + 1.0F,
                    work->scl.z + 1.0F);
        }
    }
}

static void energy_spark_draw(struct task *task)
{
    struct task_work *work = task->work;
#ifdef NONMATCHING
    /* this side draws no spark for a submode past spark_models. nor does it
     * draw a model with no child, sibling or lists, which the disc-free build's
     * zeros give */
    NJS_CNK_OBJECT *object;

    if ((Uint8)work->submode >= SPARK_MODEL_COUNT) {
        return;
    }
    object = spark_models[work->submode];
    if (!object ||
        (!object->child && !object->sibling &&
         (!object->model || !object->model->vlist || !object->model->plist))) {
        return;
    }
#endif
    njSetTexture(&biolizard_effect_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    /* @hack redundant: a scale by 1, which changes nothing */
    njScale(NULL, 1.0F, 1.0F, 1.0F);
#ifdef NONMATCHING
    /* the model the test above checked */
    njCnkSimpleDrawObject(object);
#else
    njCnkSimpleDrawObject(spark_models[work->submode]);
#endif
    njPopMatrixEx();
}
