/* the platform, STAGE in Meteor Herd's object list: a floor drawn as one of
 * two models */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "stg32/meteors.h"
#include "stg32/platform.h"
#include "stg32/scenery_shapes.h"
#include "system/task.h"
#include "system/util.h"

/* the platform has its solid children while a player lies within
 * COLLISION_RANGE */
#define COLLISION_RANGE 300.0F

/* the indexes of models[] */
enum platform_model { PLAIN_MODEL, ALT_MODEL, PLATFORM_MODELS };

NJS_TEXLIST *meteor_platform_textures[] = {&meteor_platform_texlist,
                                           &meteor_platform_alt_texlist, NULL};
/* meteor_platform_build built both models */
static Bool models_built = FALSE;
static struct model_build models[] = {
    {MODEL_BUILD_ALONE_AS_LAND, &meteor_platform_texlist,
     meteor_platform_object, NULL},
    {MODEL_BUILD_ALONE_AS_LAND, &meteor_platform_alt_texlist,
     meteor_platform_alt_object, NULL},
};

static void platform_destroy(struct task *task);
static void platform_step(struct task *task);
static void platform_draw(struct task *task);

Uint8 *meteor_platform_build(Uint8 *buffer)
{
    if (model_builds_compile(&buffer, models, PLATFORM_MODELS) ==
        PLATFORM_MODELS) {
        models_built = TRUE;
    }
    return buffer;
}

void meteor_platform_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = platform_draw;
    task->step = platform_step;
    task->destructor = platform_destroy;
}

static void platform_destroy(struct task *task)
{
    (void)task;
}

static void platform_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (player_near(&work->pos, COLLISION_RANGE)) {
        if (task->child == NULL) {
            solid_object_create_flags(task, NULL, NULL, NULL, 0,
                                      meteor_platform_collision, 0, 0);
            solid_object_create_flags(task, NULL, NULL, NULL, 0,
                                      meteor_platform_dusty_collision, 0,
                                      WORLD_DUSTY);
        }
    } else if (task->child != NULL) {
        task_destroy_children(task);
    }
}

static void platform_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- MODEL(0-1)";
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    if (work->scl.x >= 1.0F) {
        njSetTexture(&meteor_platform_alt_texlist);
        if (models_built) {
            njDirectDrawObject(models[ALT_MODEL].built);
        } else {
            njCnkEasyDrawModel(meteor_platform_alt_object->model);
        }
    } else {
        njSetTexture(&meteor_platform_texlist);
        if (models_built) {
            njDirectDrawObject(models[PLAIN_MODEL].built);
        } else {
            njCnkEasyDrawModel(meteor_platform_object->model);
        }
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}
