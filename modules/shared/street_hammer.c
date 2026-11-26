/* the hammer, HAMMER in Mission Street's object list: a solid column the
 * players can ride. it strokes along y to its pos and back, and rests at each
 * end. it can cast a shadow to the floor below it */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/surface.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/modifier_volume.h"
#include "shared/street_hammer.h"
#include "shared/street_hammer/text.inc"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* a hammer's modes, in its work's mode. MODE_TO_POS rests at the far end,
 * then strokes to pos. MODE_BACK rests at pos, then strokes back */
enum street_hammer_mode { MODE_TO_POS, MODE_BACK };

/* the hammer's settings, as street_hammer.h lists them */
#define REST(work) ((Uint8)(work)->ang.x)
#define STROKES_DOWN(work) ((work)->ang.x & 0xff00)
#define TURN(work) ((work)->ang.y & 0xff00)
#define PHASE(work) ((Uint8)(work)->ang.y)
#define SPEED(work) ((work)->ang.z & 0xfff)
#define SHADOWED(work) ((work)->ang.z & 0xf000)
#define WIDTH(work) ((work)->scl.x + 1.0F)
#define HEIGHT(work) ((work)->scl.y + 1.0F)
/* PHASE(work) counts a cycle in PHASE_STEPS parts */
#define PHASE_STEPS 256

/* the collision model's half width and half height, about its origin. the
 * hammer's radius reaches a top corner of the model scaled by HEIGHT along y
 * alone */
#define HALF_WIDTH 25.0F
#define HALF_HEIGHT 50.0F
/* a player within the hammer's radius plus REACH_MARGIN of its center is
 * near */
#define REACH_MARGIN 10.0F

/* TASK_EXTRA's record. offset is how far the hammer lies above its pos,
 * below 0 for below it. floor is the height of the floor under the hammer,
 * which a hammer with a shadow probes */
struct street_hammer_extra {
    Float offset;
    Float floor;
};
#define HAMMER(task) ((struct street_hammer_extra *)(task)->extra.p)
/* the record as an array of its two Floats, offset then floor */
union street_hammer_heights {
    struct street_hammer_extra fields;
    Float heights[2];
};
#define HEIGHTS(task)                                                          \
    (((union street_hammer_heights *)(task)->extra.p)->heights)
/* struct task under a type of its own. HAMMER_CAST reaches the record through
 * a cast of the task to it, for the draw's shadow test alone */
struct street_hammer_task {
    struct task task;
};
#define HAMMER_CAST(task)                                                      \
    ((struct street_hammer_extra *)((struct street_hammer_task *)(task))       \
         ->task.extra.p)
/* the hammer's radius, in task->unk_3c */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define RADIUS(task) (*(Float *)&(task)->unk_3c)
/* the TASK_FORCE record at index, of two */
#define FORCE(task, index) (&((struct task_force *)(task)->force.p)[index])

/* the shadow is modifier_volume_cylinder_model, SHADOW_HALF_LENGTH from its
 * middle to each end. the draw stretches it from the hammer's bottom to the
 * floor. SHADOW_OVERLAP, added to the stretch, carries each end
 * SHADOW_HALF_LENGTH * SHADOW_OVERLAP past. across, the draw scales it by
 * njSqrt(SHADOW_WIDTH) times WIDTH. a floor SHADOW_REACH or more away draws no
 * shadow */
#define SHADOW_REACH 250.0F
#define SHADOW_HALF_LENGTH 15.0F
#define SHADOW_WIDTH 3.0F
#define SHADOW_OVERLAP 0.02F

/* the glow's blend from street_hammer_glow_from toward street_hammer_glow_to
 * swings from GLOW_LOW to 1 and back every 64 steps. it follows the sine's
 * magnitude, at an angle of game_state.steps << GLOW_SHIFT */
#define GLOW_LOW 0.5F
#define GLOW_SHIFT 9

static void street_hammer_destroy(struct task *task);
static void street_hammer_step(struct task *task);
static void street_hammer_draw(struct task *task);
static void street_hammer_set_phase(struct task *task);
static void street_hammer_draw_scaled(NJS_CNK_MODEL *model, NJS_VECTOR *scale);
static void street_hammer_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                             NJS_VECTOR *scale);

#include "shared/road_furniture/hammer_textures.h"

NJS_TEXLIST street_hammer_texlist = {
#include "shared/street_hammer/street_hammer_texlist.inc"
};

