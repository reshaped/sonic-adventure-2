/* the tactics of tactics.h, and the tests of player 0's place that
 * shared/golem.h declares. boss_golem_e/tactics.c compiles this text again for
 * BOSS_GOLEM_E.PRS, with that fight's golem and floor height */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "boss_golem/footholds.h"
#include "boss_golem/golem.h"
#include "boss_golem/tactics.h"
#include "collision/collision.h"
#include "draw/lines.h"
#include "draw/motion.h"
#include "player/player.h"
#include "shared/boss.h"
#include "shared/golem.h"
#include "shared/golem_wave.h"
#include "shared/golem_util.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the fight's golem, which boss_golem_e/tactics.c makes golem_e */
#ifndef GOLEM_STATE
#define GOLEM_STATE golem
#endif

#define NEAR_RADIUS 165.0F
#define LOW_Y 160.0F
#define DARK_LOW_Y 180.0F
#define RING_SPREAD 0.02F
#define RING_RISE 0.005F
#define WAVE_SLOWDOWN 0.95F
/* the column starts COLUMN_START steps into the wave, and draws COLUMN_Y above
 * it */
#define COLUMN_START 10
#define COLUMN_Y 20.0F
/* the column's shape plays its COLUMN_FRAMES frames over each
 * wave_column_motion_steps of game_state.steps */
#define COLUMN_FRAMES 7.0F
#define WAVE_SHAPES 1
#define WAVE_ATTACK_LEVEL 2
#define HOLD_START 25.0F
#define HOLD_END 125.0F

/* a shock wave's record, in its task's extra. each alpha falls from 1 to 0
 * over a quarter turn of a cosine */
struct golem_wave {
    Float ring_growth; /* what grows the ring's scale */
    /* what grows the column's scale, and the radius of the wave's shape */
    Float column_growth;
    Float ring_alpha;
    Float column_alpha;
    NJS_VECTOR ring_scale;
    NJS_VECTOR column_scale;
    Angle column_ang; /* the column's turn about y */
    Angle column_spin; /* what column_ang turns by in a step */
    Sint32 steps; /* the wave's steps so far */
};
#define WAVE(task) ((struct golem_wave *)(task)->extra.p)

