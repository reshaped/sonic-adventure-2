/* the roller, ROLL in Prison Lane's object list: a drum that turns on its
 * frame */
#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "sound/sound.h"
#include "stg09/roller.h"
#include "system/angle.h"
#include "system/task.h"

#include "stg09/roller/text.inc"

/* the drum's sound, an id of stg09/sound_point.h's prison_lane_sounds.
 * ROLL_SOUND_UPDATES is sound_play_at_for's duration, in sound updates */
#define ROLL_SOUND 12
#define ROLL_SOUND_PRIORITY 1
#define ROLL_SOUND_VOLUME 0
#define ROLL_SOUND_UPDATES 20

#define ROLLER_SHAPES 1

/* the frame's node, whose child is the drum */
static NJS_TEXLIST roller_texlist;
static NJS_CNK_OBJECT roller_object;

static struct collision_shape roller_shapes[ROLLER_SHAPES];

static void roller_destroy(struct task *task);
static void roller_step(struct task *task);
static void roller_draw(struct task *task);

void roller_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = roller_draw;
    task->step = roller_step;
    task->destructor = roller_destroy;
    collision_init(task, roller_shapes, ROLLER_SHAPES, COLLISION_OBJECT);
}

static void roller_destroy(struct task *task)
{
}

static void roller_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: the turn made inside the test, with the return in an
     * else, where a return first would do */
    if (!placement_try_leave(task)) {
        work->timer += (Angle)(work->scl.x * ANGLE_PER_DEGREE);
    } else {
        return;
    }
    collision_register(task);
    work->collision->shapes->ang.y = work->ang.y;
    sound_play_at_for(ROLL_SOUND, work, ROLL_SOUND_PRIORITY, ROLL_SOUND_VOLUME,
                      ROLL_SOUND_UPDATES, &work->pos);
}

static void roller_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *drum;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)ROLLER_EDITOR_ROT_X;
        (void)ROLLER_EDITOR_ROT_Y;
        (void)ROLLER_EDITOR_ROT_Z;
        (void)ROLLER_EDITOR_ROTSPD;
    }
    njSetTexture(&roller_texlist);
    object = &roller_object;
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njCnkEasyDrawModel(object->model);
    njRotateZ(NULL, work->timer);
    drum = object->child;
    njCnkEasyDrawModel(drum->model);
    njPopMatrix(1);
}

#include "stg09/fixtures/roller.h"

NJS_TEXLIST *roller_texlists[2] = {
#include "stg09/roller/roller_texlists.inc"
};
/* the drum. roller_step gives the record's copy the work's ang.y at each
 * step */
static struct collision_shape roller_shapes[ROLLER_SHAPES] = {
#include "stg09/roller/roller_shapes.inc"
};
