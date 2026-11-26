/* the hanging ring, HANGRING in Pyramid Cave's object list: a ring turning on
 * an axle between two ends */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "stg28/hang_ring.h"
#include "system/angle.h"
#include "system/math.h"
#include "system/task.h"

/* the shapes of the axle's halves and its ends, on +x and on -x */
#define AXLE_SHAPES 4
enum hang_ring_axle_shape { AXLE_POS_X, AXLE_NEG_X, END_POS_X, END_NEG_X };
/* the ring collides as PARTS child tasks. each holds PART_SHAPES of its
 * cylinders, on opposite sides of the ring. each part lies PART_TURN about z
 * from the last */
#define PART_SHAPES 2
#define PARTS 5
#define PART_TURN (FULL_TURN / (PARTS * PART_SHAPES))
/* a part's turn about z, in its task's unk_3c */
#define PART_ANGLE(task) ((task)->unk_3c.l)

#define SPIN_SCALE 256.0F

/* each half of the axle runs along x from RING_EDGE out to its end. it is
 * AXLE_LENGTH + scl.x long */
#define RING_EDGE 22.0F
#define AXLE_LENGTH 38.0F

#include "stg28/hang_ring/text.inc"
#include "stg28/models/hang_ring.h"

static void hang_ring_destroy(struct task *task);
static void hang_ring_step(struct task *task);
static void hang_ring_draw(struct task *task);
static void hang_ring_step_part(struct task *part);

NJS_TEXLIST *cave_hang_ring_texlists[4] = {
#include "stg28/hang_ring/cave_hang_ring_texlists.inc"
};
/* a part's two cylinders, each along the ring's edge */
/* clang-format off */
static struct collision_shape hang_ring_part_shapes[PART_SHAPES] = {
#include "stg28/hang_ring/hang_ring_part_shapes.inc"
};
/* the axle's halves and its ends at a scl.x of 0, which the init lengthens and
 * moves out */
static struct collision_shape hang_ring_axle_shapes[AXLE_SHAPES] = {
#include "stg28/hang_ring/hang_ring_axle_shapes.inc"
};
/* clang-format on */

void cave_hang_ring_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *parts[PARTS];
    struct collision_shape shapes[AXLE_SHAPES];
    int i;
    Angle angle;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = hang_ring_draw;
    task->step = hang_ring_step;
    task->destructor = hang_ring_destroy;
    shapes[AXLE_POS_X] = hang_ring_axle_shapes[AXLE_POS_X];
    shapes[AXLE_NEG_X] = hang_ring_axle_shapes[AXLE_NEG_X];
    shapes[END_POS_X] = hang_ring_axle_shapes[END_POS_X];
    shapes[END_NEG_X] = hang_ring_axle_shapes[END_NEG_X];
    /* @hack spelling: the work read again through task, where work would do */
    shapes[AXLE_POS_X].size_y += 0.5F * task->work->scl.x;
    shapes[AXLE_POS_X].center.x += 0.5F * task->work->scl.x;
    shapes[AXLE_NEG_X].size_y += 0.5F * task->work->scl.x;
    /* @hack spelling: the half of scl.x added negated, where -= would do */
    shapes[AXLE_NEG_X].center.x += -(0.5F * task->work->scl.x);
    shapes[END_POS_X].center.x += task->work->scl.x;
    /* @hack spelling: scl.x added negated, where -= would do */
    shapes[END_NEG_X].center.x += -task->work->scl.x;
    collision_init(task, shapes, AXLE_SHAPES, COLLISION_OBJECT);
    angle = 0;
    for (i = 0; i < PARTS; i++) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((parts[i] = task_create_child(TASK_WORK, hang_ring_step_part,
                                          task)) == NULL) {
            break;
        }
        PART_ANGLE(parts[i]) = angle;
        collision_init(parts[i], hang_ring_part_shapes, PART_SHAPES,
                       COLLISION_OBJECT);
        /* @hack spelling: angle set to itself plus PART_TURN, where += would
         * do */
        angle = angle + PART_TURN;
    }
    task->unk_3c.l = work->ang.x;
}

static void hang_ring_destroy(struct task *task)
{
}

static void hang_ring_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    collision_register(task);
    /* @hack spelling: the work read again through task, where work would do */
    work->ang.x += task->work->scl.z * SPIN_SCALE;
}

static void hang_ring_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: the work read again through task, where work would do */
    Float length = task->work->scl.x + AXLE_LENGTH;
    Float stretch = (task->work->scl.x + AXLE_LENGTH) * (1.0F / AXLE_LENGTH);

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njSetTexture(&cave_hang_ring_end_texlist);
    njPushMatrixEx();
    njTranslate(NULL, -RING_EDGE - length, 0.0F, 0.0F);
    njCnkEasyDrawModel(&cave_hang_ring_end_model);
    njPopMatrixEx();
    njPushMatrixEx();
    njRotateY(NULL, HALF_TURN);
    njTranslate(NULL, -RING_EDGE - length, 0.0F, 0.0F);
    njCnkEasyDrawModel(&cave_hang_ring_end_model);
    njPopMatrixEx();
    njRotateX(NULL, work->ang.x);
    njSetTexture(&cave_hang_ring_axle_texlist);
    njPushMatrixEx();
    njTranslate(NULL, -RING_EDGE, 0.0F, 0.0F);
    njScale(NULL, stretch, 1.0F, 1.0F);
    njCnkEasyDrawModel(&cave_hang_ring_axle_model);
    njPopMatrixEx();
    njPushMatrixEx();
    njRotateY(NULL, HALF_TURN);
    njTranslate(NULL, -RING_EDGE, 0.0F, 0.0F);
    njScale(NULL, stretch, 1.0F, 1.0F);
    njCnkEasyDrawModel(&cave_hang_ring_axle_model);
    njPopMatrixEx();
    njSetTexture(&cave_hang_ring_texlist);
    njCnkEasyDrawModel(&cave_hang_ring_model);
    njPopMatrixEx();
}

static void hang_ring_step_part(struct task *part)
{
    struct task *ring = part->parent;
    struct task_work *work = part->work;
    struct task_work *ring_work = ring->work;

    work->pos = ring_work->pos;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, ring_work->ang.y);
    njRotateZ(NULL, ring_work->ang.z);
    njRotateX(NULL, ring_work->ang.x);
    njRotateZ(NULL, PART_ANGLE(part));
    current_matrix_angles_xzy(&work->ang.x, &work->ang.y, &work->ang.z);
    njPopMatrixEx();
    collision_register(part);
}
