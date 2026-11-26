/* Pyramid Cave's sandglass door. its twin is Death Chamber's,
 * stg25/sandglass_door.c */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/dust.h"
#include "shared/sandglass.h"
#include "shared/sandglass_registry.h"
#include "sound/sound.h"
#include "stg28/cave_util.h"
#include "stg28/sandglass_door.h"
#include "stg28/sandglass_registry.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

/* a sandglass's id, in its work. a door's work holds the id of the sandglass
 * it follows in the same byte */
#define GLASS_ID(work) ((Uint8)(work)->ang.z)

/* the kinds, as kind_names spells them */
enum sandglass_door_kind {
    KIND_WALL_HOLE,
    KIND_GROUND,
    KIND_GROUND_HOLE,
    KIND_WALL_DOOR,
    KIND_GROUND_HOLE_R
};
#define DOOR_KINDS 5
/* the colors, as color_names spells them */
#define DOOR_COLORS 4
#define KIND(work) ((int)njAbs((work)->ang.x) % DOOR_KINDS)
#define COLOR(work) ((int)njAbs((work)->scl.z) % DOOR_COLORS)
#define OPENS_UP(work)                                                         \
    (KIND(work) == KIND_WALL_HOLE || KIND(work) == KIND_WALL_DOOR)
#define SLIDES(work)                                                           \
    (KIND(work) == KIND_GROUND_HOLE || KIND(work) == KIND_GROUND_HOLE_R ||     \
     KIND(work) == KIND_GROUND)
#define HAS_FRAME(work)                                                        \
    (KIND(work) == KIND_WALL_HOLE || KIND(work) == KIND_GROUND_HOLE ||         \
     KIND(work) == KIND_GROUND_HOLE_R)

/* the door's modes, which index mode_steps. its work's submode holds the
 * mode its next step takes, or NO_NEXT_MODE */
enum sandglass_door_mode {
    MODE_SHUT,
    MODE_OPENING,
    MODE_BOUNCING,
    MODE_CLOSING
};
#define NO_NEXT_MODE (-1)

/* the byte of ang.z above the id sizes the door. an odd byte makes it
 * SMALL_METERS across, and an even one LARGE_METERS */
#define SIZE_SHIFT 8
#define SIZE_BITS (0xff << SIZE_SHIFT)
#define SIZE_PARITY (2 << SIZE_SHIFT)
#define SMALL_METERS 4
#define LARGE_METERS 5
#define METERS(work)                                                           \
    (((work)->ang.z & SIZE_BITS) % SIZE_PARITY ? SMALL_METERS : LARGE_METERS)
/* the door's scale, its meters over LARGE_METERS */
#define LARGE_SCALE 1.0F
#define SMALL_SCALE 0.8F
#define SCALE(work) ((work)->scl.x)
/* how far the door has moved from shut */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define TRAVEL(task) (*(Float *)&(task)->unk_3c)
/* the door's measures at a scale of 1. it is DOOR_SIZE on a side, and opens
 * by OPEN_TRAVEL. its dust takes it as THICKNESS thick */
#define DOOR_SIZE 50.0F
#define THICKNESS 10.0F
#define OPEN_TRAVEL 48.0F

/* the door opens over the first OPEN_STEPS steps of its sandglass's run. its
 * travel follows 1 - cos over a quarter turn, so it starts slow */
#define OPEN_STEPS 12
#define OPEN_TURN (QUARTER_TURN / OPEN_STEPS)
/* the open door then bounces, for BOUNCE_SHARE times the run's steps. it
 * stands BOUNCE(work) past open on one step, and short of it on the next.
 * BOUNCE(work) starts at BOUNCE_START, and keeps BOUNCE_DECAY of itself each
 * step */
#define BOUNCE(work) ((work)->scl.y)
#define BOUNCE_START 0.2F
#define BOUNCE_DECAY 0.9F
#define BOUNCE_SHARE 0.0667F
/* the door then shuts over the rest of the run, in NOTCHES notches. each
 * takes NOTCH_DEPTH off its travel, along 1 - cos over a quarter turn */
#define NOTCHES 16
#define NOTCH_DEPTH (OPEN_TRAVEL / NOTCHES)

/* the sound as the door comes open, and as it slams shut once its sandglass
 * stops running. it is an id in pyramid_cave_sound_list */
