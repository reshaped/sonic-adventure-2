/* the Egg Golem's ring of footholds in BOSS_GOLEM.PRS, which the golem's blows
 * sink and break. a foothold is the task ObjectAshibaExec, and a broken one's
 * pieces the task AshibaBrokenPartsExec.
 *
 * boss_golem_e/footholds.c compiles this text a second time for
 * BOSS_GOLEM_E.PRS, under the switches below. the offsets the tags here cite
 * are BOSS_GOLEM's. there foothold_draw is 0x20 bytes longer, so past it
 * BOSS_GOLEM_E's offsets lie 0x20 further on */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "boss_golem/footholds.h"
#include "draw/color.h"
#include "shared/golem_foothold_art.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/golem.h"
#include "shared/golem_util.h"
#include "shared/object_builds.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* each slot's foothold, or null once its task is destroyed */
static struct task *golem_footholds[GOLEM_SLOTS];

/* the text's switches, which take BOSS_GOLEM's values where the source that
 * compiles the text defines none */
#ifndef FOOTHOLD_HIT
/* golem_foothold_hit's body: THIRD_HIT_BREAKS or SECOND_HIT_BREAKS */
#define FOOTHOLD_HIT THIRD_HIT_BREAKS
/* what foothold_draw runs before it draws the mode's model */
#define FOOTHOLD_DRAW_START
/* the sound id of a sink */
#define FOOTHOLD_SINK_SOUND 11
/* how golem_footholds_lowest_near takes each foothold's state: COMPARE_EACH
 * or COMPARE_READ */
#define TAKE_LOWEST(foothold) COMPARE_EACH(foothold)
#endif

/* a foothold's modes, in its work's mode */
enum foothold_mode {
    MODE_REST,
    MODE_WOBBLE,
    MODE_SINK, /* fall to TARGET_Y */
    MODE_SETTLE, /* play the sink's motion to its end */
    MODE_GONE,
    MODE_RISE,
    MODE_FADE_IN, /* fade in where the foothold was gone */
    FOOTHOLD_MODES
};
/* a foothold's work's submode holds the mode requested for its next step, or
 * NO_MODE for none */
#define NO_MODE (-1)

/* a foothold keeps its target height, its fall's speed and its state in its
 * work's scl. STATE holds an enum golem_foothold_state */
#define TARGET_Y(work) ((work)->scl.x)
#define SPEED_Y(work) ((work)->scl.y)
#define STATE(work) (*(Sint32 *)&(work)->scl.z)
/* a foothold's slot, in its task's unk_3c */
#define SLOT(task) ((task)->unk_3c.l)

struct foothold {
    NJS_POINT3 offset; /* the wobble's offset from the foothold's pos */
    NJS_VECTOR wobble_spd; /* what offset gains a step */
    Uint8 unk_18[0xc];
    /* the wobble's turns. the draw turns the foothold about each axis by
     * offset.y times that axis's turn */
    Angle turn_x;
    Angle turn_y;
    Angle turn_z;
    Sint32 steps; /* the wobble's steps so far */
    Float frame; /* the frame of the sink's motion */
    Float alpha; /* how far it has faded in, from 0 to 1 */
    Bool solid; /* FALSE while it is gone: its sensors then lack WORLD_ADDED */
};
#define FOOTHOLD(task) ((struct foothold *)(task)->motion.p)

/* a piece of a broken foothold, among PIECES in its task's extra. it flies
 * from where it lay in the foothold. it goes once its life ends or it falls
 * below y 0 */
struct foothold_piece {
    NJS_CNK_MODEL *model;
    NJS_TEXLIST *texlist;
    Bool live;
    NJS_POINT3 pos;
    struct angles ang;
    Uint8 unk_24[0xc];
    NJS_VECTOR spd;
    struct angles spin; /* what ang turns by in a step */
    Sint32 steps;
    Sint32 life; /* the steps the piece flies */
    /* the share of its life it stays opaque, which the integer truncates to
     * 0 */
    Sint32 fade_share;
    Bool smokes; /* TRUE where the piece leaves puffs of smoke */
};
#define PIECES 25
/* @hack type: extra read as a void * through its address, where
 * (task)->extra.p would do */
#define FOOTHOLD_PIECES(task)                                                  \
    ((struct foothold_piece *)*(void **)&(task)->extra)
/* the pieces' task counts its live pieces in its unk_3c */
#define LIVE_PIECES(task) ((task)->unk_3c.l)

#define FOOTHOLD_LEVEL 2
/* GOLEM_SLOT_TURN is a twelfth of a turn, rounded down. twelve of them fall
 * four short of a full turn. the turn after each slot that is a multiple of
 * SLOTS_PER_LONG_TURN is SLOT_TURN_LONG, one longer. the four long turns make
 * up the rest */
#define SLOT_TURN_LONG (GOLEM_SLOT_TURN + 1)
#define SLOTS_PER_LONG_TURN 3
/* the slots whose footholds the fight starts without */
#define GAP_A 2
#define GAP_B 10
/* the world flags of a foothold's sensors. the side's also crashes a player
 * who runs into it fast enough */
