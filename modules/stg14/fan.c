/* the fan, FUN in Radical Highway's object list: a stand whose blades turn
 * about y. the players can stand on the blades. it is shared/street_fan.c's
 * fan with another sound, and blades that stop with time */
#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "sound/sound.h"
#include "stg14/fan.h"
#include "stg14/fan/text.inc"
#include "system/game_state.h"
#include "system/task.h"

/* the blades' disc, a cylinder centered DISC_RISE above pos */
#define DISC_RISE 50.0F
#define DISC_RADIUS 40.0F
#define DISC_HALF_HEIGHT 5.0F
#define FAN_SHAPES 1

/* the fan's sound, which each step that turns the blades plays for
 * FAN_SOUND_DURATION + 1 sound updates */
#define FAN_SOUND 6
#define FAN_SOUND_PRIORITY 1
#define FAN_SOUND_VOLUME 0
#define FAN_SOUND_DURATION 5

#include "shared/road_furniture/fan_art.h"

NJS_TEXLIST *highway_fan_texlists[2] = {
#include "stg14/fan/highway_fan_texlists.inc"
};
/* clang-format off */
static struct collision_shape fan_shapes[FAN_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, DISC_RISE, 0.0F}, DISC_RADIUS, DISC_HALF_HEIGHT, 0.0F, 0,
     {0, 0, 0}},
};
/* clang-format on */
/* the blades' turn each step */
static Angle fan_turn = 0x180;

static void fan_destroy(struct task *task);
static void fan_draw(struct task *task);
static void fan_step(struct task *task);

void highway_fan_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its SET editor
     * caption, which SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->step = fan_step;
    task->draw = fan_draw;
    task->destructor = fan_destroy;
    collision_init(task, fan_shapes, FAN_SHAPES, COLLISION_OBJECT);
}

static void fan_destroy(struct task *task)
{
}

static void fan_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    njSetTexture(&street_fan_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: object, a pointer that walks from the stand to the
     * blades, where street_fan_object read in place would do */
    object = &street_fan_object;
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateY(NULL, work->timer);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}

static void fan_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    collision_register(task);
    if (!game_state.time_stopped) {
        work->timer += fan_turn;
        sound_play_at_for(FAN_SOUND, work, FAN_SOUND_PRIORITY, FAN_SOUND_VOLUME,
                          FAN_SOUND_DURATION, &work->pos);
    }
}
