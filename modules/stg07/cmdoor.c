/* the door, CMDOOR in Aquatic Mine's object list, and the sign, SIGN. the door
 * is a model in a box that pushes what meets it. the sign is a model object */
#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg07/cmdoor.h"
#include "system/task.h"

#include "stg07/cmdoor/text.inc"

#include "stg07/scenery/cmdoor.h"

NJS_TEXLIST *cmdoor_texlists[3] = {
#include "stg07/cmdoor/cmdoor_texlists.inc"
};
static struct collision_shape cmdoor_shape = {
#include "stg07/cmdoor/cmdoor_shape.inc"
};

static void cmdoor_destroy(struct task *task);
static void cmdoor_step(struct task *task);
static void cmdoor_draw(struct task *task);

void cmdoor_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        /* @hack spelling: the draw set before the step, where struct
         * task's order would set the step first */
        task->draw = cmdoor_draw;
        task->step = cmdoor_step;
        task->destructor = cmdoor_destroy;
        collision_init(task, &cmdoor_shape, 1, COLLISION_OBJECT);
    }
}

static void cmdoor_destroy(struct task *task)
{
}

/* @hack redundant: task declared register, where a plain parameter would do */
static void cmdoor_step(register struct task *task)
{
    if (!placement_try_leave(task)) {
        collision_register(task);
    }
}

static void cmdoor_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = &cmdoor_object;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)CMDOOR_EDITOR_ROT_Y;
    }
    njSetTexture(&cmdoor_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}

void mine_sign_init(struct task *task)
{
    model_object_init(task, &mine_sign_texlist, mine_sign_object->model, NULL,
                      0, OBJECT_MAY_LEAVE);
}
