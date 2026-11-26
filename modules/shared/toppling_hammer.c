/* the toppling hammer, HAMMER3 in the object lists of both Mission Street
 * stages. it stands until its charges blow. it then topples over an edge of
 * its base, rocks and comes to rest */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/dynamite.h"
#include "shared/road_furniture/hammer.h"
#include "shared/street_hammer.h"
#include "shared/toppling_hammer.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/matrix.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the hammer's modes, which index toppling_hammer_modes */
enum toppling_hammer_mode { MODE_STAND, MODE_FALL, MODE_ROCK, MODE_LIE };
/* the work's submode holds the mode the hammer takes at its next step, or
 * NO_NEXT_MODE */
#define NEXT_MODE(work) ((work)->submode)
#define NO_NEXT_MODE (-1)

/* the hammer's settings, as toppling_hammer.h lists them */
#define LEAN_BITS 0x3ff0
#define LEAN(work) ((work)->ang.x & LEAN_BITS)
#define CHARGE_ID(work) ((Uint8)(work)->ang.z)
#define THICKNESS(work) ((work)->scl.x + 1.0F)
#define HEIGHT(work) ((work)->scl.y + 1.0F)

/* the bits of the work's tick that pick the corner of the hammer's +z face
 * it turns about. PIVOT_NEG_X picks a corner at -x, and a clear bit one at
 * +x. PIVOT_AT_BASE, bit PIVOT_AT_BASE_SHIFT, picks a corner at the base, and
 * a clear bit one at the top. about a base corner, the hammer turns from
 * upright. about a top corner, it first lies at its stop, then turns by its
 * tilt less the stop's */
#define PIVOT_NEG_X 0x1
#define PIVOT_AT_BASE 0x2
#define PIVOT_AT_BASE_SHIFT 1

/* TASK_MOTION's record, as the hammer lays it out. toppling_hammer_place
 * moves the hammer by -slide along its own z. tilt.x turns it about its
 * pivot's x axis, and tilt.y about its pivot's y axis. each step of the fall
 * adds slide_spd to slide. each step of the fall and of the rock adds each
 * tilt speed to its tilt */
struct toppling_hammer_motion {
    Float unk_00[2];
    Float slide_spd;
    Float unk_0c[2];
    Float slide;
    struct angles tilt;
    Angle tilt_spd_x;
    Angle tilt_spd_y;
};
#define MOTION(task) ((struct toppling_hammer_motion *)(task)->motion.p)

/* TASK_EXTRA's record. matrix, from the Shinobi heap, places the hammer.
 * object is the hammer's copy of street_hammer_collision, which it adds to
 * the collision world */
struct toppling_hammer_extra {
    NJS_MATRIX *matrix;
    NJS_OBJECT *object;
};
#define EXTRA(task) ((struct toppling_hammer_extra *)(task)->extra.p)
/* the hammer's radius, in task->unk_3c. it reaches from the hammer's center
 * to a corner of its top */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define RADIUS(task) (*(Float *)&(task)->unk_3c)
/* a player within the hammer's radius plus REACH_MARGIN of its center is
 * near. world_collect_near_players takes the copy only while player 0 or 1
 * is near */
#define REACH_MARGIN 10.0F

/* at a scale of 1, the hammer is a hexagonal column standing on pos. its
 * corners lie HALF_WIDTH from its axis, and its ends HALF_HEIGHT either way
 * of its center. its +z face lies CORNER_Z from the axis. the face's corners
 * lie CORNER_X either way of its center in x. the hammer topples over the
 * face's bottom edge */
#define HALF_WIDTH 25.0F
#define HALF_HEIGHT 50.0F
#define CORNER_X 12.5F
#define CORNER_Z 21.65064F

/* the fall starts with the slide's speed at FALL_PUSH, and with a burst of
 * BURST_SIZE times the thickness. the burst lies the thickness times CORNER_Z
 * above pos, and draws nearer by BURST_DEPTH times that height */
#define FALL_PUSH (-3.0F)
#define BURST_SIZE 40.0F
#define BURST_DEPTH 1.1F
/* each step of the fall scales the slide's speed by SLIDE_KEEP. the landing
 * sets the speed to REBOUND_SLIDE_SPD, as does a rock back past upright */
#define SLIDE_KEEP 0.75F
#define REBOUND_SLIDE_SPD 0.75F
/* the fall's gravity and the rock's. SLIDE_SCALE scales the slide a step of
 * the fall adds as the hammer turns */
#define FALL_GRAVITY 0.016F
#define ROCK_GRAVITY 0.44F
#define SLIDE_SCALE 1.5258789e-06F
/* the slide of a hammer that starts lying at its stop */
#define LIE_SLIDE (-9.0F)
/* the fall keeps tilt.y from 0 to TWIST_MAX, rebounding at each */
#define TWIST_MAX 0x2b00
/* a leaning hammer that rebounds from its stop keeps LEAN_BOUNCE of its
 * speed */
#define LEAN_BOUNCE 0.6667F
/* the rock ends in MODE_LIE where the hammer reaches its stop or upright with
 * tilt.y outside (0, TWIST_MAX). tilt_spd_x must lie within REST_SPEED of 0,
 * and at the stop tilt_spd_y must too */
#define REST_SPEED 64.0F

