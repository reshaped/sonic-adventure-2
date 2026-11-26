/* the rolling drum, EECONT in Crazy Gadget's and Eternal Engine's object
 * lists */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "draw/chunk.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/rolling_drum.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the set editor's labels, which no code reads */
static const char editor_rotation[12] = "<- ROT Y";
static const char editor_travel[12] = "<- LEN-TRNS";
static const char editor_length[8] = "<- LEN";
static const char editor_speed[8] = "<- SPD";

/* how far the drum has rolled along its track, from 0 to the track's length.
 * the record at task->motion holds it */
#define DISTANCE(task) (*(Float *)(task)->motion.p)
/* the track's length */
#define TRACK_LENGTH(work) ((work)->scl.y + ((work)->scl.x * 2.0F))
/* the drum's x from pos in the track's frame: half the track's length, less
 * DISTANCE */
#define DRUM_X(task, work)                                                     \
    (-DISTANCE(task) + (work)->scl.x + ((work)->scl.y * 0.5F))
/* the drum's opacity, from 0 to 1, in the task's unk_3c */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define OPACITY(task) (*(Float *)&(task)->unk_3c)
/* the drum draws opaque, without blending, while its opacity is above
 * SOLID_OPACITY */
#define SOLID_OPACITY 0.99F
#define WHITE_CLEAR 0x00ffffff
#define WHITE_OPAQUE 0xffffffff

/* the drum's cylinder. its own quarter turn about z lays its axis along the
 * track */
/* clang-format off */
static struct collision_shape drum_shape = {
    0, SHAPE_CYLINDER2, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    0, 0, {0.0F, 0.0F, 0.0F}, 50.0F, 85.0F, 0.0F, 0, {0, 0, QUARTER_TURN}};
/* clang-format on */
/* the drum's turn about x a step, in Angle units */
static Angle spin = 0x100;

static void rolling_drum_destroy(struct task *task);
static void rolling_drum_step(struct task *task);
static void rolling_drum_draw(struct task *task);

/* move the drum's child, which carries its cylinder, to offset turned and
 * placed as the work is */
/* @hack spelling: a macro written out at both sites, where a function under
 * #pragma inline would do */
#define PLACE_COLLISION(task, work, offset)                                    \
    do {                                                                       \
        (offset).y = 0.0F;                                                     \
        (offset).z = 0.0F;                                                     \
        njPushMatrixEx();                                                      \
        njUnitMatrix(NULL);                                                    \
        njTranslateEx(&(work)->pos);                                           \
        njRotateY(NULL, (work)->ang.y);                                        \
        njCalcPoint(NULL, &(offset), &(task)->child->work->pos);               \
        njPopMatrixEx();                                                       \
    } while (0)

void rolling_drum_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;
    NJS_POINT3 offset;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(Float));
    if (!task->motion.p) {
        return;
    }
    task->draw = rolling_drum_draw;
    task->step = rolling_drum_step;
    task->destructor = rolling_drum_destroy;
    child = model_object_create(task, NULL, NULL, &drum_shape, 1, 0);
    if (child) {
        child->work->ang.y = work->ang.y;
        child->work->ang.x = 0;
        child->work->ang.z = 0;
    }
    if (task->child) {
        offset.x = -DISTANCE(task);
        PLACE_COLLISION(task, work, offset);
    }
}

static void rolling_drum_destroy(struct task *task)
{
    /* @hack spelling: drum, a second name for task, where task would do */
    struct task *drum = task;

    syFree(drum->motion.p);
    drum->motion.p = NULL;
}

static void rolling_drum_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 offset;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: three calls to matrix_load_parent, in a block no path
     * reaches */
    if (0) {
        matrix_load_parent();
        matrix_load_parent();
        matrix_load_parent();
    }
    DISTANCE(task) += work->scl.z;
    if (DISTANCE(task) < 0.0F) {
        DISTANCE(task) = TRACK_LENGTH(work);
    } else if (DISTANCE(task) > TRACK_LENGTH(work)) {
        DISTANCE(task) = 0.0F;
    }
    if (DISTANCE(task) < work->scl.x) {
        OPACITY(task) = DISTANCE(task) / work->scl.x;
    } else if (DISTANCE(task) > work->scl.x + work->scl.y) {
        OPACITY(task) =
            1.0F - ((DISTANCE(task) - work->scl.x - work->scl.y) / work->scl.x);
    } else {
        OPACITY(task) = 1.0F;
    }
    if (task->child) {
        offset.x = DRUM_X(task, work);
        PLACE_COLLISION(task, work, offset);
    }
}

static void rolling_drum_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_POINT3 offset;
    /* @hack type: color an NJS_COLOR, where a Uint32 would do */
    NJS_COLOR color;

    njFogDisable();
    /* @hack redundant: a translation by offset, which nothing sets, in a
     * block no path reaches */
    if (0) {
        njTranslateEx(&offset);
    }
    object = &rolling_drum_object;
    njSetTexture(&rolling_drum_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslate(NULL, DRUM_X(task, work), 0.0F, 0.0F);
    njRotateX(NULL, game_state.steps * spin);
    if (OPACITY(task) > SOLID_OPACITY) {
        njCnkEasyDrawModel(object->model);
    } else {
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        color.color = color_blend(WHITE_CLEAR, WHITE_OPAQUE, OPACITY(task));
        model_draw_tinted(object->model, color.color);
        draw_attr_restore();
        draw_control_restore();
    }
    njPopMatrixEx();
    njFogEnable();
}
