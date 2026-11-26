/* Death Chamber's sandglass door. its twin is Pyramid Cave's,
 * stg28/sandglass_door.c. the twin has a fifth kind, and opens in fewer steps.
 * it draws its models without compiled builds */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/object_builds.h"
#include "shared/sandglass.h"
#include "shared/sandglass_registry.h"
#include "sound/sound.h"
#include "stg25/chamber_util.h"
#include "stg25/sandglass_door.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

/* a sandglass's id: the low byte of its work's ang.z. a door holds the id of
 * the sandglass it follows there too */
#define GLASS_ID(work) ((Uint8)(work)->ang.z)

/* the door's kinds, as kind_names spells them */
enum sandglass_door_kind {
    KIND_WALL_HOLE,
    KIND_GROUND,
    KIND_GROUND_HOLE,
    KIND_WALL_DOOR
};
#define DOOR_KINDS 4
/* the colors, as color_names spells them */
#define DOOR_COLORS 4
#define KIND(work) ((int)njAbs((work)->ang.x) % DOOR_KINDS)
#define COLOR(work) ((int)njAbs((work)->scl.z) % DOOR_COLORS)
#define OPENS_UP(work)                                                         \
    (KIND(work) == KIND_WALL_HOLE || KIND(work) == KIND_WALL_DOOR)
#define SLIDES(work)                                                           \
    (KIND(work) == KIND_GROUND_HOLE || KIND(work) == KIND_GROUND)
#define HAS_FRAME(work)                                                        \
    (KIND(work) == KIND_WALL_HOLE || KIND(work) == KIND_GROUND_HOLE)

/* a door's modes, in its work's mode, which index mode_steps. its submode
 * holds the mode its next step takes, or NO_NEXT_MODE */
enum sandglass_door_mode {
    MODE_SHUT,
    MODE_OPENING,
    MODE_BOUNCING,
    MODE_CLOSING
};
#define NO_NEXT_MODE (-1)

/* SIZE_BITS of ang.z, the byte above the id, size the door. an odd byte
 * makes it SMALL_METERS across, and an even one LARGE_METERS */
#define SIZE_SHIFT 8
#define SIZE_BITS (0xff << SIZE_SHIFT)
#define SIZE_PARITY (2 << SIZE_SHIFT)
#define SMALL_METERS 4
#define LARGE_METERS 5
#define METERS(work)                                                           \
    (((work)->ang.z & SIZE_BITS) % SIZE_PARITY ? SMALL_METERS : LARGE_METERS)
/* the door's scale, its meters over LARGE_METERS, which init keeps in
 * scl.x */
#define LARGE_SCALE 1.0F
#define SMALL_SCALE 0.8F
#define SCALE(work) ((work)->scl.x)
/* how far the door has moved from shut, kept in task->unk_3c */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define TRAVEL(task) (*(Float *)&(task)->unk_3c)
/* a large door's measures, which a door's scale multiplies. the door is a
 * square DOOR_SIZE on a side, and its dust takes it as THICKNESS thick. it
 * opens by OPEN_TRAVEL */
#define DOOR_SIZE 50.0F
#define THICKNESS 10.0F
#define OPEN_TRAVEL 48.0F

/* the door opens over the first OPEN_STEPS steps of its sandglass's run. its
 * travel follows 1 - cos over a quarter turn, so it starts slow */
#define OPEN_STEPS 20
#define OPEN_TURN (QUARTER_TURN / OPEN_STEPS)
/* the open door then bounces. each step it stands BOUNCE(work) past open or
 * short of it, by turns. BOUNCE(work) starts at BOUNCE_START, and keeps
 * BOUNCE_DECAY of itself each step. the bounce lasts BOUNCE_SHARE times the
 * run's steps */
#define BOUNCE(work) ((work)->scl.y)
#define BOUNCE_START 0.2F
#define BOUNCE_DECAY 0.9F
#define BOUNCE_SHARE 0.0667F
/* the door then shuts over the rest of the run, in NOTCHES notches. each
 * takes NOTCH_DEPTH off its travel along 1 - cos over a quarter turn, as the
 * opening does */
#define NOTCHES 16
#define NOTCH_DEPTH (OPEN_TRAVEL / NOTCHES)

/* the sound as the door comes open, and as it slams shut: an id of
 * death_chamber_sound_list */