/* clang-format off */
static const struct collision_shape wave_shapes[WAVE_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(WAVE_ATTACK_LEVEL),
     SHAPE_HURTABLE, {0.0F, 0.0F, 0.0F}, 15.0F, 10.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

/* golem_may_attack's near wait and up wait, in seconds, as tactics.h gives
 * them */
static Float near_seconds_per_power = 1.0F;
static Float near_seconds_per_retry = 1.5F;
static Float near_seconds = 12.0F;
static Float up_seconds_per_power = 0.25F;
static Float up_seconds_per_retry = 0.5F;
static Float up_seconds = 2.5F;
/* where each hand strikes, in the golem's own axes. golem_set_slot aims the
 * golem so that the point's heading is the slot's */
#ifdef NONMATCHING
/* this side keeps the datum in .data, where the image holds it. nothing writes
 * it, so GCC would fold it into the code or place it among the constants */
__attribute__((used, section(".data")))
#endif
static NJS_POINT3 hand_points[2] = {
#include "boss_golem/tactics/hand_points.inc"
};
static Float close_radius = 82.5F;
/* a wave's ring starts at the scale wave_ring_scale, and fades over
 * wave_ring_steps. its column starts at wave_column_width across and 1 up, and
 * fades over wave_column_steps. a step grows the column's scale across by
 * column_growth / wave_column_width_rate, and up by column_growth /
 * wave_column_height_rate */
static Float wave_ring_growth = 2.1F;
static Float wave_column_growth = 2.0F;
static Angle wave_spin = 0x200;
static Sint32 wave_ring_steps = 40;
static Sint32 wave_column_steps = 43;
static Float wave_column_width = 1.0F;
static Float wave_ring_scale = 0.6F;
static Float wave_column_width_rate = 42.0F;
static Float wave_column_height_rate = 200.0F;
static Sint32 wave_column_motion_steps = 60;
static Sint32 rest_wait_per_power = 15;
static Sint32 rest_wait_per_retry = 90;
static Sint32 rest_wait_base = 90;
static Angle turn_speed = 0x100;
/* a hop starts at the hop_wait-th step of golem's timer, at hop_speed. the
 * speed falls to 0 over half hop_steps. once the lift falls below 0, the timer
 * starts over for the next hop */
static Sint32 hop_steps = 20;
static Sint32 hop_wait = 20;
static Float hop_speed = 0.25F;
/* a bob's sine turns by bob_turn a step, and is bob_size high. it is
 * bob_late_size high over the near wait's last BOB_LATE_STEPS */
static Angle bob_turn = 0x2000;
static Float bob_size = 0.15F;
static Float bob_late_size = 0.3F;
#define BOB_LATE_STEPS 300

static void wave_destroy(struct task *task);
static void wave_step(struct task *task);
static void wave_draw(struct task *task);
static int nearest_slot(struct task *task);
static int up_wait(struct task *task);

Bool golem_player_close(struct task *task)
{
    NJS_VECTOR flat;
    /* @hack spelling: distance, a local for the length, where the length in
     * the test would do */
    Float distance;

    flat.x = player_work[0]->pos.x;
    flat.y = 0.0F;
    flat.z = player_work[0]->pos.z;
    distance = njScalor(&flat);
    return distance < close_radius;
}

Bool golem_player_near(struct task *task)
{
    NJS_VECTOR flat;
    /* @hack spelling: distance, a local for the length, where the length in
     * the test would do */
    Float distance;

    flat.x = player_work[0]->pos.x;
    flat.y = 0.0F;
    flat.z = player_work[0]->pos.z;
    distance = njScalor(&flat);
    return distance < NEAR_RADIUS;
}

Bool golem_player_low(struct task *task)
{
    /* @hack spelling: dark_story tested for FALSE first, where dark_story ?
     * DARK_LOW_Y : LOW_Y would do */
    return player_work[0]->pos.y <
           (!GOLEM_STATE->dark_story ? LOW_Y : DARK_LOW_Y);
}

Angle golem_player_heading(void)
{
    return golem_heading(player_work[0]->pos.x, player_work[0]->pos.z);
}

Angle golem_turn_to_player(void)
{
    return golem_angle_turn(GOLEM_STATE->heading, golem_player_heading());
}

struct task *golem_wave_create(struct task *parent, int state)
{
    struct task *task;

    task = task_create_child(TASK_WORK, wave_step, parent);
    task->draw = wave_draw;
    task->destructor = wave_destroy;
    task->extra.p = syMalloc(sizeof(struct golem_wave));
    /* @bug memory: writes through a null record where syMalloc has no room.
     * wave_destroy tests the record at 0x262 of the object's code, but this
     * call writes through it at 0xea */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the heap has no record, the call
     * destroys the wave's task and returns null */
    if (task->extra.p == NULL) {
        task_destroy(task);
        return NULL;
    }
#endif
    WAVE(task)->steps = 0;
    WAVE(task)->ring_alpha = 1.0F;
    WAVE(task)->column_alpha = 1.0F;
    WAVE(task)->ring_scale.x = WAVE(task)->ring_scale.y =
        WAVE(task)->ring_scale.z = wave_ring_scale;
    WAVE(task)->column_scale.x = WAVE(task)->column_scale.z = wave_column_width;
    WAVE(task)->column_scale.y = 1.0F;
    task->work->ang.y = GOLEM_STATE->slot * GOLEM_SLOT_TURN;
    golem_polar_xz(task->work->ang.y, GOLEM_FOOTHOLD_RADIUS, &task->work->pos.x,
                   &task->work->pos.z);
    /* @hack spelling: a switch for each fight, where the one switch both
     * fights share would do */
    if (!GOLEM_STATE->dark_story) {
        switch (state) {
        case FOOTHOLD_WHOLE:
            task->work->pos.y = golem_floor_y;
            break;
        case FOOTHOLD_SUNK:
            task->work->pos.y = golem_floor_y - GOLEM_FOOTHOLD_DROP;
            break;
        case FOOTHOLD_SUNK_TWICE:
        default:
            task->work->pos.y =
                golem_floor_y - (FOOTHOLD_SUNK_TWICE * GOLEM_FOOTHOLD_DROP);
        }
    } else {
        switch (state) {
        case FOOTHOLD_WHOLE:
            task->work->pos.y = golem_floor_y;
            break;
        case FOOTHOLD_SUNK:
            task->work->pos.y = golem_floor_y - GOLEM_FOOTHOLD_DROP;
            break;
        case FOOTHOLD_SUNK_TWICE:
        default:
            task->work->pos.y =
                golem_floor_y - (FOOTHOLD_SUNK_TWICE * GOLEM_FOOTHOLD_DROP);
        }
    }
    collision_init(task, wave_shapes, WAVE_SHAPES, COLLISION_OBJECT);
    WAVE(task)->ring_growth = wave_ring_growth;
    WAVE(task)->column_growth = wave_column_growth;
    WAVE(task)->column_spin = wave_spin;
    return task;
}

static void wave_destroy(struct task *task)
{
    if (task->extra.p) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void wave_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work would
     * do */
    struct task_work *work = task->work;

    WAVE(task)->ring_scale.x += WAVE(task)->ring_growth * RING_SPREAD;
    WAVE(task)->ring_scale.z = WAVE(task)->ring_scale.x;
    WAVE(task)->ring_scale.y += WAVE(task)->ring_growth * RING_RISE;
    WAVE(task)->ring_growth *= WAVE_SLOWDOWN;
    WAVE(task)->ring_alpha = njCos(
        (Angle)(QUARTER_TURN / (Float)wave_ring_steps * WAVE(task)->steps));
    if (WAVE(task)->steps >= COLUMN_START) {
        WAVE(task)->column_scale.x +=
            WAVE(task)->column_growth / wave_column_width_rate;
        WAVE(task)->column_scale.z = WAVE(task)->column_scale.x;
        WAVE(task)->column_scale.y +=
            WAVE(task)->column_growth / wave_column_height_rate;
        WAVE(task)->column_ang += WAVE(task)->column_spin;
        /* @hack spelling: the spin's product stored back, where *= would do */
        WAVE(task)->column_spin = WAVE(task)->column_spin * WAVE_SLOWDOWN;
        WAVE(task)->column_alpha =
            njCos((Angle)(QUARTER_TURN / (Float)wave_column_steps *
                          (WAVE(task)->steps - COLUMN_START)));
        work->collision->shapes->radius += WAVE(task)->column_growth;
        collision_fit_radius(task);
        WAVE(task)->column_growth *= WAVE_SLOWDOWN;
    }
    if (++WAVE(task)->steps > wave_column_steps + COLUMN_START) {
        task_kill(task);
    } else {
        collision_register(task);
    }
}

static void wave_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, twelve bytes of the frame that no code
     * touches */
    NJS_POINT3 unused;
    Float frame;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    if (WAVE(task)->steps < wave_ring_steps) {
        njPushMatrixEx();
        njSetTexture(&golem_wave_ring_texlist);
        draw_control_save();
        draw_control_clear(NJD_CONTROL_3D_OFFSET_MATERIAL);
        draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
        constant_material_set(WAVE(task)->ring_alpha, 1.0F, 1.0F, 1.0F);
        golem_model_draw_scaled_v(&golem_wave_ring_model,
                                  &WAVE(task)->ring_scale);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_control_restore();
        njPopMatrixEx();
    }
    if (WAVE(task)->steps > COLUMN_START) {
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, COLUMN_Y, 0.0F);
        njRotateY(NULL, WAVE(task)->column_ang);
        draw_control_save();
        draw_control_clear(NJD_CONTROL_3D_OFFSET_MATERIAL);
        draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE |
                         NJD_CONTROL_3D_CONSTANT_MATERIAL);
        njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ONE);
        constant_material_set(WAVE(task)->column_alpha, 1.0F, 1.0F, 1.0F);
        frame = COLUMN_FRAMES / wave_column_motion_steps *
                (game_state.steps % wave_column_motion_steps);
        njSetTexture(&golem_wave_column_texlist);
        golem_shape_motion_draw_scaled_v(
            &golem_wave_column_object, &golem_wave_column_motion,
            &golem_wave_column_shape, frame, &WAVE(task)->column_scale);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_control_restore();
        njPopMatrixEx();
    }
    njPopMatrixEx();
}

