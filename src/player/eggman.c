/* Eggman on foot, the player of PLAYER_EGGMAN outside his walker */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/polygon.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/stars.h"
#include "event/mini_event.h"
#include "hud/action_window.h"
#include "hud/hud.h"
#include "object/fall.h"
#include "player/action.h"
#include "player/eggman.h"
#include "player/player.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* his slots of model_table, which EGGMDL.PRS fills. EGGMAN_MODEL is his model,
 * which every animation of his table draws. EGGMAN_SHADOW is his body's shadow,
 * a modifier volume. the rest are nodes of his model. his draw keeps each
 * EGGMAN_AT_ node's place in the record field its suffix names, as
 * EGGMAN_AT_230's in unk_230, and each hand's and foot's in the field its
 * comment names. it turns EGGMAN_LEANS about x by the record's lean in
 * PLAYER_MODE_RUN. while he holds a task, it turns EGGMAN_HOLDS_A about x by
 * the task's motion's hold_turn. it turns EGGMAN_HOLDS_B the other way */
#define EGGMAN_MODEL 230
#define EGGMAN_AT_230 231
#define EGGMAN_LEANS 232
#define EGGMAN_AT_218 233
#define EGGMAN_SHADOW 236
#define EGGMAN_AT_224 237
#define EGGMAN_FOOT_0 238 /* kept in unk_200 */
#define EGGMAN_FOOT_1 239 /* kept in unk_20c */
#define EGGMAN_HAND_0 240 /* kept in unk_1e8 */
#define EGGMAN_HAND_1 241 /* kept in unk_1f4 */
#define EGGMAN_HOLDS_A 242
#define EGGMAN_HOLDS_B 243

/* the height of his SHAPE_STANDING shape's center above his pos, along his up.
 * his spheres' heights are the record's consts.center_height and
 * consts.target_height */
#define STANDING_CENTER 4.0F
/* PLAYER_MODE_SHAKE_HOLDING sets each axis of the held task's motion's acc to
 * his animation's frame times EGGMAN_HELD_ACC */
#define EGGMAN_HELD_ACC 0.02F
/* a landing holding a task plays ANIM_HOLD_LAND_FAST where the motion's spd.y
 * is HOLD_LAND_FAST_SPEED or below */
#define HOLD_LAND_FAST_SPEED (-5.0F)
/* his attack's animations: ANIM_ATTACK below his trot_speed, and
 * ANIM_RUNNING_ATTACK at it or above. PLAYER_MODE_EGGMAN_ATTACK lasts while
 * either plays or is asked for */
#define ANIM_ATTACK 162
#define ANIM_RUNNING_ATTACK 163
/* the shadows at his hands and feet, each shadow_model scaled across x and z.
 * a hand's shadow lies HAND_SHADOW_DROP below the hand */
#define HAND_SHADOW_DROP 2.0F
#define HAND_SHADOW_X 1.6F
#define HAND_SHADOW_Z 1.1F
#define FOOT_SHADOW_X 2.2F
#define FOOT_SHADOW_Z 0.7F

/* his collision shapes: the three of his body, then his attack's two */
#define ATTACK_SHAPE 3
#define RUNNING_ATTACK_SHAPE 4
#define SHAPES 5

/* the sounds and voices he plays. SOUND_CRY is his cry at a hit, as
 * player_take_hit plays it. PLAYER_MODE_UNK_0E plays SOUND_PUSH_START as its
 * push takes over from ANIM_PUSH_START */
#define SOUND_JUMP 0x2001
#define SOUND_BRAKE 0x2007
#define SOUND_ATTACK 0x2008
#define SOUND_PUSH_START 0x300a
#define SOUND_CRY 0x300f
#define VOICE_JUMP 0x65a
#define VOICE_PICK_UP 0x6b6
#define VOICE_SET_DOWN 0x6b8
#define VOICE_THROW 0x6ba
/* as a shake starts: here PLAYER_MODE_SHAKE_HOLDING, and in player_test_push
 * PLAYER_MODE_UNK_22 */
#define VOICE_SHAKE 0x6c0
#define VOICE_ATTACK 0x6c2
/* his attack's sound plays ATTACK_SOUND_WAIT steps after the attack starts */
#define ATTACK_SOUND_WAIT 18

/* his animations, the record's anim.actions. his draw indexes the table twice
 * for a motion: the entry's motion m, then entry m's motion. every motion m of
 * the table is also entry m's, so the second index changes nothing */
static struct player_action eggman_anims[ANIM_RUNNING_ATTACK + 1] = {
#include "player/eggman/eggman_anims.inc"
};

/* his shapes, whose centers are world positions. eggman_place_shapes moves
 * his body's three with him each step. in his attack, eggman_set_shapes turns
 * on one of the attack's two, at a hand or a foot. His spheres push and resist
 * every group but pickups. His attack shapes hurt every kind, and attack and
 * guard at level 1. */
static struct collision_shape eggman_shapes[SHAPES] = {
#include "player/eggman/eggman_shapes.inc"
};

/* the record his draw is drawing, which eggman_motion_callback reads */
static struct player *eggman_drawn;
/* the inverse of the view as his draw begins. it takes a place in the view back
 * to the world */
static NJS_MATRIX eggman_view_to_world;

static void eggman_step(struct task *task);
static void eggman_choose_mode(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record, struct player *eggman);
static void eggman_destroy(struct task *task);
static void eggman_place_shapes(struct task_work *work, struct player *record);
static void eggman_motion_callback(NJS_CNK_OBJECT *object);
static void eggman_draw(struct task *task);
static void eggman_draw_shadow(struct task_work *work, struct player *record,
                               int anim);
static void eggman_free_move(struct task_work *work, struct player *record);
static Bool eggman_try_jump(struct task_work *work,
                            struct player_motion *motion,
                            struct player *record);
static Bool eggman_take_request(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record, struct player *eggman);