/* the landing's dust, along the edges of the face the hammer lands on, as
 * dust_rect makes it */
#define DUST_SCALE 10.0F
#define DUST_DENSITY 2.0F
#define DUST_SPEED 1.0F
#define LAND_SOUND 5
#define LAND_SOUND_PRIORITY 0
/* the placement's state once the hammer has fallen */
#define STATE_FALLEN 1

/* the landing measures player 0's distance from the hammer's axis, the
 * segment from its base to its top. it shakes camera 0 at full size within
 * SHAKE_NEAR. it vibrates player 0's pad at full power within VIBRATION_FAR.
 * farther, each is scaled by its bound over that distance */
#define SHAKE_NEAR 8.0F
#define VIBRATION_FAR 160.0F

#define SHADOW_LIFT 0.1F
#define SHADOW_COLOR 0x20000000
/* street_hammer_glow_model's tint blends from GLOW_FROM, clear black, toward
 * GLOW_TO, white. the blend swings from GLOW_LOW to 1 and back every 64
 * steps. it follows the sine's magnitude, at an angle of
 * game_state.steps << GLOW_SHIFT */
#define GLOW_FROM 0x00000000
#define GLOW_TO 0xffffffff
#define GLOW_LOW 0.5F
#define GLOW_SHIFT 9
/* the see-through gray toppling_hammer_draw_sphere draws in: its alpha, red,
 * green and blue */
#define SPHERE_SHADE 0.3F
/* the evalflags that would keep an object's pos, ang and scl from applying */
#define EVAL_UNITS (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL)

static void toppling_hammer_destroy(struct task *task);
static void toppling_hammer_step(struct task *task);
static void toppling_hammer_draw(struct task *task);
static void toppling_hammer_take_mode(struct task *task);
static void toppling_hammer_stand(struct task *task);
static void toppling_hammer_fall(struct task *task);
static void toppling_hammer_rock(struct task *task);
static void toppling_hammer_lie(struct task *task);
static void toppling_hammer_part_step(struct task *task);
static void toppling_hammer_draw_shadow(struct task *task);
static void toppling_hammer_place(struct task *task);
static void toppling_hammer_polar_xz(Angle ang, Float radius, Float *x,
                                     Float *z);
static void toppling_hammer_shake(struct task *task, NJS_LINE *line);
static void toppling_hammer_draw_scaled(NJS_CNK_MODEL *model, Float scale);
static Float toppling_hammer_line_distance(NJS_POINT3 *point, NJS_LINE *line,
                                           NJS_POINT3 *nearest);

/* no header declares these two, and nothing calls them, yet they are
 * external. the image holds both, and SHC would leave out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void toppling_hammer_draw_sphere(Float scale);
Float toppling_hammer_line_distance_xz(NJS_POINT3 *point, NJS_LINE *line,
                                       NJS_POINT3 *nearest);
/* NOLINTEND(misc-use-internal-linkage) */

/* the fall turns the hammer about x, and twists it about y, as two
 * pendulums swing. toppling_hammer_turn_speed is the turn's speed at the
 * bottom of its swing. toppling_hammer_twist_speed is a leaning hammer's
 * twist's, and a flat hammer's twist takes 0 in its place */
static Float toppling_hammer_turn_speed = 2.14F;
static Float toppling_hammer_twist_speed = 0.0F;
/* the part of a tilt speed that a rebound keeps. the landing's turn keeps
 * that times the cosine of the lean. in the rock, a leaning hammer's rebound
 * from its stop keeps LEAN_BOUNCE instead */
static Float toppling_hammer_bounce = 0.85F;
/* the part's shapes, capsules along the hammer's height. a flat hammer's part
 * hurts players, and a leaning hammer's only pushes */
