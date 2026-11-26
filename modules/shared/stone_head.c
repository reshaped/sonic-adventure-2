/* the stone head, STONEHEAD in Wild Canyon's object list: a rail with a stone
 * head on it. a player who holds the head pushes it along the rail */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/canyon_util.h"
#include "shared/dust.h"
#include "shared/stone_head.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the rail's record, in its task's extra: its two ends, and the span from
 * start to end */
struct stone_head_rail {
    NJS_POINT3 start;
    NJS_POINT3 end;
    NJS_VECTOR span;
};
#define RAIL(task) ((struct stone_head_rail *)(task)->extra.p)
/* the head's TASK_FORCE records. a player who rides the head moves with
 * their speed */
#define FORCES(head) ((struct task_force *)(head)->force.p)

/* the scale of the rail and its head, from the rail's work */
#define SCALE(rail) ((rail)->work->scl.x + 1.0F)

/* the rail runs along the work's ang.y. its middle lies SCALE * RAIL_OFFSET
 * along it from the work's pos. each end lies SCALE * (RAIL_STEP * (1 +
 * scl.y) - HALF_STEP) from the middle */
#define RAIL_STEP 20.0F
#define HALF_STEP 10.0F
#define RAIL_OFFSET 12.0F
/* the head's center lies SCALE * HEAD_OFFSET behind its work's pos, along
 * its ang.y */
#define HEAD_OFFSET 12.0F

/* a place on the rail runs from 0 at the start to LAST_PLACE at the end. the
 * placement keeps the head's place, from 1 to LAST_PLACE, as its state */
#define LAST_PLACE 255
#define PLACE_SCALE (1.0F / LAST_PLACE)

/* the head's flags in the world, and its shapes in the collision checks */
#define HEAD_WORLD                                                             \
    (WORLD_SIZE_CLASS(2) | WORLD_MOVING | WORLD_NO_CLIMB | WORLD_SOLID)
#define HEAD_SHAPES 2
enum stone_head_shape { BODY, BLOCK };
/* shape BLOCK, once scaled, shrinks by BLOCK_INSET in each size and in its
 * center's height. its center moves BLOCK_BACK further back too */
#define BLOCK_INSET 0.1F
#define BLOCK_BACK 2.0F

/* the head keeps its object in the world only while a player comes within
 * REACH of it */
#define REACH 38.3F
/* how far from the head's pos, along the head's ang.y, its holder stands */
#define HOLD_DISTANCE 1.0F
/* a head that moves more than MOVE_SQUARED, squared, in a step plays
 * SLIDE_SOUND. each such step raises dust by a chance of DUST_CHANCE. the
 * dust's rectangle reaches DUST_SIZE * SCALE from its center along x and z.
 * it lies DUST_RISE * SCALE above the head's center */
#define MOVE_SQUARED 0.001F
#define SLIDE_SOUND 5
#define DUST_CHANCE 0.05F
#define DUST_SIZE 11.0F
#define DUST_RISE 1.0F
#define DUST_SCALE 2.0F
#define DUST_DENSITY 1.0F
#define DUST_SPEED 0.1F

#include "shared/stone_head/text.inc"

/* the placement editor's labels for ang.y, the low byte of ang.z, scl.x and
 * scl.y. no code reads them. each size rounds its label up to whole words, so
 * what follows starts on a word boundary */
static const char stone_head_edit_yaw[20] = "<- ANGLE Y (   )";
static const char stone_head_edit_starting_point[20] = "<- START POS(00~FF)";
static const char stone_head_edit_head_scale[16] = "<- SCALE(1.0+x)";
static const char stone_head_edit_rail_scale[24] = "<- RAIL SCALE(1.0+x)";

static NJS_TEXNAME stone_head_texlist_names[3] = {
#include "shared/stone_head/stone_head_texlist_names.inc"
};

static NJS_TEXLIST stone_head_texlist = {
#include "shared/stone_head/stone_head_texlist.inc"
};

static Sint16 stone_head_polygons[665] = {
#include "shared/stone_head/stone_head_polygons.inc"
};

#include "shared/canyon_art/geometry/stone_head_vertices.h"

static NJS_CNK_MODEL stone_head_model = {
#include "shared/stone_head/stone_head_model.inc"
};

static NJS_CNK_OBJECT stone_head_object = {
#include "shared/stone_head/stone_head_object.inc"
};