void golem_set_rest_wait(void)
{
    int wait;
    /* @hack spelling: retries, the sum's second part set apart, where the
     * one sum would do */
    int retries;

    wait = (int)((GOLEM_STATE->power * rest_wait_per_power) + rest_wait_base);
    retries = stage_get_retries() * rest_wait_per_retry;
    if (GOLEM_STATE->dark_story) {
        GOLEM_STATE->rest_wait = (wait + retries) / 2;
    } else {
        GOLEM_STATE->rest_wait = wait + retries;
    }
}

Bool golem_may_attack(struct task *task)
{
    int wait;

    /* @hack redundant: the elses, where the code after each return would
     * do */
    if (golem_player_low(task) ||
        (!GOLEM_STATE->dark_story && GOLEM_STATE->up_steps < up_wait(task))) {
        return FALSE;
    } else if (!golem_player_near(task)) {
        return GOLEM_STATE->rest_steps > GOLEM_STATE->rest_wait;
    } else {
        wait = (int)((near_seconds +
                      (near_seconds_per_retry * stage_get_retries()) -
                      ((GOLEM_STATE->power_max - GOLEM_STATE->power) *
                       near_seconds_per_power)) *
                     STEPS_PER_SECOND);
        return GOLEM_STATE->rest_steps > wait;
    }
}

