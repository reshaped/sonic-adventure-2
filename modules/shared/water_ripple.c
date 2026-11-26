/* the water ripple: a ring on the water that grows, fades and ends */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/lines.h"
#include "shared/water_ripple.h"
#include "stage/stage.h"
#include "system/task.h"
#include "shared/water_ripple/text.inc"

/* the task level of a ripple */
#define RIPPLE_LEVEL 3

enum ripple_mode { MODE_GROW, MODE_FADE, MODE_END };

/* a ripple has no records: it keeps three Floats in its task's motion, force
 * and extra. ripple_destroy clears them, so task_destroy frees nothing there.
 * GROWTH is what the size gains a step, and LIMIT the size past which the
 * ripple fades. ALPHA falls from FADE_START by FADE_STEP a step as the ripple
 * fades. the ripple ends once ALPHA reaches 0 */
/* @hack type: motion read as a Float through its address, where
 * (task)->motion.f would do */
#define GROWTH(task) (*(Float *)&(task)->motion)
/* @hack type: force read as a Float through its address, where (task)->force.f
 * would do */
#define LIMIT(task) (*(Float *)&(task)->force)
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define ALPHA(task) (*(Float *)&(task)->extra)
#define SIZE(work) ((work)->scl.x)

#define DEFAULT_GROWTH 0.025F
#define DEFAULT_LIMIT 1.0F
#define FADE_START 0.5F
#define FADE_STEP 0.02F
#define SCL_Y 5.0F
/* the ripple's brightness on the two dim stages, and on every other */
#define EGG_QUARTERS_BRIGHTNESS 0.3F
#define DEATH_CHAMBER_BRIGHTNESS 0.4F
#define FULL_BRIGHTNESS 1.0F

static void ripple_start(struct task *task);
static void ripple_step(struct task *task);
static void ripple_draw(struct task *task);
static void ripple_destroy(struct task *task);

void ripple_create(NJS_POINT3 *pos, NJS_VECTOR *growth, Float scale)
{
    struct task *task;
    struct task_work *work;

    task =
        task_create(TASK_WORK, RIPPLE_LEVEL, ripple_start, "ObjectWaterripple");
    if (task != NULL) {
        work = task->work;
        work->pos = *pos;
        if (growth != NULL) {
            GROWTH(task) = growth->x;
            LIMIT(task) = growth->z;
        } else {
            GROWTH(task) = DEFAULT_GROWTH;
            LIMIT(task) = DEFAULT_LIMIT;
        }
        SIZE(work) = scale;
        work->scl.y = SCL_Y;
        ALPHA(task) = FADE_START;
        task->destructor = ripple_destroy;
    }
}

static void ripple_start(struct task *task)
{
    struct task_work *work = task->work;

    work->mode = MODE_GROW;
    task->step = ripple_step;
    task->draw = ripple_draw;
    task->destructor = ripple_destroy;
}

static void ripple_step(struct task *task)
{
    struct task_work *work = task->work;
    Float value;

    switch (work->mode) {
    case MODE_GROW:
        value = SIZE(work) + GROWTH(task);
        if (value > LIMIT(task)) {
            work->mode = MODE_FADE;
            ALPHA(task) = FADE_START;
        }
        SIZE(work) = value;
        break;
    case MODE_FADE:
        /* @hack spelling: alpha read before the size grows, where alpha less
         * FADE_STEP read after it would do */
        value = ALPHA(task);
        SIZE(work) += GROWTH(task);
        value -= FADE_STEP;
        if (value <= 0.0F) {
            work->mode = MODE_END;
        }
        ALPHA(task) = value;
        break;
    case MODE_END:
        task_destroy(task);
        break;
    }
}

static void ripple_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float brightness;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_ARGB unused;

    njSetTexture(&ripple_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    switch (stage_current_id()) {
    case STAGE_EGG_QUARTERS:
        brightness = EGG_QUARTERS_BRIGHTNESS;
        break;
    case STAGE_DEATH_CHAMBER:
        brightness = DEATH_CHAMBER_BRIGHTNESS;
        break;
    default:
        brightness = FULL_BRIGHTNESS;
        break;
    }
    constant_material_set(ALPHA(task) - 1.0F, brightness, brightness,
                          brightness);
    njScale(NULL, SIZE(work), 1.0F, SIZE(work));
    njCnkSimpleDrawObject(&ripple_object);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
}

static void ripple_destroy(struct task *task)
{
    task->force.p = NULL;
    task->motion.p = NULL;
    task->extra.p = NULL;
}

static NJS_TEXNAME ripple_textures[1] = {
#include "shared/water_ripple/ripple_textures.inc"
};
NJS_TEXLIST ripple_texlist = {
#include "shared/water_ripple/ripple_texlist.inc"
};

/* clang-format off */
static Sint16 ripple_polygons[217] = {
#include "shared/water_ripple/ripple_polygons.inc"
};

static Sint32 ripple_vertices[291] = {
#include "shared/water_ripple/ripple_vertices.inc"
};

static NJS_CNK_MODEL ripple_model = {
#include "shared/water_ripple/ripple_model.inc"
};

NJS_CNK_OBJECT ripple_object = {
#include "shared/water_ripple/ripple_object.inc"
};

/* clang-format on */