static NJS_TEXNAME stone_head_rail_texlist_names[2] = {
#include "shared/stone_head/stone_head_rail_texlist_names.inc"
};

static NJS_TEXLIST stone_head_rail_texlist = {
#include "shared/stone_head/stone_head_rail_texlist.inc"
};

#include "shared/canyon_art/geometry/stone_head_rail.h"

static NJS_CNK_MODEL stone_head_rail_model = {
#include "shared/stone_head/stone_head_rail_model.inc"
};

static NJS_CNK_OBJECT stone_head_rail_object = {
#include "shared/stone_head/stone_head_rail_object.inc"
};

static NJS_MATERIAL stone_head_collision_materials = {
#include "shared/stone_head/stone_head_collision_materials.inc"
};

static Uint16 stone_head_collision_strips[19] = {
#include "shared/stone_head/stone_head_collision_strips.inc"
};

static NJS_MESHSET stone_head_collision_faces = {
#include "shared/stone_head/stone_head_collision_faces.inc"
};

static NJS_POINT3 stone_head_collision_points[8] = {
#include "shared/stone_head/stone_head_collision_points.inc"
};

static NJS_VECTOR stone_head_collision_normals[8] = {
#include "shared/stone_head/stone_head_collision_normals.inc"
};

static NJS_MODEL stone_head_collision_model = {
#include "shared/stone_head/stone_head_collision_model.inc"
};

static NJS_OBJECT stone_head_collision = {
#include "shared/stone_head/stone_head_collision.inc"
};

static void stone_head_destroy(struct task *task);
static void stone_head_step(struct task *task);
static void stone_head_draw(struct task *task);
static void stone_head_init_head(struct task *head);
static void stone_head_destroy_head(struct task *head);
static void stone_head_step_head(struct task *head);
static void stone_head_draw_head(struct task *head);
static void stone_head_keep_on_rail(struct task *head, struct task *rail);
static void stone_head_move_holder(struct task *head);
static void stone_head_set_ends(struct task *rail);
static NJS_POINT3 stone_head_center(NJS_POINT3 *pos, Angle ang, Float scale);
static void stone_head_place(struct task *head, struct task *rail, int place);
static void stone_head_save_place(struct task *head, struct task *rail);

