#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "object/set.h"
#include "stg27/flash_zone.h"
#include "stg27/light.h"
#include "system/task.h"

/* the steps the zone's level takes to rise to 1 */
#define RAMP_STEPS(task) ((task)->unk_3c.l)
#define RAMP_MASK 0xfff

/* the set editor's labels for the zone's ang.x, ang.y and scl.x to scl.z */
static const char editor_switch_time[20] = "<- SW TIME(0-FFF)";
static const char editor_rotation[12] = "<- ROT Y";
static const char editor_scale_x[12] = "<- SCL X";
static const char editor_scale_y[12] = "<- SCL Y";
static const char editor_scale_z[12] = "<- SCL Z";

static void flash_zone_destroy(struct task *task);
static void flash_zone_step(struct task *task);
static void flash_zone_draw(struct task *task);

void flash_zone_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = flash_zone_draw;
    task->step = flash_zone_step;
    task->destructor = flash_zone_destroy;
    work->mode = 0;
    RAMP_STEPS(task) = work->ang.x & RAMP_MASK;
    if (RAMP_STEPS(task) < 1) {
        RAMP_STEPS(task) = 1;
    }
}

static void flash_zone_destroy(struct task *task)
{
}

static void flash_zone_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 point;

    /* @hack spelling: the rest of the step in an else after the return,
     * where it could follow the if */
    if (placement_try_leave(task)) {
        return;
    } else {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, -work->ang.y);
        njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
        njCalcPoint(NULL, &camera_views[0]->pos, &point);
        njPopMatrixEx();
        if (njAbs(point.x) < work->scl.x && njAbs(point.z) < work->scl.z &&
            njAbs(point.y) < work->scl.y) {
            if (work->timer < RAMP_STEPS(task)) {
                work->timer++;
            }
        } else {
            work->timer = 0;
        }
        colony_flash += (Float)work->timer / (Float)RAMP_STEPS(task);
    }
}

static void flash_zone_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 center;
    NJS_POINT3 size;

    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * draw the image's frame */
    if (0) {
        njCalcPoint(NULL, &work->pos, &center);
        njCalcPoint(NULL, &work->scl, &size);
    }
}