/* clang-format off */
static struct collision_shape toppling_hammer_flat_shape = {
    0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
    {0.0F, 0.0F, 5.0F}, 24.5F, 24.5F, 0.0F, 0, {0, 0, 0},
};
static struct collision_shape toppling_hammer_lean_shape = {
    0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(3), 0,
    {0.0F, 0.0F, 5.0F}, 24.5F, 24.5F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
/* the landing's vibration of player 0's pad, as vibration_start takes it */
static Sint32 toppling_hammer_vibration_power = 2;
static Sint32 toppling_hammer_vibration_freq = 30;
static Sint32 toppling_hammer_vibration_time = 1;
/* the landing sound's volume */
static Sint32 toppling_hammer_land_volume = 25;
static Sint32 dat_toppling_hammer_0 = 0;
static void (*toppling_hammer_modes[])(struct task *task) = {
    toppling_hammer_stand,
    toppling_hammer_fall,
    toppling_hammer_rock,
    toppling_hammer_lie,
};
/* a matrix that zeroes y, which flattens the shadow */
/* clang-format off */
static NJS_MATRIX toppling_hammer_flatten = {
    1.0F, 0.0F, 0.0F, 0.0F,
    0.0F, 0.0F, 0.0F, 0.0F,
    0.0F, 0.0F, 1.0F, 0.0F,
    0.0F, 0.0F, 0.0F, 1.0F,
};
/* clang-format on */
/* the landing's shake of camera 0, as camera_shake takes it */
static Float toppling_hammer_shake_size = 16.0F;
static Sint32 toppling_hammer_shake_steps = 2;

static void toppling_hammer_set_next_mode(int mode, struct task *task)
{
    NEXT_MODE(task->work) = mode;
}

void toppling_hammer_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *part;
    struct task_work *part_work;
    NJS_OBJECT *object;
    Sint32 flags;
    Float thickness;
    Float height;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for the strings SHC
     * places in the object. the image holds none of its code */
    if (0) {
        (void)"<- ANGLE X/TEST(0:off)";
        (void)"<- ANGLE Y";
        (void)"<- ID";
        (void)"<- THICKNESS(1.0+x)";
        (void)"<- HEIGHT(1.0+x)";
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((part = task_create_child(TASK_WORK, toppling_hammer_part_step,
                                  task)) != NULL) {
        part_work = part->work;
        /* @hack spelling: LEAN tested as at most 1, where LEAN(work) == 0
         * would do */
        if (LEAN(work) <= 1) {
            collision_init(part, &toppling_hammer_flat_shape, 1,
                           COLLISION_OBJECT);
        } else {
            collision_init(part, &toppling_hammer_lean_shape, 1,
                           COLLISION_OBJECT);
        }
        part_work->collision->shapes->center.z *= THICKNESS(work);
        part_work->collision->shapes->radius *= THICKNESS(work);
        part_work->collision->shapes->size_y *= HEIGHT(work);
        collision_fit_radius(part);
    } else {
        task_kill(task);
        return;
    }
    task->draw = toppling_hammer_draw;
    task->step = toppling_hammer_step;
    task->destructor = toppling_hammer_destroy;
    EXTRA(task)->matrix = syMalloc(sizeof(NJS_MATRIX));
    /* @hack spelling: task->work read again for the mode, the next mode and
     * the tick, where work would do */
    task->work->mode = MODE_STAND;
    NEXT_MODE(task->work) = NO_NEXT_MODE;
    if (placement_get_state(task)) {
        toppling_hammer_set_next_mode(MODE_LIE, task);
        MOTION(task)->tilt.x = QUARTER_TURN - LEAN(work);
        MOTION(task)->tilt.y = MOTION(task)->tilt.z = 0;
        MOTION(task)->slide = LIE_SLIDE;
    } else {
        toppling_hammer_set_next_mode(MODE_STAND, task);
        MOTION(task)->tilt.x = MOTION(task)->tilt.y = MOTION(task)->tilt.z = 0;
        /* @hack spelling: each bit cleared, then set, where setting both
         * would do */
        task->work->tick &= ~PIVOT_NEG_X;
        task->work->tick |= PIVOT_NEG_X;
        task->work->tick &= ~PIVOT_AT_BASE;
        task->work->tick |= PIVOT_AT_BASE;
        MOTION(task)->slide = 0.0F;
    }
    object = world_pool_alloc();
    EXTRA(task)->object = object;
    /* @bug memory: copies street_hammer_collision through null where
     * world_pool_alloc has none to lend, at 0x1aa of the object's code.
     * toppling_hammer_destroy tests the copy for null, at 0x2a4.
     * toppling_hammer_step writes through it untested, at 0x31e */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the hammer, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = street_hammer_collision;
    object->evalflags &= ~EVAL_UNITS;
    object->scl[0] = THICKNESS(work);
    object->scl[1] = HEIGHT(work);
    object->scl[2] = THICKNESS(work);
    object->ang[0] = 0;
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y + (HALF_HEIGHT * HEIGHT(work));
    object->pos[2] = work->pos.z;
    flags = WORLD_TURNED | WORLD_MOVING | WORLD_SOLID;
    /* @hack spelling: thickness and height, locals for the radius's two
     * squares, where the sum in place would do */
    thickness = THICKNESS(work) * THICKNESS(work) * (HALF_WIDTH * HALF_WIDTH);
    height = HEIGHT(work) * HEIGHT(work) * (HALF_HEIGHT * HALF_HEIGHT);
    RADIUS(task) = njSqrt(thickness + height);
    flags |= street_hammer_size_class(RADIUS(task));
    world_add(flags, task, object);
    toppling_hammer_place(task);
}

static void toppling_hammer_destroy(struct task *task)
{
    /* @hack spelling: extra, a local for the record read for the test
     * alone, where EXTRA(task) would do */
    struct toppling_hammer_extra *extra = EXTRA(task);

    if (extra->object != NULL) {
        world_remove(task, EXTRA(task)->object);
        world_pool_free(EXTRA(task)->object);
    }
    EXTRA(task)->object = NULL;
    syFree(EXTRA(task)->matrix);
    EXTRA(task)->matrix = NULL;
}

/* run the mode's step and place the hammer. its collision model's copy
 * takes the hammer's place and angles. both TASK_FORCE records take the turn
 * since the last step, which turns a player riding the copy */
static void toppling_hammer_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: extra, a volatile local that the record is stored in
     * and that nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    struct toppling_hammer_extra *volatile extra;
    /* @hack spelling: matrix, a local for the matrix's floats, where
     * *EXTRA(task)->matrix read in place would do */
    Float *matrix;
    NJS_OBJECT *object;
    Angle ax;
    Angle ay;
    Angle az;