void golem_set_turn(struct task *task, int turn)
{
    switch (turn) {
    case GOLEM_TURN_TO_HAND_A:
        GOLEM_STATE->turn = turn;
        golem_aim_slot(task, GOLEM_HAND_A);
        break;
    case GOLEM_TURN_TO_HAND_B:
        GOLEM_STATE->turn = turn;
        golem_aim_slot(task, GOLEM_HAND_B);
        break;
    case GOLEM_TURN_TO_SLAM:
        GOLEM_STATE->turn = turn;
        golem_aim_slot(task, GOLEM_HAND_A);
        break;
    case GOLEM_TURN_TO_PLAYER:
    case GOLEM_TURN_NONE:
        GOLEM_STATE->turn = turn;
        break;
    }
}

static int nearest_slot(struct task *task)
{
    /* @hack spelling: ang, a local for the heading, where the heading in the
     * sum would do */
    Angle ang;

    ang = golem_player_heading();
    return (Uint16)(ang + (GOLEM_SLOT_TURN / 2)) / GOLEM_SLOT_TURN;
}

void golem_aim_slot(struct task *task, int hand)
{
    int slot = nearest_slot(task);

    if (GOLEM_STATE->power > GOLEM_STATE->power_low) {
        if (hand == GOLEM_HAND_B) {
            slot = (slot + 1) % GOLEM_SLOTS;
        } else {
            slot = (slot + (GOLEM_SLOTS - 1)) % GOLEM_SLOTS;
        }
    }
    golem_set_slot(task, slot, hand);
}