#define DOOR_SOUND 2
#define DOOR_SOUND_PRIORITY 0
/* the rumble as the door shuts in notches. its key is the address one byte
 * past the door's task. each closing step gives the rumble a time of n + 1
 * steps. the first starts it, and each later one finds its slot by the key.
 * n is a notch's steps, truncated, but no fewer than MIN_RUMBLE_STEPS */
#define RUMBLE_SOUND 0x1016
#define RUMBLE_PRIORITY 1
#define MIN_RUMBLE_STEPS 15
#define RUMBLE_KEY(task) ((char *)(task) + 1)

/* the closing door raises dust every n steps. n is a notch's steps,
 * truncated, where that is MIN_DUST_STEPS or more. otherwise n is
 * DUST_NOTCHES notches' steps, truncated, but no fewer than MIN_DUST_STEPS */
#define MIN_DUST_STEPS 15
#define DUST_NOTCHES 2
/* a rectangle of dust across the door's width or thickness reaches
 * DUST_MARGIN / 2 past each edge. the dust's puffs fly at DUST_SPEED */
#define DUST_MARGIN 4.0F
#define DUST_SPEED 0.1F
/* a puff's size: GROUND_DUST_SCALE for kind GROUND, and DUST_SCALE for the
 * rest */
#define DUST_SCALE 4.0F
#define GROUND_DUST_SCALE 3.0F
/* a _HOLE door's dust lies DUST_INSET from its frame's point, toward the
 * shut door's pos. the dust of kind GROUND lies DUST_LIFT above the door's
 * pos, as a slamming WALL_DOOR's does */
#define DUST_INSET 4.0F
#define DUST_LIFT 2.0F
/* the dust's random densities. each runs from its minimum to just under twice
 * it. NOTCH_DENSITY() is for a notch, and SHUT_DENSITY() for a WALL_DOOR that
 * slams */
#define NOTCH_DENSITY_MIN 0.5F
#define SHUT_DENSITY_MIN 1.0F
#define NOTCH_DENSITY()                                                        \
    (NOTCH_DENSITY_MIN + (rand() * (NOTCH_DENSITY_MIN / (RAND_MAX + 1))))
#define SHUT_DENSITY()                                                         \
    (SHUT_DENSITY_MIN + (rand() * (SHUT_DENSITY_MIN / (RAND_MAX + 1))))

#define SHAKE_CAMERA 0

/* a door farther than DRAW_RANGE from player 0 doesn't draw */
#define DRAW_RANGE 400.0F

/* the door's flags in the collision world. it takes size class
 * DOOR_SIZE_CLASS at either size */
#define DOOR_SIZE_CLASS 2
#define WORLD_FLAGS                                                            \
    (WORLD_SOLID | WORLD_MOVING | WORLD_SIZE_CLASS(DOOR_SIZE_CLASS))

/* the door's world object collides only while player_near finds a player in
 * reach. the reach is the door's scale times REACH, plus REACH_MARGIN */
#define REACH 71.0F
#define REACH_MARGIN 10.0F

/* the door draws see-through while it stands between the current camera and
 * the camera's player. the hides_player tests take the door as a square that
 * reaches SEE_MARGIN past its sides. a WALL_ door's square stops at its top,
 * and reaches SEE_BELOW below its foot */
#define SEE_MARGIN 5.0F
#define SEE_BELOW 10.0F
/* white, at a low alpha */
#define SEE_THROUGH_TINT 0x65ffffff

/* the points init keeps in the task's motion record */
struct sandglass_door_points {
    NJS_POINT3 frame; /* where a _HOLE door's frame stands */
    NJS_POINT3 shut; /* the door's pos while shut */
};
#define POINTS(task) ((struct sandglass_door_points *)(task)->motion.p)

#define CHILD_SHAPES 1

/* the models compiled into builds, in builds' order. each takes DOOR_COLORS
 * builds in a row, a color's at its index */
enum {
    WALL_MODEL,
    GROUND_MODEL,
    GROUND_HOLE_MODEL,
    WALL_FRAME_MODEL,
    GROUND_FRAME_MODEL,
    BUILT_MODELS
};
#define BUILDS (BUILT_MODELS * DOOR_COLORS)
/* the index in builds of model's build in color */
#define BUILD_INDEX(model, color) ((color) + ((model) * DOOR_COLORS))