    if (placement_try_leave(task)) {
        return;
    }
    toppling_hammer_take_mode(task);
    /* @hack spelling: task->work read again for the mode, where work would
     * do */
    toppling_hammer_modes[task->work->mode](task);
    toppling_hammer_place(task);
    extra = EXTRA(task);
    object = EXTRA(task)->object;
    matrix = *EXTRA(task)->matrix;
    object->pos[0] = matrix[MATRIX_X];
    object->pos[1] = matrix[MATRIX_Y];
    object->pos[2] = matrix[MATRIX_Z];
    matrix_angles_xyz(*EXTRA(task)->matrix, &ax, &ay, &az);
    TASK_FORCES_OF(task)
    [0].turn.x = TASK_FORCES_OF(task)[1].turn.x = ax - object->ang[0];
    TASK_FORCES_OF(task)
    [0].turn.y = TASK_FORCES_OF(task)[1].turn.y = ay - object->ang[1];
    TASK_FORCES_OF(task)
    [0].turn.z = TASK_FORCES_OF(task)[1].turn.z = az - object->ang[2];
    object->ang[0] = ax;
    object->ang[1] = ay;
    object->ang[2] = az;
    if (player_near((NJS_POINT3 *)object->pos, RADIUS(task) + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void toppling_hammer_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a store to task that no path reaches */
    if (0) {
        task = NULL;
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    matrix_multiply_before(EXTRA(task)->matrix);
    njScale(NULL, THICKNESS(work), HEIGHT(work), THICKNESS(work));
    njSetTexture(&street_hammer_texlist);
    njCnkEasyDrawModel(&street_hammer_model);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS);
    model_draw_tinted(
        &street_hammer_glow_model,
        color_blend(
            GLOW_FROM, GLOW_TO,
            (njAbs(njSin(game_state.steps << GLOW_SHIFT)) * (1.0F - GLOW_LOW)) +
                GLOW_LOW));
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
    /* @hack spelling: LEAN tested as under 1, where LEAN(work) == 0 would
     * do */
    if (LEAN(work) < 1) {
        toppling_hammer_draw_shadow(task);
    }
}

static void toppling_hammer_take_mode(struct task *task)
{
    if (NEXT_MODE(task->work) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task->work);
        NEXT_MODE(task->work) = NO_NEXT_MODE;
    }
}

/* the speed of the fall's burst */
static const NJS_VECTOR toppling_hammer_burst_spd = {0.0F, 0.0F, 0.0F};

static void toppling_hammer_stand(struct task *task)
{
    struct task_work *work = task->work;
    struct toppling_hammer_motion *motion = task->motion.p;
    /* @hack spelling: push, a local for FALL_PUSH, where FALL_PUSH in place
     * would do */
    Float push = FALL_PUSH;
    NJS_VECTOR spd;
    NJS_POINT3 pos;

    /* @hack redundant: a block no path reaches, and an empty else */
    if (0) {
        motion = NULL;
        task = NULL;
    } else {
    }
    if ((CHARGE_ID(work) != 0 && !dynamite_count(CHARGE_ID(work))) ||
        (CHARGE_ID(work) == 0 && enemy_hit_hook != NULL &&
         enemy_hit_hook(&work->pos))) {
        motion->slide_spd = push;
        spd = toppling_hammer_burst_spd;
        /* @hack spelling: task->work read again for pos, where work would
         * do */
        pos.x = task->work->pos.x;
        pos.y = task->work->pos.y + njAbs(THICKNESS(work) * CORNER_Z);
        pos.z = task->work->pos.z;
        burst_create(&pos, &spd, THICKNESS(work) * BURST_SIZE,
                     njAbs(THICKNESS(work) * CORNER_Z) * BURST_DEPTH);
        toppling_hammer_set_next_mode(MODE_FALL, task);
    }
}

/* turn the hammer about its pivot until it reaches its stop. there it lands,
 * and the rock starts. swing_x and swing_y are the angles of the turn's and
 * the twist's pendulums. each is 0 at the bottom of its swing */
static void toppling_hammer_fall(struct task *task)
{
    struct task_work *work = task->work;
    struct toppling_hammer_motion *motion = task->motion.p;
    Angle swing_x;
    Angle swing_y;
    Angle last_x;
    Float energy;
    Float speed_x;
    Float speed_y;
    Float lift;

    /* @hack redundant: the addresses of speed_y and speed_x, taken and
     * dropped */
    (void)&speed_y;
    (void)&speed_x;
    /* @hack spelling: MOTION(task) read again, where motion would do */
    swing_x =
        njAbs((Angle)(atanf(-(HALF_HEIGHT / CORNER_Z)) * ANGLE_PER_RADIAN)) +
        MOTION(task)->tilt.x + QUARTER_TURN;
    speed_x =
        njSqrt((toppling_hammer_turn_speed * toppling_hammer_turn_speed) -
               (njSqrt((CORNER_Z * CORNER_Z) + (HALF_HEIGHT * HALF_HEIGHT)) *
                (1.0F - njCos(swing_x)) * FALL_GRAVITY));
    motion->tilt_spd_x =
        speed_x / njSqrt((CORNER_Z * CORNER_Z) + (HALF_HEIGHT * HALF_HEIGHT)) *
        ANGLE_PER_RADIAN;
    swing_y = njAbs((Angle)(atanf(-(CORNER_Z / CORNER_X)) * ANGLE_PER_RADIAN)) +
              MOTION(task)->tilt.y + QUARTER_TURN;
    /* @hack spelling: LEAN tested as under 1, where LEAN(work) == 0 would
     * do */
    if (LEAN(work) < 1) {
        energy = -(njSqrt((CORNER_Z * CORNER_Z) + (CORNER_X * CORNER_X)) *
                   (1.0F - njCos(swing_y)) * FALL_GRAVITY);
    } else {
        energy = (toppling_hammer_twist_speed * toppling_hammer_twist_speed) -
                 (njSqrt((CORNER_Z * CORNER_Z) + (CORNER_X * CORNER_X)) *
                  (1.0F - njCos(swing_y)) * FALL_GRAVITY);
    }
    speed_y = njSqrt(energy);
    motion->tilt_spd_y = speed_y /
                         njSqrt((CORNER_Z * CORNER_Z) + (CORNER_X * CORNER_X)) *
                         ANGLE_PER_RADIAN;
    last_x = MOTION(task)->tilt.x;
    MOTION(task)->tilt.x += motion->tilt_spd_x;
    MOTION(task)->tilt.y += motion->tilt_spd_y;
    if (MOTION(task)->tilt.y < 0) {
        MOTION(task)->tilt.y = 0;
        motion->tilt_spd_y = -motion->tilt_spd_y * toppling_hammer_bounce;
    } else if (MOTION(task)->tilt.y >= TWIST_MAX) {
        MOTION(task)->tilt.y = TWIST_MAX;
        motion->tilt_spd_y = -motion->tilt_spd_y * toppling_hammer_bounce;
    }
    motion->slide_spd *= SLIDE_KEEP;
    MOTION(task)->slide += motion->slide_spd;
    if (MOTION(task)->tilt.x > QUARTER_TURN - LEAN(work)) {
        MOTION(task)->tilt.x = QUARTER_TURN - LEAN(work);
    }
    if (swing_x > 0) {
        lift = -((njSin(MOTION(task)->tilt.x) - njSin(last_x)) *
                 njSqrt((CORNER_Z * CORNER_Z) + (HALF_HEIGHT * HALF_HEIGHT)) *
                 swing_x * SLIDE_SCALE);
        MOTION(task)->slide -= lift;
    }
    if (MOTION(task)->tilt.x >= QUARTER_TURN - LEAN(work)) {
        NJS_POINT3 bottom;
        NJS_POINT3 top;
        NJS_LINE line;

        njPushMatrixEx();
        njUnitMatrix(NULL);
        matrix_set(EXTRA(task)->matrix);
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, -(HEIGHT(work) * HALF_HEIGHT), 0.0F);
        njGetTranslation(NULL, &bottom);
        njPopMatrixEx();
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, HEIGHT(work) * HALF_HEIGHT, 0.0F);
        njGetTranslation(NULL, &top);
        njPopMatrixEx();
        njPopMatrixEx();
        /* @hack redundant: a call no path reaches */
        if (0) {
            njPopMatrixEx();
        }
        line.px = bottom.x;
        line.py = bottom.y;
        line.pz = bottom.z;
        line.vx = top.x - line.px;
        line.vy = top.y - line.py;
        line.vz = top.z - line.pz;
        toppling_hammer_shake(task, &line);
        motion->tilt_spd_x =
            -motion->tilt_spd_x * toppling_hammer_bounce * njCos(LEAN(work));
        motion->tilt_spd_y = -motion->tilt_spd_y * toppling_hammer_bounce;
        motion->slide_spd = REBOUND_SLIDE_SPD;
        if (LEAN(work) < 1) {
            Float x;
            Float z;
            NJS_POINT3 pos;
            struct angles ang;

            /* @hack spelling: task->work read again for ang.y and pos,
             * where work would do */
            toppling_hammer_polar_xz(task->work->ang.y,
                                     (HEIGHT(work) * HALF_HEIGHT) +
                                         njAbs(THICKNESS(work) * CORNER_Z) +
                                         njAbs(MOTION(task)->slide),
                                     &x, &z);
            pos.x = task->work->pos.x + x;
            pos.y = task->work->pos.y;
            pos.z = task->work->pos.z + z;
            ang.x = ang.z = 0;
            ang.y = task->work->ang.y;
            /* @hack spelling: each half size negated inside njAbs, where njAbs
             * of the size would do */
            dust_rect(&pos, njAbs(-(THICKNESS(work) * CORNER_X)),
                      njAbs(-(HEIGHT(work) * HALF_HEIGHT)), &ang, DUST_SCALE,
                      DUST_DENSITY, DUST_SPEED);
        }
        sound_play_at(LAND_SOUND, NULL, LAND_SOUND_PRIORITY,
                      toppling_hammer_land_volume, &task->work->pos);
        placement_set_state(task, STATE_FALLEN);
        toppling_hammer_set_next_mode(MODE_ROCK, task);
    }
}

