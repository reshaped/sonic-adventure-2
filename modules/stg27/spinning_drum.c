/* CONT, a drum that spins about its length */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg27/spinning_drum/text.inc"
#include "collision/collision.h"
#include "object/set.h"
#include "stg27/lost_colony.h"
#include "system/angle.h"
#include "system/task.h"

/* the drum's turn about its length, in work->timer */
#define SPIN(work) ((work)->timer)

/* the set editor's labels for the drum's ang.y and scl.x */
static const char editor_rotation[12] = "<- ROT Y";
static const char editor_speed[12] = "<- ROT SPD";

static NJS_TEXNAME spinning_drum_names[9] = {
#include "stg27/spinning_drum/spinning_drum_names.inc"
};

static NJS_TEXLIST spinning_drum_texlist = {
#include "stg27/spinning_drum/spinning_drum_texlist.inc"
};

static Sint16 spinning_drum_polygons[557] = {
#include "stg27/spinning_drum/spinning_drum_polygons.inc"
};

static Sint32 spinning_drum_vertices[527] = {
#include "stg27/spinning_drum/spinning_drum_vertices.inc"
};

static NJS_CNK_MODEL spinning_drum_model = {
#include "stg27/spinning_drum/spinning_drum_model.inc"
};

static NJS_CNK_OBJECT spinning_drum_object = {
#include "stg27/spinning_drum/spinning_drum_object.inc"
};

NJS_TEXLIST *spinning_drum_texlists[2] = {
#include "stg27/spinning_drum/spinning_drum_texlists.inc"
};

/* clang-format off */
static struct collision_shape drum_shape = {
    0, SHAPE_CYLINDER2, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    0, 0, {0.0F, 0.0F, 0.0F}, 50.0F, 85.0F, 0.0F, 0, {0, 0, QUARTER_TURN},
};
/* clang-format on */

static void spinning_drum_destroy(struct task *task);
static void spinning_drum_step(struct task *task);
static void spinning_drum_draw(struct task *task);

void spinning_drum_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = spinning_drum_draw;
    task->step = spinning_drum_step;
    task->destructor = spinning_drum_destroy;
    collision_init(task, &drum_shape, 1, COLLISION_OBJECT);
}

static void spinning_drum_destroy(struct task *task)
{
}

static void spinning_drum_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * step the image's registers */
    if (0) {
        if (work) {
        }
    }
    collision_register(task);
    SPIN(work) += (Angle)(work->scl.x * ANGLE_PER_DEGREE);
}

static void spinning_drum_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = &spinning_drum_object;

    njSetTexture(&spinning_drum_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, SPIN(work));
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}
