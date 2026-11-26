/* the stone head, STONEHEAD in Dry Lagoon's object list: a head that a
 * player pushes along a rail */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "stg18/lagoon_util.h"
#include "stg18/stone_head.h"
#include "system/angle.h"
#include "system/task.h"

/* the rail's record, in its task's extra. span runs from start to end */
struct stone_head_rail {
    NJS_POINT3 start;
    NJS_POINT3 end;
    NJS_VECTOR span;
};
#define RAIL(task) ((struct stone_head_rail *)(task)->extra.p)

#define SCALE(rail) ((rail)->work->scl.x + 1.0F)

/* the rail runs along the z axis its work's ang.y turns. its middle lies
 * SCALE * RAIL_OFFSET along the axis from pos. each end lies
 * SCALE * (RAIL_STEP * (scl.y + 1) - HALF_STEP) from the middle */
#define RAIL_STEP 20.0F
#define HALF_STEP (RAIL_STEP / 2)
#define RAIL_OFFSET 12.0F
/* the head draws centered SCALE * HEAD_OFFSET behind its work's pos */
#define HEAD_OFFSET 12.0F
#define HOLD_DISTANCE 1.0F

/* a place on the rail runs from 0 at the start to LAST_PLACE at the end. the
 * placement's state keeps the head's place from LEAST_KEPT_PLACE up. a state
 * of 0 starts the head at ang.z's place instead */
#define LAST_PLACE 255
#define LEAST_KEPT_PLACE 1
#define PLACE_SCALE (1.0F / LAST_PLACE)

#define HEAD_SHAPES 2
enum stone_head_shape { BODY, BLOCK };
/* shape BLOCK's center moves BLOCK_BACK further back once scaled */
#define BLOCK_BACK 2.0F

#include "stg18/stone_head/text.inc"

static NJS_TEXNAME lagoon_stone_head_texlist_names[3] = {
#include "stg18/stone_head/lagoon_stone_head_texlist_names.inc"
};

static NJS_TEXLIST lagoon_stone_head_texlist = {
#include "stg18/stone_head/lagoon_stone_head_texlist.inc"
};

static Sint16 lagoon_stone_head_polygons[665] = {
#include "stg18/stone_head/lagoon_stone_head_polygons.inc"
};

#include "shared/canyon_art/geometry/stone_head_vertices.h"

static NJS_CNK_MODEL lagoon_stone_head_model = {
#include "stg18/stone_head/lagoon_stone_head_model.inc"
};

static NJS_CNK_OBJECT lagoon_stone_head_object = {
#include "stg18/stone_head/lagoon_stone_head_object.inc"
};

static NJS_TEXNAME lagoon_stone_head_rail_texlist_names[2] = {
#include "stg18/stone_head/lagoon_stone_head_rail_texlist_names.inc"
};

static NJS_TEXLIST lagoon_stone_head_rail_texlist = {
#include "stg18/stone_head/lagoon_stone_head_rail_texlist.inc"
};

#include "shared/canyon_art/geometry/stone_head_rail.h"

static NJS_CNK_MODEL lagoon_stone_head_rail_model = {
#include "stg18/stone_head/lagoon_stone_head_rail_model.inc"
};

static NJS_CNK_OBJECT lagoon_stone_head_rail_object = {
#include "stg18/stone_head/lagoon_stone_head_rail_object.inc"
};

static void rail_destroy(struct task *task);
static void rail_step(struct task *task);
static void rail_draw(struct task *task);
static void head_init(struct task *head);
static void head_destroy(struct task *head);
static void head_step(struct task *head);
static void head_draw(struct task *head);
static void head_keep_on_rail(struct task *head, struct task *rail);
static void head_move_holder(struct task *head);
static void rail_set_ends(struct task *rail);
static NJS_POINT3 head_center(NJS_POINT3 *pos, Angle ang, Float scale);
static void head_place(struct task *head, struct task *rail, int place);
static void head_save_place(struct task *head, struct task *rail);

