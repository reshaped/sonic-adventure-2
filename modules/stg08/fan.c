/* FUN in Security Hall's object list: a fan that turns about its y axis, and
 * whirs */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "stg08/fixture_art.h"
#include "stg08/prop_textures.h"
#include "stg08/security_props.h"
#include "system/task.h"

/* the fan's whir, an id in the stage's sound list. a play lasts
 * FAN_SOUND_STEPS + 1 updates, as sound_play_at_for says */
#define FAN_SOUND 5
#define FAN_SOUND_PRIORITY 1
#define FAN_SOUND_STEPS 10

NJS_TEXLIST *security_fan_textures[] = {
    &security_fan_texlist,
    NULL,
};

static Bool compiled = FALSE;
static struct model_build builds[] = {
    {MODEL_BUILD_ALONE, &security_fan_texlist, security_fan_object, NULL}};
/* the fan's turn a step. the work's timer sums the turns, which the draw
 * adds to ang.y */
static Angle spin = 0x600;

static void fan_destroy(struct task *task);
static void fan_step(struct task *task);
static void fan_draw(struct task *task);
static void fan_draw_compiled(struct task *task);

Uint8 *security_fan_build(Uint8 *buffer)
{
    if (model_builds_compile(&buffer, builds, 1) == 1) {
        compiled = TRUE;
    }
    return buffer;
}

void security_fan_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        if (compiled) {
            task->draw = fan_draw_compiled;
        } else {
            task->draw = fan_draw;
        }
        task->step = fan_step;
        task->destructor = fan_destroy;
    }
}

static void fan_destroy(struct task *task)
{
}

static void fan_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    } else {
        /* @hack spelling: the turn added in an else after the return, where a
         * statement after the if would do */
        work->timer += spin;
    }
    sound_play_at_for(FAN_SOUND, work, FAN_SOUND_PRIORITY, 0, FAN_SOUND_STEPS,
                      &work->pos);
}

static void fan_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&security_fan_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y + work->timer);
    njCnkEasyDrawModel(security_fan_object->model);
    njPopMatrix(1);
}

static void fan_draw_compiled(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&security_fan_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y + work->timer);
    njDirectDrawObject(builds[0].built);
    njPopMatrix(1);
}