static void sandglass_door_destroy(struct task *task);
static void sandglass_door_step(struct task *task);
static void sandglass_door_draw(struct task *task);
static void sandglass_door_take_next_mode(struct task *task);
static void sandglass_door_shut_step(struct task *task);
static void sandglass_door_opening_step(struct task *task);
static void sandglass_door_bouncing_step(struct task *task);
static void sandglass_door_closing_step(struct task *task);
static Bool sandglass_door_wall_hides_player(struct task *task);
static Bool sandglass_door_floor_hides_player(struct task *task);
static void sandglass_door_shake(struct task *task);
static void sandglass_door_dust(struct task *task, Float lowered);
static void sandglass_door_collider_step(struct task *task);

/* SHC expands each call of chamber_sandglass_door_set_next_mode. the image
 * also holds a copy of the function, which SHC writes only for an external
 * one. no header declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void chamber_sandglass_door_set_next_mode(struct task *task, int mode);
/* NOLINTEND(misc-use-internal-linkage) */

#include "stg25/sandglass_door/text.inc"

#include "shared/sandglass_door_art.h"

/* the doors' and the frames' models and texture lists, by kind and color */
KEPT static NJS_CNK_MODEL *door_models[DOOR_KINDS][DOOR_COLORS] = {
#include "stg25/sandglass_door/door_models.inc"
};
KEPT static NJS_TEXLIST *door_texlists[DOOR_KINDS][DOOR_COLORS] = {
#include "stg25/sandglass_door/door_texlists.inc"
};
KEPT static NJS_CNK_MODEL *frame_models[DOOR_KINDS][DOOR_COLORS] = {
#include "stg25/sandglass_door/frame_models.inc"
};
KEPT static NJS_TEXLIST *frame_texlists[DOOR_KINDS][DOOR_COLORS] = {
#include "stg25/sandglass_door/frame_texlists.inc"
};
/* each kind's collision object, which init copies into the collision world */
KEPT static NJS_OBJECT *collisions[DOOR_KINDS] = {
#include "stg25/sandglass_door/collisions.inc"
};
/* the kinds' and the colors' names, each as wide as its field in the set
 * editor's labels */
static char *kind_names[DOOR_KINDS] = {
#include "stg25/sandglass_door/kind_names.inc"
};
static char *color_names[DOOR_COLORS] = {
#include "stg25/sandglass_door/color_names.inc"
};
NJS_TEXLIST *chamber_sandglass_door_texlists[21] = {
#include "stg25/sandglass_door/chamber_sandglass_door_texlists.inc"
};
/* the camera's shake as the door comes open, and as a WALL_DOOR slams shut */
static Float shake_size = 8.0F;
static Sint32 shake_steps = 2;
/* the shape of the collision record that the door's child task carries. it is
 * a cylinder across the door, along the edge that leads as the door shuts. the
 * WALL_ kinds take wall_shape, and the GROUND kinds ground_shape. init scales a
 * copy by the door's scale */
/* clang-format off */
static struct collision_shape wall_shape = {
    0, SHAPE_CYLINDER2, PUSH_ALL | PUSH_FIRM(PUSH_ALL),
    SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000,
    {0.0F, 5.0F, 5.0F}, 5.0F, 25.0F, 0.0F, 0, {0, 0, QUARTER_TURN}};
static struct collision_shape ground_shape = {
    0, SHAPE_CYLINDER2, PUSH_ALL | PUSH_FIRM(PUSH_ALL),
    SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000,
    {0.0F, 0.0F, -15.0F}, 10.0F, 25.0F, 0.0F, 0, {0, 0, QUARTER_TURN}};
/* clang-format on */
/* the sounds' volumes: as the door comes open, as it shuts in notches, and as
 * it slams shut */
static Sint32 open_volume = 100;
static Sint32 rumble_volume = 125;
static Sint32 shut_volume = 100;
/* TRUE once one call of chamber_sandglass_door_build compiles every build */
static Bool built = FALSE;
static struct model_build builds[BUILDS] = {
#include "stg25/sandglass_door/builds.inc"
};
typedef void (*sandglass_door_mode_step)(struct task *task);

KEPT static sandglass_door_mode_step mode_steps[4] = {
#include "stg25/sandglass_door/mode_steps.inc"
};
static Float draw_range_squared = DRAW_RANGE * DRAW_RANGE;