/* rebound from the stop and rock back to it, swinging as the fall does. a
 * flat hammer rocks on its +z face, between the corners at its base and at
 * its top */
static void toppling_hammer_rock(struct task *task)
{
    struct task_work *work = task->work;
    struct toppling_hammer_motion *motion = task->motion.p;
    /* @hack spelling: motion_1 to motion_4, task->motion set inside each
     * test, where motion would do */
    struct toppling_hammer_motion *motion_1;
    struct toppling_hammer_motion *motion_2;
    struct toppling_hammer_motion *motion_3;
    struct toppling_hammer_motion *motion_4;
    Angle swing_x;
    Angle swing_y;

    /* @hack spelling: each pivot bit shifted down and compared with 1 or 0,
     * where a test of its mask would do */
    /* @hack spelling: LEAN tested against 1, where tests against 0 would do */
    if ((work->tick >> PIVOT_AT_BASE_SHIFT & 1) == 1 || LEAN(work) >= 1) {
        /* @hack spelling: MOTION(task) read again, here and below, where
         * motion would do */
        swing_x = njAbs((Angle)(atanf(-(HALF_HEIGHT / CORNER_Z)) *
                                ANGLE_PER_RADIAN)) +
                  MOTION(task)->tilt.x + QUARTER_TURN;
    } else {
        swing_x =
            njAbs((Angle)(atanf(HALF_HEIGHT / CORNER_Z) * ANGLE_PER_RADIAN)) +
            MOTION(task)->tilt.x - QUARTER_TURN;
    }
    motion->tilt_spd_x +=
        (Angle)(-(njSin(swing_x) * ROCK_GRAVITY) /
                njSqrt((CORNER_Z * CORNER_Z) + (HALF_HEIGHT * HALF_HEIGHT)) *
                ANGLE_PER_RADIAN);
    MOTION(task)->tilt.x += motion->tilt_spd_x;
    if (LEAN(work) < 1) {
        /* @hack spelling: task->work read again for the tick, where work
         * would do */
        if ((task->work->tick >> PIVOT_AT_BASE_SHIFT & 1) == 1 &&
            MOTION(task)->tilt.x > QUARTER_TURN - LEAN(work)) {
            MOTION(task)->tilt.x = QUARTER_TURN - LEAN(work);
            motion->tilt_spd_x = motion->tilt_spd_x * toppling_hammer_bounce;
            task->work->tick &= ~PIVOT_AT_BASE;
            if (((motion_1 = task->motion.p)->tilt.y <= 0 ||
                 motion_1->tilt.y >= TWIST_MAX) &&
                njAbs(motion->tilt_spd_x) < REST_SPEED &&
                njAbs(motion->tilt_spd_y) < REST_SPEED) {
                toppling_hammer_set_next_mode(MODE_LIE, task);
            }
        } else if ((task->work->tick >> PIVOT_AT_BASE_SHIFT & 1) == 0 &&
                   MOTION(task)->tilt.x < QUARTER_TURN - LEAN(work)) {
            MOTION(task)->tilt.x = QUARTER_TURN - LEAN(work);
            motion->tilt_spd_x = motion->tilt_spd_x * toppling_hammer_bounce;
            /* @hack spelling: each bit cleared, then set, where setting it
             * would do */
            task->work->tick &= ~PIVOT_AT_BASE;
            task->work->tick |= PIVOT_AT_BASE;
            if (((motion_2 = task->motion.p)->tilt.y <= 0 ||
                 motion_2->tilt.y >= TWIST_MAX) &&
                njAbs(motion->tilt_spd_x) < REST_SPEED &&
                njAbs(motion->tilt_spd_y) < REST_SPEED) {
                toppling_hammer_set_next_mode(MODE_LIE, task);
            }
        }
    } else if (MOTION(task)->tilt.x > QUARTER_TURN - LEAN(work)) {
        MOTION(task)->tilt.x = QUARTER_TURN - LEAN(work);
        motion->tilt_spd_x = -motion->tilt_spd_x * LEAN_BOUNCE;
        if (((motion_3 = task->motion.p)->tilt.y <= 0 ||
             motion_3->tilt.y >= TWIST_MAX) &&
            njAbs(motion->tilt_spd_x) < REST_SPEED &&
            njAbs(motion->tilt_spd_y) < REST_SPEED) {
            toppling_hammer_set_next_mode(MODE_LIE, task);
        }
    }
    if (MOTION(task)->tilt.x < 0) {
        MOTION(task)->tilt.x = 0;
        motion->tilt_spd_x = -motion->tilt_spd_x * toppling_hammer_bounce;
        motion->tilt_spd_y = -motion->tilt_spd_y * toppling_hammer_bounce;
        motion->slide_spd = REBOUND_SLIDE_SPD;
    }
    if ((task->work->tick & PIVOT_NEG_X) == PIVOT_NEG_X) {
        swing_y =
            njAbs((Angle)(atanf(-(CORNER_X / CORNER_Z)) * ANGLE_PER_RADIAN)) +
            MOTION(task)->tilt.y + QUARTER_TURN;
    } else {
        swing_y =
            njAbs((Angle)(atanf(CORNER_X / CORNER_Z) * ANGLE_PER_RADIAN)) +
            MOTION(task)->tilt.y - QUARTER_TURN;
    }
    motion->tilt_spd_y +=
        (Angle)(-(njSin(swing_y) * ROCK_GRAVITY) /
                njSqrt((CORNER_Z * CORNER_Z) + (CORNER_X * CORNER_X)) *
                ANGLE_PER_RADIAN);
    MOTION(task)->tilt.y += motion->tilt_spd_y;
    if ((task->work->tick & PIVOT_NEG_X) == PIVOT_NEG_X &&
        MOTION(task)->tilt.y < 0) {
        MOTION(task)->tilt.y = 0;
        motion->tilt_spd_y = motion->tilt_spd_y * toppling_hammer_bounce;
        task->work->tick &= ~PIVOT_NEG_X;
    } else if ((task->work->tick & PIVOT_NEG_X) == 0 &&
               MOTION(task)->tilt.y > 0) {
        MOTION(task)->tilt.y = 0;
        motion->tilt_spd_y = motion->tilt_spd_y * toppling_hammer_bounce;
        task->work->tick &= ~PIVOT_NEG_X;
        task->work->tick |= PIVOT_NEG_X;
    }
    if (MOTION(task)->tilt.x <= 0) {
        if (((motion_4 = task->motion.p)->tilt.y <= 0 ||
             motion_4->tilt.y >= TWIST_MAX) &&
            njAbs(motion->tilt_spd_x) < REST_SPEED) {
            toppling_hammer_set_next_mode(MODE_LIE, task);
        }
    }
}

