/* LIGHT in Security Hall's object list. each LIGHT not wholly behind the
 * camera queues itself. the drawer, a task of the lamps' own, draws the
 * queue */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/vertex_colors.h"
#include "stg08/fixture_art.h"
#include "stg08/lamp.h"
#include "system/cache.h"
#include "system/task.h"

#define LAMP_LIMIT 256
/* a LIGHT queues itself unless a ball of LAMP_RADIUS about its pos lies wholly
 * behind the camera */
#define LAMP_RADIUS 25.0F
#define DRAWER_LEVEL 4

struct lamp {
    NJS_POINT3 pos;
    Angle yaw;
    Sint16 phase;
};

NJS_TEXLIST *security_lamp_textures[] = {
    &security_lamp_texlist,
    NULL,
};

static struct lamp lamps[LAMP_LIMIT];

/* the lens blends from pulse_clear to pulse_white by the magnitude of its
 * phase's sine. the phase turns by pulse_speed a step */
static Angle pulse_speed = 512;
static Uint32 pulse_clear = 0x00ffffff;
static Uint32 pulse_white = 0xffffffff;
static int lamp_count = 0;
static struct task *drawer = NULL;

static void lamp_destroy(struct task *task);
static void lamp_step(struct task *task);
static void queue_lamp(struct task *task);

void security_lamp_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = queue_lamp;
        task->step = lamp_step;
        task->destructor = lamp_destroy;
    }
}

static void lamp_destroy(struct task *task)
{
}

static void lamp_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        work->timer += pulse_speed;
    }
}

static void lamps_draw(struct task *task)
{
    struct lamp *lamp = lamps;
    int remaining = lamp_count;
    NJS_CNK_OBJECT *object;
    Uint32 color;

    njSetTexture(&security_lamp_texlist);
    njPushMatrixEx();
    njFogDisable();
    for (; remaining > 0; remaining--, lamp++) {
        njTranslateEx(&lamp->pos);
        njRotateY(NULL, lamp->yaw);
        object = security_lamp_object;
        njCnkEasyDrawModel(object->model);
        color = color_blend(pulse_clear, pulse_white,
                            _builtin_fabsf(njSin(lamp->phase)));
        object = object->child; /* the lens, the housing's child */
        vlist_set_color(object->model->vlist, color);
        njCnkEasyDrawModel(object->model);
        matrix_load_parent();
    }
    njFogEnable();
    njPopMatrixEx();
    /* the queue empties once the last camera has drawn it */
    if (camera_current_index >= camera_active_count - 1) {
        lamp_count = 0;
    }
}

static void lamps_destroy(struct task *task)
{
    drawer = NULL;
    lamp_count = 0;
}

static void lamps_step(struct task *task)
{
}

static void queue_lamp(struct task *task)
{
    struct task_work *work = task->work;

    if (!sphere_in_front_no_depth(&work->pos, LAMP_RADIUS)) {
        return;
    }
    if (!drawer) {
        drawer = task_create(0, DRAWER_LEVEL, lamps_step, "ManDraw");
        drawer->draw = lamps_draw;
        drawer->destructor = lamps_destroy;
    }
    if (lamp_count >= LAMP_LIMIT) {
        return;
    }
    lamps[lamp_count].pos = work->pos;
    lamps[lamp_count].yaw = work->ang.y;
    lamps[lamp_count].phase = work->timer;
    lamp_count++;
}
