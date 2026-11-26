/* STAGE in Security Hall's object list: a platform that players stand on. the
 * placement's ang.y turns it, and it shows only on camera 0's floor */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg08/fixture_art.h"
#include "stg08/background.h"
#include "stg08/floor_object.h"
#include "stg08/solid_props.h"
#include "system/task.h"
#include "system/util.h"

/* the platform's record, in task->motion: the copy of
 * security_platform_collision that the collision world holds. the copy lies
 * where the platform does */
#define OBJECT(task) ((NJS_OBJECT *)(task)->motion.p)
/* world_add classes the copy by its radius + CLASS_MARGIN. players meet the
 * copy only while one is within the copy's radius + NEAR_MARGIN of the
 * platform's pos */
#define CLASS_MARGIN 5.0F
#define NEAR_MARGIN 15.0F

NJS_TEXLIST *security_platform_textures[] = {
    &security_platform_texlist,
    NULL,
};

static void platform_destroy(struct task *task);
static void platform_step(struct task *task);
static void platform_draw(struct task *task);

void security_platform_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    /* @hack spelling: the callbacks set where the allocation succeeds, and
     * the return in an else, where an early return on failure would do */
    if (task->motion.p) {
        task->draw = platform_draw;
        task->step = platform_step;
        task->destructor = platform_destroy;
    } else {
        return;
    }
    object = task->motion.p;
    *object = security_platform_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID;
    radius = object->model->r + CLASS_MARGIN;
    CLASSIFY_SOLID(flags, radius);
    world_add(flags, task, object);
}

static void platform_destroy(struct task *task)
{
    if (OBJECT(task)->model) {
        world_remove(task, task->motion.p);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void platform_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: unused, a point that no line reads */
    NJS_POINT3 unused;

    if (placement_try_leave(task)) {
        return;
    }
    SELECT_FLOOR(work, security_floor_height, ~WORLD_ADDED);
    if (OBJECT(task)->model) {
        if (player_near(&work->pos, OBJECT(task)->model->r + NEAR_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void platform_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a statement kept for its string, the set editor's
     * label for ang.y, which SHC places in this source's constants */
    (void)"<- ANGLE Y";

    if (work->tick) {
        njSetTexture(&security_platform_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        draw_control_set(DRAW_CONTROL_SHADOW);
        njCnkEasyDrawModel(&security_platform_model);
        draw_control_clear(DRAW_CONTROL_SHADOW);
        njPopMatrixEx();
    }
}
