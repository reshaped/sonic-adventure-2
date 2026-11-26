/* the reactor, DORYOKURO, and a smoke puff's step, destructor and draw. no
 * code of the stage makes a puff. the reactor's body, lid, nine panels and two
 * turrets are each a task of its own. shared/cannon_body.h, included last,
 * makes MUJINGUN and the turrets' shots */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/debris.h"
#include "shared/four_valve_tank.h"
#include "sound/sound.h"
#include "stg24/reactor.h"
#include "stg24/reactor_art.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side: panel_step calls
 * dust_create as dust.h declares it, with the speed's address */
#include "shared/dust.h"
#else
/* @bug mismatched-call: dust_create reads the dust's speed through whatever
 * R5 holds. the call at 0x8c504ef6 passes the speed by value, on the stack,
 * where dust_create takes a pointer to it in R5. R5 holds what njPopMatrixEx,
 * the call before, left there */
struct particle *dust_create_by_value(NJS_POINT3 *pos, NJS_VECTOR speed,
                                      Float size);
#endif

/* the set editor's label for the reactor's ang.y */
static const char editor_heading[12] = "<- ANGLE Y";
/* the name the fire statues give their puffs' tasks */
static const char puff_task_name[16] = "JizooFireExec";

#define PANELS 9
#define FRAGMENTS 8
/* the body and the lid collide only while player_near finds a player within
 * COLLIDE_RANGE of them */
#define COLLIDE_RANGE 500.0F
/* the copy of its collision model that the body or the lid keeps in
 * task->motion */
#define COLLISION_COPY(task) ((NJS_OBJECT *)(task)->motion.p)
/* in the reactor's task->unk_3c: a bit for each panel, 1 << i for panel i. a
 * panel's bit is set while it is broken or was never made. the bits above the
 * panels' stay set, so the word is ALL_BROKEN once every panel's is */
#define BROKEN_PANELS(task) ((task)->unk_3c.l)
#define ALL_BROKEN (-1)
/* a panel's index, in its work's tick, and the hits it has taken, in its
 * submode. it breaks once it has taken PANEL_HITS */
#define PANEL_INDEX(work) ((work)->tick)
#define HITS(work) ((work)->submode)
#define PANEL_HITS 2
#define PANEL_SHAPES 1
/* the lid's work's submode, TRUE once the lid has broken */
#define LID_BROKEN(work) ((work)->submode)
/* a turret's side, in its work's tick, which picks the model it draws */
#define TURRET_SIDE(work) ((work)->tick)
enum turret_side { LEFT_TURRET, RIGHT_TURRET };
#define BODY_FLAGS (WORLD_MOVING | WORLD_SOLID)
/* the lid's shapes: three boxes along z. LID_SHAPE takes a box's center's z,
 * and its half sizes along x and z. each box reaches LID_HALF_HEIGHT above and
 * below its center */
#define LID_SHAPES 3
#define LID_HALF_HEIGHT 300.0F
/* clang-format off */
#define LID_SHAPE(z, half_x, half_z)                                           \
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0, {0.0F, 0.0F, z},    \
     half_x, LID_HALF_HEIGHT, half_z, 0, {0, 0, 0}}
/* clang-format on */
/* a size class takes the collision model's radius plus CLASS_MARGIN */
#define CLASS_MARGIN 15.0F
/* the lid's fragments' models lie in a frame FRAGMENTS_X and FRAGMENTS_Y from
 * the lid's */
#define FRAGMENTS_X (-195.25F)
#define FRAGMENTS_Y 41.5F
/* as the lid breaks, each fragment flies from the lid's origin at
 * FRAGMENT_SPEED. one within FRAGMENT_NEAR of the origin keeps its own
 * offset as its speed. each then gains FRAGMENT_PUSH along -x and along y */