void eggman_create(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *eggman;
    struct player *record;

    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            eggman_step, "Eggman")) == NULL) {
        return;
    }
    if ((eggman = syCalloc(1, EGGMAN_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = eggman;
    work = task->work;
    /* @hack spelling: the record through the address of its first field,
     * where eggman itself would do */
    record = (struct player *)&eggman->slot;
    record->slot = slot;
    record->character = PLAYER_EGGMAN;
    /* @hack spelling: submode stored ahead of mode, where mode first would
     * do */
    work->submode = 0;
    work->mode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = eggman_anims;
    record->anim.next = ANIM_STAND;
    collision_init(task, eggman_shapes, SHAPES, COLLISION_PLAYER);
    work->collision->shapes->radius = record->consts.radius;
    eggman->by_character.eggman.texlist = texture_load_pvm_alloc("EGGTEX");
    life_icon_load(record->character);
    eggman->by_character.eggman.models = model_file_read("EGGMDL.PRS");
    eggman->by_character.eggman.motions = motion_file_read("EGGMTN.PRS");
    player_anim_step(&record->anim);
    action_window_start(record->slot);
    task->destructor = eggman_destroy;
    task->draw = eggman_draw;
}

static void eggman_step(struct task *task)
{
    struct player *eggman;
    struct task_work *work;
    struct player_motion *motion;
    struct player *record;

    eggman = task->extra.p;
    work = task->work;
    motion = task->motion.p;
    /* @hack spelling: record, a copy of eggman, where eggman alone would do */
    record = eggman;
    player_ground_step(task, work, motion, record);
    if (work->mode == PLAYER_MODE_MISS || work->mode == PLAYER_MODE_DROWN) {
        if (work->submode == PLAYER_REQUEST_RESET) {
            work->flags &= ~PLAYER_RESET_WORK_FLAGS;
            record->flags &= ~PLAYER_RESET_FLAGS;
            player_stand(work, motion, record);
        }
    } else {
        eggman_choose_mode(work, motion, record, eggman);
    }
    if (work->flags & PLAYER_WORK_BALL) {
        eggman->by_character.eggman.ball_steps++;
    }
    switch (work->mode) {
    case PLAYER_MODE_STAND:
        player_turn_to_stick(work, motion, record);
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        player_stand_anim(work, motion, record);
        break;
    case PLAYER_MODE_RUN:
        if (record->anim.last == ANIM_TURN &&
            record->anim.current != ANIM_TURN) {
            record->spd.x += record->consts.trot_speed;
        }
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        if (player_collide(work, motion, record) == PLAYER_CRASHED) {
            sound_play_at(SOUND_CRY, NULL, 0, SOUND_LOUDEST, &work->pos);
            work->mode = PLAYER_MODE_CRASH;
            record->anim.next = ANIM_CRASH;
            crash_stars_burst(record->slot);
            record->spd.x = -(record->spd.x * PLAYER_CRASH_REBOUND);
            record->spd.z = -(record->spd.z * PLAYER_CRASH_REBOUND);
        } else {
            player_take_speed(work, motion, record);
            player_run_anim(work, motion, record);
        }
        break;
    case PLAYER_MODE_PATH_RUN:
        player_accel_on_path(work, motion, record);
        player_apply_path_speed(work, motion, record);
        player_move_on_path(work, motion, record);
        player_run_anim(work, motion, record);
        break;
    case PLAYER_MODE_JUMP:
        player_turn_upright(work, motion, record);
        if (!player_steer_to_target(work, motion, record)) {
            player_accel_air(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_BOUNCE:
        player_accel_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_LAUNCH:
        if (record->spd.y >= record->consts.launch_steer_speed) {
            player_accel_launched(work, motion, record);
        } else {
            player_accel_air(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_FLING:
        player_accel_gravity(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_FALL:
        player_turn_upright(work, motion, record);
        if (record->anim.next == ANIM_FREE_FALL) {
            player_accel_gravity(work, motion, record);
        } else if (!player_steer_to_target(work, motion, record)) {
            player_accel_air(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_TEETER:
        break;
    case PLAYER_MODE_BRAKE:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_0E:
        if (record->anim.next == ANIM_PUSH_START) {
            player_accel_brake(work, motion, record);
        } else {
            player_accel_push(work, motion, record);
            if (player_action_flags(work, record) & ACTION_FLAG_400) {
                fun_8c05bce0();
            }
            if (record->anim.last == ANIM_PUSH_START) {
                sound_play_at(SOUND_PUSH_START, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
        }
        player_apply_push(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_HURT:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_CRASH:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_SLIDE:
        player_turn_to_stick(work, motion, record);
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_EGGMAN_ATTACK:
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_PLAY_ANIM:
        break;
    case PLAYER_MODE_PICK_UP:
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_PULL_HEAVY:
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_14:
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_STAND_HOLDING:
        player_turn_to_stick(work, motion, record);
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        player_hold_stand_anim(work, motion, record);
        break;
    case PLAYER_MODE_UNK_17:
        player_approach_touched(work, motion, record);
        player_accel_push(work, motion, record);
        player_apply_push(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_GRAB_PUSHABLE:
        player_approach_touched(work, motion, record);
        player_accel_push(work, motion, record);
        player_apply_push(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_THROW:
        player_turn_to_target(work, motion, record);
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_19:
        if (record->anim.last == ANIM_HOLD_TURN &&
            record->anim.current != ANIM_HOLD_TURN) {
            record->spd.x += record->consts.trot_speed;
        }
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        player_run_anim(work, motion, record);
        break;
    case PLAYER_MODE_UNK_1B:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_FALL_HOLDING:
        player_turn_upright(work, motion, record);
        if (!player_steer_to_target(work, motion, record)) {
            player_accel_air(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_JUMP_HOLDING:
        player_turn_upright(work, motion, record);
        if (!player_steer_to_target(work, motion, record)) {
            player_accel_air(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_BOUNCE_HOLDING:
        player_accel_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_LAUNCH_HOLDING:
        if (record->spd.y >= record->consts.launch_steer_speed) {
            player_accel_launched(work, motion, record);
        } else {
            player_accel_air(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_20:
        player_accel_push(work, motion, record);
        player_apply_push(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        if (player_action_flags(work, record) & ACTION_FLAG_400) {
            fun_8c05bce0();
        }
        break;
    case PLAYER_MODE_AIR_THROW:
        player_turn_upright(work, motion, record);
        player_turn_to_target(work, motion, record);
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_22:
        player_shake_touched(work, motion, record);
        break;
    case PLAYER_MODE_SHAKE_HOLDING:
        TASK_MOTION_OF(record->held)->acc.x =
            TASK_MOTION_OF(record->held)->acc.y =
                TASK_MOTION_OF(record->held)->acc.z =
                    record->anim.frame * EGGMAN_HELD_ACC;
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_PET_CHAO:
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_THROW_AT_TOUCH:
        player_turn_to_touch(work, motion, record);
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_PULL_BURIED:
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_LEDGE_HANG:
        break;
    case PLAYER_MODE_LEDGE_CLIMB:
        break;
    case PLAYER_MODE_UNK_2A:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_ASK_HINT:
        break;
    case PLAYER_MODE_CAUGHT:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_35:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_DROWN:
        /* once the animation is done, rise toward the surface of
         * WORLD_FLAT_THIN water above his first shape's center. the rise is
         * PLAYER_RISE a step at most */
        if ((record->anim.flags & PLAYER_ANIM_DONE) &&
            record->ground.above_y > work->collision->shapes->center.y &&
            (record->ground.above_flags & WORLD_FLAT_THIN)) {
            record->spd.y =
                record->ground.above_y - work->collision->shapes->center.y;
            if (record->spd.y > PLAYER_RISE) {
                record->spd.y = PLAYER_RISE;
            }
        } else {
            record->spd.y = 0.0F;
        }
        player_turn_upright(work, motion, record);
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_MISS:
        if (work->flags & PLAYER_WORK_GROUNDED) {
            record->anim.next = ANIM_MISS;
        }
        player_turn_upright(work, motion, record);
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_FREE_MOVE:
        eggman_free_move(work, record);
        break;
    }
    player_anim_step(&record->anim);
    eggman_place_shapes(work, record);
    if (player_free_move_forced(record->slot) &&
        work->mode != PLAYER_MODE_FREE_MOVE) {
        work->mode = PLAYER_MODE_FREE_MOVE;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_REQUEST | PLAYER_WORK_BALL);
    }
    eggman_check_hold(work, motion, record);
    surface_probe_player(work, record);
    if (record->anim.next != ANIM_WIN && record->anim.next != ANIM_LOSE &&
        work->mode != PLAYER_MODE_EVENT) {
        eggman_set_shapes(work, record);
        collision_register(task);
        collision_set_radius(task,
                             record->consts.height + njScalor(&record->spd));
    }
    player_finish_step(work, motion, record);
}

/* @hack redundant: eggman, the record again, where record alone would do */
/* NOLINTNEXTLINE(readability-function-size) */
static void eggman_choose_mode(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record, struct player *eggman)
{
    if (record && (record->flags & PLAYER_INVINCIBLE)) {
        work->flags |= PLAYER_WORK_UNK_8000;
        work->timer = 0;
    } else if (work->timer == 0) {
        if (player_take_hit(work, record)) {
            work->flags |= PLAYER_WORK_UNK_8000;
            if (work->flags & PLAYER_WORK_HOLDING) {
                player_release_held(work, motion, record);
            }
            work->flags &= ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_REQUEST |
                             PLAYER_WORK_HOLDING | PLAYER_WORK_BALL);
            /* @bug memory: reads record's flags with no test for null, at
             * 0x8c09c11c, though the test at 0x8c09c0b6 lets a null record
             * through to player_take_hit */
            /* @hack redundant: the empty else */
            if (record->flags & (PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER)) {
                record->flags &= ~(PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER);
            } else {
            }
        } else {
            work->flags &= ~PLAYER_WORK_UNK_8000;
        }
    } else {
        work->timer--;
        work->flags &= ~WORK_HURT;
    }
    player_offer_flute(work, record);
    player_offer_hint(work, record);
    player_offer_target_actions(work, motion, record);
    player_offer_held_actions(work, record);
    player_offer_release(work, record);
    eggman_offer_attack(work, record);
    player_offer_call(work, record);
    switch (work->mode) {
    case PLAYER_MODE_STAND:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (eggman_take_action(work, motion, record, eggman)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (eggman_try_jump(work, motion, record)) {
            break;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
            break;
        } else if (record->ball->up.y < PLAYER_SLIDE_STEEP &&
                   njScalor(&record->spd) > record->consts.slide_speed) {
            work->mode = PLAYER_MODE_SLIDE;
            if (player_turn_along_slope(work, motion, record) < 0.0F) {
                record->anim.next = ANIM_SLIDE_BACK;
            } else {
                record->anim.next = ANIM_SLIDE;
            }
        }
        break;
    case PLAYER_MODE_RUN:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (eggman_take_action(work, motion, record, eggman) ||
            player_test_fall(work, motion, record) ||
            eggman_try_jump(work, motion, record)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (player_test_stand(work, motion, record)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (player_last_side_touch(work, record) &&
            player_stick_into_offer(work, record) == PLAYER_WALL_HEAD_ON) {
            player_start_push(work, motion, record);
        }
        if (player_stick_back(work, record) &&
            record->spd.x >= record->consts.trot_speed) {
            work->mode = PLAYER_MODE_BRAKE;
            record->anim.next = ANIM_BRAKE;
            sound_play(SOUND_BRAKE, NULL, 0, 0);
            work->flags &= ~PLAYER_WORK_ON_PATH;
        }
        break;
    case PLAYER_MODE_PATH_RUN:
        work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (eggman_take_action(work, motion, record, eggman) ||
            player_test_fall(work, motion, record) ||
            eggman_try_jump(work, motion, record)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (record->spd.x < record->consts.trot_speed) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (player_test_stand(work, motion, record)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
        }
        break;
    case PLAYER_MODE_JUMP:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (eggman_take_action(work, motion, record, eggman)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
                    record->anim.next = ANIM_LAND_STILL;
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
            work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        } else {
            if (player_count_jump_hold(work, record)) {
                record->spd.y += record->consts.jump_lift * PLAYER_JUMP_LIFT;
            }
            if ((record->anim.next == ANIM_JUMP ||
                 record->anim.next == ANIM_JUMP_RISE) &&
                record->spd.y <= 0.0F) {
                record->anim.next = ANIM_JUMP_FALL;
            }
            if (player_test_ledge(work, motion, record)) {
                player_hang_ledge(work, motion, record);
                work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            }
        }
        break;
    case PLAYER_MODE_BOUNCE:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL_START;
        }
        break;
    case PLAYER_MODE_LAUNCH:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL_START;
        }
        break;
    case PLAYER_MODE_FLING:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
                    record->anim.next = ANIM_LAND_STILL;
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
        }
        break;
    case PLAYER_MODE_FALL:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
                    record->anim.next = ANIM_LAND_STILL;
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
        } else if (player_test_ledge(work, motion, record)) {
            player_hang_ledge(work, motion, record);
            work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        }
        break;
    case PLAYER_MODE_TEETER:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (eggman_take_action(work, motion, record, eggman)) {
            break;
        }
        /* @hack spelling: the test of the work's flags as the else of the
         * test of the animation, where an if of its own would do */
        if (record->anim.next == ANIM_TEETER) {
            break;
        } else if (work->flags & PLAYER_WORK_GROUNDED) {
            work->ang.x = motion->aim_x;
            work->ang.z = motion->aim_z;
            player_stand(work, motion, record);
            break;
        }
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FALL;
        break;
    case PLAYER_MODE_BRAKE:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (eggman_try_jump(work, motion, record)) {
            break;
        }
        if (record->spd.x <= 0.0F && player_stick_back(work, record)) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_TURN;
            player_turn_to_stick(work, motion, record);
            work->ang.y = motion->aim_y;
            break;
        }
        if (record->anim.next != ANIM_BRAKE) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_UNK_0E:
        if (eggman_take_request(work, motion, record, eggman) ||
            eggman_take_action(work, motion, record, eggman) ||
            eggman_try_jump(work, motion, record) ||
            player_test_fall(work, motion, record) ||
            player_test_stand(work, motion, record)) {
            sound_stop_id(PLAYER_SOUND_PUSH);
            break;
        }
        if ((work->collision->flags & COLLISION_PUSHED) &&
            !(work->flags & WORK_HURT) &&
            (!(work->collision->hit->shapes[work->collision->hit_shape].flags &
               SHAPE_UNK_2000) ||
             (work->flags & PLAYER_WORK_HOLDING)) &&
            (work->collision->flags & COLLISION_SIDE)) {
            switch (player_stick_into_offer(work, record)) {
            case PLAYER_WALL_HEAD_ON:
                break;
            case PLAYER_WALL_ASKEW:
                if (!(work->flags & PLAYER_WORK_HOLDING)) {
                    work->mode = PLAYER_MODE_RUN;
                    record->anim.next = ANIM_WALK;
                    sound_stop_id(PLAYER_SOUND_PUSH);
                }
                break;
            default:
                if (work->flags & PLAYER_WORK_HOLDING) {
                    work->mode = PLAYER_MODE_UNK_20;
                    record->anim.next = ANIM_DRAG;
                } else {
                    work->mode = PLAYER_MODE_RUN;
                    record->anim.next = ANIM_WALK;
                }
                sound_stop_id(PLAYER_SOUND_PUSH);
                break;
            }
        } else {
            player_release_held(work, motion, record);
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
            sound_stop_id(PLAYER_SOUND_PUSH);
            break;
        }
        break;
    case PLAYER_MODE_HURT:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (record->anim.next != ANIM_HURT &&
            record->anim.next != ANIM_HURT_LAND) {
            if (player_test_stand(work, motion, record)) {
                player_stand(work, motion, record);
                break;
            } else {
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
                break;
            }
        }
        /* @hack spelling: the test of the timer at or below 0, where a test at
         * 0 would do: the timer is unsigned */
        else if (work->timer <= 0) {
            if (work->flags & PLAYER_WORK_GROUNDED) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
                break;
            } else {
                work->mode = PLAYER_MODE_FALL;
                record->anim.next = ANIM_FALL;
                break;
            }
        }
        if (work->timer <= PLAYER_HURT_STEER_STEPS &&
            player_read_stick(work, record, NULL, NULL)) {
            if (!eggman_try_jump(work, motion, record) &&
                (work->flags & PLAYER_WORK_GROUNDED)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
                break;
            }
        } else if (work->timer != PLAYER_HIT_STEPS &&
                   (work->flags & PLAYER_WORK_GROUNDED)) {
            work->ang.x = motion->aim_x;
            work->ang.z = motion->aim_z;
            record->anim.next = ANIM_HURT_LAND;
            break;
        }
        break;
    case PLAYER_MODE_PLAY_ANIM:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (eggman_take_held_request(work, motion, record, eggman)) {
                break;
            }
        } else if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        break;
    case PLAYER_MODE_CRASH:
        if (record->anim.next == ANIM_CRASH) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (record->anim.next != ANIM_CRASH_END) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_SLIDE:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (eggman_try_jump(work, motion, record)) {
            break;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
            break;
        }
        if (njScalor(&motion->spd) < record->consts.slide_speed ||
            record->ball->up.y > PLAYER_SLIDE_LEVEL) {
            work->mode = PLAYER_MODE_STAND;
            record->mode_steps = 0;
            record->anim.next = ANIM_LAND_STILL;
        }
        break;
    case PLAYER_MODE_CAUGHT:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (pad_stick_pushed[record->slot] ||
            (player_pad_records[record->slot].press &
             PLAYER_STRUGGLE_BUTTONS)) {
            eggman->by_character.eggman.struggle -= PLAYER_STRUGGLE_PUSH;
        }
        if (eggman->by_character.eggman.struggle > 0) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_UNK_2A:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_ASK_HINT:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_UNK_35:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_EGGMAN_ATTACK:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        /* @hack spelling: the break inside the if, where the if alone would
         * do */
        if (record->anim.next != ANIM_ATTACK &&
            record->anim.next != ANIM_RUNNING_ATTACK &&
            record->anim.current != ANIM_ATTACK &&
            record->anim.current != ANIM_RUNNING_ATTACK) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_PICK_UP:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (eggman_take_held_request(work, motion, record, eggman)) {
                break;
            }
        } else if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (eggman_test_pick_up(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_PULL_HEAVY:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (player_test_pull_heavy(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_UNK_17:
        if (eggman_take_action(work, motion, record, eggman)) {
            break;
        }
        if (player_test_push(work, motion, record)) {
            break;
        }
        if (eggman_try_jump(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_GRAB_PUSHABLE:
        if (eggman_take_action(work, motion, record, eggman)) {
            break;
        }
        if (player_test_push(work, motion, record)) {
            break;
        }
        if (eggman_try_jump(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_STAND_HOLDING:
        if (eggman_take_held_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_hold_end(work, motion, record)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (player_test_shake_held(work, record)) {
            work->mode = PLAYER_MODE_SHAKE_HOLDING;
            record->anim.next = ANIM_HOLD_SHAKE;
            voice_play_priority_2(VOICE_SHAKE);
            player_stop(motion, record);
            break;
        }
        if (eggman_take_action(work, motion, record, eggman)) {
            break;
        }
        if (eggman_try_jump(work, motion, record)) {
            break;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = PLAYER_MODE_UNK_19;
            record->anim.next = ANIM_HOLD_WALK;
        }
        break;
    case PLAYER_MODE_UNK_19:
        if (eggman_take_held_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_fall(work, motion, record) ||
            eggman_take_action(work, motion, record, eggman) ||
            eggman_try_jump(work, motion, record) ||
            player_test_hold_end(work, motion, record) ||
            player_test_stand(work, motion, record)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (player_stick_back(work, record) &&
            record->spd.x >= record->consts.trot_speed) {
            work->mode = PLAYER_MODE_UNK_1B;
            record->anim.next = ANIM_HOLD_BRAKE;
            sound_play(SOUND_BRAKE, NULL, 0, 0);
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        break;
    case PLAYER_MODE_UNK_14:
    case PLAYER_MODE_THROW:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (eggman_take_held_request(work, motion, record, eggman)) {
                break;
            }
        } else if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (eggman_test_throw(work, record)) {
            break;
        }
        if (record->anim.next != ANIM_THROW_AT &&
            record->anim.next != ANIM_THROW &&
            record->anim.next != ANIM_SET_DOWN) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_UNK_1B:
        if (eggman_take_held_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (eggman_try_jump(work, motion, record)) {
            break;
        }
        if (record->spd.x <= 0.0F && player_stick_back(work, record)) {
            work->mode = PLAYER_MODE_UNK_19;
            record->anim.next = ANIM_HOLD_TURN;
            player_turn_to_stick(work, motion, record);
            work->ang.y = motion->aim_y;
            break;
        }
        if (record->anim.next != ANIM_HOLD_BRAKE) {
            work->mode = PLAYER_MODE_STAND_HOLDING;
            record->anim.next = ANIM_HOLD_STAND;
        }
        break;
    case PLAYER_MODE_FALL_HOLDING:
        if (eggman_take_held_request(work, motion, record, eggman)) {
            break;
        }
        if (eggman_take_action(work, motion, record, eggman)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_UNK_1B;
                record->anim.next = ANIM_HOLD_BRAKE;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (motion->spd.y > HOLD_LAND_FAST_SPEED) {
                    record->anim.next = ANIM_HOLD_STAND;
                } else {
                    record->anim.next = ANIM_HOLD_LAND_FAST;
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_UNK_19;
                player_run_anim(work, motion, record);
            }
        } else if (player_test_hold_end(work, motion, record)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
        }
        break;
    case PLAYER_MODE_JUMP_HOLDING:
        if (eggman_take_held_request(work, motion, record, eggman)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_UNK_1B;
                record->anim.next = ANIM_HOLD_BRAKE;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (motion->spd.y > HOLD_LAND_FAST_SPEED) {
                    record->anim.next = ANIM_HOLD_STAND;
                } else {
                    record->anim.next = ANIM_HOLD_LAND_FAST;
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_UNK_19;
                player_run_anim(work, motion, record);
            }
            work->flags &= ~PLAYER_WORK_BALL;
        } else {
            if (player_count_jump_hold(work, record)) {
                record->spd.y +=
                    record->consts.jump_lift * PLAYER_JUMP_HOLDING_LIFT;
            }
            if (player_test_hold_end(work, motion, record)) {
                break;
            }
            if (eggman_take_action(work, motion, record, eggman)) {
                break;
            }
            if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
                work->mode = PLAYER_MODE_FALL_HOLDING;
                record->anim.next = ANIM_HOLD_FALL_START;
            }
        }
        break;
    case PLAYER_MODE_BOUNCE_HOLDING:
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL_HOLDING;
            record->anim.next = ANIM_HOLD_FALL_START;
        }
        break;
    case PLAYER_MODE_LAUNCH_HOLDING:
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL_HOLDING;
            record->anim.next = ANIM_HOLD_FALL_START;
        }
        break;
    case PLAYER_MODE_UNK_20:
        if (eggman_take_request(work, motion, record, eggman) ||
            eggman_take_action(work, motion, record, eggman) ||
            eggman_try_jump(work, motion, record) ||
            player_test_fall(work, motion, record)) {
            sound_stop_id(PLAYER_SOUND_PUSH);
            break;
        }
        if ((work->collision->flags & COLLISION_PUSHED) &&
            !(work->flags & WORK_HURT) &&
            (!(work->collision->hit->shapes[work->collision->hit_shape].flags &
               SHAPE_UNK_2000) ||
             (work->flags & PLAYER_WORK_HOLDING)) &&
            (work->collision->flags & COLLISION_SIDE)) {
            switch (player_stick_into_offer(work, record)) {
            case PLAYER_WALL_HEAD_ON:
                player_start_push(work, motion, record);
                break;
            case PLAYER_WALL_NONE:
                if (work->flags & PLAYER_WORK_HOLDING) {
                    work->mode = PLAYER_MODE_UNK_17;
                    record->anim.next = ANIM_GRAB_STAND;
                    sound_stop_id(PLAYER_SOUND_PUSH);
                }
                break;
            default:
                if (work->flags & PLAYER_WORK_HOLDING) {
                    work->mode = PLAYER_MODE_UNK_20;
                    record->anim.next = ANIM_DRAG;
                } else {
                    work->mode = PLAYER_MODE_RUN;
                    record->anim.next = ANIM_WALK;
                }
                sound_stop_id(PLAYER_SOUND_PUSH);
                break;
            }
        } else {
            player_release_held(work, motion, record);
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
            sound_stop_id(PLAYER_SOUND_PUSH);
            break;
        }
        break;
    case PLAYER_MODE_AIR_THROW:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (eggman_take_held_request(work, motion, record, eggman)) {
                break;
            }
        } else if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_UNK_1B;
                record->anim.next = ANIM_HOLD_BRAKE;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (work->flags & PLAYER_WORK_HOLDING) {
                    if (motion->spd.y > HOLD_LAND_FAST_SPEED) {
                        record->anim.next = ANIM_HOLD_STAND;
                    } else {
                        record->anim.next = ANIM_HOLD_LAND_FAST;
                    }
                } else if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
                    record->anim.next = ANIM_LAND_STILL;
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (work->flags & PLAYER_WORK_HOLDING) {
                    work->mode = PLAYER_MODE_UNK_19;
                } else {
                    work->mode = PLAYER_MODE_RUN;
                }
                player_run_anim(work, motion, record);
            }
        } else {
            if (eggman_test_throw(work, record)) {
                break;
            }
            if (record->anim.next != ANIM_AIR_THROW_AT &&
                record->anim.next != ANIM_AIR_THROW) {
                work->mode = PLAYER_MODE_FALL;
                record->anim.next = ANIM_FALL;
            }
        }
        break;
    case PLAYER_MODE_UNK_22:
        if (eggman_take_held_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (eggman_try_jump(work, motion, record)) {
            break;
        }
        if (player_touches_shakable(work, motion, record)) {
            if (player_test_shake_end(work, record)) {
                work->mode = PLAYER_MODE_UNK_17;
                record->anim.next = ANIM_GRAB_STAND;
            }
        } else {
            player_stand(work, motion, record);
            player_release_held(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_SHAKE_HOLDING:
        if (eggman_take_held_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (eggman_try_jump(work, motion, record)) {
            break;
        }
        if (record->held == NULL ||
            record->held->work->id != FALL_KIND_NORMAL ||
            !(work->flags & PLAYER_WORK_HOLDING)) {
            player_stand(work, motion, record);
            player_release_held(work, motion, record);
            break;
        }
        if (player_test_shake_end(work, record)) {
            work->mode = PLAYER_MODE_STAND_HOLDING;
            record->anim.next = ANIM_HOLD_STAND;
            break;
        }
        break;
    case PLAYER_MODE_THROW_AT_TOUCH:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (eggman_take_held_request(work, motion, record, eggman)) {
                break;
            }
        } else if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (eggman_test_throw_at_touch(player_tasks[record->slot], work, motion,
                                       record)) {
            break;
        }
        if (record->anim.next != ANIM_THROW_AT_TOUCH) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_PET_CHAO:
        if (!eggman_take_request(work, motion, record, eggman) &&
            !player_test_fall(work, motion, record) &&
            !eggman_try_jump(work, motion, record) &&
            !player_test_petting(work, motion, record)) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_PULL_BURIED:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (eggman_take_held_request(work, motion, record, eggman)) {
                break;
            }
        } else if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (eggman_test_pull(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_LEDGE_HANG:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_ledge_hang(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_LEDGE_CLIMB:
        if (eggman_take_request(work, motion, record, eggman)) {
            break;
        }
        if (player_test_ledge_hang(work, motion, record)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_ENDING) {
            player_onto_ledge(work, motion, record);
            work->mode = PLAYER_MODE_STAND;
            break;
        }
        break;
    case PLAYER_MODE_EVENT:
        if (work->submode == PLAYER_REQUEST_RELEASE) {
            player_stand(work, motion, record);
            work->flags &=
                ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        }
        break;
    case PLAYER_MODE_FREE_MOVE:
        if (player_pad_records[record->slot].press & PDD_DGT_TA) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
        }
        break;
    }
}

static void eggman_destroy(struct task *task)
{
    struct task_work *work = task->work;
    struct player *eggman = task->extra.p;
    /* @hack spelling: the record through the address of its first field,
     * where eggman itself would do */
    struct player *record = (struct player *)&eggman->slot;

    player_unregister(record->slot);
    sound_stop_key(work);
    njReleaseTexture(eggman->by_character.eggman.texlist);
    syFree(eggman->by_character.eggman.texlist);
    eggman->by_character.eggman.texlist = NULL;
    life_icon_release(record->character);
    model_file_free(eggman->by_character.eggman.models);
    eggman->by_character.eggman.models = NULL;
    motion_file_free(eggman->by_character.eggman.motions);
    eggman->by_character.eggman.motions = NULL;
}

static void eggman_place_shapes(struct task_work *work, struct player *record)
{
    struct collision_shape *shape = work->collision->shapes;
    NJS_VECTOR up;

    up.x = up.z = 0.0F;
    up.y = STANDING_CENTER;
    turn_to_world(work, &up);
    shape->center = up;
    shape->center.x += work->pos.x;
    shape->center.y += work->pos.y;
    shape->center.z += work->pos.z;
    shape++;
    up.x = up.z = 0.0F;
    up.y = record->consts.center_height;
    turn_to_world(work, &up);
    shape->center = up;
    shape->center.x += work->pos.x;
    shape->center.y += work->pos.y;
    shape->center.z += work->pos.z;
    shape++;
    up.x = up.z = 0.0F;
    up.y = record->consts.target_height;
    turn_to_world(work, &up);
    shape->center = up;
    shape->center.x += work->pos.x;
    shape->center.y += work->pos.y;
    shape->center.z += work->pos.z;
}

/* his draw's motion callback, which Ninja calls at each node it draws. at each
 * EGGMAN_AT_ node, hand and foot, it keeps the node's place and one or two of
 * its axes. it takes each to the world by eggman_view_to_world, into the
 * record. it turns EGGMAN_LEANS and the EGGMAN_HOLDS_ nodes */
static void eggman_motion_callback(NJS_CNK_OBJECT *object)
{
    struct player *record = eggman_drawn;
    struct task_work *work;
    NJS_POINT3 place;
    NJS_VECTOR axis;

    place.x = place.y = place.z = 0.0F;
    if (object == model_table[EGGMAN_AT_230].object) {
        axis.x = 1.0F;
        axis.y = axis.z = 0.0F;
        njCalcPoint(NULL, &place, &place);
        njCalcPoint(&eggman_view_to_world, &place, &record->unk_230);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&eggman_view_to_world, &axis, &record->unk_2cc);
    } else if (object == model_table[EGGMAN_LEANS].object) {
        work = player_work[record->slot];
        if (work->mode == PLAYER_MODE_RUN) {
            njRotateX(NULL, record->lean);
        }
    } else if (object == model_table[EGGMAN_HOLDS_A].object) {
        work = player_work[record->slot];
        if (work->flags & PLAYER_WORK_HOLDING) {
            njRotateX(NULL, TASK_MOTION_OF(record->held)->hold_turn);
        }
    } else if (object == model_table[EGGMAN_HOLDS_B].object) {
        work = player_work[record->slot];
        if (work->flags & PLAYER_WORK_HOLDING) {
            njRotateX(NULL, -TASK_MOTION_OF(record->held)->hold_turn);
        }
    } else if (object == model_table[EGGMAN_AT_218].object) {
        axis.x = 1.0F;
        axis.y = axis.z = 0.0F;
        njCalcPoint(NULL, &place, &place);
        njCalcPoint(&eggman_view_to_world, &place, &record->unk_218);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&eggman_view_to_world, &axis, &record->unk_2b4);
    } else if (object == model_table[EGGMAN_AT_224].object) {
        axis.x = 1.0F;
        axis.y = axis.z = 0.0F;
        njCalcPoint(NULL, &place, &place);
        njCalcPoint(&eggman_view_to_world, &place, &record->unk_224);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&eggman_view_to_world, &axis, &record->unk_2c0);
    } else if (object == model_table[EGGMAN_HAND_0].object) {
        njCalcPoint(NULL, &place, &place);
        njCalcPoint(&eggman_view_to_world, &place, &record->unk_1e8);
        axis.z = 1.0F;
        axis.x = axis.y = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&eggman_view_to_world, &axis, &record->unk_26c);
        axis.y = -1.0F;
        axis.z = axis.x = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&eggman_view_to_world, &axis, &record->unk_284);
    } else if (object == model_table[EGGMAN_HAND_1].object) {
        njCalcPoint(NULL, &place, &place);
        njCalcPoint(&eggman_view_to_world, &place, &record->unk_1f4);
        axis.z = -1.0F;
        axis.x = axis.y = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&eggman_view_to_world, &axis, &record->unk_278);
        axis.y = -1.0F;
        axis.z = axis.x = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&eggman_view_to_world, &axis, &record->unk_290);
    } else if (object == model_table[EGGMAN_FOOT_0].object) {
        axis.y = -1.0F;
        axis.x = axis.z = 0.0F;
        njCalcPoint(NULL, &place, &place);
        njCalcPoint(&eggman_view_to_world, &place, &record->unk_200);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&eggman_view_to_world, &axis, &record->unk_29c);
    } else if (object == model_table[EGGMAN_FOOT_1].object) {
        axis.y = -1.0F;
        axis.x = axis.z = 0.0F;
        njCalcPoint(NULL, &place, &place);
        njCalcPoint(&eggman_view_to_world, &place, &record->unk_20c);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&eggman_view_to_world, &axis, &record->unk_2a8);
    }
}

/* his task's draw. as his model draws, eggman_motion_callback keeps the
 * places of his nodes. his shadow and the held task then go to those places.
 * where PLAYER_TIMER_HIDDEN leaves the model out, they go where the last draw
 * left them */
static void eggman_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct player *eggman = task->extra.p;
    /* @hack spelling: the record through the address of its first field,
     * where eggman itself would do */
    struct player *record = (struct player *)&eggman->slot;
    int anim;
    int light;

    draw_attr_save();
    draw_attr_clear(0, NJD_FST_IS);
    light = light_selected[record->slot];
    if (light != LIGHT_UNK_1 && light != LIGHT_VOLUME) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    light_apply_player(record->slot);
    eggman_drawn = record;
    njGetMatrix(&eggman_view_to_world);
    njInvertMatrix(&eggman_view_to_world);
    njCnkSetMotionCallback(eggman_motion_callback);
    anim = record->anim.current;
    if (!(work->timer & PLAYER_TIMER_HIDDEN)) {
        njSetTexture(eggman->by_character.eggman.texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScaleEx(&work->scl);
        if (work->mode == PLAYER_MODE_EVENT && record->event != NULL) {
            player_hide_event_models(record->slot);
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions->model].object,
                record->event->motion, mini_event_frame);
            player_draw_event_parts(record->slot);
        } else if (record->anim.mode != PLAYER_ANIM_BLEND) {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[anim].model].object,
                motion_table[record->anim
                                 .actions[record->anim.actions[anim].motion]
                                 .motion]
                    .motion,
                record->anim.frame);
        } else {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[anim].model].object,
                record->anim.blend, record->anim.frame);
        }
        njPopMatrixEx();
    }
    njCnkSetMotionCallback(NULL);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    draw_attr_restore();
    light_apply(light_after_draw);
    eggman_draw_shadow(work, record, anim);
    eggman_place_held(work, record);
}

/* draw his shadow as modifier volumes, where anim's model is EGGMAN_MODEL.
 * EGGMAN_SHADOW goes at unk_218, and shadow_model at each hand and foot.
 * each turns to his heading */
static void eggman_draw_shadow(struct task_work *work, struct player *record,
                               int anim)
{
    int model = record->anim.actions[anim].model;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    if (model == EGGMAN_MODEL) {
        njTranslateEx(&record->unk_218);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[EGGMAN_SHADOW].object);
        matrix_load_parent();
        njTranslateEx(&record->unk_1e8);
        njTranslate(NULL, 0.0F, -HAND_SHADOW_DROP, 0.0F);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, HAND_SHADOW_X, 1.0F, HAND_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_1f4);
        njTranslate(NULL, 0.0F, -HAND_SHADOW_DROP, 0.0F);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, HAND_SHADOW_X, 1.0F, HAND_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_200);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, FOOT_SHADOW_X, 1.0F, FOOT_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_20c);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, FOOT_SHADOW_X, 1.0F, FOOT_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* PLAYER_MODE_FREE_MOVE's step, which moves his pos through anything. the
 * stick moves him up or down while X is held, and otherwise across his
 * camera's view. a press of B places him again by stage_place_player */
static void eggman_free_move(struct task_work *work, struct player *record)
{
    Float x;
    Float y;
    Float sign;
    Angle ang;

    if (player_pad_records[record->slot].press & PDD_DGT_TB) {
        stage_place_player(record->slot);
    }
    if (player_pad_records[record->slot].on & PDD_DGT_TX) {
        /* @hack type: the axes through int, where their own Sint16 would
         * do */
        x = (Float)(int)player_pad_records[record->slot].x1;
        y = (Float)(int)player_pad_records[record->slot].y1;
        if (x > PAD_DEAD_ZONE || x < -PAD_DEAD_ZONE || y > PAD_DEAD_ZONE ||
            y < -PAD_DEAD_ZONE) {
            if (y != 0.0F) {
                sign = y / njSqrt(y * y);
            } else {
                sign = 0.0F;
            }
            work->pos.y -= sign * FREE_MOVE_STEP;
        }
    } else {
        /* @hack type: the axes through int, where their own Sint16 would
         * do */
        x = (Float)(int)player_pad_records[record->slot].x1;
        y = (Float)(int)player_pad_records[record->slot].y1;
        /* @hack spelling: the move in an else after the return, where the
         * statements alone would do */
        if (x <= PAD_DEAD_ZONE && x >= -PAD_DEAD_ZONE && y <= PAD_DEAD_ZONE &&
            y >= -PAD_DEAD_ZONE) {
            return;
        } else {
            ang = (Angle)(atan2f(y, x) * ANGLE_PER_RADIAN) -
                  camera_views[record->slot]->ang.y;
            work->pos.x += njCos(ang) * FREE_MOVE_STEP;
            work->pos.z += njSin(ang) * FREE_MOVE_STEP;
        }
    }
}

/* at a press of jump, jump and return TRUE, and return FALSE otherwise. a
 * held task that player_action_flags gives ACTION_FLAG_200 goes up with him,
 * in PLAYER_MODE_JUMP_HOLDING. he lets go of any other, and jumps in
 * PLAYER_MODE_JUMP. the speed of the task he stands on then becomes his
 * motion's acc */
static Bool eggman_try_jump(struct task_work *work,
                            struct player_motion *motion, struct player *record)
{
    /* @hack spelling: flags, a local for player_action_flags's result set
     * before its one use */
    int flags;
    struct task_force *force;

    if (player_jump_pressed(work, record)) {
        if (work->flags & PLAYER_WORK_HOLDING) {
            flags = player_action_flags(work, record);
            /* @hack spelling: the release in an else after the return, where
             * the statement alone would do */
            if (flags & ACTION_FLAG_200) {
                work->mode = PLAYER_MODE_JUMP_HOLDING;
                record->spd.y = record->consts.jump_speed;
                record->anim.next = ANIM_HOLD_STAND;
                work->flags &= ~PLAYER_WORK_ON_SHAPE;
                record->jump_steps = 0;
                sound_play(SOUND_JUMP, NULL, 0, 0);
                voice_play_priority_2(VOICE_JUMP);
                return TRUE;
            } else {
                player_release_held(work, motion, record);
            }
        }
        work->mode = PLAYER_MODE_JUMP;
        record->spd.y = record->consts.jump_speed;
        if (record->ground_task) {
            force = record->ground_task->force.p;
            if (force) {
                player_set_acc(record->slot, force->speed.x, force->speed.y,
                               force->speed.z);
            }
        }
        record->anim.next = ANIM_JUMP;
        work->flags &= ~PLAYER_WORK_ON_SHAPE;
        record->jump_steps = 0;
        sound_play(SOUND_JUMP, NULL, 0, 0);
        voice_play_priority_2(VOICE_JUMP);
        return TRUE;
    }
    return FALSE;
}

/* answer the request that PLAYER_WORK_REQUEST flags in the work's submode, as
 * Eggman holding nothing, and return TRUE. it returns FALSE where no request
 * waits, or for one he has no answer to. PLAYER_REQUEST_JOIN_PATH returns FALSE
 * too. it sets PLAYER_WORK_ON_PATH where he is in PLAYER_MODE_RUN, or runs
 * above running_speed. PLAYER_WORK_REQUEST stays set. PLAYER_REQUEST_HAND makes
 * handed the held task, and handed must not be null, unchecked */
/* @hack redundant: eggman, the record again, where record alone would do */
static Bool eggman_take_request(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record, struct player *eggman)
{
    if (!(work->flags & PLAYER_WORK_REQUEST)) {
        return FALSE;
    }
    switch (work->submode) {
    case PLAYER_REQUEST_BOUNCE:
        work->mode = PLAYER_MODE_BOUNCE;
        record->anim.next = ANIM_LAUNCH;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_LAUNCH:
        work->mode = PLAYER_MODE_LAUNCH;
        record->anim.next = ANIM_LAUNCH;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_FREE_FALL:
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FREE_FALL;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_FLING:
        work->mode = PLAYER_MODE_FLING;
        record->anim.next = ANIM_LAUNCH;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_UPDRAFT:
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_UNK_23;
        if (work->flags & PLAYER_WORK_BALL) {
            work->flags &= ~PLAYER_WORK_BALL;
        }
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_RUN_PATH:
        work->mode = PLAYER_MODE_PATH_RUN;
        record->anim.next = ANIM_RUN;
        record->path_distance = 0.0F;
        if (work->flags & PLAYER_WORK_BALL) {
            work->flags &= ~PLAYER_WORK_BALL;
        }
        work->flags |= PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_ANIM:
        work->mode = PLAYER_MODE_PLAY_ANIM;
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_RELEASE:
        player_stand(work, motion, record);
        if (work->flags & PLAYER_WORK_BALL) {
            work->flags &= ~PLAYER_WORK_BALL;
        }
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_JOIN_PATH:
        if (work->mode == PLAYER_MODE_RUN ||
            record->spd.x > record->consts.running_speed) {
            work->flags |= PLAYER_WORK_ON_PATH;
        }
        return FALSE;
    case PLAYER_REQUEST_CAPTURE:
        work->mode = PLAYER_MODE_CAUGHT;
        record->anim.next = ANIM_CAUGHT;
        eggman->by_character.eggman.struggle = PLAYER_STRUGGLE;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_KNOCK_BACK:
        work->mode = PLAYER_MODE_CRASH;
        record->anim.next = ANIM_CRASH;
        sound_play_at(SOUND_CRY, NULL, 0, SOUND_LOUDEST, &work->pos);
        crash_stars_burst(record->slot);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_SCARE:
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = record->request_arg;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_BREATHE:
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = ANIM_BREATHE;
        record->underwater_steps = 0;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_HAND:
        work->mode = PLAYER_MODE_STAND_HOLDING;
        record->anim.next = ANIM_HOLD_STAND;
        record->held = record->handed;
        record->handed = NULL;
        /* @bug memory: writes the handed task's work with no test for null,
         * at 0x8c09ef06, and eggman_take_held_request does too, at
         * 0x8c09f1e4. eggman_test_throw and eggman_test_throw_at_touch test
         * a held task's work for null, at 0x8c09f812 and 0x8c09fa9a */
        record->held->work->flags |= WORK_HELD_BY_PLAYER;
        work->flags |= PLAYER_WORK_HOLDING;
        work->flags &= ~PLAYER_WORK_UNK_8;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_DROWN:
        work->mode = PLAYER_MODE_DROWN;
        record->anim.next = ANIM_DROWN;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_RESET:
        work->flags &= ~PLAYER_RESET_WORK_FLAGS;
        record->flags &= ~PLAYER_RESET_FLAGS;
        player_stand(work, motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_MISS:
        work->mode = PLAYER_MODE_MISS;
        record->anim.next = ANIM_MISS;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

/* a throw lets go of the task from frame THROW_FRAME on the ground, and
 * AIR_THROW_FRAME in the air. ANIM_THROW_AT_TOUCH lets go from
 * THROW_AT_TOUCH_FRAME. ANIM_SET_DOWN sets the task down at any frame up to
 * SET_DOWN_FRAME */
#define THROW_FRAME 3.0F
#define AIR_THROW_FRAME 2.0F
#define THROW_AT_TOUCH_FRAME 7.0F
#define SET_DOWN_FRAME 5.0F
/* the frame of PLAYER_MODE_PICK_UP's animation from which he picks up */
#define PICK_UP_FRAME 5.0F
/* the speed a throw at a target gives the task. a throw ahead gives it
 * (THROW_AHEAD_X, THROW_AHEAD_Y, 0) in his own axes */
#define THROW_SPEED 3.0F
#define THROW_AHEAD_X 2.0F
#define THROW_AHEAD_Y 0.8F
/* the chance that a pass of ANIM_PULL's loop goes on to ANIM_PULL_LIFT where he
 * takes no stick. where he takes it, the pull always goes on */
#define PULL_LIFT_CHANCE 0.1F

/* how far a held task lies from his hands. through a throw it lies HOLD_REACH
 * along his hand's z axis, unk_26c. it then lies HOLD_OUT along the hand's down
 * axis, unk_284. otherwise it lies HOLD_OUT down from between his hands */
#define HOLD_REACH 2.5F
#define HOLD_OUT 1.2F

/* the radius eggman_set_shapes gives his first shape each step, in place of
 * the consts.radius his create gives it */
#define BODY_RADIUS 2.7F
/* the attack and guard of his body's shapes under PLAYER_WORK_BALL, unless
 * PLAYER_INVINCIBLE gives them INVINCIBLE_LEVEL. they are 0 otherwise */
#define BALL_LEVEL 1

static void eggman_attack(struct task_work *work, struct player_motion *motion,
                          struct player *record);

Bool eggman_take_held_request(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *eggman)
{
    if (!(work->flags & PLAYER_WORK_REQUEST)) {
        return FALSE;
    }
    switch (work->submode) {
    case PLAYER_REQUEST_BOUNCE:
        work->mode = PLAYER_MODE_BOUNCE_HOLDING;
        record->anim.next = ANIM_HOLD_JUMP;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_LAUNCH:
        work->mode = PLAYER_MODE_LAUNCH_HOLDING;
        record->anim.next = ANIM_HOLD_JUMP;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_UPDRAFT:
        work->mode = PLAYER_MODE_FALL_HOLDING;
        record->anim.next = ANIM_HOLD_FALL;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_RUN_PATH:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_PATH_RUN;
        record->anim.next = ANIM_RUN;
        record->path_distance = 0.0F;
        work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
        work->flags |= PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_ANIM:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_PLAY_ANIM;
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        break;
    case PLAYER_REQUEST_RELEASE:
        work->mode = PLAYER_MODE_STAND_HOLDING;
        record->anim.next = ANIM_HOLD_STAND;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_JOIN_PATH:
        if (work->mode == PLAYER_MODE_UNK_19 ||
            record->spd.x > record->consts.running_speed) {
            work->flags |= PLAYER_WORK_ON_PATH;
        }
        return FALSE;
    case PLAYER_REQUEST_CAPTURE:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_CAUGHT;
        record->anim.next = ANIM_CAUGHT;
        /* @hack spelling: struggle written through eggman, where record
         * would do, since the two are one record */
        eggman->by_character.eggman.struggle = PLAYER_STRUGGLE;
        player_stop(motion, record);
        /* @hack redundant: the test of PLAYER_WORK_BALL before it is
         * cleared, where the clearing alone would do */
        if (work->flags & PLAYER_WORK_BALL) {
            work->flags &= ~PLAYER_WORK_BALL;
        }
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_KNOCK_BACK:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_CRASH;
        record->anim.next = ANIM_CRASH;
        sound_play_at(SOUND_CRY, NULL, 0, SOUND_LOUDEST, &work->pos);
        crash_stars_burst(record->slot);
        work->flags &=
            ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        break;
    case PLAYER_REQUEST_HAND:
        work->mode = PLAYER_MODE_STAND_HOLDING;
        record->anim.next = ANIM_HOLD_STAND;
        record->held = record->handed;
        record->handed = NULL;
        record->held->work->flags |= WORK_HELD_BY_PLAYER;
        work->flags |= PLAYER_WORK_HOLDING;
        /* @hack spelling: PLAYER_WORK_UNK_8 and PLAYER_WORK_ON_PATH cleared
         * by a statement each, where one would do */
        work->flags &= ~PLAYER_WORK_UNK_8;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_SCARE:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = record->request_arg;
        work->flags &=
            ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        break;
    case PLAYER_REQUEST_DROWN:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_DROWN;
        record->anim.next = ANIM_DROWN;
        work->flags &=
            ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        break;
    case PLAYER_REQUEST_RESET:
        player_release_held(work, motion, record);
        work->flags &= ~PLAYER_RESET_WORK_FLAGS;
        record->flags &= ~PLAYER_RESET_FLAGS;
        player_stand(work, motion, record);
        work->flags &=
            ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        break;
    case PLAYER_REQUEST_MISS:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_MISS;
        record->anim.next = ANIM_MISS;
        work->flags &=
            ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

Bool eggman_take_action(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *eggman)
{
    /* @hack spelling: listed set as it is declared, ahead of the test of the
     * press, where a statement before the loop would do */
    Sint8 *listed = record->actions;
    int count = record->action_count;
    int action;
    int i;

    if (!pad_action_pressed[record->slot] || count == 0) {
        return FALSE;
    }
    action = record->action;
    for (i = 0; i < count; i++, listed++) {
        if (action == *listed) {
            break;
        }
    }
    /* @hack spelling: count == i, where i == count would do */
    if (count == i) {
        action = record->actions[0];
    }
    record->action_taken = action;
    switch (action) {
    case PLAYER_MODE_GRAB_PUSHABLE:
        player_hold(work, record);
        return TRUE;
    case PLAYER_MODE_UNK_17:
        player_grab_shakable(work, record);
        return TRUE;
    case PLAYER_MODE_PICK_UP:
        player_pick_up(work, record);
        return TRUE;
    case PLAYER_MODE_THROW:
        player_throw_held(work, record);
        return TRUE;
    case PLAYER_MODE_AIR_THROW:
        player_air_throw_held(work, record);
        return TRUE;
    case PLAYER_MODE_THROW_AT_TOUCH:
        player_throw_at_touch(work, record);
        return TRUE;
    case PLAYER_MODE_PET_CHAO:
        player_pet_chao(work, record);
        return TRUE;
    case PLAYER_MODE_UNK_14:
        player_set_down_held(work, motion, record);
        return TRUE;
    case PLAYER_MODE_LET_GO:
        player_release_and_stand(work, motion, record);
        return TRUE;
    case PLAYER_MODE_UNK_2A:
        player_call(work, record);
        return TRUE;
    case PLAYER_MODE_PULL_HEAVY:
        player_pull_heavy(work, record);
        return TRUE;
    case PLAYER_MODE_ASK_HINT:
        player_ask_hint(work, record);
        return TRUE;
    case PLAYER_MODE_PULL_BURIED:
        player_pull_buried(work, record);
        return TRUE;
    case PLAYER_MODE_EGGMAN_ATTACK:
        eggman_attack(work, motion, record);
        return TRUE;
    }
    return FALSE;
}

Bool eggman_offer_attack(struct task_work *work, struct player *record)
{
    if (work->flags & PLAYER_WORK_HOLDING) {
        return FALSE;
    }
    if (!(work->flags & PLAYER_WORK_GROUNDED)) {
        return FALSE;
    }
    if (work->mode != PLAYER_MODE_STAND && work->mode != PLAYER_MODE_RUN) {
        return FALSE;
    }
    OFFER(record, PLAYER_MODE_EGGMAN_ATTACK);
    return TRUE;
}

static void eggman_attack(struct task_work *work, struct player_motion *motion,
                          struct player *record)
{
    work->mode = PLAYER_MODE_EGGMAN_ATTACK;
    if (record->spd.x < record->consts.trot_speed) {
        record->anim.next = ANIM_ATTACK;
    } else {
        record->anim.next = ANIM_RUNNING_ATTACK;
    }
    record->anim.after = ANIM_STAND;
    work->flags |= PLAYER_WORK_ATTACK;
    player_stop(motion, record);
    player_sound_after(SOUND_ATTACK, ATTACK_SOUND_WAIT);
    voice_play_priority_2(VOICE_ATTACK);
}

Bool eggman_test_pick_up(struct task_work *work, struct player_motion *motion,
                         struct player *record)
{
    int flags;

    player_approach_touched(work, motion, record);
    if (record->anim.frame < PICK_UP_FRAME) {
        return FALSE;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((flags = player_action_flags(work, record)) != 0 &&
        player_face_offer(work, motion, record) == PLAYER_WALL_HEAD_ON) {
        if (flags & ACTION_FLAG_200) {
            work->mode = PLAYER_MODE_STAND_HOLDING;
            sound_play(PLAYER_SOUND_PICK_UP, NULL, 0, 0);
            record->held = record->hold_offer;
            record->held->work->flags |= WORK_HELD_BY_PLAYER;
            work->flags |= PLAYER_WORK_HOLDING;
            work->flags &= ~PLAYER_WORK_UNK_8;
            voice_play_priority_2(VOICE_PICK_UP);
            return TRUE;
        }
    } else {
        /* @hack redundant: an empty else */
    }
    player_stand(work, motion, record);
    return FALSE;
}

Bool eggman_test_pull(struct task_work *work, struct player_motion *motion,
                      struct player *record)
{
    int flags;

    player_approach_touched(work, motion, record);
    if (record->anim.current == ANIM_PULL &&
        record->anim.frame < record->anim.actions[record->anim.current].rate &&
        (player_read_stick(work, record, NULL, NULL) ||
         (Float)rand() * RAND_SCALE < PULL_LIFT_CHANCE)) {
        record->anim.next = ANIM_PULL_LIFT;
        record->anim.after = ANIM_HOLD_STAND;
        return FALSE;
    }
    if (record->anim.current == ANIM_PULL_LIFT) {
        if (!(work->flags & PLAYER_WORK_HOLDING)) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((flags = player_action_flags(work, record)) != 0 &&
                player_face_offer(work, motion, record) ==
                    PLAYER_WALL_HEAD_ON) {
                if (flags & ACTION_FLAG_1000) {
                    voice_play_priority_2(VOICE_PICK_UP);
                    sound_play(PLAYER_SOUND_PICK_UP, NULL, 0, 0);
                    record->held = record->hold_offer;
                    record->held->work->flags |=
                        WORK_HELD_BY_PLAYER | FALL_UNK_200;
                    record->held->work->id = FALL_KIND_NORMAL;
                    work->flags |= PLAYER_WORK_HOLDING;
                    work->flags &= ~PLAYER_WORK_UNK_8;
                    return FALSE;
                }
            }
        } else if (player_action_flags(work, record)) {
            return FALSE;
        } else {
            /* @hack redundant: an empty else */
        }
        player_stand(work, motion, record);
    } else if (record->anim.current == ANIM_HOLD_STAND) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((flags = player_action_flags(work, record)) != 0) {
            if (flags & ACTION_FLAG_200) {
                work->mode = PLAYER_MODE_STAND_HOLDING;
                return TRUE;
            }
        }
    }
    return FALSE;
}

Bool eggman_test_throw(struct task_work *work, struct player *record)
{
    struct near_target *entry;
    struct task_work *held;
    struct task_work *target;
    struct collision_shape *shape;
    Float nearest;
    NJS_VECTOR speed;

    if (record->slot == 0) {
        entry = near_targets;
    } else {
        entry = near_targets_player1;
    }
    if (record->held == NULL) {
        if (!(work->flags & PLAYER_WORK_HOLDING)) {
            return FALSE;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        return TRUE;
    }
    if (((record->anim.next == ANIM_THROW_AT &&
          record->anim.frame >= THROW_FRAME) ||
         (record->anim.next == ANIM_AIR_THROW_AT &&
          record->anim.frame >= AIR_THROW_FRAME)) &&
        entry->work != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((held = record->held->work) != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            /* @bug logic: throws at the list's first target whatever its
             * kind and id, where no nearer item or enemy follows it. the
             * first is taken untested at 0x8c09f81e, though the loop's tests
             * at 0x8c09f832 and 0x8c09f840 pass over such a target */
            target = entry->work;
            nearest = entry->distance;
            for (; entry->work != NULL; entry++) {
                if ((entry->work->collision->kind == COLLISION_ITEM ||
                     entry->work->collision->kind == COLLISION_ENEMY) &&
                    entry->work->id != FALL_KIND_NORMAL &&
                    entry->distance < nearest) {
                    nearest = entry->distance;
                    target = entry->work;
                }
            }
            shape = target->collision->shapes;
            speed.x = (target->pos.x + shape->center.x) - work->pos.x;
            speed.y = (target->pos.y + shape->center.y) - work->pos.y;
            speed.z = (target->pos.z + shape->center.z) - work->pos.z;
            speed.x = njSqrt((speed.x * speed.x) + (speed.z * speed.z));
            speed.z = 0.0F;
            njUnitVector(&speed);
            speed.x *= THROW_SPEED;
            speed.y *= THROW_SPEED;
            turn_to_world(work, &speed);
            TASK_MOTION_OF(record->held)->spd = speed;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        voice_play_priority_2(VOICE_THROW);
        return TRUE;
    } else if ((record->anim.next == ANIM_THROW &&
                record->anim.frame >= THROW_FRAME) ||
               (record->anim.next == ANIM_AIR_THROW &&
                record->anim.frame >= AIR_THROW_FRAME)) {
        held = record->held->work;
        if (held != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            speed.x = THROW_AHEAD_X;
            speed.y = THROW_AHEAD_Y;
            speed.z = 0.0F;
            turn_to_world(work, &speed);
            TASK_MOTION_OF(record->held)->spd = speed;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        voice_play_priority_2(VOICE_THROW);
        return TRUE;
    } else if (record->anim.next == ANIM_SET_DOWN &&
               record->anim.frame <= SET_DOWN_FRAME) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((held = record->held->work) != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            player_set_down(work, held, record);
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        voice_play_priority_2(VOICE_SET_DOWN);
        return TRUE;
    }
    return FALSE;
}

Bool eggman_test_throw_at_touch(struct task *task, struct task_work *work,
                                struct player_motion *motion,
                                struct player *record)
{
    struct task *touched;
    struct task_work *held;
    struct task_work *target;
    struct collision_shape *shape;
    NJS_VECTOR speed;

    if (record->held == NULL) {
        if (!(work->flags & PLAYER_WORK_HOLDING)) {
            return FALSE;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        return TRUE;
    }
    /* @hack spelling: the call inside the test, and the else that tests the
     * animation again, where the call after the animation's test, with a
     * plain else, would do */
    if (record->anim.next == ANIM_THROW_AT_TOUCH &&
        record->anim.frame >= THROW_AT_TOUCH_FRAME &&
        (touched = collision_first_with_id(task, SHAPE_ID_FACED)) != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((held = record->held->work) != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            target = touched->work;
            shape = target->collision->shapes;
            speed.x = (target->pos.x + shape->center.x) - held->pos.x;
            speed.y = (target->pos.y + shape->center.y) - held->pos.y;
            speed.z = (target->pos.z + shape->center.z) - held->pos.z;
            speed.x = njSqrt((speed.x * speed.x) + (speed.z * speed.z));
            speed.z = 0.0F;
            njUnitVector(&speed);
            speed.x *= THROW_SPEED;
            speed.y *= THROW_SPEED;
            turn_to_world(work, &speed);
            TASK_MOTION_OF(record->held)->spd = speed;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        voice_play_priority_2(VOICE_THROW);
        return TRUE;
    } else if (record->anim.next == ANIM_THROW_AT_TOUCH &&
               record->anim.frame >= THROW_AT_TOUCH_FRAME) {
        held = record->held->work;
        if (held != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            speed.x = THROW_AHEAD_X;
            speed.y = THROW_AHEAD_Y;
            speed.z = 0.0F;
            turn_to_world(work, &speed);
            TASK_MOTION_OF(record->held)->spd = speed;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        voice_play_priority_2(VOICE_THROW);
        return TRUE;
    }
    return FALSE;
}

void eggman_check_hold(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    struct task_work *held;
    struct collision_work *collision;
    struct collision_work *hit;

    if (record->held == NULL) {
        return;
    }
    /* @hack spelling: held, a local for the held task's work, where the work
     * read at its one use would do */
    held = record->held->work;
    /* @hack spelling: an else after the return, where an if of its own would
     * do */
    /* @hack spelling: the assignments inside the else's test, where
     * statements of their own would do */
    if (player_action_flags(work, record) & ACTION_FLAG_200) {
        return;
    } else if ((hit = (collision = work->collision)->hit) != NULL &&
               held == hit->owner->work) {
        return;
    }
    player_release_held(work, motion, record);
    /* @hack redundant: a read of hit that nothing uses */
    hit = collision->hit;
    if (record->held == NULL) {
        if (work->flags & PLAYER_WORK_GROUNDED) {
            player_stand(work, motion, record);
        } else {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
        }
    }
}

void eggman_place_held(struct task_work *work, struct player *record)
{
    struct task_work *held;
    NJS_POINT3 pos;
    NJS_VECTOR out;

    if (record->held == NULL) {
        return;
    }
    if (!(player_action_flags(work, record) & ACTION_FLAG_200)) {
        return;
    }
    /* @hack spelling: held, a local for the held task's work, where the work
     * read at its one use would do */
    held = record->held->work;
    if (record->anim.next == ANIM_THROW_AT || record->anim.next == ANIM_THROW ||
        record->anim.next == ANIM_AIR_THROW_AT ||
        record->anim.next == ANIM_AIR_THROW ||
        record->anim.next == ANIM_THROW_AT_TOUCH) {
        pos.x = record->unk_1e8.x + (record->unk_26c.x * HOLD_REACH);
        pos.y = record->unk_1e8.y + (record->unk_26c.y * HOLD_REACH);
        pos.z = record->unk_1e8.z + (record->unk_26c.z * HOLD_REACH);
        pos.x += record->unk_284.x * HOLD_OUT;
        pos.y += record->unk_284.y * HOLD_OUT;
        pos.z += record->unk_284.z * HOLD_OUT;
    } else {
        pos.x = (record->unk_1e8.x + record->unk_1f4.x) * 0.5F;
        pos.y = (record->unk_1e8.y + record->unk_1f4.y) * 0.5F;
        pos.z = (record->unk_1e8.z + record->unk_1f4.z) * 0.5F;
        out.x = record->unk_284.x + record->unk_290.x;
        out.y = record->unk_284.y + record->unk_290.y;
        out.z = record->unk_284.z + record->unk_290.z;
        if (njScalor(&out) == 0.0F) {
            /* @hack spelling: one assignment to the three, where a statement
             * for each would do */
            out.x = out.y = out.z = 0.0F;
        } else {
            njUnitVector(&out);
        }
        pos.x += out.x * HOLD_OUT;
        pos.y += out.y * HOLD_OUT;
        pos.z += out.z * HOLD_OUT;
    }
    held->pos = pos;
}

void eggman_set_shapes(struct task_work *work, struct player *record)
{
    int i;
    int attack;
    int guard;
    /* @hack spelling: levels, a local for attack's bits, where the
     * expression at each use would do */
    int levels;
    struct collision_shape *shapes;
    /* @hack redundant: first and second, volatile copies of work's
     * collision record that nothing reads */
    /* NOLINTBEGIN(clang-diagnostic-unused-but-set-variable) */
    struct collision_work *volatile first;
    struct collision_work *volatile second;
    /* NOLINTEND(clang-diagnostic-unused-but-set-variable) */

    if (work->collision == NULL) {
        return;
    }
    work->collision->shapes[0].radius = BODY_RADIUS;
    /* @hack spelling: the first shape's attack and guard each cleared by an
     * assignment of the levels masked, where &= would do */
    shapes = (first = work->collision)->shapes;
    shapes[0].levels = shapes[0].levels & ~SHAPE_ATTACK;
    shapes = (second = work->collision)->shapes;
    shapes[0].levels = shapes[0].levels & ~SHAPE_GUARD;
    for (i = ATTACK_SHAPE; i < SHAPES; i++) {
        work->collision->shapes[i].flags |= SHAPE_OFF;
    }
    shapes = work->collision->shapes;
    if (record->flags & PLAYER_INVINCIBLE) {
        attack = INVINCIBLE_LEVEL;
        guard = INVINCIBLE_LEVEL;
    } else if (work->flags & PLAYER_WORK_BALL) {
        attack = BALL_LEVEL;
        guard = BALL_LEVEL;
    } else {
        attack = 0;
        guard = 0;
    }
    levels = attack & SHAPE_ATTACK;
    /* @hack spelling: the three shapes each in the order 0, 2, 1, where 0, 1,
     * 2 would do */
    shapes[0].levels = (shapes[0].levels & ~SHAPE_ATTACK) | levels;
    shapes[2].levels = (shapes[2].levels & ~SHAPE_ATTACK) | levels;
    shapes[1].levels = (shapes[1].levels & ~SHAPE_ATTACK) | levels;
    /* @hack spelling: guard shifted in place, where the shift at each use
     * would do */
    guard <<= SHAPE_GUARD_SHIFT;
    shapes[0].levels =
        (shapes[0].levels & ~SHAPE_GUARD) | (guard & SHAPE_GUARD);
    shapes[2].levels =
        (shapes[2].levels & ~SHAPE_GUARD) | (guard & SHAPE_GUARD);
    shapes[1].levels =
        (shapes[1].levels & ~SHAPE_GUARD) | (guard & SHAPE_GUARD);
    /* @hack spelling: an else after the return, where the code after the if
     * would do */
    if (work->mode != PLAYER_MODE_EGGMAN_ATTACK) {
        return;
    } else {
        if (record->anim.current == ANIM_ATTACK) {
            work->collision->shapes[ATTACK_SHAPE].flags &= ~SHAPE_OFF;
            work->collision->shapes[ATTACK_SHAPE].center = record->unk_1e8;
        } else {
            work->collision->shapes[RUNNING_ATTACK_SHAPE].flags &= ~SHAPE_OFF;
            work->collision->shapes[RUNNING_ATTACK_SHAPE].center =
                record->unk_200;
        }
    }
}