static void toppling_hammer_lie(struct task *task)
{
    struct task_work *work = task->work;
    struct toppling_hammer_motion *motion = task->motion.p;

    motion->tilt.x = QUARTER_TURN - LEAN(work);
    /* @hack spelling: MOTION(task) read again for tilt.y, where motion
     * would do */
    MOTION(task)->tilt.y = 0;
}

/* while the hammer falls, the part takes the hammer's place and turn and
 * registers its record. at other times it collides with nothing */
static void toppling_hammer_part_step(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *work = task->work;

    if (parent->work->mode == MODE_FALL) {
        njPushMatrixEx();
        matrix_set(EXTRA(parent)->matrix);
        njGetTranslation(NULL, &work->pos);
        current_matrix_angles_yxz(&work->ang.x, &work->ang.y, &work->ang.z);
        njPopMatrixEx();
        collision_register(task);
    }
}

/* draw the hammer flattened onto the plane SHADOW_LIFT above pos, in
 * SHADOW_COLOR */
static void toppling_hammer_draw_shadow(struct task *task)
{
    /* @hack spelling: row, a local for the matrix's floats, where matrix
     * read in place would do */
    Float *row;
    struct task_work *work = task->work;
    NJS_MATRIX matrix;

    row = matrix;
    njPushMatrixEx();
    /* @hack redundant: a store to task that no path reaches */
    if (0) {
        task = NULL;
    }
    njSetMatrix(NULL, &toppling_hammer_flatten);
    matrix_multiply_before(EXTRA(task)->matrix);
    njScale(NULL, THICKNESS(work), HEIGHT(work), THICKNESS(work));
    njGetMatrix(&matrix);
    njPopMatrixEx();
    row[MATRIX_Y] = work->pos.y + SHADOW_LIFT;
    njPushMatrixEx();
    njMultiMatrix(NULL, &matrix);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
    model_draw_tinted(&street_hammer_model, SHADOW_COLOR);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
}

