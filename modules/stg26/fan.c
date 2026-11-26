/* FAN in Egg Quarters' object list. shared/fan_motion.h turns the fan, and
 * this source draws it */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/machinery_assets.h"
#include "stg26/machinery.h"
#include "system/task.h"
#include "system/util.h"

static Sint16 fan_polygons[129] = {
#include "stg26/fan/fan_polygons.inc"
};

static Sint32 fan_vertices[103] = {
#include "stg26/fan/fan_vertices.inc"
};

NJS_CNK_MODEL fan_model = {
#include "stg26/fan/fan_model.inc"
};

static NJS_CNK_OBJECT fan_object = {
#include "stg26/fan/fan_object.inc"
};

NJS_TEXLIST *quarters_fan_texlists[2] = {
#include "stg26/fan/quarters_fan_texlists.inc"
};

#define FAN_INIT quarters_fan_init
#include "shared/fan_motion.h"

static void fan_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, FAN_ANGLE(task));
    /* @hack spelling: task->work read again for scl, where work would do */
    scale = task->work->scl.x + 1.0F;
    njScale(NULL, scale, scale, scale);
    njSetTexture(&fan_texlist);
    njCnkEasyDrawModel(&fan_model);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