#define FOOTHOLD_WORLD_FLAGS (WORLD_SIZE_CLASS(1) | WORLD_MOVING | WORLD_SOLID)
#define SIDE_WORLD_FLAGS (FOOTHOLD_WORLD_FLAGS | GROUND_CRASHES)
/* a solid foothold's sensor keeps WORLD_ADDED while a player is within
 * SENSOR_RANGE of it. its side's sensor keeps it within SIDE_RANGE */
#define SENSOR_RANGE 97.0F
#define SIDE_RANGE 85.0F
#define FOOTHOLD_BUILDS 2
/* each step of a sink adds SINK_GRAVITY to the fall's speed, then keeps
 * SINK_DRAG of it. the sink's motion runs to SINK_LAST_FRAME as the foothold
 * falls, and on to its end once it lands */
#define SINK_GRAVITY 0.08F
#define SINK_DRAG 0.997F
#define SINK_LAST_FRAME 36.0F
#define SHAKE_CAMERA 0
/* a piece's spins and its smoke's speed and radius each take a random factor.
 * the factor runs from SPREAD_LOW to SPREAD_LOW + SPREAD. the smoke's scale
 * takes one from SMOKE_SCALE_LOW to SMOKE_SCALE_LOW + SMOKE_SCALE_SPREAD. each
 * spin turns either way at even odds */
#define SPREAD_LOW 0.9F
#define SPREAD 0.2F
#define SMOKE_SCALE_LOW 0.8F
#define SMOKE_SCALE_SPREAD 0.4F
#define EVEN_ODDS 0.5F
#define PIECE_GRAVITY 0.08F
#define PIECE_DRAG 0.997F
#define WHITE 0x00ffffff
#ifdef NONMATCHING
/* this side shifts the byte unsigned. an int can't hold a byte above 0x7f
 * shifted into its top byte */
#define ALPHA_BITS(value) ((Uint32)(value) << ALPHA_SHIFT)
#else
#define ALPHA_BITS(value) ((value) << ALPHA_SHIFT)
#endif

/* golem_foothold_hit's two bodies, for task, the foothold at slot. under
 * THIRD_HIT_BREAKS each hit sinks the foothold a state. the third hit breaks
 * it where the slot is even. under SECOND_HIT_BREAKS only a whole foothold
 * sinks. the second hit breaks it where the slot is even */
#define THIRD_HIT_BREAKS                                                       \
    TARGET_Y(task->work) = task->work->pos.y - GOLEM_FOOTHOLD_DROP;            \
    SPEED_Y(task->work) = 0.0F;                                                \
    if (++STATE(task->work) == FOOTHOLD_GONE) {                                \
        if (slot % 2 == 0) {                                                   \
            golem_foothold_break(SLOT(task));                                  \
        } else {                                                               \
            STATE(task->work) = FOOTHOLD_SUNK_TWICE;                           \
        }                                                                      \
    } else {                                                                   \
        wobble_start(task);                                                    \
        golem_foothold_request(task, MODE_WOBBLE);                             \
    }
#define SECOND_HIT_BREAKS                                                      \
    if (STATE(task->work) == FOOTHOLD_WHOLE) {                                 \
        STATE(task->work)++;                                                   \
        TARGET_Y(task->work) = task->work->pos.y - GOLEM_FOOTHOLD_DROP;        \
        SPEED_Y(task->work) = 0.0F;                                            \
        wobble_start(task);                                                    \
        golem_foothold_request(task, MODE_WOBBLE);                             \
    } else if (slot % 2 == 0 && STATE(task->work) == FOOTHOLD_SUNK) {          \
        golem_foothold_break(SLOT(task));                                      \
    }
/* TAKE_LOWEST's two bodies, which give the same lowest. COMPARE_READ compares
 * a state only where it read one. COMPARE_EACH compares state at an empty slot
 * too. state then holds the last state read, or FOOTHOLD_GONE, and neither can
 * lower lowest */
/* @hack spelling: a compare for each foothold, where one for each state read
 * would do */
#define COMPARE_EACH(foothold)                                                 \
    if (foothold) {                                                            \
        state = STATE((foothold)->work);                                       \
    }                                                                          \
    if (state < lowest) {                                                      \
        lowest = state;                                                        \
    }
#define COMPARE_READ(foothold)                                                 \
    if (foothold) {                                                            \
        state = STATE((foothold)->work);                                       \
        if (state < lowest) {                                                  \
            lowest = state;                                                    \
        }                                                                      \
    }

#ifdef NONMATCHING
/* this side returns whether the sensors went in, for create_foothold */
static Bool foothold_add_sensors(struct task *task);
#else
static void foothold_add_sensors(struct task *task);
#endif
static void foothold_destroy(struct task *task);
static void foothold_step(struct task *task);
static void foothold_draw(struct task *task);
static void foothold_take_mode(struct task *task);
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* request mode for task's next step. only this source calls it, and SHC
 * expands each call. it is external so that SHC still writes its body, which
 * the image holds */