#define DOOR_SOUND 2
#define DOOR_SOUND_PRIORITY 0
/* the rumble as the door shuts in notches. each closing step gives it a time
 * of n + 1 steps. n is a notch's steps, truncated, but no fewer than
 * MIN_RUMBLE_STEPS. by RUMBLE_KEY, each step after the first finds the
 * rumble the first one started */
#define RUMBLE_SOUND 0x1016
#define RUMBLE_PRIORITY 1
#define MIN_RUMBLE_STEPS 10
#define RUMBLE_KEY(task) ((char *)(task) + 1)

/* the closing door raises dust every n steps. n is a notch's steps,
 * truncated, where that is MIN_DUST_STEPS or more. otherwise n is
 * DUST_NOTCHES notches' steps, truncated, but no fewer than MIN_DUST_STEPS */
#define MIN_DUST_STEPS 15
#define DUST_NOTCHES 2
/* a rectangle of dust across the door's width or thickness reaches half of
 * DUST_MARGIN past each end */
#define DUST_MARGIN 4.0F
/* the size of a puff of dust, and of one of KIND_GROUND's */
#define DUST_SCALE 4.0F
#define GROUND_DUST_SCALE 3.0F
#define DUST_SPEED 0.1F
/* a door with a frame raises its dust DUST_INSET in from the frame's point.
 * the inset runs toward the shut door's pos */
#define DUST_INSET 4.0F
/* how far above the door's pos KIND_GROUND's dust lies, and KIND_WALL_DOOR's
 * as it slams shut */
#define DUST_LIFT 2.0F
/* the dust's random densities, each from its _MIN up to just under twice it.
 * NOTCH_DENSITY() is a notch's, and SHUT_DENSITY() KIND_WALL_DOOR's as it
 * slams shut */
#define NOTCH_DENSITY_MIN 0.5F
#define SHUT_DENSITY_MIN 1.0F
#define NOTCH_DENSITY()                                                        \
    (NOTCH_DENSITY_MIN + (rand() * (NOTCH_DENSITY_MIN / (RAND_MAX + 1))))
#define SHUT_DENSITY()                                                         \
    (SHUT_DENSITY_MIN + (rand() * (SHUT_DENSITY_MIN / (RAND_MAX + 1))))

#define SHAKE_CAMERA 0
#define SHAKE_FULL_RANGE 1.0F

/* the flags of the door's world object: its copy of its kind's collision
 * object, in the collision world. the size class is DOOR_SIZE_CLASS at
 * either size */
#define DOOR_SIZE_CLASS 2
#define WORLD_FLAGS                                                            \
    (WORLD_SOLID | WORLD_MOVING | WORLD_SIZE_CLASS(DOOR_SIZE_CLASS))

/* the world object collides only while player_near finds a player in reach
 * of the door's pos. the reach is REACH_MARGIN plus REACH times the door's
 * scale */
#define REACH 71.0F
#define REACH_MARGIN 10.0F

/* the door draws see-through while it stands between the current camera and
 * the camera's player. the hides_player tests take the door as a square that
 * reaches SEE_MARGIN past its sides. the square of a door that opens up
 * reaches SEE_BELOW below pos, and stops at the door's top */
#define SEE_MARGIN 5.0F
#define SEE_BELOW 10.0F
/* white, in ARGB, at a low alpha */
#define SEE_THROUGH_TINT 0x65ffffff

/* the points init keeps in its task's motion record */
struct sandglass_door_points {
    NJS_POINT3 frame; /* where a door with a frame draws it */
    NJS_POINT3 shut; /* the door's pos while shut */
};
#define POINTS(task) ((struct sandglass_door_points *)(task)->motion.p)

/* the collision shapes of the child of a door that opens up */
enum { CHILD_CYLINDER, CHILD_BOX, CHILD_SHAPES };

static void sandglass_door_destroy(struct task *task);
static void sandglass_door_step(struct task *task);
static void sandglass_door_draw(struct task *task);
static void sandglass_door_take_next_mode(struct task *task);
static void sandglass_door_shut_step(struct task *task);
static void sandglass_door_opening_step(struct task *task);
static void sandglass_door_bouncing_step(struct task *task);
static void sandglass_door_closing_step(struct task *task);
static struct task *sandglass_door_find_glass(int id);
static Bool sandglass_door_wall_hides_player(struct task *task);
static Bool sandglass_door_floor_hides_player(struct task *task);
static void sandglass_door_shake(struct task *task);
static void sandglass_door_dust(struct task *task, Float lowered);
static void sandglass_door_collider_step(struct task *task);

