/* the hammer, HAMMER in Radical Highway's object list: a solid column the
 * players can ride. it strokes along y to its pos and back, and rests at each
 * end. it is stg13/hammer.c's hammer with a glow */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "stg14/hammer.h"
#include "stg14/hammer/text.inc"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* a hammer's modes, in its work's mode. each is a rest, then a stroke.
 * MODE_TO_POS rests at the far end and strokes to pos. MODE_BACK rests at pos
 * and strokes back */
enum hammer_mode { MODE_TO_POS, MODE_BACK };

/* the settings the set file gives a hammer in its work, as hammer.h lists
 * them */
#define HIGH_BYTE 0xff00
#define REST(work) ((Uint8)(work)->ang.x)
#define STROKES_DOWN(work) ((work)->ang.x & HIGH_BYTE)
#define TURN(work) ((work)->ang.y & HIGH_BYTE)
#define PHASE(work) ((Uint8)(work)->ang.y)
#define SPEED(work) ((Uint16)(work)->ang.z)
#define WIDTH_SCALE(work) ((work)->scl.x + 1.0F)
#define HEIGHT_SCALE(work) ((work)->scl.y + 1.0F)
#define LENGTH(work) ((work)->scl.z)
/* PHASE puts the hammer PHASE / PHASES of a cycle ahead of game_state.steps */
#define PHASES 256

/* a hammer's radius is the length of (HALF_WIDTH, HALF_HEIGHT *
 * HEIGHT_SCALE). WIDTH_SCALE doesn't scale it */
#define HALF_WIDTH 25.0F
#define HALF_HEIGHT 50.0F
/* the world collects a hammer's copy only while a player lies near: within
 * its radius plus REACH_MARGIN */
#define REACH_MARGIN 10.0F
/* the glow's color blends glow_color_low toward glow_color_high by
 * |sin(game_state.steps << GLOW_RATE_SHIFT)| * 0.5 + 0.5. so it pulses every
 * 64 steps, between the color halfway to glow_color_high and glow_color_high
 * itself */
#define GLOW_RATE_SHIFT 9

/* how far the hammer lies above pos, below 0 for below it. it is a Float in
 * task->extra, which the destructor clears so that task_destroy frees
 * nothing */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define OFFSET(task) (*(Float *)&(task)->extra)
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define RADIUS(task) (*(Float *)&(task)->unk_3c)
/* TASK_FORCE's records, whose speed.y moves a riding player with the
 * hammer */
#define FORCES(task) ((struct task_force *)(task)->force.p)

#include "shared/road_furniture/hammer_textures.h"

NJS_TEXLIST street_hammer_texlist = {
#include "stg14/hammer/street_hammer_texlist.inc"
};

#include "shared/road_furniture/hammer_glow.h"

NJS_CNK_MODEL street_hammer_glow_model = {
#include "stg14/hammer/street_hammer_glow_model.inc"
};

#include "shared/road_furniture/hammer_column.h"

NJS_CNK_MODEL street_hammer_model = {
#include "stg14/hammer/street_hammer_model.inc"
};

#include "shared/road_furniture/hammer_collision.h"

NJS_OBJECT street_hammer_collision = {
#include "stg14/hammer/street_hammer_collision.inc"
};

NJS_TEXLIST *highway_hammer_texlists[2] = {
#include "stg14/hammer/highway_hammer_texlists.inc"
};
static Uint32 glow_color_low = 0x00000000;
static Uint32 glow_color_high = 0xffffffff;

static void hammer_destroy(struct task *task);
static void hammer_step(struct task *task);
static void hammer_draw(struct task *task);
static void hammer_set_phase(struct task *task);