void golem_foothold_request(struct task *task, int mode);
/* NOLINTEND(misc-use-internal-linkage) */
static void mode_rest(struct task *task);
static void wobble_start(struct task *task);
static void mode_wobble(struct task *task);
static void mode_sink(struct task *task);
static void mode_settle(struct task *task);
static void mode_gone(struct task *task);
static void mode_rise(struct task *task);
static void mode_fade_in(struct task *task);
static void foothold_rise(int slot);
static void foothold_bring_back(int slot);
static void side_step(struct task *task);
static void side_destroy(struct task *task);
static void foothold_shatter(struct task *task);
static void pieces_destroy(struct task *task);
static void pieces_step(struct task *task);
static void pieces_draw(struct task *task);

/* the steps a gone foothold takes to fade in, and a sunk one to rise */
static Sint32 restore_steps = 240;
static Sint32 sink_volume = 25;
/* TRUE once one call of golem_footholds_build has built both models */
static Bool built = FALSE;
static struct model_build builds[FOOTHOLD_BUILDS] = {
    {MODEL_BUILD_ALONE_AS_LAND, &golem_foothold_a_texlist,
     &golem_foothold_a_object, NULL},
    {MODEL_BUILD_ALONE_AS_LAND, &golem_foothold_b_texlist,
     &golem_foothold_b_object, NULL},
};
static void (*mode_steps[FOOTHOLD_MODES])(struct task *task) = {
    mode_rest, mode_wobble, mode_sink,    mode_settle,
    mode_gone, mode_rise,   mode_fade_in,
};
static Uint32 dat_footholds_1 = 0x80808080;
/* a wobble starts at the speeds wobble_start_x to wobble_start_z, and the
 * turns wobble_turn_x to wobble_turn_z. each step keeps wobble_drag of the
 * speed, and takes wobble_pull times the offset off it. that pulls the offset
 * back toward 0. the foothold sinks after wobble_steps */
static Float wobble_start_x = 0.1F;
static Float wobble_start_y = 1.0F;
static Float wobble_start_z = 0.2F;
static Angle wobble_turn_x = 64;
static Angle wobble_turn_y = 0;
static Angle wobble_turn_z = 32;
static Sint32 wobble_steps = 60;
static Float wobble_drag = 0.9F;
static Float wobble_pull = 1.99F;
static Float shake_size = 100.0F;
static Sint32 shake_steps = 2;
static Float piece_spin_drag = 0.95F;
static Float dat_footholds_2 = 0.75F;
static Angle piece_spin = 0x400;
static Sint32 piece_life = 90;
static Float piece_fade_share = 0.333F;
/* a piece puffs smoke at smoke_chance as the foothold breaks, and again at each
 * step it flies */
static Float shatter_smoke_radius = 20.0F;
static Float shatter_smoke_scale = 10.0F;
static Float smoke_chance = 0.1F;
static Float piece_smoke_scale = 10.0F;