/* SHC expands each call of this function in place. it writes the function's
 * own code, which the image holds, only for an external function. no other
 * source calls it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void cave_sandglass_door_set_next_mode(struct task *task, int mode);
/* NOLINTEND(misc-use-internal-linkage) */

#include "stg28/sandglass_door/text.inc"

#include "shared/sandglass_door_art.h"

KEPT static NJS_CNK_MODEL *door_models[DOOR_KINDS][DOOR_COLORS] = {
#include "stg28/sandglass_door/door_models.inc"
};
KEPT static NJS_TEXLIST *door_texlists[DOOR_KINDS][DOOR_COLORS] = {
#include "stg28/sandglass_door/door_texlists.inc"
};
KEPT static NJS_CNK_MODEL *frame_models[DOOR_KINDS][DOOR_COLORS] = {
#include "stg28/sandglass_door/frame_models.inc"
};
KEPT static NJS_TEXLIST *frame_texlists[DOOR_KINDS][DOOR_COLORS] = {
#include "stg28/sandglass_door/frame_texlists.inc"
};
KEPT static NJS_OBJECT *collisions[DOOR_KINDS] = {
#include "stg28/sandglass_door/collisions.inc"
};
/* the kinds' and the colors' names. each is padded to the width of its blank
 * field in init's door or color label */
static char *kind_names[DOOR_KINDS] = {
#include "stg28/sandglass_door/kind_names.inc"
};
static char *color_names[DOOR_COLORS] = {
#include "stg28/sandglass_door/color_names.inc"
};
NJS_TEXLIST *cave_sandglass_door_texlists[21] = {
#include "stg28/sandglass_door/cave_sandglass_door_texlists.inc"
};
/* the camera shake as the door comes open, or as KIND_WALL_DOOR slams shut.
 * shake_size is its size within SHAKE_FULL_RANGE of player 0 */
static Float shake_size = 8.0F;
static Sint32 shake_steps = 2;
/* clang-format off */
/* the child's shapes at a scale of 1. the cylinder lies across the door's
 * bottom, and the box above it */
static struct collision_shape child_shapes[CHILD_SHAPES] = {
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, 5.0F, 5.0F}, 5.0F, 30.0F, 0.0F, 0,
     {0, 0, QUARTER_TURN}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL), SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000,
     {0.0F, 25.0F, 5.0F}, 25.0F, 20.0F, 5.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Sint32 open_volume = 100;
static Sint32 rumble_volume = 125;
static Sint32 shut_volume = 100;
typedef void (*sandglass_door_mode_step)(struct task *task);

KEPT static sandglass_door_mode_step mode_steps[4] = {
#include "stg28/sandglass_door/mode_steps.inc"
};

