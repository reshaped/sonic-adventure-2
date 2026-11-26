#include <ninja.h>
#include <sg_xpt.h>
#include "boss_bigbogy/glow.h"
#include "boss_bigbogy/model_art.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/game_state.h"

/* a glow's record, laid over its task's work. alpha follows a sine of phase,
 * which gains step each step */
struct glow_work {
    /* @hack type: minimum and maximum as Sint8s, read through Uint8 casts,
     * where Uint8s would do */
    Sint8 minimum;
    Sint8 maximum;
    Uint8 unused_02;
    Uint8 restored;
    Sint16 unused_04;
    Uint16 period;
    Angle step;
    Angle phase;
    NJS_CNK_OBJECT *object;
    Float unused_14[2];
    Float alpha;
};
#define GLOW(task) ((struct glow_work *)(task)->work)
/* the byte of a polygon list that holds the diffuse alpha of its first
 * chunk, a material. it lies past the chunk's 4-byte head and its blue, green
 * and red */
#define DIFFUSE_ALPHA 7

static struct task *active_glow = NULL; /* the living glow, or null */
static struct bigbogy_glow default_glow = {50, 180, 50, 180,
                                           &bigbogy_glow_object};

static Uint8 *model_alpha(NJS_CNK_OBJECT *object)
{
    Uint8 *list;
    if (!object || !object->model) {
        return NULL;
    }
    list = (Uint8 *)object->model->plist;
    if (!list) {
        return NULL;
    }
    list += DIFFUSE_ALPHA;
    return list;
}

static Uint8 set_alpha(NJS_CNK_OBJECT *object, Uint8 alpha)
{
    Uint8 *material = model_alpha(object);
    if (material) {
        *material = alpha;
        return alpha;
    }
    /* @hack implicit: no value for a missing alpha. SHC returns
     * model_alpha's null as 0, where return 0 would do */
#ifdef NONMATCHING
    /* the hack's plain form: GCC refuses a return with no value in a function
     * that returns one */
    return 0;
#else
    return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
}

static void glow_destroy(struct task *task)
{
    struct glow_work *glow = GLOW(task);
    set_alpha(glow->object, glow->restored);
    active_glow = NULL;
}

static void glow_step(struct task *task)
{
    struct glow_work *glow = GLOW(task);
    Float amplitude;
    if (game_state.time_stopped) {
        return;
    }
    glow->phase += glow->step;
    if (glow->phase > FULL_TURN) {
        glow->phase -= FULL_TURN;
    }
    amplitude = ((Uint8)glow->maximum - (Uint8)glow->minimum) * 0.5F;
    glow->alpha =
        (Uint8)glow->minimum + amplitude + (amplitude * njSin(glow->phase));
    if (glow->alpha > (Uint8)glow->maximum) {
        glow->alpha = (Uint8)glow->maximum;
    }
    if (glow->alpha < (Uint8)glow->minimum) {
        glow->alpha = (Uint8)glow->minimum;
    }
    set_alpha(glow->object, (Uint8)glow->alpha);
}

struct task *bigbogy_glow_create(struct task *parent,
                                 const struct bigbogy_glow *spec)
{
    const struct bigbogy_glow *settings;
    struct task *task;
    struct glow_work *glow;
    if (active_glow) {
        return NULL;
    }
    if (!spec) {
        settings = &default_glow;
    } else {
        settings = spec;
    }
    if (!settings->object || !parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK | TASK_MOTION, glow_step, parent);
    if (!task) {
        return NULL;
    }
    glow = GLOW(task);
    glow->minimum = settings->minimum;
    glow->maximum = settings->maximum;
    glow->restored = settings->restored;
    glow->period = settings->period;
    glow->step = FULL_TURN / glow->period;
    glow->phase = -QUARTER_TURN; /* the sine's low: alpha starts at minimum */
    glow->alpha = settings->minimum;
    glow->object = settings->object;
    task->draw = NULL;
    task->destructor = glow_destroy;
    active_glow = task;
    return task;
}