NJS_TEXLIST *lagoon_stone_head_texlists[3] = {
#include "stg18/stone_head/lagoon_stone_head_texlists.inc"
};
/* the shapes at a scale of 1. an attack of 0 lies above no guard, so their
 * SHAPE_HURTS_ALL hurts nothing */
static struct collision_shape head_shapes[HEAD_SHAPES] = {
#include "stg18/stone_head/head_shapes.inc"
};

void lagoon_stone_head_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->extra.p = syMalloc(sizeof(struct stone_head_rail));
    if (!task->extra.p) {
        task_destroy(task);
        return;
    }
    if (!task_create_child(TASK_MOTION | TASK_WORK, head_init, task)) {
        task_destroy(task);
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's order
     * would set the step first */
    task->draw = rail_draw;
    task->step = rail_step;
    task->destructor = rail_destroy;
    rail_set_ends(task);
}

static void rail_destroy(struct task *task)
{
    /* @hack spelling: head, a local for task's child, where task->child
     * passed in place would do */
    struct task *head = task->child;

    head_save_place(head, task);
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void rail_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, where the
     * call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void rail_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: task->work read again for scl, where work would do */
    njScale(NULL, SCALE(task), 1.0F, SCALE(task) * (task->work->scl.y + 1.0F));
    njSetTexture(&lagoon_stone_head_rail_texlist);
    njCnkEasyDrawModel(&lagoon_stone_head_rail_model);
    njPopMatrixEx();
}

static void head_init(struct task *head)
{
    struct task *rail = head->parent;
    struct task_work *work = head->work;
    struct collision_shape shapes[HEAD_SHAPES];

    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    head->draw = head_draw;
    head->step = head_step;
    head->destructor = head_destroy;
    /* @hack spelling: head->work read again, where work would do */
    head->work->ang = rail->work->ang;
    if (placement_get_state(rail) == 0) {
        head_place(head, rail, (Uint8)rail->work->ang.z);
    } else {
        head_place(head, rail, (Uint8)placement_get_state(rail));
    }
    shapes[BODY] = head_shapes[BODY];
    shapes[BLOCK] = head_shapes[BLOCK];
    shapes[BODY].radius *= SCALE(rail);
    shapes[BODY].size_y *= SCALE(rail);
    shapes[BODY].center.y *= SCALE(rail);
    shapes[BODY].center.z *= SCALE(rail);
    shapes[BLOCK].radius *= SCALE(rail);
    shapes[BLOCK].size_y *= SCALE(rail);
    shapes[BLOCK].size_z *= SCALE(rail);
    shapes[BLOCK].center.y *= SCALE(rail);
    shapes[BLOCK].center.z =
        (shapes[BLOCK].center.z * SCALE(rail)) - BLOCK_BACK;
    collision_init(head, shapes, HEAD_SHAPES, COLLISION_OBJECT);
    work->flags |= FALL_OFFERABLE;
    work->id = FALL_KIND_11;
}

static void head_destroy(struct task *head)
{
}

static void head_step(struct task *head)
{
    head_keep_on_rail(head, head->parent);
    head_move_holder(head);
    fall_step_medium(head);
}

static void head_draw(struct task *head)
{
    NJS_POINT3 pos;

    pos = head_center(&head->work->pos, head->work->ang.y, SCALE(head->parent));
    njPushMatrixEx();
    njTranslateEx(&pos);
    njRotateY(NULL, head->work->ang.y);
    njSetTexture(&lagoon_stone_head_texlist);
    lagoon_model_draw_scaled(&lagoon_stone_head_model, SCALE(head->parent));
    njPopMatrixEx();
}

/* move the head's pos in x and z to the nearest point of the rail. past an
 * end, pos takes the end's y too */
