/* a log, one of LOG_VARIANTS models for the course's land. its placement's
 * ang.x chooses the model */
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/race.h"
#include "chaostgrace/course_models.h"
#include "chaostgrace/props.h"
#include "collision/collision.h"
#include "object/set.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"

enum { LOG_LANDS = 3, LOG_VARIANTS = 3 };
/* the 16 bits of an angle in the placement's 32-bit ang.x */
#define ANGLE_BITS 0xffff
KEPT static NJS_TEXLIST *log_textures[LOG_LANDS] = {
#include "chaostgrace/log/log_textures.inc"
};
KEPT static NJS_CNK_OBJECT *log_models[LOG_LANDS][LOG_VARIANTS] = {
#include "chaostgrace/log/log_models.inc"
};

static void log_step(struct task *task)
{
    struct task_work *work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a comparison of work->ang.x and ang.y under if (0),
     * which never runs */
    if (0) {
        if (work->ang.x == work->ang.y) {
        }
    }
}

static void log_draw(struct task *task)
{
    struct task_work *work = task->work;
    Sint32 land = garden_race_course()->land;
    int variant = work->ang.x & ANGLE_BITS;
    njSetTexture(log_textures[land]);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawObject(log_models[land][variant % LOG_VARIANTS]);
    njPopMatrixEx();
}

static void log_destroy(struct task *task)
{
}

void race_log_init(struct task *task)
{
    struct task_work *work = task->work;
    task->step = log_step;
    task->draw = log_draw;
    task->destructor = log_destroy;
    work->ang.x = (Uint16)work->ang.x;
}

/* a log's capsule, turned a quarter turn about z to lie along x */
/* clang-format off */
static const struct collision_shape log_capsule = {
    (Sint8)194, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(3), 0, {0.0F, 3.0F, 0.0F}, 10.0F, 110.0F, 0.0F, 0,
    {0, 0, QUARTER_TURN}};
/* clang-format on */
/* the labels the placement editor gives a log's fields */
struct log_editor_labels {
    char model[28], yaw[20];
};
static const struct log_editor_labels log_editor_labels = {
#include "chaostgrace/log/log_editor_labels.inc"
};
