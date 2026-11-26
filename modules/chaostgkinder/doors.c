#include <ninja.h>
#include "chaostgkinder/door_art.h"

#include "chaostgkinder/objects.h"
#include "system/task.h"

#define DOOR_SLOTS 8
#define DOOR_MODELS 8
#define DOOR_RATE 0.3F
#define DOOR_OPEN_FRAME 7.0F

struct door_animation {
    NJS_CNK_OBJECT *object;
    NJS_MOTION *motion;
};
/* each door's model and motion, by its placement's ang.z */
static const struct door_animation actions[DOOR_MODELS] = {
#include "chaostgkinder/doors/actions.inc"
};

enum door_mode { DOOR_START, DOOR_RUN };
enum door_state { DOOR_CLOSED, DOOR_OPENING, DOOR_OPEN, DOOR_CLOSING };

static struct task *doors[DOOR_SLOTS];

struct task *kinder_door(int slot)
{
    return doors[slot];
}

static void draw_door(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&kinder_object_texlist);
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawMotion(actions[work->ang.z].object,
                        actions[work->ang.z].motion, work->scl.x);
    njPopMatrixEx();
}

static void door_destroy(struct task *task)
{
}

void kinder_door_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: a switch of one case, where an if would do */
    switch (work->mode) {
    case DOOR_START:
        task->draw = draw_door;
        task->destructor = door_destroy;
        doors[work->ang.x] = task;
        work->mode = DOOR_RUN;
        break;
    }
    switch (work->submode) {
    case DOOR_CLOSED:
    case DOOR_OPEN:
        break;
    case DOOR_OPENING:
        if ((work->scl.x += DOOR_RATE) > DOOR_OPEN_FRAME) {
            work->scl.x = DOOR_OPEN_FRAME;
            work->submode = DOOR_OPEN;
        }
        break;
    case DOOR_CLOSING:
        if ((work->scl.x -= DOOR_RATE) < 0.0F) {
            work->scl.x = 0.0F;
            work->submode = DOOR_CLOSED;
        }
        break;
    }
}

void kinder_door_open(struct task *task)
{
    task->work->submode = DOOR_OPENING;
}

void kinder_door_close(struct task *task)
{
    task->work->submode = DOOR_CLOSING;
}