#include "shared/road_furniture/hammer_glow.h"

NJS_CNK_MODEL street_hammer_glow_model = {
#include "shared/street_hammer/street_hammer_glow_model.inc"
};

#include "shared/road_furniture/hammer_column.h"

NJS_CNK_MODEL street_hammer_model = {
#include "shared/street_hammer/street_hammer_model.inc"
};

#include "shared/road_furniture/hammer_collision.h"

NJS_OBJECT street_hammer_collision = {
#include "shared/street_hammer/street_hammer_collision.inc"
};

NJS_TEXLIST *street_hammer_texlists[2] = {
#include "shared/street_hammer/street_hammer_texlists.inc"
};
/* the glow's colors, clear black and white */
static Uint32 street_hammer_glow_from = 0x00000000;
static Uint32 street_hammer_glow_to = 0xffffffff;

#ifdef NONMATCHING
/* this side repairs the bug tagged at the probe's calls: the floor takes the
 * height surface_probe_floor returns */
static void street_hammer_probe_floor(struct task *task, struct angles *angle)
{
    struct street_hammer_extra *hammer = HAMMER(task);
    NJS_POINT3 *pos = &task->work->pos;
    hammer->floor =
        surface_probe_floor(pos->x, pos->y + hammer->offset, pos->z, angle);
}
#else
/* surface_probe_floor under a second name, declared to return an int. a
 * caller takes the int from R0, which holds no result: the height comes
 * back in FR0 */
int surface_probe_floor_int(Float x, Float y, Float z, struct angles *ang);
#endif

void street_hammer_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float area;
    struct angles ang;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = street_hammer_draw;
    task->step = street_hammer_step;
    task->destructor = street_hammer_destroy;
    street_hammer_set_phase(task);
    HAMMER(task)->offset = 0.0F;
    /* @hack redundant: a block no path reaches, kept for its strings, which SHC
     * still places in this source's constants, and for its store. the image
     * holds none of its code, so the store is a stand-in, without which SHC
     * writes other code */
    if (0) {
        (void)"<- UPDOWN / REST(   )";
        (void)"(0:UP not0:DOWN)<    >";
        (void)"DOWN";
        (void)" UP ";
        (void)"<- ANG Y / RUG(  %)";
        (void)"<- KAGE(   ) / SPEED";
        (void)"ON ";
        (void)"OFF";
        (void)"<- WIDTH SCALE(1.0+x)";
        (void)"<- HEIGHT SCALE(1.0+x)";
        (void)"<- HOVER HEIGHT";
        task->work->ang.x |= task->work->ang.y;
    }
    object = world_pool_alloc();
    task->motion.p = object;
    /* @bug memory: copies street_hammer_collision through null where
     * world_pool_alloc has none to lend, though street_hammer_destroy tests
     * task->motion for null. street_hammer_step reads through it untested:
     * 0x44, 0x14e and 0x25c of the object's code */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the hammer, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = street_hammer_collision;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    /* @hack spelling: task->work read again for the scales, the turn, the
     * height and the shadow, where work would do */
    object->scl[0] = WIDTH(task->work);
    object->scl[1] = HEIGHT(task->work);
    object->scl[2] = WIDTH(task->work);
    object->ang[1] = TURN(task->work);
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_MOVING | WORLD_SOLID;
    area =
        HEIGHT(task->work) * HEIGHT(task->work) * (HALF_HEIGHT * HALF_HEIGHT);
    RADIUS(task) = njSqrt(area + (HALF_WIDTH * HALF_WIDTH));
    flags |= street_hammer_size_class(RADIUS(task));
    world_add(flags, task, object);
    if (SHADOWED(task->work)) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side, by
         * street_hammer_probe_floor */
        street_hammer_probe_floor(task, &ang);
#else
        Float *heights;

        /* @hack type: the floor stored through heights, set to the record
         * as an array inside the store, and the offset read from that
         * array, where HAMMER(task)'s fields would do */
        /* @bug mismatched-call: takes R0 for the floor's height, where
         * surface_probe_floor returns it in FR0. the floor is whatever R0
         * holds as the probe returns (LDS R0,FPUL at 0x12e of the object's
         * code). the shadow then reaches toward that height */
        (heights = HEIGHTS(task))[1] = surface_probe_floor_int(
            work->pos.x, work->pos.y + HEIGHTS(task)[0], work->pos.z, &ang);
#endif
    }
}