#define FRAGMENT_SPEED 2.0F
#define FRAGMENT_NEAR 0.5F
#define FRAGMENT_PUSH 1.0F
/* each fragment scores FRAGMENT_SCORE. its floor lies far below the stage */
#define FRAGMENT_SCORE 20
#define FRAGMENT_FLOOR (-100000.0F)
/* the break shakes player 0's pad */
#define BREAK_POWER 6
#define BREAK_PULSES 4
#define BREAK_INTERVAL 8
#define BREAK_FREQ 70
/* a turret turns TURRET_TURN_SHARE of the way toward player 0 each step, up
 * to TURRET_MAX_TURN. it keeps within TURRET_MAX_SWING of a quarter turn past
 * the reactor's heading */
#define TURRET_TURN_SHARE 0.2F
#define TURRET_MAX_TURN 32
#define TURRET_MAX_SWING 0x800
/* a turret may fire on the first FIRE_STEPS steps of each FIRE_PERIOD. of
 * those, it fires only on a step whose count has FIRE_STEP_BIT set, at a
 * chance of FIRE_CHANCE */
#define FIRE_PERIOD 130
#define FIRE_STEPS 30
#define FIRE_STEP_BIT 2
#define FIRE_CHANCE 0.08F
/* a shot leaves the muzzle, MUZZLE_Y and MUZZLE_Z from the turret. it moves
 * along -z at turret_muzzle_speed, less up to SHOT_SLOW. it also gains up to
 * SHOT_SPREAD along +x, and up to SHOT_DROP along -y */
#define MUZZLE_Y (-5.0F)
#define MUZZLE_Z (-35.0F)
#define SHOT_SLOW 0.1F
#define SHOT_SPREAD 0.1F
#define SHOT_DROP 0.01F
/* a panel draws bright on the steps of each PANEL_BLINK_PERIOD past
 * PANEL_BRIGHT_AFTER, counted from 0. it draws dim on the rest */
#define PANEL_BLINK_PERIOD 70
#define PANEL_BRIGHT_AFTER 50
/* each step, a hit panel makes dust at a chance of DUST_CHANCE. the dust
 * starts at the panel's pos, at a scale of DUST_SIZE. the panel passes a speed
 * of DUST_SPEED along its z, which dust_create never reads */
#define DUST_CHANCE 0.3F
#define DUST_SPEED 3.8F
#define DUST_SIZE 5.0F
/* the coils sway by two sums of coil_waves' COIL_WAVES waves. one is at the
 * step, and one COIL_LEAD steps on. the first coil moves along x and z by the
 * later sum, and along y by the step's. the second coil takes them the other
 * way */
#define COIL_WAVES 3
#define COIL_LEAD 30

/* a wave of the coils' sway: the angle it turns each step, and its
 * amplitude */
struct coil_wave {
    Angle rate;
    Float amplitude;
};
/* each panel's turn about y, past the reactor's */
/* clang-format off */
static Angle panel_headings[PANELS] = {
    0, 0, 0, 0, 0,
    -NJM_DEG_ANG(36), NJM_DEG_ANG(36), -NJM_DEG_ANG(74), NJM_DEG_ANG(74),
};
/* clang-format on */
/* the waves the coils sway by while the lid stands */
/* clang-format off */
static struct coil_wave coil_waves[COIL_WAVES] = {
    {0x2800, 0.25F},
    {0x1a00, 0.35F},
    {0x1300, 0.25F},
};
/* clang-format on */
static struct task *active_reactor = NULL;
struct task *engine_reactor_lid = NULL;
static struct collision_shape lid_shapes[] = {
    LID_SHAPE(0.0F, 50.0F, 60.0F), LID_SHAPE(250.0F, 30.0F, 200.0F),
    LID_SHAPE(-250.0F, 30.0F, 200.0F)};
