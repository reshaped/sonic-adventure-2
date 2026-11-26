/* the fan, FUN in Mission Street's object list: a stand whose blades turn
 * about y. the players can stand on the blades */
#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/street_fan.h"
#include "shared/street_fan/text.inc"
#include "sound/sound.h"
#include "system/task.h"

/* the blades' disc, a cylinder centered DISC_RISE above pos */
#define DISC_RISE 50.0F
#define DISC_RADIUS 40.0F
#define DISC_HALF_HEIGHT 5.0F
#define FAN_SHAPES 1

/* the fan's sound, which each step plays for FAN_SOUND_DURATION + 1 sound
 * updates */
#define FAN_SOUND 14
#define FAN_SOUND_PRIORITY 1
#define FAN_SOUND_VOLUME 0
#define FAN_SOUND_DURATION 10

#include "shared/road_furniture/fan_art.h"

static void street_fan_destroy(struct task *task);
static void street_fan_draw(struct task *task);
static void street_fan_step(struct task *task);

NJS_TEXLIST *street_fan_texlists[2] = {
#include "shared/street_fan/street_fan_texlists.inc"
};
/* clang-format off */
static struct collision_shape street_fan_shapes[FAN_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, DISC_RISE, 0.0F}, DISC_RADIUS, DISC_HALF_HEIGHT, 0.0F, 0,
     {0, 0, 0}},
};
/* clang-format on */
/* the blades' turn each step */
static Angle street_fan_turn = 0x180;

void street_fan_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its string, which SHC
     * still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    if (!placement_try_leave(task)) {
        task->step = street_fan_step;
        task->draw = street_fan_draw;
        task->destructor = street_fan_destroy;
        collision_init(task, street_fan_shapes, FAN_SHAPES, COLLISION_OBJECT);
    }
}

static void street_fan_destroy(struct task *task)
{
}

static void street_fan_draw(struct task *task)
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

static void street_fan_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: the else, where the code after the test would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        collision_register(task);
        work->timer += street_fan_turn;
        sound_play_at_for(FAN_SOUND, work, FAN_SOUND_PRIORITY, FAN_SOUND_VOLUME,
                          FAN_SOUND_DURATION, &work->pos);
    }
}