static void street_hammer_destroy(struct task *task)
{
    if (task->motion.p != NULL) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    task->motion.p = NULL;
}

static void street_hammer_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Angle ang;
    Float stroke;
    struct angles floor_ang;
    /* @hack redundant: a word of the frame that nothing reads or writes */
    Sint32 unused[1];

    if (placement_try_leave(task)) {
        return;
    }
    street_hammer_set_phase(task);
    /* @hack spelling: task->work read again for the timer, the speed, the
     * stroke's way and the shadow, where work would do */
    ang = (Uint16)(task->work->timer * SPEED(task->work));
    ang += work->mode != MODE_TO_POS ? HALF_TURN : 0;
    stroke = njSin(ang + QUARTER_TURN);
    /* @hack redundant: a store to stroke that no path reaches */
    if (0) {
        stroke = 0.0F;
    }
    if (STROKES_DOWN(task->work)) {
        HAMMER(task)->offset = (0.5F + (stroke * 0.5F)) * work->scl.z;
    } else {
        HAMMER(task)->offset = -((0.5F + (stroke * 0.5F)) * work->scl.z);
    }
    /* @hack redundant: a goto to the statement after it */
    goto move;
move:
    object = task->motion.p;
    FORCE(task, 0)->speed.y = FORCE(task, 1)->speed.y =
        work->pos.y + HAMMER(task)->offset - object->pos[1];
    object->pos[1] = work->pos.y + HAMMER(task)->offset;
    if (player_near((NJS_POINT3 *)object->pos, RADIUS(task) + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    if (SHADOWED(task->work)) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged in street_hammer_init, by
         * street_hammer_probe_floor */
        street_hammer_probe_floor(task, &floor_ang);
#else
        /* @hack type: the floor stored through heights, a pointer to the
         * record as an array, and the offset read from that array, where
         * HAMMER(task)'s fields would do */
        Float *heights = HEIGHTS(task);

        /* @bug mismatched-call: takes R0 for the floor's height, as
         * street_hammer_init does (LDS R0,FPUL at 0x2d4 of the object's
         * code) */
        heights[1] =
            surface_probe_floor_int(work->pos.x, work->pos.y + HEIGHTS(task)[0],
                                    work->pos.z, &floor_ang);
#endif
    }
}

static void street_hammer_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;
    /* @hack spelling: depth declared ahead of height, where the order the
     * draw sets them in would do */
    Float depth;
    Float height;
    /* @hack spelling: bottom, a local for the hammer's bottom, where the sum
     * in place would do */
    Float bottom;
    Float stretch;
    Float width;
    Float *radius;
    Float saved;
    Uint32 color;
    /* @hack redundant: hammer, a volatile local that the record is stored
     * in and that nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    struct street_hammer_extra *volatile hammer;

    njPushMatrixEx();
    /* @hack redundant: a statement that names njPopMatrixEx and does
     * nothing */
    (void)njPopMatrixEx;
    njTranslate(NULL, work->pos.x, work->pos.y + HAMMER(task)->offset,
                work->pos.z);
    /* @hack spelling: task->work read again for the turn, the scales and
     * the shadow, where work would do */
    njRotateY(NULL, TURN(task->work));
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    scale.x = scale.z = WIDTH(task->work);
    scale.y = HEIGHT(task->work);
    njSetTexture(&street_hammer_texlist);
    street_hammer_draw_scaled(&street_hammer_model, &scale);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
    if (SHADOWED(task->work)) {
        /* @hack type: the offset read through the task as a record of its
         * own, where HAMMER(task) would do */
        /* @bug logic: the test measures from pos.y less the offset, where the
         * hammer lies at pos.y plus it. a hammer above pos seems nearer a floor
         * below it than it is (FADD at 0x3d4 of the object's code) */
        if (njAbs(HAMMER(task)->floor - work->pos.y +
                  HAMMER_CAST(task)->offset) < SHADOW_REACH) {
            njPushMatrixEx();
            height = HEIGHT(task->work) * HALF_HEIGHT;
            hammer = HAMMER(task);
            bottom = work->pos.y + HAMMER(task)->offset - height;
            /* @hack type: the floor read as the record's second Float, where
             * HAMMER(task)->floor would do */
            /* @hack spelling: depth set inside the product for stretch,
             * where a statement of its own would do */
            stretch =
                (depth = njAbs(bottom - ((Float *)task->extra.p)[1]) * 0.5F) *
                (1.0F / SHADOW_HALF_LENGTH);
            njTranslate(NULL, 0.0F, -depth - height, 0.0F);
            /* @hack redundant: a goto to the statement after it */
            goto shadow;
        shadow:
            draw_control_set(DRAW_CONTROL_SHADOW);
            width = WIDTH(task->work) > stretch ? WIDTH(task->work) : stretch;
            /* @hack spelling: radius stepped 20 bytes from the model's
             * address, where &modifier_volume_cylinder_model.r would do */
            radius = (Float *)&modifier_volume_cylinder_model;
            radius = (Float *)((Uint8 *)radius + 20);
            saved = *radius;
            *radius *= width;
            njScale(NULL, njSqrt(SHADOW_WIDTH) * WIDTH(task->work),
                    stretch + SHADOW_OVERLAP,
                    njSqrt(SHADOW_WIDTH) * WIDTH(task->work));
            njCnkModDrawModel(&modifier_volume_cylinder_model);
            *radius = saved;
            draw_control_clear(DRAW_CONTROL_SHADOW);
            njPopMatrixEx();
        }
    }
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS);
    color = color_blend(
        street_hammer_glow_from, street_hammer_glow_to,
        (njAbs(njSin(game_state.steps << GLOW_SHIFT)) * (1.0F - GLOW_LOW)) +
            GLOW_LOW);
    njPushMatrixEx();
    scale.x = scale.z = WIDTH(task->work);
    scale.y = HEIGHT(task->work);
    street_hammer_draw_tinted_scaled(&street_hammer_glow_model, color, &scale);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
    /* @hack redundant: a call no path reaches */
    if (0) {
        draw_control_clear(DRAW_CONTROL_SHADOW);
    }
}