static void head_keep_on_rail(struct task *head, struct task *rail)
{
    struct task_work *work = head->work;
    NJS_VECTOR offset;
    NJS_VECTOR span;
    Float length_squared;
    Float along;
    Float share;

    span.x = RAIL(rail)->span.x;
    span.z = RAIL(rail)->span.z;
    length_squared = (span.x * span.x) + (span.z * span.z);
    offset.x = work->pos.x - RAIL(rail)->start.x;
    offset.z = work->pos.z - RAIL(rail)->start.z;
    along = (offset.x * span.x) + (offset.z * span.z);
    if (along > 0.0F) {
        if (along >= length_squared) {
            work->pos = RAIL(rail)->end;
        } else {
            share = along / length_squared;
            work->pos.x = RAIL(rail)->start.x + (share * span.x);
            work->pos.z = RAIL(rail)->start.z + (share * span.z);
        }
    } else {
        work->pos = RAIL(rail)->start;
    }
}

static void head_move_holder(struct task *head)
{
    struct task_work *work = head->work;
    Float x;
    Float z;

    if ((work->collision->flags & COLLISION_TOUCHED) &&
        collision_next_player(head) && player_records[0]->held == head) {
        player_set_angles(0, 0, work->ang.y ^ HALF_TURN, 0);
        lagoon_polar_xz(work->ang.y, HOLD_DISTANCE, &x, &z);
        player_set_pos(0, work->pos.x + x, work->pos.y, work->pos.z + z);
    }
}

static void rail_set_ends(struct task *rail)
{
    struct task_work *work = rail->work;
    Float middle_x;
    Float middle_z;
    Float half_x;
    Float half_z;

    lagoon_polar_xz(work->ang.y, (work->scl.x + 1.0F) * RAIL_OFFSET, &middle_x,
                    &middle_z);
    /* @hack spelling: rail->work read again for scl, where work would do */
    lagoon_polar_xz(work->ang.y,
                    (SCALE(rail) * (rail->work->scl.y + 1.0F) * RAIL_STEP) -
                        (SCALE(rail) * HALF_STEP),
                    &half_x, &half_z);
    RAIL(rail)->start.x = work->pos.x + middle_x + half_x;
    RAIL(rail)->start.y = work->pos.y;
    RAIL(rail)->start.z = work->pos.z + middle_z + half_z;
    RAIL(rail)->end.x = work->pos.x + middle_x - half_x;
    RAIL(rail)->end.y = work->pos.y;
    RAIL(rail)->end.z = work->pos.z + middle_z - half_z;
    RAIL(rail)->span.x = RAIL(rail)->end.x - RAIL(rail)->start.x;
    RAIL(rail)->span.y = RAIL(rail)->end.y - RAIL(rail)->start.y;
    RAIL(rail)->span.z = RAIL(rail)->end.z - RAIL(rail)->start.z;
}

static NJS_POINT3 head_center(NJS_POINT3 *pos, Angle ang, Float scale)
{
    NJS_POINT3 center;
    Float x;
    Float z;

    center = *pos;
    lagoon_polar_xz(ang, -(scale * HEAD_OFFSET), &x, &z);
    center.x += x;
    center.z += z;
    return center;
}

static void head_place(struct task *head, struct task *rail, int place)
{
    struct stone_head_rail *ends = RAIL(rail);

    head->work->pos.x = ends->start.x + (ends->span.x * place * PLACE_SCALE);
    head->work->pos.y = ends->start.y;
    head->work->pos.z = ends->start.z + (ends->span.z * place * PLACE_SCALE);
}

static void head_save_place(struct task *head, struct task *rail)
{
    struct stone_head_rail *ends = RAIL(rail);
    int place;

    if (ends->start.x != ends->end.x) {
        place = (head->work->pos.x - ends->start.x) / ends->span.x * LAST_PLACE;
    } else {
        place = (head->work->pos.z - ends->start.z) / ends->span.z * LAST_PLACE;
    }
    if (place <= 0) {
        place = LEAST_KEPT_PLACE;
    }
    if (place > LAST_PLACE) {
        place = LAST_PLACE;
    }
    placement_set_state(rail, place);
}