Uint8 *golem_footholds_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, builds, FOOTHOLD_BUILDS) ==
        FOOTHOLD_BUILDS) {
        built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

#ifdef NONMATCHING
/* this side's golem_footholds_create works as footholds.h says. create_foothold
 * tests each task and allocation that the matching side writes through */
static struct task *create_foothold(int slot, Angle heading)
{
    struct task *task;
    struct task_work *work;
    struct foothold *foothold;

    task = task_create(TASK_WORK | TASK_FORCE, FOOTHOLD_LEVEL, foothold_step,
                       "ObjectAshibaExec");
    if (task == NULL) {
        return NULL;
    }
    foothold = syCalloc(1, sizeof(*foothold));
    if (foothold == NULL) {
        task_destroy(task);
        return NULL;
    }
    task->motion.p = foothold;
    task->destructor = foothold_destroy;
    SLOT(task) = slot;
    work = task->work;
    golem_polar_xz(heading, GOLEM_FOOTHOLD_RADIUS, &work->pos.x, &work->pos.z);
    work->pos.y = golem_floor_y;
    work->ang.y = heading;
    work->mode = MODE_REST;
    work->submode = NO_MODE;
    SPEED_Y(work) = 0.0F;
    TARGET_Y(work) = golem_floor_y;
    STATE(work) = FOOTHOLD_WHOLE;
    foothold->solid = TRUE;
    foothold->alpha = 1.0F;
    if (!foothold_add_sensors(task)) {
        task_destroy(task);
        return NULL;
    }
    task->draw = foothold_draw;
    return task;
}

Bool golem_footholds_create(void)
{
    struct task *created[GOLEM_SLOTS] = {NULL};
    Angle heading = 0;
    int slot;

    for (slot = 0; slot < GOLEM_SLOTS; ++slot) {
        if (golem_footholds[slot] == NULL) {
            created[slot] = create_foothold(slot, heading);
            if (created[slot] == NULL) {
                while (slot > 0) {
                    --slot;
                    if (created[slot] != NULL) {
                        task_destroy(created[slot]);
                    }
                }
                return FALSE;
            }
        }
        heading +=
            slot % SLOTS_PER_LONG_TURN == 0 ? SLOT_TURN_LONG : GOLEM_SLOT_TURN;
    }
    for (slot = 0; slot < GOLEM_SLOTS; ++slot) {
        if (created[slot] != NULL) {
            golem_footholds[slot] = created[slot];
        }
    }
    return TRUE;
}
#else
void golem_footholds_create(void)
{
    Float x;
    Float z;
    Angle ang = 0;
    int i;

    for (i = 0; i < GOLEM_SLOTS; i++) {
        /* @hack spelling: the task read again from golem_footholds at each
         * use, where a local would do */
        golem_footholds[i] = task_create(TASK_WORK | TASK_FORCE, FOOTHOLD_LEVEL,
                                         foothold_step, "ObjectAshibaExec");
        golem_footholds[i]->draw = foothold_draw;
        golem_footholds[i]->destructor = foothold_destroy;
        SLOT(golem_footholds[i]) = i;
        golem_footholds[i]->motion.p = syMalloc(sizeof(struct foothold));
        golem_polar_xz(ang, GOLEM_FOOTHOLD_RADIUS, &x, &z);
        golem_footholds[i]->work->pos.x = x;
        golem_footholds[i]->work->pos.y = golem_floor_y;
        golem_footholds[i]->work->pos.z = z;
        golem_footholds[i]->work->ang.x = 0;
        golem_footholds[i]->work->ang.y = ang;
        golem_footholds[i]->work->ang.z = 0;
        golem_footholds[i]->work->mode = MODE_REST;
        golem_footholds[i]->work->submode = NO_MODE;
        SPEED_Y(golem_footholds[i]->work) = 0.0F;
        TARGET_Y(golem_footholds[i]->work) = golem_floor_y;
        STATE(golem_footholds[i]->work) = FOOTHOLD_WHOLE;
        FOOTHOLD(golem_footholds[i])->solid = TRUE;
        FOOTHOLD(golem_footholds[i])->alpha = 1.0F;
        if (i % SLOTS_PER_LONG_TURN == 0) {
            ang += SLOT_TURN_LONG;
        } else {
            ang += GOLEM_SLOT_TURN;
        }
        foothold_add_sensors(golem_footholds[i]);
    }
}
#endif

void golem_footholds_open_gaps(void)
{
    FOOTHOLD(golem_footholds[GAP_A])->solid = FALSE;
    FOOTHOLD(golem_footholds[GAP_A])->alpha = 0.0F;
    STATE(golem_footholds[GAP_A]->work) = FOOTHOLD_GONE;
    golem_foothold_request(golem_footholds[GAP_A], MODE_GONE);
    FOOTHOLD(golem_footholds[GAP_B])->solid = FALSE;
    FOOTHOLD(golem_footholds[GAP_B])->alpha = 0.0F;
    STATE(golem_footholds[GAP_B]->work) = FOOTHOLD_GONE;
    golem_foothold_request(golem_footholds[GAP_B], MODE_GONE);
}

#ifdef NONMATCHING
/* this side returns a Bool, as the declaration above says */
static Bool foothold_add_sensors(struct task *task)
#else
static void foothold_add_sensors(struct task *task)
#endif
{
    struct task_work *work = task->work;
    NJS_OBJECT *object = world_pool_alloc();
    struct task *side;
    struct task_work *side_work;

    /* @bug memory: the copies write through each sensor's object unchecked,
     * though world_pool_alloc returns null once its pool is lent out. the
     * writes are at 0x54e and 0x5d2 of the object's code. side_destroy and
     * foothold_destroy test the objects at 0x10a8 and 0x664 */
#ifdef NONMATCHING
    /* this side repairs the tag above: it returns FALSE where the pool has no
     * object */
    if (object == NULL) {
        return FALSE;
    }
#endif

    if (!(side = task_create_child(TASK_WORK, side_step, task))) {
        /* @bug memory: where the heap can't give the side's task, the call
         * returns at 0x524 of the object's code, and the side's object stays
         * lent from the pool for good */
        /* @bug memory: where the side's task fails, foothold_step writes
         * through task->extra, which the early return leaves null.
         * foothold_destroy tests extra at 0x664 of the object's code, but
         * foothold_step writes through it at 0x6b6 */
#ifdef NONMATCHING
        /* this side repairs the two bugs tagged above. it gives the object back
         * to the pool and returns FALSE. create_foothold then destroys the
         * foothold */
        world_pool_free(object);
        return FALSE;
#else
        return;
#endif
    }
    side->destructor = side_destroy;
    side_work = side->work;
    side_work->pos = work->pos;
    side_work->ang = work->ang;
    side->extra.p = object;
    *object = golem_foothold_side_object;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
    object->ang[0] = side_work->ang.x;
    object->ang[1] = side_work->ang.y;
    object->ang[2] = side_work->ang.z;
    object->pos[0] = side_work->pos.x;
    object->pos[1] = side_work->pos.y;
    object->pos[2] = side_work->pos.z;
    world_add(SIDE_WORLD_FLAGS, side, object);
    object = world_pool_alloc();
#ifdef NONMATCHING
    /* this side repairs, for the second object, the bug tagged at the first. it
     * returns FALSE where the pool has no object */
    if (object == NULL) {
        return FALSE;
    }
#endif
    task->extra.p = object;
    *object = golem_foothold_sensor_object;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
    object->ang[0] = work->ang.x;
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(FOOTHOLD_WORLD_FLAGS, task, object);
#ifdef NONMATCHING
    /* this side returns that the sensors went in */
    return TRUE;
#endif
}

static void foothold_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
#ifdef NONMATCHING
    /* this side clears the slot only where it holds this foothold. a foothold
     * destroyed before golem_footholds_create puts it in its slot leaves the
     * slot as it is */
    if (golem_footholds[SLOT(task)] == task) {
        golem_footholds[SLOT(task)] = NULL;
    }
#else
    golem_footholds[SLOT(task)] = NULL;
#endif
}