/* set the work's mode, timer and tick to the hammer's place in its cycle,
 * from game_state.steps. timer counts a stroke's steps, and is 0 through a
 * rest. tick counts the steps left of a rest, and is 0 through a stroke */
static void street_hammer_set_phase(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 stroke;
    Uint32 rest;
    Uint32 half;
    Uint32 cycle;
    Uint32 place;

    /* @hack spelling: task->work read again for all but the mode, where work
     * would do */
    stroke = HALF_TURN / SPEED(task->work);
    rest = REST(task->work);
    half = stroke + rest;
    cycle = half * 2;
    if (cycle != 0) {
        place = (Sint32)((Float)game_state.steps +
                         ((Float)cycle * (Float)PHASE(task->work) *
                          (1.0F / PHASE_STEPS))) %
                cycle;
    } else {
        place = 0;
    }
    task->work->tick = 0;
    if (place >= half) {
        task->work->timer = place - half;
        work->mode = MODE_BACK;
        if (task->work->timer > stroke) {
            task->work->tick = half - task->work->timer;
            task->work->timer = 0;
            work->mode = MODE_TO_POS;
        }
    } else {
        task->work->timer = place;
        work->mode = MODE_TO_POS;
        if (task->work->timer > stroke) {
            task->work->tick = half - task->work->timer;
            task->work->timer = 0;
            work->mode = MODE_BACK;
        }
    }
}

Uint32 street_hammer_size_class(Float radius)
{
    /* @hack spelling: an else after each return, where each test after the
     * if before it would do */
    if (radius > WORLD_SIZE_CLASS_1_MAX) {
        return WORLD_SIZE_CLASS(0);
    } else if (radius > WORLD_SIZE_CLASS_2_MAX) {
        return WORLD_SIZE_CLASS(1);
    } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
        return WORLD_SIZE_CLASS(2);
    } else {
        return WORLD_SIZE_CLASS(3);
    }
}

static void street_hammer_draw_scaled(NJS_CNK_MODEL *model, NJS_VECTOR *scale)
{
    Float largest;
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius;
    Float saved;

    largest = scale->x > scale->y ? scale->x : scale->y;
    largest = largest > scale->z ? largest : scale->z;
    radius = &model->r;
    saved = *radius;
    *radius *= largest;
    njScaleEx(scale);
    njCnkEasyDrawModel(model);
    *radius = saved;
}

static void street_hammer_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                             NJS_VECTOR *scale)
{
    Float largest;
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius;
    Float saved;

    largest = scale->x > scale->y ? scale->x : scale->y;
    largest = largest > scale->z ? largest : scale->z;
    radius = &model->r;
    saved = *radius;
    *radius *= largest;
    njScaleEx(scale);
    model_draw_tinted(model, color);
    *radius = saved;
}