void cave_sandglass_door_init(struct task *task)
{
    /* @hack spelling: task->work read again for the kind, the scale, the mode
     * and the child's copies, where work would do */
    struct task_work *work = task->work;
    struct task *child;
    NJS_OBJECT *object;
    struct collision_shape shape[CHILD_SHAPES];
    Float x;
    Float z;

#ifdef NONMATCHING
    /* GCC can't tell that child is set where it is read. it is read only
     * under the second OPENS_UP test, which passes only where the first did.
     * no call between the two tests writes ang.x */
    child = NULL;
#endif
    if (placement_try_leave(task)) {
        return;
    }
    if (OPENS_UP(task->work)) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((child = task_create_child(TASK_WORK, sandglass_door_collider_step,
                                       task)) == NULL) {
            return;
        }
    }
    task->draw = sandglass_door_draw;
    task->step = sandglass_door_step;
    task->destructor = sandglass_door_destroy;
    /* @hack redundant: the SET editor's labels, named in a block no path
     * reaches */
    if (0) {
        (void)"<- DOOR(             )";
        (void)"<- ANGLE Y";
        (void)"<- SIZE( m)/ID(   )";
        (void)"<- COLOR(      )";
    }
    /* @hack redundant: a store of the mode in a block no path reaches */
    if (0) {
        task->work->mode = MODE_SHUT;
    }
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies the kind's collision object through null where
     * world_pool_alloc has none to lend (MOV.L at 0x8c505960).
     * sandglass_door_destroy tests task->extra for null (TST at 0x8c505c68).
     * sandglass_door_step writes the door's pos through it untested (FMOV.S
     * at 0x8c505dd2) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the door, with its collider where it has one, and set.c makes
     * it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    SCALE(task->work) =
        METERS(task->work) == LARGE_METERS ? LARGE_SCALE : SMALL_SCALE;
    /* @hack spelling: the copy of the collision object written in each arm,
     * where one before the switch would do */
    switch (KIND(task->work)) {
    case KIND_WALL_HOLE:
    case KIND_WALL_DOOR:
        *object = *collisions[KIND(task->work)];
        POINTS(task)->frame = work->pos;
        POINTS(task)->frame.y += SCALE(task->work) * DOOR_SIZE;
        break;
    case KIND_GROUND:
    case KIND_GROUND_HOLE:
    case KIND_GROUND_HOLE_R:
        *object = *collisions[KIND(task->work)];
        POINTS(task)->frame = work->pos;
        cave_polar_xz(work->ang.y, SCALE(task->work) * (DOOR_SIZE / 2), &x, &z);
        POINTS(task)->frame.x += x;
        POINTS(task)->frame.z += z;
        break;
    }
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    object->scl[0] = object->scl[1] = object->scl[2] = SCALE(task->work);
    object->ang[0] = 0;
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_FLAGS, task, object);
    if (OPENS_UP(task->work)) {
        shape[CHILD_CYLINDER] = child_shapes[CHILD_CYLINDER];
        shape[CHILD_BOX] = child_shapes[CHILD_BOX];
        shape[CHILD_CYLINDER].radius *= SCALE(task->work);
        shape[CHILD_CYLINDER].size_y *= SCALE(task->work);
        shape[CHILD_CYLINDER].center.y *= SCALE(task->work);
        shape[CHILD_CYLINDER].center.z *= SCALE(task->work);
        shape[CHILD_BOX].radius *= SCALE(task->work);
        shape[CHILD_BOX].size_y *= SCALE(task->work);
        shape[CHILD_BOX].size_z *= SCALE(task->work);
        shape[CHILD_BOX].center.y *= SCALE(task->work);
        shape[CHILD_BOX].center.z *= SCALE(task->work);
        collision_init(child, shape, CHILD_SHAPES, COLLISION_OBJECT);
        child->work->pos = task->work->pos;
        child->work->ang.x = child->work->ang.z = 0;
        child->work->ang.y = task->work->ang.y;
    }
    TRAVEL(task) = 0.0F;
    task->work->mode = MODE_SHUT;
    task->work->submode = NO_NEXT_MODE;
    POINTS(task)->shut = work->pos;
}