void highway_hammer_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float height;
    Float area;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = hammer_draw;
    task->step = hammer_step;
    task->destructor = hammer_destroy;
    hammer_set_phase(task);
    OFFSET(task) = 0.0F;
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
        (void)"<- UPDOWN SPEED";
        (void)"<- WIDTH SCALE(1.0+x)";
        (void)"<- HEIGHT SCALE(1.0+x)";
        (void)"<- HOVER HEIGHT";
        task->work->ang.x |= task->work->ang.y;
    }
    object = world_pool_alloc();
    task->motion.p = object;
    /* @bug memory: copies street_hammer_collision through null where
     * world_pool_alloc has none to lend, though hammer_destroy tests
     * task->motion for null. hammer_step reads through it untested: the copy
     * at 0x8c5010be, the test at 0x8c50119a and the read at 0x8c5012c2 */
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
    /* @hack spelling: task->work read again for the scales and the turn,
     * where work would do */
    object->scl[0] = WIDTH_SCALE(task->work);
    object->scl[1] = HEIGHT_SCALE(task->work);
    object->scl[2] = WIDTH_SCALE(task->work);
    object->ang[1] = TURN(task->work);
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_MOVING | WORLD_SOLID;
    /* @hack spelling: height set and then raised by 1, where HEIGHT_SCALE(work)
     * in one statement would do */
    height = task->work->scl.y;
    height += 1.0F;
    /* @hack spelling: area, a local for the product, where the product
     * written in the call would do */
    area = height * height * (HALF_HEIGHT * HALF_HEIGHT);
    RADIUS(task) = njSqrt(area + (HALF_WIDTH * HALF_WIDTH));
    flags |= highway_hammer_size_class(RADIUS(task));
    world_add(flags, task, object);
}

static void hammer_destroy(struct task *task)
{
    if (task->motion.p != NULL) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    task->extra.p = NULL;
    task->motion.p = NULL;
}

static void hammer_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Angle ang;
    Float stroke;

    if (placement_try_leave(task)) {
        return;
    }
    if (!game_state.time_stopped) {
        hammer_set_phase(task);
        /* @hack spelling: task->work read again for the timer, the speed and
         * the stroke's way, where work would do */
        ang = (Uint16)(task->work->timer * SPEED(task->work));
        ang += work->mode != MODE_TO_POS ? HALF_TURN : 0;
        stroke = njSin(ang + QUARTER_TURN);
        if (STROKES_DOWN(task->work)) {
            OFFSET(task) = (0.5F + (stroke * 0.5F)) * LENGTH(work);
        } else {
            OFFSET(task) = -((0.5F + (stroke * 0.5F)) * LENGTH(work));
        }
    }
    object = task->motion.p;
    FORCES(task)
    [0].speed.y = FORCES(task)[1].speed.y =
        work->pos.y + OFFSET(task) - object->pos[1];
    object->pos[1] = work->pos.y + OFFSET(task);
    if (player_near((NJS_POINT3 *)object->pos, RADIUS(task) + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void hammer_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + OFFSET(task), work->pos.z);
    /* @hack spelling: task->work read again for the turn and the scale,
     * where work would do */
    njRotateY(NULL, TURN(task->work));
    njScale(NULL, WIDTH_SCALE(task->work), HEIGHT_SCALE(task->work),
            WIDTH_SCALE(task->work));
    njSetTexture(&street_hammer_texlist);
    njCnkEasyDrawModel(&street_hammer_model);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS);
    model_draw_tinted(&street_hammer_glow_model,
                      color_blend(glow_color_low, glow_color_high,
                                  (_builtin_fabsf(njSin(game_state.steps
                                                        << GLOW_RATE_SHIFT)) *
                                   0.5F) +
                                      0.5F));
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* set the work's mode, timer and tick from where game_state.steps puts the
 * hammer in its cycle. timer counts the stroke's steps, and stays 0 through a
 * rest. tick counts the steps left of a rest, and is 0 through a stroke */
static void hammer_set_phase(struct task *task)
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
                          (1.0F / PHASES))) %
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

Uint32 highway_hammer_size_class(Float radius)
{
    /* @hack spelling: the tests chained with else, though each branch
     * returns, where tests of their own would do */
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