/* set TASK_EXTRA's matrix from the hammer's pos, turn and slide, and its
 * tilts about its pivot */
static void toppling_hammer_place(struct task *task)
{
    struct task_work *work = task->work;
    Float x;

    x = (work->tick & PIVOT_NEG_X) ? -(THICKNESS(work) * CORNER_X)
                                   : THICKNESS(work) * CORNER_X;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y + (HALF_HEIGHT * HEIGHT(work)),
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    njTranslate(NULL, 0.0F, 0.0F, -MOTION(task)->slide);
    /* @hack spelling: PIVOT_AT_BASE shifted down and masked, through
     * task->work read again, where work->tick & PIVOT_AT_BASE would do */
    if (task->work->tick >> PIVOT_AT_BASE_SHIFT & 1) {
        njTranslate(NULL, x, -(HEIGHT(work) * HALF_HEIGHT),
                    THICKNESS(work) * CORNER_Z);
        njRotateX(NULL, MOTION(task)->tilt.x);
        njRotateY(NULL, MOTION(task)->tilt.y);
        njTranslate(NULL, -x, HEIGHT(work) * HALF_HEIGHT,
                    -(THICKNESS(work) * CORNER_Z));
    } else {
        njTranslate(NULL, x, -(HEIGHT(work) * HALF_HEIGHT),
                    THICKNESS(work) * CORNER_Z);
        njRotateX(NULL, QUARTER_TURN - LEAN(work));
        njTranslate(NULL, -x, HEIGHT(work) * HALF_HEIGHT,
                    -(THICKNESS(work) * CORNER_Z));
        njTranslate(NULL, x, HEIGHT(work) * HALF_HEIGHT,
                    THICKNESS(work) * CORNER_Z);
        njRotateX(NULL, MOTION(task)->tilt.x + LEAN(work) - QUARTER_TURN);
        njRotateY(NULL, MOTION(task)->tilt.y);
        njTranslate(NULL, -x, -(HEIGHT(work) * HALF_HEIGHT),
                    -(THICKNESS(work) * CORNER_Z));
    }
    matrix_get(EXTRA(task)->matrix);
    njPopMatrixEx();
}