Uint8 *chamber_sandglass_door_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, builds, BUILDS) == BUILDS) {
        built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void chamber_sandglass_door_init(struct task *task)
{
    /* @hack spelling: task->work read again for the scale, the kind, the
     * mode and the child's copies, where work would do */
    struct task_work *work = task->work;
    struct task *child;
    NJS_OBJECT *object;
    Float x;
    Float z;
    /* @hack type: copy, an array of one record, scaled through shape, a
     * pointer to it, where one record scaled in place would do */
    struct collision_shape copy[1];
    struct collision_shape *shape;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (!(child = task_create_child(TASK_WORK, sandglass_door_collider_step,
                                    task))) {
        return;
    }
    task->draw = sandglass_door_draw;
    task->step = sandglass_door_step;
    task->destructor = sandglass_door_destroy;
    /* @hack redundant: the set editor's labels, in a block no path reaches */
    if (0) {
        (void)"<- DOOR(           )";
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
     * world_pool_alloc has no object to lend, at 0x8c504072.
     * sandglass_door_destroy tests task->extra for null at 0x8c5042f6, but
     * sandglass_door_step writes through it untested at 0x8c504430 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the door and its collider, and set.c makes it again on a later
     * pass */
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
        POINTS(task)->frame.y += DOOR_SIZE * SCALE(task->work);
        break;
    case KIND_GROUND:
    case KIND_GROUND_HOLE:
        *object = *collisions[KIND(task->work)];
        POINTS(task)->frame = work->pos;
        chamber_polar_xz(work->ang.y, SCALE(task->work) * (DOOR_SIZE / 2), &x,
                         &z);
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
    /* @hack redundant: the kind spelled out around a select on a constant,
     * where KIND would do */
    copy[0] = *((int)_builtin_fabsf(0 ? 0.0F : (Float)task->work->ang.x) %
                                DOOR_KINDS ==
                            KIND_WALL_HOLE ||
                        KIND(task->work) == KIND_WALL_DOOR
                    ? &wall_shape
                    : &ground_shape);
    shape = copy;
    shape->radius *= SCALE(task->work);
    shape->size_y *= SCALE(task->work);
    shape->center.y *= SCALE(task->work);
    shape->center.z *= SCALE(task->work);
    collision_init(child, copy, CHILD_SHAPES, COLLISION_OBJECT);
    child->work->pos = task->work->pos;
    child->work->ang.x = child->work->ang.z = 0;
    child->work->ang.y = task->work->ang.y;
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
    Float x;
    Float z;
    NJS_POINT3 old;

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
    old = work->pos;
    switch (KIND(task->work)) {
    case KIND_WALL_HOLE:
    case KIND_WALL_DOOR:
        work->pos.y = POINTS(task)->shut.y + TRAVEL(task);
        break;
    case KIND_GROUND:
    case KIND_GROUND_HOLE:
        chamber_polar_xz(work->ang.y, TRAVEL(task), &x, &z);
        work->pos.x = POINTS(task)->shut.x + x;
        work->pos.z = POINTS(task)->shut.z + z;
        break;
    }
    TASK_FORCES_OF(task)
    [0].speed.x = TASK_FORCES_OF(task)[1].speed.x = work->pos.x - old.x;
    TASK_FORCES_OF(task)
    [0].speed.y = TASK_FORCES_OF(task)[1].speed.y = work->pos.y - old.y;
    TASK_FORCES_OF(task)
    [0].speed.z = TASK_FORCES_OF(task)[1].speed.z = work->pos.z - old.z;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    if (player_near((NJS_POINT3 *)object->pos,
                    (SCALE(task->work) * REACH) + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    /* the door's child task carries its collision record, and the door's own
     * work has none. so this call changes nothing */
    if (task->work->mode == MODE_CLOSING) {
        collision_register(task);
    }
}

static void sandglass_door_draw(struct task *task)
{
    /* @hack spelling: task->work read again for all but pos and ang.y, where
     * work would do */
    struct task_work *work = task->work;
    NJS_POINT3 player;
    Float dx;

    player = player_work[0]->pos;
    /* @hack spelling: dx, the x distance held in a local, where the sum
     * written out would do */
    dx = player.x - work->pos.x;
    if ((dx * dx) + ((player.y - work->pos.y) * (player.y - work->pos.y)) +
            ((player.z - work->pos.z) * (player.z - work->pos.z)) >
        draw_range_squared) {
        return;
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    if (HAS_FRAME(task->work)) {
        /* @hack redundant: a block no path reaches, which names builds, turns,
         * and draws a frame for the GROUND and WALL_DOOR kinds, which have
         * none */
        if (0) {
            (void)builds;
            njRotateY(NULL, task->work->ang.y);
            switch (KIND(task->work)) {
            case KIND_GROUND:
                njTranslate(NULL, 0.0F, SCALE(task->work) * DOOR_SIZE, 0.0F);
                goto draw_frame;
            case KIND_WALL_DOOR:
                njTranslate(NULL, 0.0F, 0.0F,
                            SCALE(task->work) * (DOOR_SIZE / 2));
                njRotateY(NULL, HALF_TURN);
                /* @hack spelling: a jump to the statement after it, where
                 * falling through would do */
                goto draw_frame;
            draw_frame:
                njSetTexture(
                    frame_texlists[KIND(task->work)][COLOR(task->work)]);
                chamber_model_draw_scaled(
                    frame_models[KIND(task->work)][COLOR(task->work)],
                    SCALE(task->work));
            }
            njPopMatrixEx();
        } else {
            njPushMatrixEx();
            njTranslateEx(&POINTS(task)->frame);
            njRotateY(NULL, work->ang.y);
            njSetTexture(frame_texlists[KIND(task->work)][COLOR(task->work)]);
            if (KIND(task->work) == KIND_GROUND_HOLE) {
                njRotateY(NULL, HALF_TURN);
            }
            if (built) {
                if (KIND(task->work) == KIND_GROUND_HOLE) {
                    /* @hack redundant: the build's index stored in index,
                     * which nothing reads */
                    int index;
                    chamber_direct_draw_scaled(
                        builds[(index = BUILD_INDEX(GROUND_FRAME_MODEL,
                                                    COLOR(task->work)))]
                            .built,
                        SCALE(task->work));
                } else {
                    /* @hack redundant: the build's index stored in index,
                     * which nothing reads */
                    int index;
                    chamber_direct_draw_scaled(
                        builds[(index = BUILD_INDEX(WALL_FRAME_MODEL,
                                                    COLOR(task->work)))]
                            .built,
                        SCALE(task->work));
                }
            } else {
                chamber_model_draw_scaled(
                    frame_models[KIND(task->work)][COLOR(task->work)],
                    SCALE(task->work));
            }
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
        chamber_model_draw_tinted_scaled(
            door_models[KIND(task->work)][COLOR(task->work)], SEE_THROUGH_TINT,
            SCALE(task->work));
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    } else if (built) {
        switch (KIND(task->work)) {
        case KIND_WALL_HOLE:
        case KIND_WALL_DOOR: {
            /* @hack spelling: draw_work, a local for task->work, where
             * task->work read in place would do */
            struct task_work *draw_work = task->work;
            chamber_direct_draw_scaled(
                builds[BUILD_INDEX(WALL_MODEL, COLOR(task->work))].built,
                SCALE(draw_work));
        } break;
        case KIND_GROUND: {
            /* @hack spelling: draw_work, a local for task->work, where
             * task->work read in place would do */
            struct task_work *draw_work = task->work;
            /* @hack redundant: the build's index stored in index, which
             * nothing reads */
            int index;
            chamber_direct_draw_scaled(
                builds[(index = BUILD_INDEX(GROUND_MODEL, COLOR(task->work)))]
                    .built,
                SCALE(draw_work));
        } break;
        case KIND_GROUND_HOLE: {
            /* @hack spelling: draw_work, a local for task->work, where
             * task->work read in place would do */
            struct task_work *draw_work = task->work;
            /* @hack redundant: the build's index stored in index, which
             * nothing reads */
            int index;
            chamber_direct_draw_scaled(
                builds[(index =
                            BUILD_INDEX(GROUND_HOLE_MODEL, COLOR(task->work)))]
                    .built,
                SCALE(draw_work));
        } break;
        }
    } else {
        /* @hack spelling: draw_work, a local for task->work, where task->work
         * read in place would do */
        struct task_work *draw_work = task->work;
        chamber_model_draw_scaled(
            door_models[KIND(task->work)][COLOR(task->work)], SCALE(draw_work));
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

void chamber_sandglass_door_set_next_mode(struct task *task, int mode)
{
    task->work->submode = mode;
}

static void sandglass_door_shut_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    struct task *glass;

    glass = chamber_sandglass_door_find_glass(GLASS_ID(work));
    if (glass && glass->work->mode == SANDGLASS_MODE_RUNNING) {
        chamber_sandglass_door_set_next_mode(task, MODE_OPENING);
    }
}

static void sandglass_door_opening_step(struct task *task)
{
    /* @hack spelling: task->work read again for the scale and the bounce,
     * where work would do */
    struct task_work *work = task->work;
    struct task *glass;
    /* @hack spelling: cosine, a local for the cosine, where the cosine in the
     * product would do */
    Float cosine;

    glass = chamber_sandglass_door_find_glass(GLASS_ID(work));
    if (!glass) {
        return;
    }
    cosine = njCos(SANDGLASS(glass)->time * OPEN_TURN);
    TRAVEL(task) = (1.0F - cosine) * SCALE(task->work) * OPEN_TRAVEL;
    if (SANDGLASS(glass)->time >= OPEN_STEPS) {
        sandglass_door_shake(task);
        TRAVEL(task) = SCALE(task->work) * OPEN_TRAVEL;
        BOUNCE(task->work) = BOUNCE_START;
        sound_play_at(DOOR_SOUND, NULL, DOOR_SOUND_PRIORITY, open_volume,
                      &work->pos);
        chamber_sandglass_door_set_next_mode(task, MODE_BOUNCING);
    }
    if (glass->work->mode != SANDGLASS_MODE_RUNNING) {
        TRAVEL(task) = 0.0F;
        sound_play_at(DOOR_SOUND, NULL, DOOR_SOUND_PRIORITY, shut_volume,
                      &work->pos);
        chamber_sandglass_door_set_next_mode(task, MODE_SHUT);
    }
}

static void sandglass_door_bouncing_step(struct task *task)
{
    /* @hack spelling: task->work read again for all but pos, where work would
     * do */
    struct task_work *work = task->work;
    struct task *glass;

    glass = chamber_sandglass_door_find_glass(GLASS_ID(task->work));
    if (!glass) {
        return;
    }
    TRAVEL(task) = (SCALE(task->work) * OPEN_TRAVEL) +
                   (njCos((SANDGLASS(glass)->time - OPEN_STEPS) * HALF_TURN) *
                    BOUNCE(task->work));
    BOUNCE(task->work) *= BOUNCE_DECAY;
    if (SANDGLASS(glass)->time >=
        (SANDGLASS_RUN_STEPS(glass) * BOUNCE_SHARE) + OPEN_STEPS) {
        TRAVEL(task) = SCALE(task->work) * OPEN_TRAVEL;
        chamber_sandglass_door_set_next_mode(task, MODE_CLOSING);
    }
    if (glass->work->mode != SANDGLASS_MODE_RUNNING) {
        TRAVEL(task) = 0.0F;
        sound_play_at(DOOR_SOUND, NULL, DOOR_SOUND_PRIORITY, shut_volume,
                      &work->pos);
        chamber_sandglass_door_set_next_mode(task, MODE_SHUT);
    }
}

static void sandglass_door_closing_step(struct task *task)
{
    /* @hack spelling: task->work read again for all but pos and ang.y, where
     * work would do */
    struct task_work *work = task->work;
    struct task *glass =
        chamber_sandglass_door_find_glass(GLASS_ID(task->work));
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
                chamber_polar_xz(work->ang.y, thickness * 0.5F, &x, &z);
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
            chamber_sandglass_door_set_next_mode(task, MODE_SHUT);
        }
    }
}

struct task *chamber_sandglass_door_find_glass(int id)
{
    struct task *found = NULL;
    struct task *glass;
    Sint32 found_time;
    int found_mode;
    int i;
    /* @hack spelling: glass->work read again for the mode, where glass_work
     * would do */
    struct task_work *glass_work;

    if (!sandglasses) {
        return NULL;
    }
    for (i = 0; i < SANDGLASS_SLOTS; i++) {
        glass = sandglasses[i];
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
    /* @hack spelling: ratio holding player.z, then player.z - camera.z,
     * before the ratio itself, where each written in place would do */
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
    x = camera.x + (ratio * (player.x - camera.x));
    y = camera.y + (ratio * (player.y - camera.y));
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
    Float z;
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
    x = camera.x + (ratio * (player.x - camera.x));
    /* @hack spelling: z's sum with its terms in the other order from x's,
     * where x's order would do */
    z = (ratio * (player.z - camera.z)) + camera.z;
    width = SCALE(task->work) * DOOR_SIZE;
    return x > -(width * 0.5F) - SEE_MARGIN &&
           x < (width * 0.5F) + SEE_MARGIN &&
           z > -(width * 0.5F) - SEE_MARGIN && z < (width * 0.5F) + SEE_MARGIN;
}

/* shake the camera for shake_steps, by shake_size over the door's distance
 * from player 0. the shake is no larger than shake_size. where the camera's
 * running shake is larger, it goes on instead */
static void sandglass_door_shake(struct task *task)
{
    struct task_work *work = task->work;
    Float distance;
    Float size;

    distance = njDistanceP2P(&work->pos, &player_work[0]->pos);
    if (distance > 1.0F) {
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

/* lowered is how far the notches have shut the door from open. only a
 * WALL_DOOR's dust reads it */
static void sandglass_door_dust(struct task *task, Float lowered)
{
    struct task_work *work = task->work;
    Float x;
    Float z;
    Float half_lowered;
    Float width;
    Float thickness;
    struct angles ang;
    NJS_POINT3 pos;
    NJS_POINT3 offset;

    width = SCALE(work) * DOOR_SIZE;
    thickness = SCALE(work) * THICKNESS;
    switch (KIND(work)) {
    case KIND_WALL_HOLE:
        ang.x = 0;
        ang.y = work->ang.y;
        ang.z = 0;
        pos = POINTS(task)->frame;
        chamber_polar_xz(work->ang.y, thickness * 0.5F, &x, &z);
        pos.x += x;
        pos.z += z;
        pos.y -= DUST_INSET;
        dust_smoke_rect(&pos, (width + DUST_MARGIN) * 0.5F,
                        (thickness + DUST_MARGIN) * 0.5F, &ang, DUST_SCALE,
                        NOTCH_DENSITY(), DUST_SPEED);
        break;
    case KIND_GROUND_HOLE:
        ang.x = QUARTER_TURN;
        ang.y = work->ang.y;
        ang.z = 0;
        pos = POINTS(task)->frame;
        chamber_polar_xz(work->ang.y, -DUST_INSET, &x, &z);
        pos.x += x;
        pos.z += z;
        pos.y += thickness * 0.5F;
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
        /* @hack redundant: a burst in a block no path reaches */
        if (0) {
            dust_smoke_rect(&pos, lowered, 0.0F, &ang, DUST_SCALE, 0.0F,
                            DUST_SPEED);
        }
        ang.x = 0;
        ang.y = work->ang.y;
        ang.z = 0;
        pos = work->pos;
        chamber_polar_xz(work->ang.y, thickness * 0.5F, &x, &z);
        pos.x += x;
        pos.z += z;
        pos.y += lowered;
        dust_smoke_rect(&pos, (width + DUST_MARGIN) * 0.5F,
                        (thickness + DUST_MARGIN) * 0.5F, &ang, DUST_SCALE,
                        NOTCH_DENSITY(), DUST_SPEED);
        ang.x = QUARTER_TURN;
        ang.y = QUARTER_TURN + work->ang.y;
        ang.z = 0;
        pos = work->pos;
        chamber_point_turn_y(-(thickness * 0.5F), (DUST_MARGIN - width) * 0.5F,
                             ang.y, &offset);
        pos.x += offset.x;
        pos.z += offset.z;
        half_lowered = lowered * 0.5F;
        pos.y += half_lowered;
        dust_smoke_rect(&pos, (thickness + DUST_MARGIN) * 0.5F, half_lowered,
                        &ang, DUST_SCALE, NOTCH_DENSITY(), DUST_SPEED);
        ang.y = work->ang.y - QUARTER_TURN;
        pos = work->pos;
        chamber_point_turn_y(thickness * 0.5F, (DUST_MARGIN - width) * 0.5F,
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