NJS_TEXLIST *stone_head_texlists[3] = {
#include "shared/stone_head/stone_head_texlists.inc"
};
/* clang-format off */
static struct collision_shape stone_head_shapes[HEAD_SHAPES] = {
    {13, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, 0,
     {0.0F, 10.0F, 0.0F}, 6.6666665F, 10.0F, 0.0F, 0, {0, 0, 0}},
    {12, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_2000,
     {0.0F, 10.0F, -10.0F}, 10.0F, 10.0F, 10.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Sint32 stone_head_slide_volume = SOUND_LOUDEST;

void stone_head_init(struct task *task)
{
    if (!half_try_leave(task) && !placement_try_leave(task)) {
        task->extra.p = syMalloc(sizeof(struct stone_head_rail));
        if (!task->extra.p) {
            task_destroy(task);
            return;
        }
        if (!task_create_child(TASK_MOTION | TASK_WORK | TASK_FORCE,
                               stone_head_init_head, task)) {
            task_destroy(task);
            return;
        }
        task->draw = stone_head_draw;
        task->step = stone_head_step;
        task->destructor = stone_head_destroy;
        stone_head_set_ends(task);
    }
}

static void stone_head_destroy(struct task *task)
{
    /* @hack spelling: head, a local for task's child, where task->child
     * passed in place would do */
    struct task *head = task->child;

    stone_head_save_place(head, task);
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void stone_head_step(struct task *task)
{
    struct task_work *work = task->work;

    if (half_try_leave(task)) {
        return;
    }
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * step the image's frame */
    if (0) {
        int state = work->mode;

        placement_set_state(task, state);
        placement_set_state(task, state);
    }
}

static void stone_head_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, SCALE(task), 1.0F, SCALE(task) * (task->work->scl.y + 1.0F));
    njSetTexture(&stone_head_rail_texlist);
    njCnkEasyDrawModel(&stone_head_rail_model);
    njPopMatrixEx();
}

static void stone_head_init_head(struct task *head)
{
    struct task *rail = head->parent;
    struct task_work *work = head->work;
    NJS_OBJECT *object;
    struct collision_shape shapes[HEAD_SHAPES];
#ifdef NONMATCHING
    /* the rail's state, which this side reads once */
    Sint8 state;
#endif

    head->draw = stone_head_draw_head;
    head->step = stone_head_step_head;
    head->destructor = stone_head_destroy_head;
    head->work->ang = rail->work->ang;
#ifdef NONMATCHING
    /* without a placement, placement_get_state's result is undefined. the
     * image's result is the low byte of the rail's ang.y, which the copy above
     * leaves in R0. this side gives the same */
    if (rail->placement != NULL) {
        state = placement_get_state(rail);
    } else {
        state = (Sint8)rail->work->ang.y;
    }
    if (state == 0) {
        stone_head_place(head, rail, (Uint8)rail->work->ang.z);
    } else {
        stone_head_place(head, rail, (Uint8)state);
    }
#else
    if (placement_get_state(rail) == 0) {
        stone_head_place(head, rail, (Uint8)rail->work->ang.z);
    } else {
        stone_head_place(head, rail, (Uint8)placement_get_state(rail));
    }
#endif
    object = world_pool_alloc();
    head->extra.p = object;
    /* @bug memory: copies the shape through null where world_pool_alloc has
     * none to lend, though stone_head_destroy_head tests head->extra for
     * null. stone_head_step_head stores to the shape's pos untested: 0x1ea,
     * 0x3c8 and 0x4e8 of the object's code */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the head
     * takes this init as its step again, to try on its next step */
    if (object == NULL) {
        head->step = stone_head_init_head;
        head->draw = NULL;
        head->destructor = NULL;
        return;
    }
#endif
    *object = stone_head_collision;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    object->scl[0] = object->scl[1] = object->scl[2] = SCALE(rail);
    object->ang[0] = 0;
    object->ang[1] = head->work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(HEAD_WORLD, head, object);
    shapes[BODY] = stone_head_shapes[BODY];
    shapes[BLOCK] = stone_head_shapes[BLOCK];
    shapes[BODY].radius *= SCALE(rail);
    shapes[BODY].size_y *= SCALE(rail);
    shapes[BODY].center.y *= SCALE(rail);
    shapes[BODY].center.z *= SCALE(rail);
    shapes[BLOCK].radius *= SCALE(rail);
    shapes[BLOCK].size_y *= SCALE(rail);
    shapes[BLOCK].size_z *= SCALE(rail);
    shapes[BLOCK].center.y *= SCALE(rail);
    shapes[BLOCK].radius -= BLOCK_INSET;
    shapes[BLOCK].size_y -= BLOCK_INSET;
    shapes[BLOCK].size_z -= BLOCK_INSET;
    shapes[BLOCK].center.y -= BLOCK_INSET;
    shapes[BLOCK].center.z =
        (shapes[BLOCK].center.z * SCALE(rail)) - BLOCK_BACK;
    collision_init(head, shapes, HEAD_SHAPES, COLLISION_OBJECT);
    work->flags |= FALL_OFFERABLE;
    work->id = FALL_KIND_11;
}

static void stone_head_destroy_head(struct task *head)
{
    if (head->extra.p) {
        world_remove(head, head->extra.p);
        world_pool_free(head->extra.p);
    }
    head->extra.p = NULL;
}

static void stone_head_step_head(struct task *head)
{
    struct task *rail = head->parent;
    struct task_work *work = head->work;
    NJS_OBJECT *object = head->extra.p;
    NJS_POINT3 pos;
    NJS_POINT3 before;
    struct angles ang;
    Float scale;
    Float size;

    /* @hack redundant: a call of sound_stop_key that no path reaches, a
     * stand-in that gives the step the image's registers */
    if (0) {
        sound_stop_key(rail);
    }
    /* @hack redundant: an empty block no path reaches, a second such
     * stand-in */
    if (0) {
    }
    before = stone_head_center(&head->work->pos, head->work->ang.y,
                               SCALE(head->parent));
    stone_head_keep_on_rail(head, head->parent);
    stone_head_move_holder(head);
    fall_step_medium(head);
    pos = stone_head_center(&head->work->pos, head->work->ang.y,
                            SCALE(head->parent));
    FORCES(head)[0].speed.x = FORCES(head)[1].speed.x = pos.x - before.x;
    FORCES(head)[0].speed.y = FORCES(head)[1].speed.y = pos.y - before.y;
    FORCES(head)[0].speed.z = FORCES(head)[1].speed.z = pos.z - before.z;
    object->pos[0] = pos.x;
    object->pos[1] = pos.y;
    object->pos[2] = pos.z;
    if (player_near((NJS_POINT3 *)object->pos, REACH)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    if (njScalor2(&FORCES(head)[0].speed) > MOVE_SQUARED) {
        sound_play(SLIDE_SOUND, head, 0, 0);
        sound_set_volume(head, SOUND_ANY_ID, stone_head_slide_volume);
        if ((Float)rand() * RAND_SCALE < DUST_CHANCE) {
            ang.x = ang.z = 0;
            ang.y = work->ang.y;
            scale = SCALE(rail);
            size = scale * DUST_SIZE;
            pos.y += DUST_RISE * scale;
            dust_smoke_rect(&pos, size, size, &ang, SCALE(rail) * DUST_SCALE,
                            ((Float)rand() * RAND_SCALE) + DUST_DENSITY,
                            DUST_SPEED);
        }
    } else {
        sound_stop_key(head);
    }
}

static void stone_head_draw_head(struct task *head)
{
    NJS_POINT3 pos;

    pos = stone_head_center(&head->work->pos, head->work->ang.y,
                            SCALE(head->parent));
    njPushMatrixEx();
    njTranslateEx(&pos);
    njRotateY(NULL, head->work->ang.y);
    njSetTexture(&stone_head_texlist);
    canyon_model_draw_scaled(&stone_head_model, SCALE(head->parent));
    njPopMatrixEx();
}

/* move the head's pos onto the rail, to its nearest point in x and z. past
 * either end, the head takes that end's pos, y and all */
static void stone_head_keep_on_rail(struct task *head, struct task *rail)
{
    struct task_work *work = head->work;
    NJS_VECTOR offset;
    NJS_VECTOR span;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused;
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

/* turn the player in slot 0 or 1 who holds the head to face it. put the
 * player HOLD_DISTANCE from the head's pos */
static void stone_head_move_holder(struct task *head)
{
    struct task_work *work = head->work;
    int slot;
    Float x;
    Float z;

    if ((work->collision->flags & COLLISION_TOUCHED) &&
        collision_next_player(head)) {
        if (player_records[0] && player_records[0]->held == head) {
            slot = 0;
        } else if (player_records[1] && player_records[1]->held == head) {
            slot = 1;
        } else {
            return;
        }
        player_set_angles(slot, 0, work->ang.y ^ HALF_TURN, 0);
        canyon_polar_xz(work->ang.y, HOLD_DISTANCE, &x, &z);
        player_set_pos(slot, work->pos.x + x, work->pos.y, work->pos.z + z);
    }
}

static void stone_head_set_ends(struct task *rail)
{
    struct task_work *work = rail->work;
    Float middle_x;
    Float middle_z;
    Float half_x;
    Float half_z;

    canyon_polar_xz(work->ang.y, (work->scl.x + 1.0F) * RAIL_OFFSET, &middle_x,
                    &middle_z);
    canyon_polar_xz(work->ang.y,
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

/* the head's center: pos moved HEAD_OFFSET * scale back along ang */
static NJS_POINT3 stone_head_center(NJS_POINT3 *pos, Angle ang, Float scale)
{
    NJS_POINT3 center;
    Float x;
    Float z;

    center = *pos;
    canyon_polar_xz(ang, -(scale * HEAD_OFFSET), &x, &z);
    center.x += x;
    center.z += z;
    return center;
}

static void stone_head_place(struct task *head, struct task *rail, int place)
{
    struct stone_head_rail *ends = rail->extra.p;

    head->work->pos.x = ends->start.x + (ends->span.x * place * PLACE_SCALE);
    head->work->pos.y = ends->start.y;
    head->work->pos.z = ends->start.z + (ends->span.z * place * PLACE_SCALE);
}

static void stone_head_save_place(struct task *head, struct task *rail)
{
    struct stone_head_rail *ends = rail->extra.p;
    int place;

    if (ends->start.x != ends->end.x) {
        place = (head->work->pos.x - ends->start.x) / ends->span.x * LAST_PLACE;
    } else {
        place = (head->work->pos.z - ends->start.z) / ends->span.z * LAST_PLACE;
    }
    if (place <= 0) {
        place = 1;
    }
    if (place > LAST_PLACE) {
        place = LAST_PLACE;
    }
    placement_set_state(rail, place);
}