static void foothold_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    foothold_take_mode(task);
    /* @hack spelling: the mode read through task->work, where work would do */
    mode_steps[task->work->mode](task);
    object = task->extra.p;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    if (FOOTHOLD(task)->solid &&
        player_near((NJS_POINT3 *)object->pos, SENSOR_RANGE)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void foothold_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 color;

    if (work->mode == MODE_GONE) {
        return;
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: the mode read through task->work, where work would do */
    if (task->work->mode == MODE_WOBBLE) {
        njTranslateEx(&FOOTHOLD(task)->offset);
        njRotateZ(NULL, FOOTHOLD(task)->offset.y * FOOTHOLD(task)->turn_z);
        njRotateY(NULL, FOOTHOLD(task)->offset.y * FOOTHOLD(task)->turn_y);
        njRotateX(NULL, FOOTHOLD(task)->offset.y * FOOTHOLD(task)->turn_x);
    }
    FOOTHOLD_DRAW_START
    if (task->work->mode == MODE_SINK || task->work->mode == MODE_SETTLE) {
        if (SLOT(task) % 2 == 0) {
            njSetTexture(&golem_foothold_a_texlist);
            njCnkEasyDrawMotion(&golem_foothold_a_object,
                                golem_foothold_sink_motion,
                                FOOTHOLD(task)->frame);
        } else {
            njSetTexture(&golem_foothold_b_texlist);
            njCnkEasyDrawMotion(&golem_foothold_b_object,
                                golem_foothold_sink_motion,
                                FOOTHOLD(task)->frame);
        }
    } else if (task->work->mode == MODE_FADE_IN) {
        color = ALPHA_BITS((Uint8)(FOOTHOLD(task)->alpha * 255.0F)) | WHITE;
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        njSetTexture(&golem_foothold_a_texlist);
        model_draw_tinted(&golem_foothold_a_model, color);
        draw_attr_restore();
        draw_control_restore();
    } else if (SLOT(task) % 2 == 0) {
        njSetTexture(&golem_foothold_a_texlist);
        if (built) {
            njDirectDrawObject(builds[0].built);
        } else {
            njCnkEasyDrawModel(&golem_foothold_a_model);
        }
    } else {
        njSetTexture(&golem_foothold_b_texlist);
        if (built) {
            njDirectDrawObject(builds[1].built);
        } else {
            njCnkEasyDrawModel(&golem_foothold_b_model);
        }
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void foothold_take_mode(struct task *task)
{
    if (task->work->submode != NO_MODE) {
        task->work->mode = task->work->submode;
        task->work->submode = NO_MODE;
    }
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
void golem_foothold_request(struct task *task, int mode)
{
    task->work->submode = mode;
}
/* NOLINTEND(misc-use-internal-linkage) */

static void mode_rest(struct task *task)
{
}

static void wobble_start(struct task *task)
{
    FOOTHOLD(task)->offset.x = 0.0F;
    FOOTHOLD(task)->offset.y = 0.0F;
    FOOTHOLD(task)->offset.z = 0.0F;
    FOOTHOLD(task)->wobble_spd.x = wobble_start_x;
    FOOTHOLD(task)->wobble_spd.y = wobble_start_y;
    FOOTHOLD(task)->wobble_spd.z = wobble_start_z;
    FOOTHOLD(task)->turn_x = wobble_turn_x;
    FOOTHOLD(task)->turn_y = wobble_turn_y;
    FOOTHOLD(task)->turn_z = wobble_turn_z;
    FOOTHOLD(task)->steps = 0;
}

static void mode_wobble(struct task *task)
{
    FOOTHOLD(task)->offset.x += FOOTHOLD(task)->wobble_spd.x;
    FOOTHOLD(task)->offset.y += FOOTHOLD(task)->wobble_spd.y;
    FOOTHOLD(task)->offset.z += FOOTHOLD(task)->wobble_spd.z;
    FOOTHOLD(task)->wobble_spd.x =
        (FOOTHOLD(task)->wobble_spd.x * wobble_drag) -
        (FOOTHOLD(task)->offset.x * wobble_pull);
    FOOTHOLD(task)->wobble_spd.y =
        (FOOTHOLD(task)->wobble_spd.y * wobble_drag) -
        (FOOTHOLD(task)->offset.y * wobble_pull);
    FOOTHOLD(task)->wobble_spd.z =
        (FOOTHOLD(task)->wobble_spd.z * wobble_drag) -
        (FOOTHOLD(task)->offset.z * wobble_pull);
    if (++FOOTHOLD(task)->steps > wobble_steps) {
        golem_foothold_request(task, MODE_SINK);
        FOOTHOLD(task)->offset.x = 0.0F;
        FOOTHOLD(task)->offset.y = 0.0F;
        FOOTHOLD(task)->offset.z = 0.0F;
        FOOTHOLD(task)->wobble_spd.x = 0.0F;
        FOOTHOLD(task)->wobble_spd.y = 0.0F;
        FOOTHOLD(task)->wobble_spd.z = 0.0F;
        FOOTHOLD(task)->steps = 0;
        FOOTHOLD(task)->frame = 0.0F;
        sound_play(FOOTHOLD_SINK_SOUND, task, 0, 0);
        sound_set_volume(task, SOUND_ANY_ID, sink_volume);
    }
}

static void mode_sink(struct task *task)
{
    struct task_work *work = task->work;
    Float distance;
    Sint32 steps;

    SPEED_Y(work) -= SINK_GRAVITY;
    /* @hack spelling: the speed and the target read through task->work, where
     * work would do */
    SPEED_Y(task->work) *= SINK_DRAG;
    work->pos.y += SPEED_Y(task->work);
    if (FOOTHOLD(task)->frame < SINK_LAST_FRAME) {
        FOOTHOLD(task)->frame += 1.0F;
    }
    if (work->pos.y <= TARGET_Y(task->work)) {
        distance = njDistanceP2P(&work->pos, &player_work[0]->pos);
        /* @hack spelling: steps, a local for shake_steps, where the datum read
         * in place would do */
        steps = shake_steps;
        camera_shake(SHAKE_CAMERA, steps, shake_size / distance);
        FOOTHOLD(task)->frame = SINK_LAST_FRAME;
        work->pos.y = TARGET_Y(task->work);
        FOOTHOLD(task)->steps = 0;
        golem_foothold_request(task, MODE_SETTLE);
    }
}

static void mode_settle(struct task *task)
{
    if (FOOTHOLD(task)->frame < golem_foothold_sink_motion[0].nbFrame - 1) {
        FOOTHOLD(task)->frame += 1.0F;
    } else {
        golem_foothold_request(task, MODE_REST);
        FOOTHOLD(task)->offset.y = 0.0F;
        FOOTHOLD(task)->wobble_spd.y = 0.0F;
    }
}

static void mode_gone(struct task *task)
{
}

/* rise to TARGET_Y. the first force record's speed carries player 0 along
 * where it rides the foothold */
static void mode_rise(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: rise, a copy of work, where work would do */
    struct task_work *rise = work;

    rise->pos.y += SPEED_Y(rise);
    /* @hack spelling: the speed and the target read through task->work, where
     * rise would do */
    TASK_FORCES_OF(task)->speed.y = SPEED_Y(task->work);
    if (rise->pos.y >= TARGET_Y(task->work)) {
        rise->pos.y = TARGET_Y(task->work);
        TASK_FORCES_OF(task)->speed.y = 0.0F;
        golem_foothold_request(task, MODE_REST);
    }
}

static void mode_fade_in(struct task *task)
{
    FOOTHOLD(task)->alpha += 1.0F / restore_steps;
    if (FOOTHOLD(task)->alpha >= 1.0F) {
        FOOTHOLD(task)->alpha = 1.0F;
        golem_foothold_request(task, MODE_REST);
    }
}

void golem_foothold_break(int slot)
{
    struct task *task = golem_footholds[slot];

    if (slot % 2 == 0) {
        foothold_shatter(task);
        FOOTHOLD(task)->solid = FALSE;
        FOOTHOLD(task)->alpha = 0.0F;
        STATE(task->work) = FOOTHOLD_GONE;
        golem_foothold_request(task, MODE_GONE);
    }
}

void golem_foothold_hit(int slot)
{
    struct task *task = golem_footholds[slot];

    if (task) {
        FOOTHOLD_HIT
    }
}

static void foothold_rise(int slot)
{
    struct task *task = golem_footholds[slot];

    if (task) {
        TARGET_Y(task->work) = golem_floor_y;
        SPEED_Y(task->work) =
            (TARGET_Y(task->work) - task->work->pos.y) / restore_steps;
        STATE(task->work) = FOOTHOLD_WHOLE;
        golem_foothold_request(task, MODE_RISE);
    }
}

static void foothold_bring_back(int slot)
{
    struct task *task = golem_footholds[slot];

    if (task) {
        task->work->pos.y = golem_floor_y;
        SPEED_Y(task->work) = 0.0F;
        STATE(task->work) = FOOTHOLD_WHOLE;
        FOOTHOLD(task)->alpha = 0.0F;
        FOOTHOLD(task)->solid = TRUE;
        golem_foothold_request(task, MODE_FADE_IN);
    }
}

void golem_footholds_restore(void)
{
    int i;

    for (i = 0; i < GOLEM_SLOTS; i++) {
        switch (STATE(golem_footholds[i]->work)) {
        case FOOTHOLD_SUNK:
        case FOOTHOLD_SUNK_TWICE:
            foothold_rise(i);
            break;
        case FOOTHOLD_GONE:
            foothold_bring_back(i);
            break;
        }
    }
}

int golem_foothold_state(int slot)
{
    if (!golem_footholds[slot]) {
        return FOOTHOLD_GONE;
    }
    return STATE(golem_footholds[slot]->work);
}

int golem_footholds_lowest_state(void)
{
    int i;
    int state;
    int lowest = FOOTHOLD_GONE;

    for (i = 0; i < GOLEM_SLOTS; i++) {
        if (!golem_footholds[i]) {
            state = FOOTHOLD_GONE;
        } else {
            state = STATE(golem_footholds[i]->work);
        }
        if (state < lowest) {
            lowest = state;
        }
    }
    return lowest;
}

int golem_footholds_lowest_near(int slot)
{
    int next = (slot + 1) % GOLEM_SLOTS;
    int last = (slot + (GOLEM_SLOTS - 1)) % GOLEM_SLOTS;
    int state;
    int lowest;

    lowest = FOOTHOLD_GONE;
    state = FOOTHOLD_GONE;
    TAKE_LOWEST(golem_footholds[next])
    TAKE_LOWEST(golem_footholds[slot])
    TAKE_LOWEST(golem_footholds[last])
    return lowest;
}

static void side_step(struct task *task)
{
    struct task *foothold = task->parent;
    struct task_work *work = task->work;
    NJS_OBJECT *object = task->extra.p;

    work->pos = foothold->work->pos;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    if (FOOTHOLD(foothold)->solid &&
        player_near((NJS_POINT3 *)object->pos, SIDE_RANGE)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void side_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void foothold_shatter(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = golem_foothold_pieces[0].child;
    struct task *pieces;
    struct foothold_piece *piece;
    NJS_VECTOR drift;
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    NJS_VECTOR place;
    Float smoke_scale;
    Float smoke_radius;
    int i;

    /* @bug uninitialized: each piece's smoke takes its speed from drift,
     * which no path sets. the reads at 0x12ce, 0x12e2 and 0x12f8 of the
     * object's code take whatever the stack held */
#ifdef NONMATCHING
    /* this side repairs the tag above: drift is 0, so the smoke takes no
     * speed */
    drift.x = drift.y = drift.z = 0.0F;
#endif
    pieces = task_create(TASK_WORK, FOOTHOLD_LEVEL, pieces_step,
                         "AshibaBrokenPartsExec");
    if (!(pieces->extra.p = syCalloc(PIECES, sizeof(struct foothold_piece)))) {
        task_kill(pieces);
        /* @bug memory: where syCalloc fails, the call kills the pieces' task
         * at 0x1102 of the object's code. it goes on to write each piece
         * through the null extra, first at 0x11a4 */
#ifdef NONMATCHING
        /* this side repairs the tag above: the call returns once it has
         * killed the task, and the foothold shatters into no pieces */
        return;
#endif
    }
    pieces->draw = pieces_draw;
    pieces->destructor = pieces_destroy;
    LIVE_PIECES(pieces) = 0;
    for (i = 0; i < PIECES; i++) {
        piece = FOOTHOLD_PIECES(pieces) + i;
        if (!object) {
            piece->live = FALSE;
            continue;
        }
        golem_point_turn_y(object->pos[0], object->pos[2], work->ang.y, &place);
        piece->pos.x = work->pos.x + place.x;
        piece->pos.y = work->pos.y + object->pos[1];
        piece->pos.z = work->pos.z + place.z;
        piece->ang.x = object->ang[0];
        piece->ang.y = work->ang.y + object->ang[1];
        piece->ang.z = object->ang[2];
        piece->spd.x = 0.0F;
        piece->spd.y = 0.0F;
        piece->spd.z = 0.0F;
        piece->model = object->model;
        piece->texlist = &golem_foothold_pieces_texlist;
        piece->spin.x =
            piece_spin * (SPREAD_LOW + (rand() * (SPREAD * RAND_SCALE)));
        if (rand() * RAND_SCALE < EVEN_ODDS) {
            piece->spin.x = -piece->spin.x;
        }
        piece->spin.y =
            piece_spin * (SPREAD_LOW + (rand() * (SPREAD * RAND_SCALE)));
        if (rand() * RAND_SCALE < EVEN_ODDS) {
            piece->spin.y = -piece->spin.y;
        }
        piece->spin.z =
            piece_spin * (SPREAD_LOW + (rand() * (SPREAD * RAND_SCALE)));
        if (rand() * RAND_SCALE < EVEN_ODDS) {
            piece->spin.z = -piece->spin.z;
        }
        piece->steps = 0;
        piece->life = piece_life;
        piece->live = TRUE;
        piece->smokes = TRUE;
        /* @bug arithmetic: each piece fades over its whole life, where
         * piece_fade_share would keep it opaque for its first third.
         * fade_share is an integer, so the store at 0x12ae of the object's
         * code truncates piece_fade_share to 0. pieces_draw's test of it, at
         * 0x1ada, then never holds */
        piece->fade_share = piece_fade_share;
        pos = piece->pos;
        spd.x = drift.x * (SPREAD_LOW + (rand() * (SPREAD * RAND_SCALE)));
        spd.y = drift.y * (SPREAD_LOW + (rand() * (SPREAD * RAND_SCALE)));
        spd.z = drift.z * (SPREAD_LOW + (rand() * (SPREAD * RAND_SCALE)));
        smoke_radius = shatter_smoke_radius *
                       (SPREAD_LOW + (rand() * (SPREAD * RAND_SCALE)));
        smoke_scale =
            shatter_smoke_scale *
            (SMOKE_SCALE_LOW + (rand() * (SMOKE_SCALE_SPREAD * RAND_SCALE)));
        if (piece->smokes && rand() * RAND_SCALE < smoke_chance) {
            golem_dust_smoke_behind(&pos, &spd, smoke_radius, smoke_scale);
        }
        LIVE_PIECES(pieces)++;
        object = object->sibling;
    }
}

static void pieces_destroy(struct task *task)
{
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void pieces_step(struct task *task)
{
    struct foothold_piece *piece;
    int i;

    for (i = 0; i < PIECES; i++) {
        piece = FOOTHOLD_PIECES(task) + i;
        if (piece->live) {
            piece->pos.x += piece->spd.x;
            piece->pos.y += piece->spd.y;
            piece->pos.z += piece->spd.z;
            piece->spd.y -= PIECE_GRAVITY;
            piece->spd.x *= PIECE_DRAG;
            piece->spd.y *= PIECE_DRAG;
            piece->spd.z *= PIECE_DRAG;
            piece->ang.x += piece->spin.x;
            piece->ang.y += piece->spin.y;
            piece->ang.z += piece->spin.z;
            piece->spin.x = piece->spin.x * piece_spin_drag;
            piece->spin.y = piece->spin.y * piece_spin_drag;
            piece->spin.z = piece->spin.z * piece_spin_drag;
            /* @bug logic: a flying piece's smoke takes a scale from 9 to 9.2,
             * where piece_smoke_scale, 10, times the random factor would give
             * 9 to 11. the random share of SPREAD is added after the product
             * at 0x1518 of the object's code, not within it */
            if (piece->smokes && rand() * RAND_SCALE < smoke_chance) {
                golem_dust_smoke_behind(&piece->pos, &piece->spd, 0.0F,
                                        (piece_smoke_scale * SPREAD_LOW) +
                                            (rand() * (SPREAD * RAND_SCALE)));
            }
            if (++piece->steps >= piece->life || piece->pos.y < 0.0F) {
                piece->live = FALSE;
                LIVE_PIECES(task)--;
            }
        }
    }
    if (LIVE_PIECES(task) <= 0) {
        task_kill(task);
    }
}

static void pieces_draw(struct task *task)
{
    struct foothold_piece *piece;
    Float age;
    Float rest;
    Uint8 alpha;
    Uint8 shade;
    Uint32 color;
    int i;

    njSetTexture(FOOTHOLD_PIECES(task)->texlist);
    for (i = 0; i < PIECES; i++) {
        piece = FOOTHOLD_PIECES(task) + i;
        if (piece->live) {
            age = (Float)piece->steps / (Float)piece->life;
            rest = 1.0F - age;
            if (piece->fade_share > age) {
                alpha = 0xff;
            } else {
                alpha = rest / (1.0F - piece->fade_share) * 255.0F;
            }
            shade = rest * 255.0F;
            color = ALPHA_BITS(alpha) | (shade << RED_SHIFT) |
                    (shade << GREEN_SHIFT) | shade;
            njPushMatrixEx();
            njTranslateEx(&piece->pos);
            njRotateXYZ(NULL, piece->ang.x, piece->ang.y, piece->ang.z);
            draw_control_save();
            draw_attr_save();
            draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
            njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
            model_draw_tinted(piece->model, color);
            draw_attr_restore();
            draw_control_restore();
            njPopMatrixEx();
        }
    }
}