/* turn (0, 0, radius) by ang about y. *x and *z take the result's x and
 * z */
static void toppling_hammer_polar_xz(Angle ang, Float radius, Float *x,
                                     Float *z)
{
    *x = njSin(ang) * radius;
    *z = njCos(ang) * radius;
}

/* shake camera 0 and vibrate player 0's pad by player 0's distance from
 * line's segment. a camera that already shakes harder keeps its shake, and
 * the pad stays still */
static void toppling_hammer_shake(struct task *task, NJS_LINE *line)
{
    Float distance;
    Float size;
    Sint32 power;

    distance = toppling_hammer_line_distance(&player_work[0]->pos, line, NULL);
    power = toppling_hammer_vibration_power;
    if (distance > SHAKE_NEAR) {
        size = toppling_hammer_shake_size / (distance * (1.0F / SHAKE_NEAR));
    } else {
        size = toppling_hammer_shake_size;
    }
    if (distance > VIBRATION_FAR) {
        power = power / (distance * (1.0F / VIBRATION_FAR));
    }
    if (camera_stacks[0].shake_steps > 0) {
        if (size >= camera_stacks[0].shake_size) {
            camera_shake(0, toppling_hammer_shake_steps, size);
            vibration_start(0, power, toppling_hammer_vibration_freq,
                            toppling_hammer_vibration_time);
        }
    } else {
        camera_shake(0, toppling_hammer_shake_steps, size);
        vibration_start(0, power, toppling_hammer_vibration_freq,
                        toppling_hammer_vibration_time);
    }
}

/* draw model scaled by scale, with its r scaled while it draws */
static void toppling_hammer_draw_scaled(NJS_CNK_MODEL *model, Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r read
     * and written in place would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    njCnkEasyDrawModel(model);
    *radius = saved;
}

/* draw debug_models' sphere scaled by scale, in a flat see-through
 * gray */
void toppling_hammer_draw_sphere(Float scale)
{
    /* @hack spelling: radius, a pointer to the sphere's r, where the r read
     * and written in place would do */
    Float *radius = &debug_models[DEBUG_MODEL_SPHERE]->model->r;
    Float saved = *radius;

    *radius *= scale;
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ISA);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS | NJD_FST_IL);
    constant_material_set(SPHERE_SHADE, SPHERE_SHADE, SPHERE_SHADE,
                          SPHERE_SHADE);
    njPushMatrixEx();
    toppling_hammer_draw_scaled(debug_models[DEBUG_MODEL_SPHERE]->model, scale);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

/* the distance from point to the segment that runs from line's p by its v.
 * where nearest isn't null, it takes the segment's point nearest point */
static Float toppling_hammer_line_distance(NJS_POINT3 *point, NJS_LINE *line,
                                           NJS_POINT3 *nearest)
{
    NJS_VECTOR delta;
    NJS_POINT3 start;
    NJS_POINT3 closest;
    Float along;
    Float length_squared;

    start.x = line->px;
    start.y = line->py;
    start.z = line->pz;
    length_squared =
        (line->vx * line->vx) + (line->vy * line->vy) + (line->vz * line->vz);
    delta.x = point->x - start.x;
    delta.y = point->y - start.y;
    delta.z = point->z - start.z;
    along = (delta.x * line->vx) + (delta.y * line->vy) + (delta.z * line->vz);
    /* @hack redundant: a store to length_squared that no path reaches */
    if (0) {
        length_squared = 0.0F;
    }
    if (along > 0.0F) {
        if (along >= length_squared) {
            closest.x = start.x + line->vx;
            closest.y = start.y + line->vy;
            closest.z = start.z + line->vz;
        } else {
            along /= length_squared;
            closest.x = start.x + (along * line->vx);
            closest.y = start.y + (along * line->vy);
            closest.z = start.z + (along * line->vz);
        }
    } else {
        closest = start;
    }
    if (nearest) {
        *nearest = closest;
    }
    return njDistanceP2P(&closest, point);
}

/* toppling_hammer_line_distance, but it finds the segment's nearest point
 * by x and z alone */
Float toppling_hammer_line_distance_xz(NJS_POINT3 *point, NJS_LINE *line,
                                       NJS_POINT3 *nearest)
{
    NJS_VECTOR delta;
    NJS_POINT3 start;
    NJS_POINT3 closest;
    Float length_squared;
    Float along;

    start.x = line->px;
    start.y = line->py;
    start.z = line->pz;
    length_squared = (line->vx * line->vx) + (line->vz * line->vz);
    delta.x = point->x - start.x;
    delta.y = point->y - start.y;
    delta.z = point->z - start.z;
    along = (delta.x * line->vx) + (delta.z * line->vz);
    if (along > 0.0F) {
        if (along >= length_squared) {
            closest.x = start.x + line->vx;
            closest.y = start.y + line->vy;
            closest.z = start.z + line->vz;
        } else {
            along /= length_squared;
            closest.x = start.x + (along * line->vx);
            closest.y = start.y + (along * line->vy);
            closest.z = start.z + (along * line->vz);
        }
    } else {
        closest = start;
    }
    if (nearest) {
        *nearest = closest;
    }
    return njDistanceP2P(&closest, point);
}