void golem_turn(struct task *task)
{
    switch (GOLEM_STATE->turn) {
    case GOLEM_TURN_TO_PLAYER:
        golem_angle_approach(&GOLEM_STATE->heading, golem_player_heading(),
                             turn_speed);
        break;
    case GOLEM_TURN_TO_HAND_A:
    case GOLEM_TURN_TO_HAND_B:
    case GOLEM_TURN_TO_SLAM:
        golem_angle_approach(&GOLEM_STATE->heading, GOLEM_STATE->aim_heading,
                             turn_speed);
        break;
    }
}

void golem_set_slot(struct task *task, int slot, int hand)
{
    GOLEM_STATE->slot = slot;
    if (GOLEM_STATE->power <= GOLEM_STATE->power_low) {
        GOLEM_STATE->aim_heading = GOLEM_STATE->slot * GOLEM_SLOT_TURN;
    } else {
        GOLEM_STATE->aim_heading =
            (GOLEM_STATE->slot * GOLEM_SLOT_TURN) -
            golem_heading(hand_points[hand].x, hand_points[hand].z);
    }
}

Bool golem_player_facing_up(void)
{
    /* @hack spelling: TRUE and FALSE returned apart, where the test's value
     * would do */
    if (golem_angle_turn(QUARTER_TURN - player_work[0]->ang.y,
                         golem_player_heading()) < 0) {
        return TRUE;
    }
    return FALSE;
}

void golem_hover(struct task *task)
{
    int wait;
    Float size;

    switch (GOLEM_STATE->action) {
    case GOLEM_ACTION_AIM_HAND_A:
    case GOLEM_ACTION_AIM_HAND_B:
    case GOLEM_ACTION_FACE_PLAYER:
        if (GOLEM_STATE->timer++ == 0) {
            GOLEM_STATE->lift_speed = hop_speed;
        } else if (GOLEM_STATE->timer >= hop_wait) {
            GOLEM_STATE->lift += GOLEM_STATE->lift_speed;
            /* hop_steps is even, so halving it as an int loses nothing */
            /* NOLINTBEGIN(bugprone-integer-division) */
            GOLEM_STATE->lift_speed -= hop_speed / (hop_steps / 2);
            /* NOLINTEND(bugprone-integer-division) */
        }
        if (GOLEM_STATE->lift < 0.0F) {
            GOLEM_STATE->lift = 0.0F;
            GOLEM_STATE->lift_speed = hop_speed;
            GOLEM_STATE->timer = 0;
        }
        break;
    case GOLEM_ACTION_REST:
        if (golem_player_near(task)) {
            wait = (int)((near_seconds +
                          (near_seconds_per_retry * stage_get_retries()) -
                          ((GOLEM_STATE->power_max - GOLEM_STATE->power) *
                           near_seconds_per_power)) *
                         STEPS_PER_SECOND);
            if (GOLEM_STATE->action_steps > wait - BOB_LATE_STEPS) {
                size = bob_late_size;
            } else {
                size = bob_size;
            }
            GOLEM_STATE->lift =
                njSin(GOLEM_STATE->action_steps * bob_turn) * size;
        }
        break;
    default:
        GOLEM_STATE->lift = 0.0F;
        break;
    }
}

Bool golem_holding(struct task *task)
{
    /* @hack spelling: record and motion, locals for the motion's record,
     * where task->extra's in the call would do */
    struct boss_record *record;
    struct motion_control *motion;
    Float frame;

    record = task->extra.p;
    motion = record->block;
    frame = motion_frame(motion);
    return frame > HOLD_START && frame < HOLD_END;
}

void golem_count_up_steps(struct task *task)
{
    if (golem_player_low(task)) {
        GOLEM_STATE->up_steps = 0;
    } else {
        GOLEM_STATE->up_steps++;
    }
}

static int up_wait(struct task *task)
{
    return (int)((up_seconds + (up_seconds_per_retry * stage_get_retries()) -
                  ((GOLEM_STATE->power_max - GOLEM_STATE->power) *
                   up_seconds_per_power)) *
                 STEPS_PER_SECOND);
}