static Float turret_muzzle_speed = 1.2F;
/* clang-format off */
static struct collision_shape panel_shape[PANEL_SHAPES] = {
    {10, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNTURNED | SHAPE_UNK_2000, {0, 0, 0}, 5.0F, 0, 0, 0, {0, 0, 0}},
};
/* clang-format on */
/* what a puff's speed along y, frame and size gain each step */
static Float puff_rise = 0.05F;
static Float puff_frame_step = 0.3F;
static Float puff_growth = 0.22F;
/* a new puff's speed spread and spin range, in degrees, and its shape. the
 * values are those stg07/fire_statue.c's puffs take as fire_spread, fire_spin
 * and fire_shapes */
static Float puff_spread = 0.1F;
static Float puff_spin = 3.0F;
/* clang-format off */
static struct collision_shape puff_shape[] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_ALL, SHAPE_HURTABLE, {0, 0, 0}, 2.0F,
     0, 0, 0, {0, 0, 0}},
};
/* clang-format on */

static Float wave_offset(struct coil_wave *wave, int count, int step)
{
    Float sum = 0.0F;
    while (count--) {
        sum += njSin(wave->rate * (Uint16)step) * wave->amplitude;
        ++wave;
    }
    return sum;
}

static void reactor_destroy(struct task *task);
static void reactor_step(struct task *task);
static void reactor_draw(struct task *task);
static void lid_step(struct task *task);
static int lid_start(struct task *task);
static void turret_step(struct task *task);
static int turret_start(struct task *task);
static void panel_step(struct task *task);
static int panel_start(struct task *task);
static struct task *turret_shot_create(NJS_POINT3 *pos, NJS_VECTOR *speed);

/* set flags to BODY_FLAGS with radius's size class. a radius past
 * WORLD_SIZE_CLASS_1_MAX is class 0, which leaves flags as they are */
#define SET_SIZE_CLASS(flags, radius)                                          \
    do {                                                                       \
        if (!((radius) > WORLD_SIZE_CLASS_1_MAX)) {                            \
            if ((radius) > WORLD_SIZE_CLASS_2_MAX) {                           \
                (flags) = BODY_FLAGS | WORLD_SIZE_CLASS(1);                    \
            } else if ((radius) > WORLD_SIZE_CLASS_3_MAX) {                    \
                (flags) = BODY_FLAGS | WORLD_SIZE_CLASS(2);                    \
            } else {                                                           \
                (flags) = BODY_FLAGS | WORLD_SIZE_CLASS(3);                    \
            }                                                                  \
        }                                                                      \
    } while (0)