static void sandglass_door_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void sandglass_door_step(struct task *task)
{
    /* @hack spelling: task->work read again for the mode, the kind and the
     * scale, where work would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 before;
    Float x;
    Float z;

    if (placement_try_leave(task)) {
        return;
    }
    sandglass_door_take_next_mode(task);
    mode_steps[task->work->mode](task);
    /* @hack spelling: a jump to the statement after it, where falling through
     * would do */
    goto move;
move:
    object = task->extra.p;
    /* @hack spelling: the copy of pos in a comma before the switch's kind,
     * where a statement of its own would do */
    switch ((before = work->pos, KIND(task->work))) {
    case KIND_WALL_HOLE:
    case KIND_WALL_DOOR:
        work->pos.y = POINTS(task)->shut.y + TRAVEL(task);
        break;
    case KIND_GROUND:
    case KIND_GROUND_HOLE:
    case KIND_GROUND_HOLE_R:
        cave_polar_xz(work->ang.y, TRAVEL(task), &x, &z);
        work->pos.x = POINTS(task)->shut.x + x;
        work->pos.z = POINTS(task)->shut.z + z;
        break;
    }
    /* @hack redundant: a store of the kind to the mode, in a block no path
     * reaches */
    if (0) {
        task->work->mode = KIND(task->work);
    }
    /* @hack redundant: the same store again, in a block of its own */
    if (0) {
        task->work->mode = KIND(task->work);
    }
    TASK_FORCES_OF(task)
    [0].speed.x = TASK_FORCES_OF(task)[1].speed.x = work->pos.x - before.x;
    TASK_FORCES_OF(task)
    [0].speed.y = TASK_FORCES_OF(task)[1].speed.y = work->pos.y - before.y;
    TASK_FORCES_OF(task)
    [0].speed.z = TASK_FORCES_OF(task)[1].speed.z = work->pos.z - before.z;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    if (player_near((NJS_POINT3 *)object->pos,
                    REACH_MARGIN + (SCALE(task->work) * REACH))) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void sandglass_door_draw(struct task *task)
{
    /* @hack redundant: a select on a constant around task->work, where
     * task->work alone would do */
    /* @hack spelling: task->work read again for all but pos and ang.y, where
     * work would do */
    struct task_work *work = 1.0F ? task->work : 0;

    /* @hack redundant: an empty block no path reaches */
    if (0) {
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    if (HAS_FRAME(task->work)) {
        /* @hack redundant: a block no path reaches, which draws the frame a
         * second way */
        if (0) {
            Float extent = -DOOR_SIZE;
            /* @hack redundant: a translate in a block no path reaches */
            njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
            switch (KIND(task->work)) {
            case KIND_WALL_HOLE:
                njTranslate(NULL, 0.0F, SCALE(task->work) * DOOR_SIZE, 0.0F);
                break;
            case KIND_GROUND_HOLE:
                njTranslate(NULL, 0.0F, 0.0F, SCALE(task->work) * extent);
                njRotateY(NULL, HALF_TURN);
                break;
            case KIND_GROUND_HOLE_R:
                njTranslate(NULL, 0.0F, 0.0F, SCALE(task->work) * extent);
                break;
            }
            njSetTexture(frame_texlists[KIND(task->work)][COLOR(task->work)]);
            cave_model_draw_scaled(
                frame_models[KIND(task->work)][COLOR(task->work)],
                SCALE(task->work));
            njPopMatrixEx();
        } else {
            njPushMatrixEx();
            njTranslateEx(&POINTS(task)->frame);
            njRotateY(NULL, work->ang.y);
            if (KIND(task->work) == KIND_GROUND_HOLE) {
                njRotateY(NULL, HALF_TURN);
            }
            njSetTexture(frame_texlists[KIND(task->work)][COLOR(task->work)]);
            cave_model_draw_scaled(
                frame_models[KIND(task->work)][COLOR(task->work)],
                SCALE(task->work));
            njPopMatrixEx();
        }
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(door_texlists[KIND(task->work)][COLOR(task->work)]);
    if ((OPENS_UP(task->work) && sandglass_door_wall_hides_player(task)) ||
        (SLIDES(task->work) && sandglass_door_floor_hides_player(task))) {
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        cave_model_draw_tinted_scaled(
            door_models[KIND(task->work)][COLOR(task->work)], SEE_THROUGH_TINT,
            SCALE(task->work));
        draw_attr_restore();
        draw_control_restore();
    } else {
        cave_model_draw_scaled(door_models[KIND(task->work)][COLOR(task->work)],
                               SCALE(task->work));
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void sandglass_door_take_next_mode(struct task *task)
{
    if (task->work->submode != NO_NEXT_MODE) {
        task->work->mode = task->work->submode;
        task->work->submode = NO_NEXT_MODE;
    }
}

void cave_sandglass_door_set_next_mode(struct task *task, int mode)
{
    task->work->submode = mode;
}

static void sandglass_door_shut_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    struct task *glass = sandglass_door_find_glass(GLASS_ID(work));

    if (glass && glass->work->mode == SANDGLASS_MODE_RUNNING) {
        cave_sandglass_door_set_next_mode(task, MODE_OPENING);
    }
}

static void sandglass_door_opening_step(struct task *task)
{
    /* @hack spelling: task->work read again for the scale and the bounce, where
     * work would do */
    struct task_work *work = task->work;
    struct task *glass = sandglass_door_find_glass(GLASS_ID(work));

    if (glass) {
        TRAVEL(task) = (1.0F - njCos(SANDGLASS(glass)->time * OPEN_TURN)) *
                       SCALE(task->work) * OPEN_TRAVEL;
        if (SANDGLASS(glass)->time >= OPEN_STEPS) {
            sandglass_door_shake(task);
            TRAVEL(task) = SCALE(task->work) * OPEN_TRAVEL;
            BOUNCE(task->work) = BOUNCE_START;
            sound_play_at(DOOR_SOUND, NULL, DOOR_SOUND_PRIORITY, open_volume,
                          &work->pos);
            cave_sandglass_door_set_next_mode(task, MODE_BOUNCING);
        }
        if (glass->work->mode != SANDGLASS_MODE_RUNNING) {
            TRAVEL(task) = 0.0F;
            sound_play_at(DOOR_SOUND, NULL, DOOR_SOUND_PRIORITY, shut_volume,
                          &work->pos);
            cave_sandglass_door_set_next_mode(task, MODE_SHUT);
        }
    }
}

static void sandglass_door_bouncing_step(struct task *task)
{
    /* @hack spelling: task->work read again for all but pos, where work would
     * do */
    struct task_work *work = task->work;
    struct task *glass = sandglass_door_find_glass(GLASS_ID(task->work));

    if (glass) {
        TRAVEL(task) =
            (SCALE(task->work) * OPEN_TRAVEL) +
            (njCos((SANDGLASS(glass)->time - OPEN_STEPS) * HALF_TURN) *
             BOUNCE(task->work));
        BOUNCE(task->work) *= BOUNCE_DECAY;
        if (SANDGLASS(glass)->time >=
            OPEN_STEPS + (SANDGLASS_RUN_STEPS(glass) * BOUNCE_SHARE)) {
            TRAVEL(task) = SCALE(task->work) * OPEN_TRAVEL;
            cave_sandglass_door_set_next_mode(task, MODE_CLOSING);
        }
        if (glass->work->mode != SANDGLASS_MODE_RUNNING) {
            TRAVEL(task) = 0.0F;
            sound_play_at(DOOR_SOUND, NULL, DOOR_SOUND_PRIORITY, shut_volume,
                          &work->pos);
            cave_sandglass_door_set_next_mode(task, MODE_SHUT);
        }
    }
}

static void sandglass_door_closing_step(struct task *task)
{
    /* @hack spelling: task->work read again for all but pos and ang.y, where
     * work would do */
    struct task_work *work = task->work;
    struct task *glass = sandglass_door_find_glass(GLASS_ID(task->work));
    Float notch_steps;
    Float whole;
    Float part;
    Float width;
    Float thickness;
    Float x;
    Float z;
    int closing_steps;
    int elapsed;
    int rumble_steps;
    int dust_steps;
    struct angles ang;
    NJS_POINT3 pos;

    if (glass) {
        closing_steps = SANDGLASS_RUN_STEPS(glass) -
                        (SANDGLASS_RUN_STEPS(glass) * BOUNCE_SHARE) -
                        OPEN_STEPS;
        notch_steps = closing_steps * (1.0F / NOTCHES);
        elapsed = SANDGLASS(glass)->time -
                  (SANDGLASS_RUN_STEPS(glass) * BOUNCE_SHARE) - OPEN_STEPS;
        rumble_steps = notch_steps;
        if (rumble_steps < MIN_RUMBLE_STEPS) {
            rumble_steps = MIN_RUMBLE_STEPS;
        }
        sound_play_at_for(RUMBLE_SOUND, RUMBLE_KEY(task), RUMBLE_PRIORITY,
                          rumble_volume, rumble_steps, &work->pos);
        whole = SCALE(task->work) * (int)((Float)elapsed / notch_steps) *
                NOTCH_DEPTH;
        part = SCALE(task->work) *
               (1.0F - njCos(float_wrap((Float)elapsed, notch_steps) /
                             notch_steps * (Float)QUARTER_TURN)) *
               NOTCH_DEPTH;
        TRAVEL(task) = (SCALE(task->work) * OPEN_TRAVEL) - whole - part;
        if (TRAVEL(task) <= 0.0F) {
            TRAVEL(task) = 0.0F;
        }
        dust_steps = notch_steps;
        if (dust_steps < MIN_DUST_STEPS) {
            dust_steps = notch_steps * DUST_NOTCHES;
            if (dust_steps < MIN_DUST_STEPS) {
                dust_steps = MIN_DUST_STEPS;
            }
        }
        if (elapsed % dust_steps == 0) {
            sandglass_door_dust(task, whole + part);
        }
        if (glass->work->mode != SANDGLASS_MODE_RUNNING) {
            if (KIND(task->work) == KIND_WALL_DOOR) {
                width = SCALE(task->work) * DOOR_SIZE;
                thickness = SCALE(task->work) * THICKNESS;
                ang.x = 0;
                ang.y = work->ang.y;
                ang.z = 0;
                pos = work->pos;
                cave_polar_xz(work->ang.y, thickness * 0.5F, &x, &z);
                pos.x += x;
                pos.z += z;
                pos.y += DUST_LIFT;
                dust_smoke_rect(&pos, (width + DUST_MARGIN) * 0.5F,
                                (thickness + DUST_MARGIN) * 0.5F, &ang,
                                DUST_SCALE, SHUT_DENSITY(), DUST_SPEED);
                sandglass_door_shake(task);
            }
            TRAVEL(task) = 0.0F;
            sound_play_at(DOOR_SOUND, NULL, DOOR_SOUND_PRIORITY, shut_volume,
                          &work->pos);
            cave_sandglass_door_set_next_mode(task, MODE_SHUT);
        }
    }
}

/* the sandglass of id that a door follows, or null where cave_sandglasses
 * holds none of id. it is the running one of id with the fewest steps run,
 * the first of a tie. where none of id runs, it is the first of id */
static struct task *sandglass_door_find_glass(int id)
{
    int found_time;
    int found_mode;
    struct task *found = NULL;
    struct task *glass;
    unsigned int i;
    /* @hack spelling: glass->work read again for the mode, where glass_work
     * would do */
    struct task_work *glass_work;

    if (!cave_sandglasses) {
        return NULL;
    }
    for (i = 0; i < SANDGLASS_SLOTS; i++) {
        glass = cave_sandglasses[i];
        if (glass) {
            glass_work = glass->work;
            if (glass_work && GLASS_ID(glass_work) == id) {
                if (!found) {
                    found = glass;
                    found_mode = glass->work->mode;
                    found_time = SANDGLASS(glass)->time;
                } else if (glass->work->mode == SANDGLASS_MODE_RUNNING &&
                           (found_mode != SANDGLASS_MODE_RUNNING ||
                            SANDGLASS(glass)->time < found_time)) {
                    found = glass;
                    found_mode = glass->work->mode;
                    found_time = SANDGLASS(glass)->time;
                }
            }
        }
    }
    return found;
}

static Bool sandglass_door_wall_hides_player(struct task *task)
{
    /* @hack spelling: task->work read again for the scale, where work
     * would do */
    struct task_work *work = task->work;
    NJS_POINT3 player;
    NJS_POINT3 camera;
    /* @hack spelling: ratio, a local for player.z and then the difference
     * before it takes the ratio, where each read in place would do */
    Float ratio;
    Float x;
    Float y;
    Float width;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, -work->ang.y);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    njCalcPoint(NULL, &player_work[camera_current_index]->pos, &player);
    njCalcPoint(NULL, &camera_views[camera_current_index]->pos, &camera);
    njPopMatrixEx();
    ratio = player.z;
    if (ratio == camera.z || ratio * camera.z > 0.0F) {
        return FALSE;
    }
    ratio -= camera.z;
    ratio = -camera.z / ratio;
    x = ((player.x - camera.x) * ratio) + camera.x;
    y = ((player.y - camera.y) * ratio) + camera.y;
    width = SCALE(task->work) * DOOR_SIZE;
    return x > -(width * 0.5F) - SEE_MARGIN &&
           x < (width * 0.5F) + SEE_MARGIN && y > -SEE_BELOW && y < width;
}

static Bool sandglass_door_floor_hides_player(struct task *task)
{
    /* @hack spelling: task->work read again for the scale, where work
     * would do */
    struct task_work *work = task->work;
    NJS_POINT3 player;
    NJS_POINT3 camera;
    /* @hack spelling: player_y, a local for player.y, where player.y read in
     * place would do */
    Float player_y;
    Float ratio;
    Float x;
    Float y;
    Float width;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, -work->ang.y);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    njCalcPoint(NULL, &player_work[camera_current_index]->pos, &player);
    njCalcPoint(NULL, &camera_views[camera_current_index]->pos, &camera);
    njPopMatrixEx();
    player_y = player.y;
    if (player_y == camera.y || player_y * camera.y > 0.0F) {
        return FALSE;
    }
    ratio = -camera.y / (player_y - camera.y);
    x = ((player.x - camera.x) * ratio) + camera.x;
    y = ((player.z - camera.z) * ratio) + camera.z;
    width = SCALE(task->work) * DOOR_SIZE;
    return x > -(width * 0.5F) - SEE_MARGIN &&
           x < (width * 0.5F) + SEE_MARGIN &&
           y > -(width * 0.5F) - SEE_MARGIN && y < (width * 0.5F) + SEE_MARGIN;
}

static void sandglass_door_shake(struct task *task)
{
    struct task_work *work = task->work;
    Float distance = njDistanceP2P(&work->pos, &player_work[0]->pos);
    Float size;

    if (distance > SHAKE_FULL_RANGE) {
        size = shake_size / distance;
    } else {
        size = shake_size;
    }
    /* @hack spelling: the shake in each branch, where one test with || would
     * do */
    if (camera_stacks[SHAKE_CAMERA].shake_steps > 0) {
        if (camera_stacks[SHAKE_CAMERA].shake_size <= size) {
            camera_shake(SHAKE_CAMERA, shake_steps, size);
        }
    } else {
        camera_shake(SHAKE_CAMERA, shake_steps, size);
    }
}

/* lowered is how far the door has shut from open, at its scale. only
 * KIND_WALL_DOOR's dust reads it */
static void sandglass_door_dust(struct task *task, Float lowered)
{
    /* @hack spelling: task->work read again for the kind, where work
     * would do */
    struct task_work *work = task->work;
    Float width = SCALE(work) * DOOR_SIZE;
    Float thickness = SCALE(work) * THICKNESS;
    Float x;
    Float z;
    Float half_lowered;
    struct angles ang;
    NJS_POINT3 pos;
    NJS_POINT3 offset;

    /* @hack redundant: rand named in a statement that does nothing */
    (void)rand;
    switch (KIND(task->work)) {
    case KIND_WALL_HOLE:
        ang.x = 0;
        ang.y = work->ang.y;
        ang.z = 0;
        pos = POINTS(task)->frame;
        cave_polar_xz(work->ang.y, (thickness * 0.5F), &x, &z);
        pos.x += x;
        pos.z += z;
        pos.y -= DUST_INSET;
        dust_smoke_rect(&pos, (width + DUST_MARGIN) * 0.5F,
                        (thickness + DUST_MARGIN) * 0.5F, &ang, DUST_SCALE,
                        NOTCH_DENSITY(), DUST_SPEED);
        break;
    case KIND_GROUND_HOLE:
    case KIND_GROUND_HOLE_R:
        ang.x = QUARTER_TURN;
        ang.y = work->ang.y;
        ang.z = 0;
        pos = POINTS(task)->frame;
        cave_polar_xz(work->ang.y, -DUST_INSET, &x, &z);
        pos.x += x;
        pos.z += z;
        pos.y += (thickness * 0.5F);
        dust_smoke_rect(&pos, (width + DUST_MARGIN) * 0.5F,
                        (thickness + DUST_MARGIN) * 0.5F, &ang, DUST_SCALE,
                        NOTCH_DENSITY(), DUST_SPEED);
        break;
    case KIND_GROUND:
        ang.x = 0;
        ang.y = work->ang.y;
        ang.z = 0;
        pos = work->pos;
        pos.y += DUST_LIFT;
        dust_smoke_rect(&pos, (width + DUST_MARGIN) * 0.5F,
                        (width + DUST_MARGIN) * 0.5F, &ang, GROUND_DUST_SCALE,
                        NOTCH_DENSITY(), DUST_SPEED);
        break;
    case KIND_WALL_DOOR:
        ang.x = 0;
        ang.y = work->ang.y;
        ang.z = 0;
        pos = work->pos;
        cave_polar_xz(work->ang.y, (thickness * 0.5F), &x, &z);
        pos.x += x;
        pos.z += z;
        pos.y += lowered;
        dust_smoke_rect(&pos, (width + DUST_MARGIN) * 0.5F,
                        (thickness + DUST_MARGIN) * 0.5F, &ang, DUST_SCALE,
                        NOTCH_DENSITY(), DUST_SPEED);
        ang.x = QUARTER_TURN;
        ang.y = work->ang.y + QUARTER_TURN;
        ang.z = 0;
        pos = work->pos;
        cave_point_turn_y(-(thickness * 0.5F), (DUST_MARGIN - width) * 0.5F,
                          ang.y, &offset);
        pos.x += offset.x;
        pos.z += offset.z;
        half_lowered = lowered * 0.5F;
        pos.y += half_lowered;
        dust_smoke_rect(&pos, (thickness + DUST_MARGIN) * 0.5F, half_lowered,
                        &ang, DUST_SCALE, NOTCH_DENSITY(), DUST_SPEED);
        ang.y = work->ang.y - QUARTER_TURN;
        pos = work->pos;
        cave_point_turn_y((thickness * 0.5F), (DUST_MARGIN - width) * 0.5F,
                          ang.y, &offset);
        pos.x += offset.x;
        pos.z += offset.z;
        pos.y += half_lowered;
        dust_smoke_rect(&pos, (thickness + DUST_MARGIN) * 0.5F, half_lowered,
                        &ang, DUST_SCALE, NOTCH_DENSITY(), DUST_SPEED);
        break;
    }
}

static void sandglass_door_collider_step(struct task *task)
{
    struct task *door = task->parent;

    task->work->pos = door->work->pos;
    task->work->ang.y = door->work->ang.y;
    collision_register(task);
}
