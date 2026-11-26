/* the line, LINE in Mad Space's object list */
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg44/lights.h"
#include "stg44/scenery_shapes.h"
#include "system/task.h"

#define SPEED_UNIT 0.01F
#define RISE 150.0F

NJS_TEXLIST *mad_space_line_textures[] = {&mad_space_line_texlist, NULL};

static void line_destroy(struct task *task);
static void line_step(struct task *task);
static void line_draw(struct task *task);

void mad_space_line_init(struct task *task)
{
    struct task_work *work = task->work;
    if (!placement_try_leave(task)) {
        task->draw = line_draw;
        task->step = line_step;
        task->destructor = line_destroy;
        work->mode = 0;
    }
}

static void line_destroy(struct task *task)
{
}

static void line_step(struct task *task)
{
    struct task_work *work = task->work;
    if (!placement_try_leave(task)) {
        /* @hack redundant: a block no path reaches, kept for its strings,
         * which SHC still places in this source's constants, and its draws */
        if (0) {
            NJS_CNK_OBJECT *object = &mad_space_line_object;
            (void)"<- ANGLE X";
            (void)"<- ANGLE Y";
            (void)"<- ANGLE Z";
            (void)"<- SPD(0-100)";
            model_draw_tinted(object->model, COLOR_NONE);
            model_draw_tinted(object->child->model, COLOR_NONE);
        }
        work->scl.z += work->scl.x * SPEED_UNIT;
        if (work->scl.z > 1.0F) {
            work->scl.z = 0.0F;
        }
    }
}

static void line_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    Float travel;

    njSetTexture(&mad_space_line_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    travel = work->scl.z;
    object = &mad_space_line_object;
    njTranslate(NULL, 0.0F, travel * RISE, 0.0F);
    model_draw_tinted(object->model,
                      color_blend(COLOR_NONE, COLOR_CLEAR_ALPHA, travel));
    njPopMatrixEx();
}
