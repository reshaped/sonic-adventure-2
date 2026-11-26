/* the planter, PLANTER in Mad Space's object list: a decoration that turns */
#include <ninja.h>

#include "object/set.h"
#include "stg44/planter.h"
#include "stg44/scenery_shapes.h"
#include "system/game_state.h"
#include "system/task.h"

/* the child's turn about y for each count of game_state.steps */
#define TURN_PER_STEP 0x180

NJS_TEXLIST *planter_textures[] = {&planter_texlist, NULL};

static void planter_destroy(struct task *task);
static void planter_step(struct task *task);
static void planter_draw(struct task *task);

void planter_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = planter_draw;
        task->step = planter_step;
        task->destructor = planter_destroy;
        work->mode = 0;
    }
}

static void planter_destroy(struct task *task)
{
}

static void planter_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        /* @hack redundant: a block no path reaches, kept for its strings,
         * which SHC still places in this source's constants, and its draws */
        if (0) {
            NJS_CNK_OBJECT *object = &planter_object;
            (void)"<- ANGLE X";
            (void)"<- ANGLE Y";
            (void)"<- ANGLE Z";
            njCnkEasyDrawModel(object->model);
            njCnkEasyDrawModel(object->child->model);
        }
    }
}

static void planter_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    njSetTexture(&planter_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    /* @hack spelling: object set to &planter_object and then to its child,
     * where planter_object.child would do */
    object = &planter_object;
    object = object->child;
    njRotateY(NULL, game_state.steps * TURN_PER_STEP);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}
