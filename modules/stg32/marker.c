/* the marker, DMARK in Meteor Herd's object list. both sizes collide as
 * herd_marker_collision */
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg32/machinery.h"
#include "stg32/scenery_shapes.h"
#include "system/task.h"

#define LAMP_PERIOD 16
#define LAMP_REST 12
#define LAMP_FADE 3

NJS_TEXLIST *herd_marker_textures[] = {&herd_large_marker_texlist,
                                       &herd_small_marker_texlist, NULL};
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;
static void marker_destroy(struct task *task);
static void marker_draw(struct task *task);
static void marker_step(struct task *task);

void herd_marker_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* solid_object_init_flags replaces this step and destructor with the solid
     * object's own. they stay only where the heap can't give its record */
    task->step = marker_step;
    task->draw = marker_draw;
    task->destructor = marker_destroy;
    solid_object_init_flags(task, NULL, NULL, NULL, 0, herd_marker_collision,
                            OBJECT_NEAR_ONLY, 0);
}

static void marker_destroy(struct task *task)
{
}

static void marker_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *lamp;
    /* @hack type: color a Sint32, where the Uint32 color_blend returns would
     * do */
    Sint32 color;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGSPD Y";
        (void)"<- MODEL";
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (work->scl.x >= 1.0F) {
        njSetTexture(&herd_large_marker_texlist);
        object = herd_large_marker;
        njCnkEasyDrawModel(object->model);
    } else {
        njSetTexture(&herd_small_marker_texlist);
        object = herd_small_marker;
        njCnkEasyDrawModel(object->model);
    }
    color = color_blend(bright_color, dim_color,
                        blink_level(LAMP_PERIOD, LAMP_REST, LAMP_FADE));
    lamp = object->child;
    model_draw_tinted(lamp->model, color);
    njPopMatrixEx();
}

static void marker_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
}