void reactor_lid_init(struct task *task)
{
    /* @hack spelling: transform, a local for njCalcPoint, where njCalcPoint
     * called by name would do */
    void (*transform)(NJS_MATRIX *, NJS_POINT3 *, NJS_POINT3 *);
    struct task_work *work = task->work;
    struct task *child;
    int i;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    if (!task->motion.p) {
        return;
    }
    task->draw = reactor_draw;
    task->step = reactor_step;
    task->destructor = reactor_destroy;
    transform = njCalcPoint;
    work->mode = 0;
    *COLLISION_COPY(task) = reactor_body_collision[0];
    object = COLLISION_COPY(task);
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->ang[1] = work->ang.y;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    transform(NULL, (NJS_POINT3 *)reactor_body_collision->pos,
              (NJS_POINT3 *)object->pos);
    njPopMatrixEx();
    flags = BODY_FLAGS;
    radius = object->model->r + CLASS_MARGIN;
    SET_SIZE_CLASS(flags, radius);
    world_add(flags, task, object);
    if (!active_reactor) {
        active_reactor = task;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    child = task_create_child(TASK_WORK | TASK_FORCE, lid_step, task);
    if (child) {
        transform(NULL, (NJS_POINT3 *)reactor_lid_model->pos,
                  &child->work->pos);
        child->work->ang = work->ang;
        child->work->tick = 0;
        if (!lid_start(child)) {
            task_destroy(child);
        }
    }
    njTranslate(NULL, reactor_body->pos[0], reactor_body->pos[1],
                reactor_body->pos[2]);
    BROKEN_PANELS(task) = ALL_BROKEN;
    /* @hack redundant: two tests of transform, in a block no path reaches */
    if (0) {
        if (transform) {
        }
        if (transform) {
        }
    }
    for (i = 0; i < PANELS; ++i) {
        child = task_create_child(TASK_WORK, panel_step, task);
        if (child) {
            transform(NULL, &reactor_panel_positions[i], &child->work->pos);
            child->work->ang = work->ang;
            child->work->ang.y += panel_headings[i];
            PANEL_INDEX(child->work) = i;
            BROKEN_PANELS(task) &= ~(1 << i);
            if (!panel_start(child)) {
                task_destroy(child);
            }
        }
    }
    child = task_create_child(TASK_WORK | TASK_FORCE, turret_step, task);
    if (child) {
        transform(NULL, (NJS_POINT3 *)reactor_left_gun->pos, &child->work->pos);
        child->work->ang = work->ang;
        TURRET_SIDE(child->work) = LEFT_TURRET;
        if (!turret_start(child)) {
            task_destroy(child);
        }
    }
    child = task_create_child(TASK_WORK | TASK_FORCE, turret_step, task);
    if (child) {
        transform(NULL, (NJS_POINT3 *)reactor_right_gun->pos,
                  &child->work->pos);
        child->work->ang = work->ang;
        TURRET_SIDE(child->work) = RIGHT_TURRET;
        if (!turret_start(child)) {
            task_destroy(child);
        }
    }
    njPopMatrixEx();
}

static void reactor_destroy(struct task *task)
{
    if (COLLISION_COPY(task)->model) {
        world_remove(task, COLLISION_COPY(task));
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
    if (active_reactor == task) {
        active_reactor = NULL;
    }
}

static void reactor_step(struct task *task)
{
    struct task_work *work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
    if (player_near((NJS_POINT3 *)COLLISION_COPY(task)->pos, COLLIDE_RANGE)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

#define TRANSLATE_NODE(node)                                                   \
    njTranslate(NULL, (node)->pos[0], (node)->pos[1], (node)->pos[2])

static void reactor_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float wave;
    Float later;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njSetTexture(&reactor_rim_textures);
    TRANSLATE_NODE(reactor_rim);
    texture_anim_apply(reactor_rim->model, reactor_rim_anim, game_state.steps);
    njCnkEasyDrawModel(reactor_rim->model);
    matrix_load_parent();
    njSetTexture(&reactor_textures);
    TRANSLATE_NODE(reactor_body);
    njCnkEasyDrawModel(reactor_body->model);
    njPushMatrixEx();
    if (engine_reactor_lid) {
        wave = wave_offset(coil_waves, COIL_WAVES, game_state.steps);
        later =
            wave_offset(coil_waves, COIL_WAVES, game_state.steps + COIL_LEAD);
    } else {
        wave = 0.0F;
        later = 0.0F;
    }
    njTranslate(NULL, reactor_first_coil->pos[0] + later,
                reactor_first_coil->pos[1] + wave,
                reactor_first_coil->pos[2] + later);
    njCnkEasyDrawModel(reactor_first_coil->model);
    matrix_load_parent();
    njTranslate(NULL, reactor_second_coil->pos[0] + wave,
                reactor_second_coil->pos[1] + later,
                reactor_second_coil->pos[2] + wave);
    njCnkEasyDrawModel(reactor_second_coil->model);
    matrix_load_parent();
    TRANSLATE_NODE(reactor_right_support);
    njCnkEasyDrawModel(reactor_right_support->model);
    matrix_load_parent();
    TRANSLATE_NODE(reactor_left_support);
    njCnkEasyDrawModel(reactor_left_support->model);
    njPopMatrix(3);
}

static void reactor_break(struct task *task)
{
    struct task_work *work = task->work;
    struct angles rotation;
    NJS_POINT3 pos;
    NJS_VECTOR speed;
    NJS_CNK_OBJECT *node;
    NJS_TEXLIST *textures;
    Float length;
    int i;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslate(NULL, FRAGMENTS_X, FRAGMENTS_Y, 0.0F);
    for (i = 0; i < FRAGMENTS; i++) {
        textures = reactor_fragment_textures[i];
        node = reactor_fragments[i];
        rotation.x = 0;
        rotation.y = work->ang.y;
        rotation.z = 0;
        speed = *(NJS_VECTOR *)node->pos;
        speed.x += FRAGMENTS_X;
        speed.y += FRAGMENTS_Y;
        length = njScalor(&speed);
        if (length > FRAGMENT_NEAR) {
#ifdef NONMATCHING
            /* the plain form the hack tag names */
            length = FRAGMENT_SPEED / length;
            speed.x *= length;
            speed.y *= length;
            speed.z *= length;
#else
            /* @hack spelling: a Float pointer stepped through speed, where
             * the members scaled one by one would do */
            Float *coordinate = &speed.x;
            length = FRAGMENT_SPEED / length;
            *coordinate++ *= length;
            *coordinate++ *= length;
            *coordinate *= length;
#endif
        }
        speed.x -= FRAGMENT_PUSH;
        speed.y += FRAGMENT_PUSH;
        njCalcPoint(NULL, (NJS_POINT3 *)node->pos, &pos);
        njCalcVector(NULL, &speed, &speed);
        debris_launch_flags(&pos, &rotation, FRAGMENT_FLOOR, &speed,
                            node->model, textures,
                            DEBRIS_LANDING_BURST | DEBRIS_VANISH);
        score_add(FRAGMENT_SCORE);
    }
    njPopMatrixEx();
    vibration_pulses_create(0, BREAK_POWER, BREAK_PULSES, BREAK_INTERVAL,
                            BREAK_FREQ);
}

static void lid_draw(struct task *task)
{
    struct task_work *work = task->work;
    if (LID_BROKEN(work)) {
        return;
    }
    njSetTexture(&reactor_lid_textures);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(reactor_lid_model->model);
    njPopMatrixEx();
}

static void lid_step(struct task *task)
{
    struct task_work *work = task->work;
    if (BROKEN_PANELS(task->parent) == ALL_BROKEN && !LID_BROKEN(work)) {
        LID_BROKEN(work) = TRUE;
        reactor_break(task);
        engine_reactor_lid = NULL;
    }
    if (LID_BROKEN(work)) {
        work->collision->shapes->flags |= SHAPE_OFF;
    } else {
        work->collision->shapes->flags &= ~SHAPE_OFF;
    }
    if (!LID_BROKEN(work) && player_near(&work->pos, COLLIDE_RANGE)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    collision_register(task);
}

static void lid_destroy(struct task *task)
{
    if (task->motion.p) {
        world_remove(task, COLLISION_COPY(task));
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
    if (engine_reactor_lid == task) {
        engine_reactor_lid = NULL;
    }
}

static int lid_start(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;
    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    if (!task->motion.p) {
        return FALSE;
    }
    task->draw = lid_draw;
    task->step = lid_step;
    task->destructor = lid_destroy;
    if (!engine_reactor_lid) {
        engine_reactor_lid = task;
    }
    *COLLISION_COPY(task) = reactor_lid_collision[0];
    object = COLLISION_COPY(task);
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->ang[1] = work->ang.y;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, (NJS_POINT3 *)reactor_lid_collision->pos,
                (NJS_POINT3 *)object->pos);
    njPopMatrixEx();
    flags = BODY_FLAGS;
    radius = object->model->r + CLASS_MARGIN;
    SET_SIZE_CLASS(flags, radius);
    world_add(flags, task, object);
    collision_init(task, lid_shapes, LID_SHAPES, COLLISION_OBJECT);
    return TRUE;
}

static void turret_draw(struct task *task)
{
    struct task_work *work = task->work;
    njSetTexture(&reactor_textures);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y - QUARTER_TURN);
    if (TURRET_SIDE(task->work) != LEFT_TURRET) {
        njCnkEasyDrawModel(reactor_right_gun->model);
    } else {
        njCnkEasyDrawModel(reactor_left_gun->model);
    }
    njPopMatrixEx();
}

static void turret_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR direction;
    NJS_POINT3 muzzle;
    Uint32 frame;
    Sint16 turn; /* a change of angle, wrapped the shorter way round */
    if (BROKEN_PANELS(task->parent) == ALL_BROKEN) {
        return;
    }
    if (player_work[0]) {
#ifdef NONMATCHING
        /* the plain form the hack tag names */
        direction.x = work->pos.x - player_work[0]->pos.x;
        direction.y = work->pos.y - player_work[0]->pos.y;
        direction.z = work->pos.z - player_work[0]->pos.z;
#else
        /* @hack spelling: three Float pointers stepped through the vectors,
         * where the members' differences written out would do */
        Float *left;
        Float *right;
        Float *out;
        out = &direction.x;
        left = &work->pos.x;
        right = &player_work[0]->pos.x;
        *out++ = *left++ - *right++;
        *out++ = *left++ - *right++;
        *out = *left - *right;
#endif
        turn = (Angle)(atan2f(direction.x, direction.z) *
                       (Float)ANGLE_PER_RADIAN) -
               work->ang.y;
        /* @hack spelling: turn scaled through a pointer to it and a Float copy,
         * where turn *= TURRET_TURN_SHARE would do */
        {
            Sint16 *angle = &turn;
            Float amount = *angle;
            *angle = amount * TURRET_TURN_SHARE;
        }
        if (turn > TURRET_MAX_TURN) {
            turn = TURRET_MAX_TURN;
        }
        if (turn < -TURRET_MAX_TURN) {
            turn = -TURRET_MAX_TURN;
        }
        work->ang.y += turn;
        turn = work->ang.y - task->parent->work->ang.y - QUARTER_TURN;
        if (turn > TURRET_MAX_SWING) {
            turn = TURRET_MAX_SWING;
        }
        if (turn < -TURRET_MAX_SWING) {
            turn = -TURRET_MAX_SWING;
        }
        work->ang.y = turn + task->parent->work->ang.y + QUARTER_TURN;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((frame = game_state.steps) % FIRE_PERIOD < FIRE_STEPS &&
        (frame & FIRE_STEP_BIT) && rand() * RAND_SCALE < FIRE_CHANCE) {
        direction.x = rand() * (SHOT_SPREAD * RAND_SCALE);
        direction.y = (rand() * (SHOT_DROP * RAND_SCALE)) - SHOT_DROP;
        direction.z =
            -turret_muzzle_speed + (rand() * (SHOT_SLOW * RAND_SCALE));
        muzzle.x = 0.0F;
        muzzle.y = MUZZLE_Y;
        muzzle.z = MUZZLE_Z;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &muzzle, &muzzle);
        njCalcVector(NULL, &direction, &direction);
        njPopMatrixEx();
        turret_shot_create(&muzzle, &direction);
    }
}

static void turret_destroy(struct task *task)
{
}

static int turret_start(struct task *task)
{
    /* @hack redundant: work, a local for task->work that nothing reads */
    struct task_work *work = task->work;
    task->draw = turret_draw;
    task->step = turret_step;
    task->destructor = turret_destroy;
    return TRUE;
}

static void panel_draw(struct task *task)
{
    struct task_work *work = task->work;
    if (game_state.steps % PANEL_BLINK_PERIOD > PANEL_BRIGHT_AFTER) {
        njSetTexture(&reactor_panel_bright);
    } else {
        njSetTexture(&reactor_panel_dim);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(reactor_panel->model);
    njPopMatrixEx();
}

static void panel_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR speed;
    if (work->flags & WORK_HURT) {
        work->flags &= ~WORK_HURT;
        ++HITS(work);
        if (HITS(work) >= PANEL_HITS) {
            BROKEN_PANELS(task->parent) |= 1 << PANEL_INDEX(task->work);
            if (task->placement) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        } else {
            collision_register(task);
        }
        return;
    }
    if (HITS(work) && rand() * RAND_SCALE < DUST_CHANCE) {
        speed.x = 0.0F;
        speed.y = 0.0F;
        speed.z = DUST_SPEED;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &speed, &speed);
        njPopMatrixEx();
#ifdef NONMATCHING
        /* this side repairs the call that the tag at dust_create_by_value
         * names: dust_create takes the speed's address */
        dust_create(&work->pos, &speed, DUST_SIZE);
#else
        dust_create_by_value(&work->pos, speed, DUST_SIZE);
#endif
    }
    collision_register(task);
}

static void panel_destroy(struct task *task)
{
    enemy_destroy(task);
}

static int panel_start(struct task *task)
{
    task->draw = panel_draw;
    task->step = panel_step;
    task->destructor = panel_destroy;
    enemy_work_alloc(task);
    collision_init(task, panel_shape, PANEL_SHAPES, COLLISION_ITEM);
    return TRUE;
}

/* a puff's task keeps three values in pointer fields, which point at nothing.
 * they are its size, its frame and its color. the size is half the quad's
 * width. its work's scl is its speed a step, and ang.x its spin a step. ang.z
 * is its quad's turn, and ang.y its quad's edge_angle */
/* @hack type: the size read as a Float through motion's address, where
 * (task)->motion.f would do */
#define PUFF_SIZE(task) (*(Float *)&(task)->motion)
/* @hack type: the frame read as a Float through force's address, where
 * (task)->force.f would do */
#define PUFF_FRAME(task) (*(Float *)&(task)->force)
#define PUFF_COLOR(task) ((Uint32)(task)->extra.l)
/* the share of its speed a puff keeps each step */
#define PUFF_DRAG 0.98F
/* a puff's frame picks its texture of reactor_smoke_textures. the puff ends
 * once its frame reaches PUFF_FRAMES, or once its shape touches anything */
#define PUFF_FRAMES 10.0F

void reactor_puff_step(struct task *task)
{
    struct task_work *work = task->work;
    work->pos.x += work->scl.x;
    work->pos.y += work->scl.y;
    work->pos.z += work->scl.z;
    work->ang.z += work->ang.x;
    PUFF_SIZE(task) += puff_growth;
    work->scl.x *= PUFF_DRAG;
    work->scl.y = (work->scl.y * PUFF_DRAG) + puff_rise;
    work->scl.z *= PUFF_DRAG;
    PUFF_FRAME(task) += puff_frame_step;
    if (PUFF_FRAME(task) >= PUFF_FRAMES) {
        task_destroy(task);
    } else if (work->collision) {
        if (work->collision->flags & COLLISION_TOUCHED) {
            task_destroy(task);
        } else {
            collision_register(task);
        }
    }
}

/* task_destroy frees task->motion, task->force and task->extra once the
 * destructor returns. a puff's three hold values, not blocks of the heap, so
 * the destructor clears them */
void reactor_puff_destroy(struct task *task)
{
    task->force.p = NULL;
    task->motion.p = NULL;
    task->extra.p = NULL;
}

void reactor_puff_draw(struct task *task)
{
    struct task_work *work = task->work;
    njSetTexture(&reactor_smoke_textures);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njFogDisable();
    particle_draw_quad(&work->pos, PUFF_FRAME(task), work->ang.y, work->ang.z,
                       PUFF_SIZE(task), PUFF_SIZE(task), PUFF_COLOR(task), 0,
                       0.0F, 0.0F);
    njFogEnable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

#define ENGINE_REACTOR
#define CANNON_INIT engine_cannon_init
#define SHOT_HIT                                                               \
    (SHAPE_ATTACK_LEVEL(1) | SHAPE_GUARD_LEVEL(1) | SHAPE_HURT |               \
     SHAPE_HURTS_PLAYERS)
#define SHOT_FLAGS 1
#define SHOT_CONTROLLER engine_reactor_lid
#include "shared/cannon_body.h"
