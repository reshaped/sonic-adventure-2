/* FAN in Death Chamber's object list. shared/fan_motion.h turns the fan, and
 * this source draws it */
#include <ninja.h>

#include "shared/machinery_assets.h"
#include "stg25/chamber_util.h"
#include "stg25/fan.h"
#include "system/task.h"
#include "system/util.h"

NJS_TEXLIST *chamber_fan_texlists[] = {&fan_texlist, NULL};

#define FAN_INIT chamber_fan_init
#include "shared/fan_motion.h"

static void fan_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, FAN_ANGLE(task));
    njSetTexture(&fan_texlist);
    /* @hack spelling: task->work read again for scl, where work would do */
    chamber_model_draw_scaled(&fan_model, task->work->scl.x + 1.0F);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
