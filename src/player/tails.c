/* Tails on foot, the player of PLAYER_TAILS outside his walker */
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
#include "player/player.h"
#include "player/tails.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#ifdef NONMATCHING
/* this side turns a vector by zam_vec3_turn_yxz. for a finite vector it gives
 * the shipped bits, as turn.h says */
#include "engine/dc/turn.h"
#endif

/* his slots of model_table, beside tails.h's. every animation of his table
 * poses TAILS_MODEL. the other slots here are nodes of it, but for
 * TAILS_SHADOW. his draw keeps a TAILS_AT_ node's place in the record field
 * its suffix names */
#define TAILS_MODEL 208
#define TAILS_AT_230 209
/* in PLAYER_MODE_RUN his draw turns this node about x by the record's lean */
#define TAILS_LEANS 210
#define TAILS_SHADOW 214 /* his body's shadow, a modifier volume */
#define TAILS_AT_224 215
#define TAILS_FOOT_0 216 /* kept in unk_200 */
#define TAILS_FOOT_1 217 /* kept in unk_20c */
#define TAILS_HAND_0 218 /* kept in unk_1e8 */
#define TAILS_HAND_1 219 /* kept in unk_1f4 */
/* while he holds a task, his draw turns these about z by the held task's
 * motion's hold_turn, negated */
#define TAILS_HOLDS_A 220
#define TAILS_HOLDS_B 221
/* the node his tails draw at. his draw keeps its place in unk_23c */
#define TAILS_TAIL_ROOT 223

/* the slot of Sonic's ball, sonic.c's SONIC_BALL. tails_draw_shadows gives
 * it the ball's shadow at his pos, as Sonic's draw does, but no entry of his
 * table poses it */
#define BALL_MODEL 6

/* PLAYER_MODE_TAILS_FLY's animations. he flies in ANIM_FLY. falling, he goes
 * on to ANIM_FLY_FALL, or within FLY_LOW above the ground to ANIM_FLY_LOW.
 * PLAYER_REQUEST_HOVER's answer plays ANIM_FLY_LOW too */
#define ANIM_FLY 90
#define ANIM_FLY_FALL 91
#define ANIM_FLY_LOW 93
#define FLY_LOW 15.0F
/* his tail attack's animation, and the slot of motion_table that holds its
 * motion. over that motion the tips of his tails spin a whole turn about him.
 * they lie TIP_HEIGHT up his y, and TIP_REACH either way along his z */
#define ANIM_TAIL_ATTACK 94
#define MOTION_TAIL_ATTACK 94
#define TIP_HEIGHT 3.0F
#define TIP_REACH 6.0F
/* the spin, as sonic.c and player/knuckles.h number theirs. his table's entry
 * loops in PLAYER_ANIM_LOOP_PACE, as theirs do. his steps start it only as
 * they answer a PLAYER_REQUEST_ANIM of it. in it, on the ground, his draw
 * squashes his body. it sinks the body SQUASH_DROP down his y, and tilts it
 * EIGHTH_TURN about z. it then scales it by SQUASH_X, SQUASH_Y and SQUASH_Z */
#define ANIM_SPIN 11
#define SQUASH_DROP 1.0F
#define SQUASH_X 0.7F
#define SQUASH_Y 1.1F
#define SQUASH_Z 0.8F

/* the work's flags that a mode taken for a request or an action clears */
#define REQUEST_CLEARS                                                         \
    (PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL)

/* a landing drops him onto the ground from up to LAND_SNAP above it, unless the
 * ground is water. PLAYER_MODE_FLING's landing doesn't */
#define LAND_SNAP 1.0F
/* outside his flight, his task eases tilt.z back to 0 by TILT_Z_BACK a step,
 * and tilt.x by TILT_X_BACK */
#define TILT_Z_BACK 0x400
#define TILT_X_BACK 0x300

/* his tails' jiggles as tails_create makes them. each is a chain, whose param
 * tails_move_tail_jiggles replaces at each step. tail_a's sway starts at the
 * angle SWAY_START_ANGLE, and tail_b's at its mirror. TAIL_PHASE_STEP is each
 * tail's first phase_step. nothing reads it before a sway replaces it */
#define TAIL_PARAM 0.4F
#define SWAY_START_ANGLE 0x3000
#define TAIL_PHASE_STEP 0x700
/* his tails' spin about their x. in flight and a fast run its speed rises to
 * SPIN_TOP by SPIN_GAIN a step. the tail attack keeps the speed. otherwise the
 * speed falls to 0 by SPIN_EASE a step. once it is 0, the spin's angle eases
 * back to 0 by SPIN_EASE too */
#define SPIN_TOP 0x1000
#define SPIN_GAIN 0x80
#define SPIN_EASE 0x800
/* the turns his tails' copies ease to, a step at a time. tail_a's copy takes
 * each turn about x and y as written, and tail_b's the mirror of it. the
 * record's tail_tilt turns both about z.
 *
 * in flight and a fast run, they ease by FLY_EASE a step. they ease to
 * SPIN_TURN_X about x, FLY_TURN_Y about y and a tilt of FLY_TILT */
#define FLY_EASE 0x800
#define SPIN_TURN_X 0xc000
#define FLY_TURN_Y 0x4800
#define FLY_TILT (-0x5000)
/* in the tail attack, they ease by ATTACK_EASE to SPIN_TURN_X and 0. they tilt
 * to ATTACK_TILT by ATTACK_TILT_EASE */
#define ATTACK_EASE 0x1000
#define ATTACK_TILT 0x1000
#define ATTACK_TILT_EASE 0x2000
/* otherwise they ease by REST_EASE to REST_TURN_X and 0. they tilt back to 0 by
 * REST_TILT_EASE */
#define REST_EASE 0x8000
#define REST_TURN_X 0xee00
#define REST_TILT_EASE 0x800
/* as chains, his tails sway while he moves less than STILL_MOVE a step, in the
 * modes tails_move_tail_jiggles lists. PLAYER_MODE_STAND is one. a swaying
 * tail's force.y is SWAY_FORCE times the sine of its sway's angle, negated */
#define STILL_MOVE 0.01F
#define SWAY_FORCE 0.08F
/* as chains, his tails are JIGGLE_CHAIN_CAPPED while he is on the ground. on
 * the step a tail turns so, its force becomes (GROUND_PUSH, 0, 0) */
#define GROUND_PUSH 10.0F
/* in a landing's animation, a chain whose tip lies below LANDING_TIP_LOW takes
 * a force of (LANDING_PUSH_X, LANDING_PUSH_Y, 0) */
#define LANDING_TIP_LOW 8.0F
#define LANDING_PUSH_X 2.0F
#define LANDING_PUSH_Y 0.4F

/* his collision shapes, by their index: his body, the tips of his tails, and
 * SHAPE_ABOVE. SHAPE_ABOVE lies ABOVE_HEIGHT above his pos, and ABOVE_BACK
 * behind it. tails_set_shapes gives his body BODY_RADIUS, or FLY_BODY_RADIUS in
 * his flight */
#define SHAPE_BODY 0
#define SHAPE_TIP_PLUS_Z 1
#define SHAPE_TIP_MINUS_Z 2
#define SHAPE_ABOVE 3
#define SHAPES 4
#define ABOVE_HEIGHT 8.25F
#define ABOVE_BACK 2.0F
#define BODY_RADIUS 4.0F
#define FLY_BODY_RADIUS 2.25F
/* the attack and guard levels his step gives his body's shape under
 * PLAYER_WORK_BALL, unless PLAYER_INVINCIBLE gives them INVINCIBLE_LEVEL.
 * they are 0 otherwise. tails_set_shapes sets them again later in the same
 * step, to BALL_LEVEL or 0 */
#define BALL_LEVEL 1

/* the shadows at his hands and feet are shadow_model, scaled. a hand's scales
 * along z by HAND_SHADOW_Z, and a foot's along x by FOOT_SHADOW_X.
 * BALL_MODEL's shadow lies BALL_SHADOW_RISE above his pos, scaled by
 * BALL_SHADOW_SIZE along x and z */
#define HAND_SHADOW_Z 0.7F
#define FOOT_SHADOW_X 2.6F
#define BALL_SHADOW_RISE 0.3F
#define BALL_SHADOW_SIZE 5.0F

/* the sounds and voices he plays. PLAYER_MODE_UNK_0E plays VOICE_PUSH as its
 * push takes over from ANIM_PUSH_START. VOICE_CRASH is his crash's, into a wall
 * or by PLAYER_REQUEST_KNOCK_BACK. VOICE_SHAKE plays as
 * PLAYER_MODE_SHAKE_HOLDING's shake starts */
#define SOUND_JUMP 0x2000
#define SOUND_BRAKE 0x2002
#define SOUND_FLAP 0x2009
#define SOUND_TAIL_ATTACK 0x200a
#define VOICE_JUMP 0x65b
#define VOICE_FLY 0x65d
#define VOICE_CRASH 0x68b
#define VOICE_MISS 0x68d
#define VOICE_PICK_UP 0x6b9
#define VOICE_SET_DOWN 0x6bb
#define VOICE_THROW 0x6bd
#define VOICE_PUSH 0x6bf
#define VOICE_SHAKE 0x6c3
#define VOICE_TAIL_ATTACK 0x6c5

/* each step of a flight adds FLAP_STEP to his flap. each unit of climb adds
 * half of it again. the flap ends at FLAP_PERIOD, and plays its sound with
 * FLAP_PRIORITY, FLAP_VOLUME and FLAP_DURATION */
#define FLAP_STEP 8
#define FLAP_PERIOD 64
#define FLAP_PRIORITY 1
#define FLAP_VOLUME 60
#define FLAP_DURATION 6
/* his flight's lifts against a fall. a spd.y below LIFT_FAST_FALL from
 * LIFT_LOW up to LIFT_HIGH above the ground takes LIFT_SHARE of its excess. so
 * does one below LIFT_SLOW_FALL under LIFT_LOW. a spd.y below GLIDE_FALL takes
 * GLIDE_SHARE of its excess too. while he holds jump or the action button, he
 * hovers and takes no lift */
#define LIFT_HIGH 50.0F
#define LIFT_LOW 25.0F
#define LIFT_FAST_FALL (-3.0F)
#define LIFT_SLOW_FALL (-0.3F)
#define LIFT_SHARE 0.25F
#define GLIDE_FALL (-2.0F)
#define GLIDE_SHARE 0.1F
/* in flight the stick's heading steers him. within STEER_TURN of his own
 * heading it pushes him STEER_SHARE of its strength along his x. further off,
 * it only turns him. above running_speed, a heading more than STEER_BRAKE_TURN
 * off brakes him instead */
#define STEER_TURN 0x1000
#define STEER_SHARE 0.044F
#define STEER_BRAKE_TURN 0x6000

/* a throw lets go of the task from frame THROW_FRAME on the ground, and
 * AIR_THROW_FRAME in the air. ANIM_THROW_AT_TOUCH lets go from
 * THROW_AT_TOUCH_FRAME. ANIM_SET_DOWN sets the task down at any frame up to
 * SET_DOWN_FRAME */
#define THROW_FRAME 3.0F
#define AIR_THROW_FRAME 2.0F
#define THROW_AT_TOUCH_FRAME 7.0F
#define SET_DOWN_FRAME 3.0F
/* the frame of PLAYER_MODE_PICK_UP's animation from which he picks up */
#define PICK_UP_FRAME 3.0F
/* the speed a throw at a target gives the task. a throw ahead gives it
 * (THROW_AHEAD_X, THROW_AHEAD_Y, 0) in his own axes */
#define THROW_SPEED 3.0F
#define THROW_AHEAD_X 2.0F
#define THROW_AHEAD_Y 0.8F
/* the chance that a pass of ANIM_PULL's loop goes on to ANIM_PULL_LIFT without
 * the stick pushed. with it pushed, the pull always goes on */
#define PULL_LIFT_CHANCE 0.1F
/* how far a held task lies from his hands. through a throw it lies HOLD_REACH
 * along his hand's z axis, unk_26c. it then lies HOLD_OUT along the hand's down
 * axis, unk_284. otherwise it lies HOLD_OUT down from between his hands */
#define HOLD_REACH 2.5F
#define HOLD_OUT 1.2F

/* his animations, the record's anim.actions. his draw indexes the table twice
 * for a motion: the entry's motion m, then entry m's motion. every motion m of
 * the table is also entry m's, so the second index changes nothing */
static struct player_action tails_anims[116] = {
#include "player/tails/tails_anims.inc"
};

/* his shapes, whose centers are world positions. tails_place_shapes places
 * his body's each step. tails_set_shapes turns on the tips of his tails in his
 * tail attack, and SHAPE_ABOVE in his flight. it places each as it turns it on.
 * The non-body shapes push and resist every group but pickups, hurt every kind,
 * guard at level 2 and attack at level 1. */
static struct collision_shape tails_shapes[SHAPES] = {
#include "player/tails/tails_shapes.inc"
};

/* in his flight, tilt.z eases toward tilt_z_scale times the record's accel.x,
 * by tilt_z_step a step. tilt.x eases toward tilt_x_scale times its spd.z, by
 * tilt_x_step. a target more than EIGHTH_TURN from 0 first moves EIGHTH_TURN
 * toward 0 */
static Sint32 tilt_z_scale = 0x2f000;
static Angle tilt_z_step = 0x250;
static Sint32 tilt_x_scale = 0xc400;
static Angle tilt_x_step = 0x190;

/* TAILS_HEAD and his eyes, which player_look_start turns */
static NJS_CNK_OBJECT *tails_look_nodes[3];

static void tails_step(struct task *task);
static void tails_choose_mode(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *tails);

void tails_create(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *tails;
    struct player *record;

    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            tails_step, "MilesTailsPrower")) == NULL) {
        return;
    }
    if ((tails = syCalloc(1, TAILS_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = tails;
    work = task->work;
    /* @hack spelling: the record through the address of its first field,
     * where tails itself would do */
    record = (struct player *)&tails->slot;
    record->slot = slot;
    record->character = PLAYER_TAILS;
    /* @hack spelling: the two stores chained, where two statements would do */
    work->mode = work->submode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = tails_anims;
    record->anim.next = ANIM_STAND;
    collision_init(task, tails_shapes, SHAPES, COLLISION_PLAYER);
#ifdef NONMATCHING
    /* this side repairs the tag in tails_step's PLAYER_MODE_DROWN case: the
     * create empties the slot and destroys the task where collision_init
     * leaves the work no collision record, as sonic_create's side does */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    tails->by_character.tails.texlist = texture_load_pvm_alloc("MILESTEX");
    life_icon_load(record->character);
    tails->by_character.tails.models = model_file_read("MILESMDL.PRS");
    tails->by_character.tails.motions = motion_file_read("MILESMTN.PRS");
    player_anim_step(&record->anim);
    tails_look_nodes[0] = model_table[TAILS_HEAD].object;
    tails_look_nodes[1] = model_table[TAILS_EYE_0].object;
    tails_look_nodes[2] = model_table[TAILS_EYE_1].object;
    player_look_start(slot, tails_look_nodes);
    tails->by_character.tails.tail_a =
        jiggle_create(model_table[TAILS_TAIL].object);
    tails->by_character.tails.tail_a->kind = JIGGLE_CHAIN;
    tails->by_character.tails.tail_a->param = TAIL_PARAM;
    tails->by_character.tails.tail_a->by_kind.angle = SWAY_START_ANGLE;
    tails->by_character.tails.tail_a->phase_step = TAIL_PHASE_STEP;
    tails->by_character.tails.tail_a->shape_a =
        model_table[TAILS_TAIL_SHAPE_A].object;
    tails->by_character.tails.tail_a->shape_b =
        model_table[TAILS_TAIL_SHAPE_B].object;
    tails->by_character.tails.tail_a->copy->evalflags &= ~NJD_EVAL_UNIT_ANG;
    tails->by_character.tails.tail_b =
        jiggle_create(model_table[TAILS_TAIL].object);
    tails->by_character.tails.tail_b->kind = JIGGLE_CHAIN;
    tails->by_character.tails.tail_b->param = TAIL_PARAM;
    tails->by_character.tails.tail_b->by_kind.angle =
        FULL_TURN - SWAY_START_ANGLE;
    tails->by_character.tails.tail_b->phase_step = TAIL_PHASE_STEP;
    tails->by_character.tails.tail_b->shape_a =
        model_table[TAILS_TAIL_SHAPE_A].object;
    tails->by_character.tails.tail_b->shape_b =
        model_table[TAILS_TAIL_SHAPE_B].object;
    tails->by_character.tails.tail_b->copy->evalflags &= ~NJD_EVAL_UNIT_ANG;
    action_window_start(record->slot);
    task->destructor = tails_destroy;
    task->draw = tails_draw;
}

/* his task's step. in PLAYER_MODE_MISS and PLAYER_MODE_DROWN he answers only
 * PLAYER_REQUEST_RESET, and chooses no mode. in PLAYER_MODE_EVENT, and while
 * ANIM_WIN or ANIM_LOSE is asked for, the step skips three things. it calls no
 * tails_set_shapes, leaves his tilt as it is, and registers his task for no
 * collision checks */
static void tails_step(struct task *task)
{
    struct player *tails;
    struct task_work *work;
    struct player_motion *motion;
    struct player *record;

    tails = task->extra.p;
    work = task->work;
    motion = task->motion.p;
    /* @hack spelling: record, a copy of tails, where tails alone would do */
    record = tails;
    player_ground_step(task, work, motion, record);
    if (work->mode == PLAYER_MODE_MISS || work->mode == PLAYER_MODE_DROWN) {
        if (work->submode == PLAYER_REQUEST_RESET) {
            work->flags &= ~PLAYER_RESET_WORK_FLAGS;
            record->flags &= ~PLAYER_RESET_FLAGS;
            player_stand(work, motion, record);
        }
    } else {
        tails_choose_mode(work, motion, record, tails);
    }
    /* @hack spelling: the locals declared in the block, where declarations
     * at the head of the function would do */
    if (work->collision) {
        int attack;
        int guard;
        struct collision_shape *shape;

        /* INVINCIBLE_LEVEL lasts only where the step calls no
         * tails_set_shapes */
        shape = work->collision->shapes;
        /* @hack spelling: the stores of each arm, and the else's chained,
         * where one statement after the arms would do */
        if (record->flags & PLAYER_INVINCIBLE) {
            attack = INVINCIBLE_LEVEL;
            guard = INVINCIBLE_LEVEL;
        } else if (work->flags & PLAYER_WORK_BALL) {
            attack = BALL_LEVEL;
            guard = BALL_LEVEL;
        } else {
            guard = attack = 0;
        }
        /* @hack type: the masks unsigned, where SHAPE_ATTACK's and
         * SHAPE_GUARD's own complements would do */
        shape->levels = (shape->levels & ~(Uint32)SHAPE_ATTACK) |
                        SHAPE_ATTACK_LEVEL(attack & SHAPE_ATTACK);
        shape->levels = (shape->levels & ~(Uint32)SHAPE_GUARD) |
                        (SHAPE_GUARD_LEVEL(guard) & SHAPE_GUARD);
    }
    if (work->flags & PLAYER_WORK_BALL) {
        tails->by_character.tails.ball_steps++;
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
            voice_play_priority_2(VOICE_CRASH);
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
                voice_play_priority_2(VOICE_PUSH);
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
    case PLAYER_MODE_TAILS_TAIL_ATTACK:
        player_turn_to_stick(work, motion, record);
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_TAILS_FLY:
        player_turn_upright(work, motion, record);
        tails_fly(work, motion, record, tails);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_PLAY_ANIM:
    case PLAYER_MODE_TAILS_HOVER:
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
                    record->anim.frame * PLAYER_HELD_ACC;
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
        /* @bug memory: reads the work's collision record with no test for
         * null, at 0x8c088540. the step tests the record for null at
         * 0x8c087bae, so it can be null. tails_choose_mode reads it untested
         * too, at 0x8c0895ac and 0x8c08a654 */
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
        tails_free_move(work, record);
        break;
    }
    player_anim_step(&record->anim);
    tails_move_tail_jiggles(work, record, tails);
    tails_place_shapes(work, record, tails);
    if (player_free_move_forced(record->slot) &&
        work->mode != PLAYER_MODE_FREE_MOVE) {
        work->mode = PLAYER_MODE_FREE_MOVE;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_REQUEST | PLAYER_WORK_BALL);
    }
    tails_drop_held(work, motion, record);
    surface_probe_player(work, record);
    /* @hack redundant: the block, which shapes the frame */
    {
        NJS_VECTOR behind;
        struct angles ang;
        struct ground_probe *probe;

        probe = &tails->by_character.tails.behind;
        /* @bug uninitialized: subtracts his radius from behind.x, which
         * nothing sets first, at 0x8c0886e8 */
#ifdef NONMATCHING
        /* this side keeps the tag's result: behind.x starts at 0, so the
         * probe's point lies his radius behind him. the image's step stores
         * work->collision->shapes in behind.x's frame word at 0x8c087bb8.
         * it does so wherever the work holds a collision record, as it
         * always does on this side. that pointer's bits, read as a float,
         * lie within 2^-101 of 0. so taking his radius from them gives his
         * radius negated, as taking it from 0 does */
        behind.x = 0.0F;
#endif
        behind.x -= record->consts.radius;
        behind.y = TAILS_BEHIND_HEIGHT;
        behind.z = 0.0F;
        ang = work->ang;
#ifdef NONMATCHING
        /* for a finite vector, zam_vec3_turn_yxz gives the bits of the
         * matrix calls below, as engine/dc/turn.h says */
        zam_vec3_turn_yxz(ang.x, -ang.y, ang.z, &behind.x, &behind.x);
#else
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, ang.z);
        njRotateX(NULL, ang.x);
        njRotateY(NULL, -ang.y);
        njCalcVector(NULL, &behind, &behind);
        njPopMatrixEx();
#endif
        njAddVector(&behind, &work->pos);
        surface_probe_ground(&behind, probe);
    }
    if (record->anim.next != ANIM_WIN && record->anim.next != ANIM_LOSE &&
        work->mode != PLAYER_MODE_EVENT) {
        tails_set_shapes(work, record, tails);
        if (work->mode == PLAYER_MODE_TAILS_FLY) {
            Angle ang;
            Angle step;

            ang = (Angle)(tilt_z_scale * record->accel.x);
            if (angle_distance(ang, 0) > EIGHTH_TURN) {
                ang = angle_approach(ang, 0, EIGHTH_TURN);
            }
            step = tilt_z_step;
            tails->by_character.tails.tilt.z =
                angle_approach(tails->by_character.tails.tilt.z, ang, step);
            ang = (Angle)(tilt_x_scale * record->spd.z);
            if (angle_distance(ang, 0) > EIGHTH_TURN) {
                ang = angle_approach(ang, 0, EIGHTH_TURN);
            }
            step = tilt_x_step;
            tails->by_character.tails.tilt.x =
                angle_approach(tails->by_character.tails.tilt.x, ang, step);
        } else {
            tails->by_character.tails.tilt.z = angle_approach(
                tails->by_character.tails.tilt.z, 0, TILT_Z_BACK);
            tails->by_character.tails.tilt.x = angle_approach(
                tails->by_character.tails.tilt.x, 0, TILT_X_BACK);
        }
        collision_register(task);
        collision_set_radius(task,
                             record->consts.height + njScalor(&record->spd));
    }
    player_finish_step(work, motion, record);
}

/* @hack redundant: tails, the record again, where record alone would do */
/* NOLINTNEXTLINE(readability-function-size) */
static void tails_choose_mode(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *tails)
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
             * 0x8c0889b4. the test of record at 0x8c08894e lets a null record
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
    player_offer_hint(work, record);
    player_offer_target_actions(work, motion, record);
    player_offer_held_actions(work, record);
    player_offer_release(work, record);
    player_offer_call(work, record);
    switch (work->mode) {
    case PLAYER_MODE_STAND:
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (tails_take_action(work, motion, record, tails)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (tails_test_jump(work, motion, record)) {
            break;
        }
        if (tails_test_tail_attack(work, record, tails)) {
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
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (tails_take_action(work, motion, record, tails) ||
            player_test_fall(work, motion, record) ||
            tails_test_jump(work, motion, record)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (tails_test_tail_attack(work, record, tails)) {
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
            record->anim.after = ANIM_STAND;
            sound_play(SOUND_BRAKE, NULL, 0, 0);
            work->flags &= ~PLAYER_WORK_ON_PATH;
        }
        break;
    case PLAYER_MODE_PATH_RUN:
        work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (tails_take_action(work, motion, record, tails) ||
            player_test_fall(work, motion, record) ||
            tails_test_jump(work, motion, record)) {
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
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (tails_take_action(work, motion, record, tails)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_STAND;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND_STILL;
                } else {
                    record->anim.next = ANIM_LAND;
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
            work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            if (!(record->ground.below_flags & WORLD_FLAT) &&
                work->pos.y > record->ground.below_y &&
                _builtin_fabsf(work->pos.y - record->ground.below_y) <=
                    LAND_SNAP) {
                work->pos.y = record->ground.below_y;
            }
        } else if (player_jump_pressed(work, record)) {
            tails_fly_start(work, motion, record, tails);
            break;
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
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL_START;
        }
        break;
    case PLAYER_MODE_LAUNCH:
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL_START;
        }
        break;
    case PLAYER_MODE_FLING:
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_STAND;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND_STILL;
                } else {
                    record->anim.next = ANIM_LAND;
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
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            tails->by_character.tails.state &= ~TAILS_FLYING;
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_STAND;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND_STILL;
                } else {
                    record->anim.next = ANIM_LAND;
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
            if (!(record->ground.below_flags & WORLD_FLAT) &&
                work->pos.y > record->ground.below_y &&
                _builtin_fabsf(work->pos.y - record->ground.below_y) <=
                    LAND_SNAP) {
                work->pos.y = record->ground.below_y;
            }
        } else if (player_jump_pressed(work, record)) {
            tails_fly_start(work, motion, record, tails);
            break;
        } else if (player_test_ledge(work, motion, record)) {
            player_hang_ledge(work, motion, record);
            work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        }
        break;
    case PLAYER_MODE_TEETER:
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (tails_take_action(work, motion, record, tails)) {
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
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (tails_test_jump(work, motion, record)) {
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
        if (tails_take_request(work, motion, record, tails) ||
            tails_take_action(work, motion, record, tails) ||
            tails_test_jump(work, motion, record) ||
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
        if (tails_take_request(work, motion, record, tails)) {
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
            if (!tails_test_jump(work, motion, record) &&
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
            if (tails_take_held_request(work, motion, record, tails)) {
                break;
            }
        } else if (tails_take_request(work, motion, record, tails)) {
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
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (tails_test_jump(work, motion, record)) {
            break;
        }
        if (tails_test_tail_attack(work, record, tails)) {
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
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (pad_stick_pushed[record->slot] ||
            (player_pad_records[record->slot].press &
             PLAYER_STRUGGLE_BUTTONS)) {
            tails->by_character.tails.struggle -= PLAYER_STRUGGLE_PUSH;
        }
        if (tails->by_character.tails.struggle > 0) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_UNK_2A:
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_ASK_HINT:
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_UNK_35:
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_TAILS_FLY:
        if (tails_take_request(work, motion, record, tails) ||
            tails_take_action(work, motion, record, tails)) {
            tails->by_character.tails.state &= ~TAILS_FLYING;
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            tails->by_character.tails.state &= ~TAILS_FLYING;
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_STAND;
                tails->by_character.tails.unk_310 = TAILS_UNK_310_LANDED;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                record->anim.next = ANIM_LAND_STILL;
                tails->by_character.tails.unk_310 = TAILS_UNK_310_LANDED;
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
                tails->by_character.tails.unk_310 = TAILS_UNK_310_LANDED;
            }
            if (!(record->ground.below_flags & WORLD_FLAT) &&
                work->pos.y > record->ground.below_y &&
                _builtin_fabsf(work->pos.y - record->ground.below_y) <=
                    LAND_SNAP) {
                work->pos.y = record->ground.below_y;
            }
        } else if (record->spd.y < 0.0F &&
                   record->ground.below_y > work->pos.y - FLY_LOW) {
            record->anim.next = ANIM_FLY_LOW;
        } else if (record->spd.y < 0.0F) {
            if (record->anim.current == ANIM_FLY) {
                record->anim.next = ANIM_FLY_FALL;
            }
        } else if (record->anim.current != ANIM_FLY) {
            record->anim.next = ANIM_FLY;
        }
        break;
    case PLAYER_MODE_TAILS_TAIL_ATTACK:
        if (tails_take_request(work, motion, record, tails)) {
            tails->by_character.tails.state &= ~TAILS_ATTACKING;
            break;
        }
        if (record->anim.next == ANIM_STAND) {
            work->mode = PLAYER_MODE_STAND;
            record->anim.next = ANIM_STAND;
            tails->by_character.tails.state &= ~TAILS_ATTACKING;
            break;
        }
        break;
    case PLAYER_MODE_PICK_UP:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (tails_take_held_request(work, motion, record, tails)) {
                break;
            }
        } else if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (tails_test_pick_up(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_PULL_HEAVY:
        if (tails_take_request(work, motion, record, tails)) {
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
        if (tails_take_action(work, motion, record, tails)) {
            break;
        }
        if (player_test_push(work, motion, record)) {
            break;
        }
        if (tails_test_jump(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_GRAB_PUSHABLE:
        if (tails_take_action(work, motion, record, tails)) {
            break;
        }
        if (player_test_push(work, motion, record)) {
            break;
        }
        if (tails_test_jump(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_STAND_HOLDING:
        if (tails_take_held_request(work, motion, record, tails)) {
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
        if (tails_take_action(work, motion, record, tails)) {
            break;
        }
        if (tails_test_jump(work, motion, record)) {
            break;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = PLAYER_MODE_UNK_19;
            record->anim.next = ANIM_HOLD_WALK;
        }
        break;
    case PLAYER_MODE_UNK_19:
        if (tails_take_held_request(work, motion, record, tails)) {
            break;
        }
        if (player_test_fall(work, motion, record) ||
            tails_take_action(work, motion, record, tails) ||
            tails_test_jump(work, motion, record) ||
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
            if (tails_take_held_request(work, motion, record, tails)) {
                break;
            }
        } else if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (tails_test_throw(work, record)) {
            break;
        }
        if (record->anim.next != ANIM_THROW_AT &&
            record->anim.next != ANIM_THROW &&
            record->anim.next != ANIM_SET_DOWN) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_UNK_1B:
        if (tails_take_held_request(work, motion, record, tails)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (tails_test_jump(work, motion, record)) {
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
        if (tails_take_held_request(work, motion, record, tails)) {
            break;
        }
        if (tails_take_action(work, motion, record, tails)) {
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
                if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
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
            if (!(record->ground.below_flags & WORLD_FLAT) &&
                work->pos.y > record->ground.below_y &&
                _builtin_fabsf(work->pos.y - record->ground.below_y) <=
                    LAND_SNAP) {
                work->pos.y = record->ground.below_y;
            }
        } else if (player_test_hold_end(work, motion, record)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
        }
        break;
    case PLAYER_MODE_JUMP_HOLDING:
        if (tails_take_held_request(work, motion, record, tails)) {
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
                if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
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
            if (!(record->ground.below_flags & WORLD_FLAT) &&
                work->pos.y > record->ground.below_y &&
                _builtin_fabsf(work->pos.y - record->ground.below_y) <=
                    LAND_SNAP) {
                work->pos.y = record->ground.below_y;
            }
        } else {
            if (player_count_jump_hold(work, record)) {
                record->spd.y +=
                    record->consts.jump_lift * PLAYER_JUMP_HOLDING_LIFT;
            }
            if (player_test_hold_end(work, motion, record)) {
                break;
            }
            if (tails_take_action(work, motion, record, tails)) {
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
        if (tails_take_request(work, motion, record, tails) ||
            tails_take_action(work, motion, record, tails) ||
            tails_test_jump(work, motion, record) ||
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
            if (tails_take_held_request(work, motion, record, tails)) {
                break;
            }
        } else if (tails_take_request(work, motion, record, tails)) {
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
                    if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                        record->anim.next = ANIM_HOLD_STAND;
                    } else {
                        record->anim.next = ANIM_HOLD_LAND_FAST;
                    }
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    player_stand_anim(work, motion, record);
                } else {
                    record->anim.next = ANIM_LAND;
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
            if (!(record->ground.below_flags & WORLD_FLAT) &&
                work->pos.y > record->ground.below_y &&
                _builtin_fabsf(work->pos.y - record->ground.below_y) <=
                    LAND_SNAP) {
                work->pos.y = record->ground.below_y;
            }
        } else {
            if (tails_test_throw(work, record)) {
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
        if (tails_take_held_request(work, motion, record, tails)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (tails_test_jump(work, motion, record)) {
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
        if (tails_take_held_request(work, motion, record, tails)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (tails_test_jump(work, motion, record)) {
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
            if (tails_take_held_request(work, motion, record, tails)) {
                break;
            }
        } else if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (tails_test_throw_at(player_tasks[record->slot], work, motion,
                                record)) {
            break;
        }
        if (record->anim.next != ANIM_THROW_AT_TOUCH) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_PET_CHAO:
        if (!tails_take_request(work, motion, record, tails) &&
            !player_test_fall(work, motion, record) &&
            !tails_test_jump(work, motion, record) &&
            !player_test_petting(work, motion, record)) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_PULL_BURIED:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (tails_take_held_request(work, motion, record, tails)) {
                break;
            }
        } else if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (tails_test_pull(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_LEDGE_HANG:
        if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        if (player_test_ledge_hang(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_LEDGE_CLIMB:
        if (tails_take_request(work, motion, record, tails)) {
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
    case PLAYER_MODE_TAILS_HOVER:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (tails_take_held_request(work, motion, record, tails)) {
                break;
            }
        } else if (tails_take_request(work, motion, record, tails)) {
            break;
        }
        break;
    case PLAYER_MODE_EVENT:
        if (work->submode == PLAYER_REQUEST_RELEASE) {
            player_stand(work, motion, record);
            work->flags &= ~REQUEST_CLEARS;
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

/* his draw's state, in bss that no object holds. the image's symbols.txt
 * gives each its address. tails_drawn is the record his draw is drawing, which
 * tails_motion_callback reads. tails_view_to_world is the inverse of the view
 * as the draw begins. it takes a place in the view back to the world.
 * tails_tail_matrix is the matrix of his tails' root as the callback last met
 * it */
extern struct player *tails_drawn;
extern NJS_MATRIX tails_view_to_world;
extern NJS_MATRIX tails_tail_matrix;
#ifdef NONMATCHING
/* this side defines them, which only the image's symbols.txt places */
struct player *tails_drawn;
NJS_MATRIX tails_view_to_world;
NJS_MATRIX tails_tail_matrix;
#endif

/* how a swaying tail picks its sway. the chain kinds read none of a jiggle's
 * by_kind, phase and phase_step. so the sway keeps its state there. its angle
 * is by_kind.angle, its countdown phase, and its speed phase_step. as the
 * countdown runs out, the speed becomes sway_speed and a random part of
 * sway_speed_range more. the countdown becomes sway_steps and a random part of
 * sway_steps_range more */
static Sint32 sway_speed = 0x1c0;
static Sint32 sway_speed_range = 0x80;
static Sint32 sway_steps = 0;
static Sint32 sway_steps_range = 0;
/* as his tails stop easing to a shape, they turn JIGGLE_CHAIN_EASED. tail_b's
 * phase then counts their steps, from jump_ease_start in PLAYER_MODE_JUMP and
 * from 0 otherwise. the tails turn JIGGLE_CHAIN once the count reaches
 * ease_steps and their spin is 0 */
static Sint32 jump_ease_start = 0;
static Sint32 ease_steps = 0;
/* each step, a sway's speed other than 0 turns its angle, and eases to
 * sway_speed_end by sway_speed_ease */
static Sint32 sway_speed_end = 0;
static Sint32 sway_speed_ease = 0;

static void tails_motion_callback(NJS_CNK_OBJECT *object);
static void tails_draw_shadows(struct task_work *work, struct player *record,
                               int anim);
static void tails_place_held(struct task_work *work, struct player *record);

void tails_destroy(struct task *task)
{
    struct task_work *work = task->work;
    struct player *tails = task->extra.p;
    /* @hack spelling: the record through the address of its first field,
     * where tails itself would do */
    struct player *record = (struct player *)&tails->slot;

    if (tails->by_character.tails.tail_a != NULL) {
        jiggle_free(tails->by_character.tails.tail_a);
        tails->by_character.tails.tail_a = NULL;
    }
    if (tails->by_character.tails.tail_b != NULL) {
        jiggle_free(tails->by_character.tails.tail_b);
    }
    player_unregister(record->slot);
    sound_stop_key(work);
    njReleaseTexture(tails->by_character.tails.texlist);
    syFree(tails->by_character.tails.texlist);
    tails->by_character.tails.texlist = NULL;
    life_icon_release(record->character);
    model_file_free(tails->by_character.tails.models);
    tails->by_character.tails.models = NULL;
    motion_file_free(tails->by_character.tails.motions);
    tails->by_character.tails.motions = NULL;
}

void tails_place_shapes(struct task_work *work, struct player *record,
                        struct player *tails)
{
    struct collision_shape *shape = work->collision->shapes;
    NJS_VECTOR offset;
    Angle spin;

    offset.x = offset.z = 0.0F;
    offset.y = record->consts.center_height;
    turn_to_world(work, &offset);
    shape->center = offset;
    shape->center.x += work->pos.x;
    shape->center.y += work->pos.y;
    shape->center.z += work->pos.z;
    if (work->mode == PLAYER_MODE_TAILS_TAIL_ATTACK) {
        spin = (Angle)(record->anim.frame /
                       motion_table[MOTION_TAIL_ATTACK].motion->nbFrame *
                       FULL_TURN);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njRotateY(NULL, spin);
        offset.x = 0.0F;
        offset.y = TIP_HEIGHT;
        offset.z = TIP_REACH;
        njCalcPoint(NULL, &offset, &tails->by_character.tails.tip_plus_z);
        offset.x = 0.0F;
        offset.y = TIP_HEIGHT;
        offset.z = -TIP_REACH;
        njCalcPoint(NULL, &offset, &tails->by_character.tails.tip_minus_z);
        njPopMatrixEx();
    }
}

/* his draw's motion callback, which Ninja calls at each node it draws. it keeps
 * the place and one or two axes of his hands and feet. so it does of the
 * TAILS_AT_ nodes, TAILS_HEAD and TAILS_TAIL_ROOT. it takes each to the world
 * by tails_view_to_world, into the record. at TAILS_TAIL_ROOT it keeps Ninja's
 * matrix too, in tails_tail_matrix. it turns TAILS_LEANS and the TAILS_HOLDS_
 * nodes */
static void tails_motion_callback(NJS_CNK_OBJECT *object)
{
    struct player *record = tails_drawn;
    /* @hack spelling: the player's work in a local of the three arms that
     * test it, where player_work[record->slot] in each test would do */
    struct task_work *work;
    NJS_POINT3 point;
    NJS_VECTOR dir;

    point.x = point.y = point.z = 0.0F;
    if (object == model_table[TAILS_AT_230].object) {
        dir.x = 1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &point, &point);
        njCalcPoint(&tails_view_to_world, &point, &record->unk_230);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&tails_view_to_world, &dir, &record->unk_2cc);
    } else if (object == model_table[TAILS_LEANS].object) {
        work = player_work[record->slot];
        if (work->mode == PLAYER_MODE_RUN) {
            njRotateX(NULL, record->lean);
        }
    } else if (object == model_table[TAILS_HOLDS_A].object) {
        work = player_work[record->slot];
        if (work->flags & PLAYER_WORK_HOLDING) {
            njRotateZ(NULL, -TASK_MOTION_OF(record->held)->hold_turn);
        }
    } else if (object == model_table[TAILS_HOLDS_B].object) {
        /* @hack spelling: an arm of its own for TAILS_HOLDS_B, with
         * TAILS_HOLDS_A's body, where one arm that tests both would do */
        work = player_work[record->slot];
        if (work->flags & PLAYER_WORK_HOLDING) {
            njRotateZ(NULL, -TASK_MOTION_OF(record->held)->hold_turn);
        }
    } else if (object == model_table[TAILS_HEAD].object) {
        dir.x = 1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &point, &point);
        njCalcPoint(&tails_view_to_world, &point, &record->unk_218);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&tails_view_to_world, &dir, &record->unk_2b4);
    } else if (object == model_table[TAILS_AT_224].object) {
        dir.x = 1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &point, &point);
        njCalcPoint(&tails_view_to_world, &point, &record->unk_224);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&tails_view_to_world, &dir, &record->unk_2c0);
    } else if (object == model_table[TAILS_HAND_0].object) {
        njCalcPoint(NULL, &point, &point);
        njCalcPoint(&tails_view_to_world, &point, &record->unk_1e8);
        dir.z = 1.0F;
        dir.x = dir.y = 0.0F;
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&tails_view_to_world, &dir, &record->unk_26c);
        dir.y = -1.0F;
        dir.z = dir.x = 0.0F;
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&tails_view_to_world, &dir, &record->unk_284);
    } else if (object == model_table[TAILS_HAND_1].object) {
        njCalcPoint(NULL, &point, &point);
        njCalcPoint(&tails_view_to_world, &point, &record->unk_1f4);
        dir.z = -1.0F;
        dir.x = dir.y = 0.0F;
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&tails_view_to_world, &dir, &record->unk_278);
        dir.y = -1.0F;
        dir.z = dir.x = 0.0F;
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&tails_view_to_world, &dir, &record->unk_290);
    } else if (object == model_table[TAILS_FOOT_0].object) {
        dir.y = -1.0F;
        dir.x = dir.z = 0.0F;
        njCalcPoint(NULL, &point, &point);
        njCalcPoint(&tails_view_to_world, &point, &record->unk_200);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&tails_view_to_world, &dir, &record->unk_29c);
    } else if (object == model_table[TAILS_FOOT_1].object) {
        dir.y = -1.0F;
        dir.x = dir.z = 0.0F;
        njCalcPoint(NULL, &point, &point);
        njCalcPoint(&tails_view_to_world, &point, &record->unk_20c);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&tails_view_to_world, &dir, &record->unk_2a8);
    } else if (object == model_table[TAILS_TAIL_ROOT].object) {
        dir.y = -1.0F;
        dir.x = dir.z = 0.0F;
        njCalcPoint(NULL, &point, &point);
        njCalcPoint(&tails_view_to_world, &point, &record->unk_23c);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&tails_view_to_world, &dir, &record->gun_dir);
        njGetMatrix(&tails_tail_matrix);
    }
}

void tails_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct player *tails = task->extra.p;
    /* @hack spelling: the record through the address of its first field,
     * where tails itself would do */
    struct player *record = (struct player *)&tails->slot;
    int anim;
    int light;
    NJS_VECTOR squash;
    /* @hack spelling: the body's draw called through a local, where
     * njCnkSimpleDrawMotion called by its name would do */
    void (*draw)(NJS_CNK_OBJECT *object, NJS_MOTION *motion, Float frame);

    draw_attr_save();
    draw_attr_clear(0, NJD_FST_IS);
    light = light_selected[record->slot];
    if (light != LIGHT_UNK_1 && light != LIGHT_VOLUME) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    light_apply_player(record->slot);
    if (tails->by_character.tails.unk_30b & TAILS_30B_UNK_1) {
        tails->by_character.tails.unk_310 = 1.0F;
    }
    tails_drawn = record;
    njGetMatrix(&tails_view_to_world);
    njInvertMatrix(&tails_view_to_world);
    njCnkSetMotionCallback(tails_motion_callback);
    anim = record->anim.current;
    if (!(work->timer & PLAYER_TIMER_HIDDEN)) {
        njSetTexture(tails->by_character.tails.texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        if (anim == ANIM_SPIN && (work->flags & PLAYER_WORK_GROUNDED)) {
            squash.x = 0.0F;
            squash.y = -SQUASH_DROP;
            squash.z = 0.0F;
            njTranslateEx(&squash);
            njRotateZ(NULL, EIGHTH_TURN);
            squash.x = SQUASH_X;
            squash.y = SQUASH_Y;
            squash.z = SQUASH_Z;
            njScaleV(NULL, &squash);
        }
        njScaleEx(&work->scl);
        if (work->mode == PLAYER_MODE_TAILS_FLY) {
            njRotateZ(NULL, tails->by_character.tails.tilt.z);
            njRotateX(NULL, tails->by_character.tails.tilt.x);
        }
        draw = njCnkSimpleDrawMotion;
        if (work->mode == PLAYER_MODE_EVENT && record->event != NULL) {
            player_hide_event_models(record->slot);
            draw(model_table[record->anim.actions->model].object,
                 record->event->motion, mini_event_frame);
            player_draw_event_parts(record->slot);
        } else if (record->anim.mode != PLAYER_ANIM_BLEND) {
            draw(model_table[record->anim.actions[anim].model].object,
                 motion_table[record->anim
                                  .actions[record->anim.actions[anim].motion]
                                  .motion]
                     .motion,
                 record->anim.frame);
        } else {
            draw(model_table[record->anim.actions[anim].model].object,
                 record->anim.blend, record->anim.frame);
        }
        njPushMatrix(&tails_tail_matrix);
        njRotateZ(NULL, tails->by_character.tails.tail_tilt);
        njRotateX(NULL, tails->by_character.tails.spin);
        njCnkSimpleDrawObject(tails->by_character.tails.tail_a->copy);
        njPopMatrixEx();
        njPushMatrix(&tails_tail_matrix);
        njRotateZ(NULL, tails->by_character.tails.tail_tilt);
        njRotateX(NULL, tails->by_character.tails.spin);
        njCnkSimpleDrawObject(tails->by_character.tails.tail_b->copy);
        njPopMatrixEx();
        njPopMatrixEx();
    }
    njCnkSetMotionCallback(NULL);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    draw_attr_restore();
    light_apply(light_after_draw);
    tails_draw_shadows(work, record, anim);
    tails_place_held(work, record);
}

/* draw his shadows as modifier volumes, by anim's model. for TAILS_MODEL,
 * TAILS_SHADOW goes at unk_218, and shadow_model at each hand and foot. for
 * BALL_MODEL, shadow_model goes at his pos. each turns to his heading. other
 * models cast none */
static void tails_draw_shadows(struct task_work *work, struct player *record,
                               int anim)
{
    int model = record->anim.actions[anim].model;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    if (model == TAILS_MODEL) {
        njTranslateEx(&record->unk_218);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[TAILS_SHADOW].object);
        matrix_load_parent();
        njTranslateEx(&record->unk_1e8);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, 1.0F, 1.0F, HAND_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_1f4);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, 1.0F, 1.0F, HAND_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_200);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, FOOT_SHADOW_X, 1.0F, 1.0F);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_20c);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, FOOT_SHADOW_X, 1.0F, 1.0F);
        njCnkModDrawModel(shadow_model);
    } else if (model == BALL_MODEL) {
        njTranslate(NULL, work->pos.x, work->pos.y + BALL_SHADOW_RISE,
                    work->pos.z);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, BALL_SHADOW_SIZE, 1.0F, BALL_SHADOW_SIZE);
        njCnkModDrawModel(shadow_model);
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

void tails_move_tail_jiggles(struct task_work *work, struct player *record,
                             struct player *tails)
{
    NJS_VECTOR moved;
    Angle turn;
    Float height;

    /* his pos's height, plus TAILS_BEHIND_HEIGHT, above the ground that the
     * last step's behind probe found. it becomes both tails' param */
    height = work->pos.y - tails->by_character.tails.behind.below_y +
             TAILS_BEHIND_HEIGHT;
    if (height < 0.0F) {
        height = 0.0F;
    }
    /* @hack spelling: one chained store to both tails, where a store to
     * each would do */
    tails->by_character.tails.tail_b->param =
        tails->by_character.tails.tail_a->param = height;
    if (work->mode == PLAYER_MODE_TAILS_FLY ||
        (work->mode == PLAYER_MODE_RUN &&
         record->spd.x > record->consts.running_speed)) {
        if (tails->by_character.tails.tail_a->kind != JIGGLE_EASE_A) {
            tails->by_character.tails.tail_a->kind = JIGGLE_EASE_A;
            tails->by_character.tails.tail_b->kind = JIGGLE_EASE_A;
        }
        tails->by_character.tails.spin_speed = angle_approach(
            tails->by_character.tails.spin_speed, SPIN_TOP, SPIN_GAIN);
        tails->by_character.tails.spin += tails->by_character.tails.spin_speed;
        tails->by_character.tails.tail_a->copy->ang[0] =
            angle_approach(tails->by_character.tails.tail_a->copy->ang[0],
                           SPIN_TURN_X, FLY_EASE);
        tails->by_character.tails.tail_b->copy->ang[0] =
            angle_approach(tails->by_character.tails.tail_b->copy->ang[0],
                           FULL_TURN - SPIN_TURN_X, FLY_EASE);
        tails->by_character.tails.tail_a->copy->ang[1] =
            angle_approach(tails->by_character.tails.tail_a->copy->ang[1],
                           FLY_TURN_Y, FLY_EASE);
        tails->by_character.tails.tail_b->copy->ang[1] =
            angle_approach(tails->by_character.tails.tail_b->copy->ang[1],
                           FULL_TURN - FLY_TURN_Y, FLY_EASE);
        tails->by_character.tails.tail_tilt = angle_approach(
            tails->by_character.tails.tail_tilt, FLY_TILT, FLY_EASE);
    } else if (work->mode == PLAYER_MODE_TAILS_TAIL_ATTACK) {
        if (tails->by_character.tails.tail_a->kind != JIGGLE_EASE_B) {
            tails->by_character.tails.tail_a->kind = JIGGLE_EASE_B;
            tails->by_character.tails.tail_b->kind = JIGGLE_EASE_B;
        }
        tails->by_character.tails.spin += tails->by_character.tails.spin_speed;
        tails->by_character.tails.tail_a->copy->ang[0] =
            angle_approach(tails->by_character.tails.tail_a->copy->ang[0],
                           SPIN_TURN_X, ATTACK_EASE);
        tails->by_character.tails.tail_b->copy->ang[0] =
            angle_approach(tails->by_character.tails.tail_b->copy->ang[0],
                           FULL_TURN - SPIN_TURN_X, ATTACK_EASE);
        tails->by_character.tails.tail_a->copy->ang[1] = angle_approach(
            tails->by_character.tails.tail_a->copy->ang[1], 0, ATTACK_EASE);
        tails->by_character.tails.tail_b->copy->ang[1] = angle_approach(
            tails->by_character.tails.tail_b->copy->ang[1], 0, ATTACK_EASE);
        tails->by_character.tails.tail_tilt = angle_approach(
            tails->by_character.tails.tail_tilt, ATTACK_TILT, ATTACK_TILT_EASE);
    } else {
        if (tails->by_character.tails.tail_a->kind == JIGGLE_EASE_A ||
            tails->by_character.tails.tail_a->kind == JIGGLE_EASE_B) {
            tails->by_character.tails.tail_a->kind = JIGGLE_CHAIN_EASED;
            tails->by_character.tails.tail_b->kind = JIGGLE_CHAIN_EASED;
            tails->by_character.tails.tail_a->phase = 0;
            tails->by_character.tails.tail_a->force.x = 0.0F;
            tails->by_character.tails.tail_a->force.y = 0.0F;
            tails->by_character.tails.tail_a->force.z = 0.0F;
            tails->by_character.tails.tail_a->turn = 0;
            tails->by_character.tails.tail_b->phase = 0;
            tails->by_character.tails.tail_b->force.x = 0.0F;
            tails->by_character.tails.tail_b->force.y = 0.0F;
            tails->by_character.tails.tail_b->force.z = 0.0F;
            tails->by_character.tails.tail_b->turn = 0;
            if (work->mode == PLAYER_MODE_JUMP) {
                tails->by_character.tails.tail_b->phase = jump_ease_start;
                tails->by_character.tails.tail_a->phase = jump_ease_start;
            }
        } else if (tails->by_character.tails.tail_b->kind ==
                   JIGGLE_CHAIN_EASED) {
            if (tails->by_character.tails.tail_b->phase++ >= ease_steps &&
                tails->by_character.tails.spin == 0) {
                tails->by_character.tails.tail_a->kind = JIGGLE_CHAIN;
                tails->by_character.tails.tail_b->kind = JIGGLE_CHAIN;
                tails->by_character.tails.tail_a->phase_step = 0;
                tails->by_character.tails.tail_a->phase = 0;
                tails->by_character.tails.tail_a->by_kind.angle = 0;
                tails->by_character.tails.tail_b->phase_step = 0;
                tails->by_character.tails.tail_b->phase = 0;
                tails->by_character.tails.tail_b->by_kind.angle = 0;
            }
        } else {
            moved = work->pos;
            njSubVector(&moved, &tails->by_character.tails.last_pos);
            turn = angle_difference(tails->by_character.tails.last_ang_y,
                                    work->ang.y);
            turn_from_world(work, &moved);
            if ((work->mode == PLAYER_MODE_STAND ||
                 work->mode == PLAYER_MODE_STAND_HOLDING ||
                 work->mode == PLAYER_MODE_PET_CHAO ||
                 work->mode == PLAYER_MODE_SHAKE_HOLDING ||
                 work->mode == PLAYER_MODE_UNK_17 ||
                 work->mode == PLAYER_MODE_GRAB_PUSHABLE ||
                 work->mode == PLAYER_MODE_UNK_22 ||
                 work->mode == PLAYER_MODE_UNK_0E ||
                 work->mode == PLAYER_MODE_LEDGE_HANG ||
                 work->mode == PLAYER_MODE_UNK_20 ||
                 work->mode == PLAYER_MODE_UNK_14 ||
                 work->mode == PLAYER_MODE_TEETER ||
                 work->mode == PLAYER_MODE_PICK_UP ||
                 work->mode == PLAYER_MODE_LET_GO) &&
                njScalor(&moved) < STILL_MOVE) {
                tails->by_character.tails.tail_a->force = moved;
                tails->by_character.tails.tail_a->turn = turn;
                if (tails->by_character.tails.tail_a->phase-- <= 0) {
                    tails->by_character.tails.tail_a->phase_step =
                        sway_speed + (sway_speed_range * (rand() * RAND_SCALE));
                    tails->by_character.tails.tail_a->phase =
                        sway_steps + (sway_steps_range * (rand() * RAND_SCALE));
                }
                if (tails->by_character.tails.tail_a->phase_step != 0) {
                    tails->by_character.tails.tail_a->by_kind.angle +=
                        tails->by_character.tails.tail_a->phase_step;
                    tails->by_character.tails.tail_a->force.y = -(
                        njSin(tails->by_character.tails.tail_a->by_kind.angle) *
                        SWAY_FORCE);
                    tails->by_character.tails.tail_a->phase_step =
                        angle_approach(
                            tails->by_character.tails.tail_a->phase_step,
                            sway_speed_end, sway_speed_ease);
                }
                tails->by_character.tails.tail_b->force = moved;
                tails->by_character.tails.tail_b->turn = turn;
                if (tails->by_character.tails.tail_b->phase-- <= 0) {
                    tails->by_character.tails.tail_b->phase_step =
                        sway_speed + (sway_speed_range * (rand() * RAND_SCALE));
                    tails->by_character.tails.tail_b->phase =
                        sway_steps + (sway_steps_range * (rand() * RAND_SCALE));
                }
                if (tails->by_character.tails.tail_b->phase_step != 0) {
                    tails->by_character.tails.tail_b->by_kind.angle +=
                        tails->by_character.tails.tail_b->phase_step;
                    tails->by_character.tails.tail_b->force.y = -(
                        njSin(tails->by_character.tails.tail_b->by_kind.angle) *
                        SWAY_FORCE);
                    tails->by_character.tails.tail_b->phase_step =
                        angle_approach(
                            tails->by_character.tails.tail_b->phase_step,
                            sway_speed_end, sway_speed_ease);
                }
            } else {
                tails->by_character.tails.tail_a->force = moved;
                tails->by_character.tails.tail_a->turn = turn;
                tails->by_character.tails.tail_b->force = moved;
                tails->by_character.tails.tail_b->turn = turn;
            }
            if (work->flags & PLAYER_WORK_GROUNDED) {
                if (tails->by_character.tails.tail_a->kind !=
                    JIGGLE_CHAIN_CAPPED) {
                    tails->by_character.tails.tail_a->force.x = GROUND_PUSH;
                    tails->by_character.tails.tail_a->force.y = 0.0F;
                    tails->by_character.tails.tail_a->force.z = 0.0F;
                }
                if (tails->by_character.tails.tail_b->kind !=
                    JIGGLE_CHAIN_CAPPED) {
                    tails->by_character.tails.tail_b->force.x = GROUND_PUSH;
                    tails->by_character.tails.tail_b->force.y = 0.0F;
                    tails->by_character.tails.tail_b->force.z = 0.0F;
                }
                tails->by_character.tails.tail_a->kind = JIGGLE_CHAIN_CAPPED;
                tails->by_character.tails.tail_b->kind = JIGGLE_CHAIN_CAPPED;
            } else {
                tails->by_character.tails.tail_a->kind = JIGGLE_CHAIN;
                tails->by_character.tails.tail_b->kind = JIGGLE_CHAIN;
            }
            switch (record->anim.current) {
            case ANIM_LAND_STILL:
            case ANIM_LAND:
            case ANIM_LAND_FAST:
                if (tails->by_character.tails.tail_a
                        ->points[JIGGLE_CHAIN_POINTS - 1]
                        .y < LANDING_TIP_LOW) {
                    tails->by_character.tails.tail_a->force.x = LANDING_PUSH_X;
                    tails->by_character.tails.tail_a->force.y = LANDING_PUSH_Y;
                    tails->by_character.tails.tail_a->force.z = 0.0F;
                }
                if (tails->by_character.tails.tail_b
                        ->points[JIGGLE_CHAIN_POINTS - 1]
                        .y < LANDING_TIP_LOW) {
                    tails->by_character.tails.tail_b->force.x = LANDING_PUSH_X;
                    tails->by_character.tails.tail_b->force.y = LANDING_PUSH_Y;
                    tails->by_character.tails.tail_b->force.z = 0.0F;
                }
                break;
            }
        }
        tails->by_character.tails.spin_speed =
            angle_approach(tails->by_character.tails.spin_speed, 0, SPIN_EASE);
        tails->by_character.tails.spin += tails->by_character.tails.spin_speed;
        if (tails->by_character.tails.spin_speed == 0) {
            tails->by_character.tails.spin =
                angle_approach(tails->by_character.tails.spin, 0, SPIN_EASE);
        }
        tails->by_character.tails.tail_a->copy->ang[0] =
            angle_approach(tails->by_character.tails.tail_a->copy->ang[0],
                           REST_TURN_X, REST_EASE);
        tails->by_character.tails.tail_b->copy->ang[0] =
            angle_approach(tails->by_character.tails.tail_b->copy->ang[0],
                           FULL_TURN - REST_TURN_X, REST_EASE);
        tails->by_character.tails.tail_a->copy->ang[1] = angle_approach(
            tails->by_character.tails.tail_a->copy->ang[1], 0, REST_EASE);
        tails->by_character.tails.tail_b->copy->ang[1] = angle_approach(
            tails->by_character.tails.tail_b->copy->ang[1], 0, REST_EASE);
        tails->by_character.tails.tail_tilt = angle_approach(
            tails->by_character.tails.tail_tilt, 0, REST_TILT_EASE);
    }
    jiggle_step(tails->by_character.tails.tail_a);
    jiggle_step(tails->by_character.tails.tail_b);
    tails->by_character.tails.last_pos = work->pos;
    tails->by_character.tails.last_ang_y = work->ang.y;
}

void tails_free_move(struct task_work *work, struct player *record)
{
    Float stick_x;
    Float stick_y;
    Float sign;
    Angle heading;

    if (player_pad_records[record->slot].press & PDD_DGT_TB) {
        stage_place_player(record->slot);
    }
    if (player_pad_records[record->slot].on & PDD_DGT_TX) {
        /* @hack type: the stick's axes converted through int, where their
         * own conversion to Float would do */
        stick_x = (Float)(int)player_pad_records[record->slot].x1;
        stick_y = (Float)(int)player_pad_records[record->slot].y1;
        if (stick_x > PAD_DEAD_ZONE || stick_x < -PAD_DEAD_ZONE ||
            stick_y > PAD_DEAD_ZONE || stick_y < -PAD_DEAD_ZONE) {
            if (stick_y != 0.0F) {
                sign = stick_y / njSqrt(stick_y * stick_y);
            } else {
                sign = 0.0F;
            }
            work->pos.y -= sign * FREE_MOVE_STEP;
        }
    } else {
        /* @bug logic: reads slot 0's stick whatever the player's slot, at
         * 0x8c08c2b2, where the arm above reads the player's own, at
         * 0x8c08c26a */
        /* @hack type: the stick's axes converted through int, where their
         * own conversion to Float would do */
        stick_x = (Float)(int)player_pad_records[0].x1;
        stick_y = (Float)(int)player_pad_records[0].y1;
        if (stick_x <= PAD_DEAD_ZONE && stick_x >= -PAD_DEAD_ZONE &&
            stick_y <= PAD_DEAD_ZONE && stick_y >= -PAD_DEAD_ZONE) {
            return;
        } else {
            /* @hack spelling: the move in an else after the return, where
             * the move after the if would do */
            heading = (Angle)(atan2f(stick_y, stick_x) * ANGLE_PER_RADIAN) -
                      camera_views[record->slot]->ang.y;
            work->pos.x += njCos(heading) * FREE_MOVE_STEP;
            work->pos.z += njSin(heading) * FREE_MOVE_STEP;
        }
    }
}

Bool tails_test_jump(struct task_work *work, struct player_motion *motion,
                     struct player *record)
{
    int flags;
    struct task_force *force;

    if (player_jump_pressed(work, record)) {
        if (work->flags & PLAYER_WORK_HOLDING) {
            flags = player_action_flags(work, record);
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
                /* @hack spelling: the release in an else after the return,
                 * where the release after the if would do */
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

Bool tails_take_request(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *tails)
{
    if (!(work->flags & PLAYER_WORK_REQUEST)) {
        return FALSE;
    }
    switch (work->submode) {
    case PLAYER_REQUEST_BOUNCE:
        work->mode = PLAYER_MODE_BOUNCE;
        record->anim.next = ANIM_LAUNCH;
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_LAUNCH:
        work->mode = PLAYER_MODE_LAUNCH;
        record->anim.next = ANIM_LAUNCH;
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_FREE_FALL:
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FREE_FALL;
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_FLING:
        work->mode = PLAYER_MODE_FLING;
        record->anim.next = ANIM_LAUNCH;
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_UPDRAFT:
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_UNK_23;
        /* @hack redundant: a test of PLAYER_WORK_BALL before its clear,
         * which the clear doesn't need */
        if (work->flags & PLAYER_WORK_BALL) {
            work->flags &= ~PLAYER_WORK_BALL;
        }
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_RUN_PATH:
        work->mode = PLAYER_MODE_PATH_RUN;
        record->anim.next = ANIM_RUN;
        record->path_distance = 0.0F;
        /* @hack redundant: a test of PLAYER_WORK_BALL before its clear,
         * which the clear doesn't need */
        if (work->flags & PLAYER_WORK_BALL) {
            work->flags &= ~PLAYER_WORK_BALL;
        }
        work->flags |= PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_ANIM:
        work->mode = PLAYER_MODE_PLAY_ANIM;
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_RELEASE:
        player_stand(work, motion, record);
        /* @hack redundant: a test of PLAYER_WORK_BALL before its clear,
         * which the clear doesn't need */
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
        if (work->mode == PLAYER_MODE_TAILS_TAIL_ATTACK &&
            record->spd.x > record->consts.running_speed) {
            work->flags |= PLAYER_WORK_ON_PATH;
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_RUN;
            tails->by_character.tails.state &= ~TAILS_ATTACKING;
            break;
        }
        return FALSE;
    case PLAYER_REQUEST_CAPTURE:
        work->mode = PLAYER_MODE_CAUGHT;
        record->anim.next = ANIM_CAUGHT;
        tails->by_character.tails.struggle = PLAYER_STRUGGLE;
        player_stop(motion, record);
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_KNOCK_BACK:
        work->mode = PLAYER_MODE_CRASH;
        record->anim.next = ANIM_CRASH;
        voice_play_priority_2(VOICE_CRASH);
        crash_stars_burst(record->slot);
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_SCARE:
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = record->request_arg;
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_BREATHE:
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = ANIM_BREATHE;
        record->underwater_steps = 0;
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_HAND:
        work->mode = PLAYER_MODE_STAND_HOLDING;
        record->anim.next = ANIM_HOLD_STAND;
        record->held = record->handed;
        record->handed = NULL;
        /* @bug memory: writes the handed task's work with no test for null,
         * at 0x8c08c722. tails_test_throw tests a held task's work for null,
         * at 0x8c08cff2. tails_take_held_request, tails_test_pick_up,
         * tails_test_pull and tails_place_held read such a work untested
         * too */
        record->held->work->flags |= WORK_HELD_BY_PLAYER;
        work->flags |= PLAYER_WORK_HOLDING;
        work->flags &= ~PLAYER_WORK_UNK_8;
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_FLY:
        tails_fly_start(work, motion, record, tails);
        break;
    case PLAYER_REQUEST_HOVER:
        work->mode = PLAYER_MODE_TAILS_HOVER;
        record->anim.next = ANIM_FLY_LOW;
        work->flags &= ~PLAYER_WORK_BALL;
        work->flags |= PLAYER_WORK_ATTACK;
        tails->by_character.tails.state |= TAILS_FLYING;
        break;
    case PLAYER_REQUEST_DROWN:
        work->mode = PLAYER_MODE_DROWN;
        record->anim.next = ANIM_DROWN;
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_RESET:
        work->flags &= ~PLAYER_RESET_WORK_FLAGS;
        record->flags &= ~PLAYER_RESET_FLAGS;
        player_stand(work, motion, record);
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_MISS:
        work->mode = PLAYER_MODE_MISS;
        record->anim.next = ANIM_MISS;
        work->flags &= ~REQUEST_CLEARS;
        voice_play_priority_2(VOICE_MISS);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

Bool tails_take_held_request(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record, struct player *tails)
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
        work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        work->flags |= PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_ANIM:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_PLAY_ANIM;
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &= ~REQUEST_CLEARS;
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
        tails->by_character.tails.struggle = PLAYER_STRUGGLE;
        player_stop(motion, record);
        /* @hack redundant: a test of PLAYER_WORK_BALL before its clear,
         * which the clear doesn't need */
        if (work->flags & PLAYER_WORK_BALL) {
            work->flags &= ~PLAYER_WORK_BALL;
        }
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_KNOCK_BACK:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_CRASH;
        record->anim.next = ANIM_CRASH;
        voice_play_priority_2(VOICE_CRASH);
        crash_stars_burst(record->slot);
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_FLY:
        tails_fly_start(work, motion, record, tails);
        break;
    case PLAYER_REQUEST_HOVER:
        work->mode = PLAYER_MODE_TAILS_HOVER;
        record->anim.next = ANIM_FLY_LOW;
        work->flags &= ~PLAYER_WORK_BALL;
        work->flags |= PLAYER_WORK_ATTACK;
        tails->by_character.tails.state |= TAILS_FLYING;
        break;
    case PLAYER_REQUEST_HAND:
        work->mode = PLAYER_MODE_STAND_HOLDING;
        record->anim.next = ANIM_HOLD_STAND;
        record->held = record->handed;
        record->handed = NULL;
        record->held->work->flags |= WORK_HELD_BY_PLAYER;
        work->flags |= PLAYER_WORK_HOLDING;
        work->flags &= ~PLAYER_WORK_UNK_8;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_SCARE:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = record->request_arg;
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_DROWN:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_DROWN;
        record->anim.next = ANIM_DROWN;
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_RESET:
        player_release_held(work, motion, record);
        work->flags &= ~PLAYER_RESET_WORK_FLAGS;
        record->flags &= ~PLAYER_RESET_FLAGS;
        player_stand(work, motion, record);
        work->flags &= ~REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_MISS:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_MISS;
        record->anim.next = ANIM_MISS;
        work->flags &= ~REQUEST_CLEARS;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

Bool tails_take_action(struct task_work *work, struct player_motion *motion,
                       struct player *record, struct player *tails)
{
    Sint8 *actions = record->actions;
    int count;
    int action;
    int i;

    count = record->action_count;
    if (!pad_action_pressed[record->slot]) {
        return FALSE;
    }
    if (count == 0) {
        return FALSE;
    }
    action = record->action;
    for (i = 0; i < count; i++) {
        if (action == *actions) {
            break;
        }
        actions++;
    }
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
    case PLAYER_MODE_PET_CHAO:
        player_pet_chao(work, record);
        return TRUE;
    case PLAYER_MODE_THROW_AT_TOUCH:
        player_throw_at_touch(work, record);
        return TRUE;
    case PLAYER_MODE_PULL_BURIED:
        player_pull_buried(work, record);
        return TRUE;
    }
    return FALSE;
}

Bool tails_test_pick_up(struct task_work *work, struct player_motion *motion,
                        struct player *record)
{
    int flags;

    player_approach_touched(work, motion, record);
    if (record->anim.frame >= PICK_UP_FRAME) {
        /* @hack spelling: flags assigned inside the test, where a statement
         * of its own before it would do */
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
    }
    return FALSE;
}

Bool tails_test_pull(struct task_work *work, struct player_motion *motion,
                     struct player *record)
{
    int flags;

    player_approach_touched(work, motion, record);
    if (record->anim.current == ANIM_PULL &&
        record->anim.frame < record->anim.actions[record->anim.current].rate &&
        (player_read_stick(work, record, NULL, NULL) ||
         rand() * RAND_SCALE < PULL_LIFT_CHANCE)) {
        record->anim.next = ANIM_PULL_LIFT;
        record->anim.after = ANIM_HOLD_STAND;
        return FALSE;
    }
    if (record->anim.current == ANIM_PULL_LIFT) {
        if (!(work->flags & PLAYER_WORK_HOLDING)) {
            /* @hack spelling: flags assigned inside the test, where a
             * statement of its own before it would do */
            if ((flags = player_action_flags(work, record)) != 0 &&
                player_face_offer(work, motion, record) ==
                    PLAYER_WALL_HEAD_ON) {
                if (flags & ACTION_FLAG_1000) {
                    voice_play_priority_2(VOICE_PICK_UP);
                    fun_8c05bce0();
                    record->held = record->hold_offer;
                    record->held->work->flags |=
                        WORK_HELD_BY_PLAYER | FALL_UNK_200;
                    record->held->work->id = FALL_KIND_NORMAL;
                    work->flags |= PLAYER_WORK_HOLDING;
                    work->flags &= ~PLAYER_WORK_UNK_8;
                    return FALSE;
                }
            } else {
                /* @hack redundant: an empty else */
            }
        } else if (player_action_flags(work, record)) {
            return FALSE;
        }
        player_stand(work, motion, record);
    } else if (record->anim.current == ANIM_HOLD_STAND) {
        /* @hack redundant: a test of flags for 0, with flags assigned inside
         * it, ahead of the test of ACTION_FLAG_200, which needs neither */
        if ((flags = player_action_flags(work, record)) != 0) {
            if (flags & ACTION_FLAG_200) {
                work->mode = PLAYER_MODE_STAND_HOLDING;
                return TRUE;
            }
        }
    }
    return FALSE;
}

Bool tails_test_throw(struct task_work *work, struct player *record)
{
    struct near_target *target;
    struct task_work *held;
    struct task_work *nearest;
    Float distance;
    struct collision_shape *shape;
    NJS_VECTOR throw;

    if (record->slot == 0) {
        target = near_targets;
    } else {
        target = near_targets_player1;
    }
    if (!record->held) {
        if (work->flags & PLAYER_WORK_HOLDING) {
            work->flags &= ~PLAYER_WORK_HOLDING;
            return TRUE;
        }
        return FALSE;
    }
    if (((record->anim.next == ANIM_THROW_AT &&
          record->anim.frame >= THROW_FRAME) ||
         (record->anim.next == ANIM_AIR_THROW_AT &&
          record->anim.frame >= AIR_THROW_FRAME)) &&
        target->work) {
        /* @hack spelling: held assigned inside the test, where a statement
         * of its own before it would do */
        if ((held = record->held->work) != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            /* @bug logic: throws at the list's first target whatever its
             * kind and id, where no nearer item or enemy follows it. the
             * first is taken untested at 0x8c08cffe, though the loop's tests
             * at 0x8c08d012 and 0x8c08d020 pass over such a target */
            nearest = target->work;
            distance = target->distance;
            while (target->work) {
                if ((target->work->collision->kind == COLLISION_ITEM ||
                     target->work->collision->kind == COLLISION_ENEMY) &&
                    target->work->id != FALL_KIND_NORMAL &&
                    target->distance < distance) {
                    distance = target->distance;
                    nearest = target->work;
                }
                target++;
            }
            shape = nearest->collision->shapes;
            throw.x = nearest->pos.x + shape->center.x - work->pos.x;
            throw.y = nearest->pos.y + shape->center.y - work->pos.y;
            throw.z = nearest->pos.z + shape->center.z - work->pos.z;
            throw.x = njSqrt((throw.x * throw.x) + (throw.z * throw.z));
            throw.z = 0.0F;
            njUnitVector(&throw);
            throw.x *= THROW_SPEED;
            throw.y *= THROW_SPEED;
            turn_to_world(work, &throw);
            TASK_MOTION_OF(record->held)->spd = throw;
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
            throw.x = THROW_AHEAD_X;
            throw.y = THROW_AHEAD_Y;
            throw.z = 0.0F;
            turn_to_world(work, &throw);
            TASK_MOTION_OF(record->held)->spd = throw;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        voice_play_priority_2(VOICE_THROW);
        return TRUE;
    } else if (record->anim.next == ANIM_SET_DOWN &&
               record->anim.frame <= SET_DOWN_FRAME) {
        /* @hack spelling: held assigned inside the test, where a statement
         * of its own before it would do */
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

Bool tails_test_throw_at(struct task *task, struct task_work *work,
                         struct player_motion *motion, struct player *record)
{
    struct task *target;
    struct task_work *held;
    struct task_work *aim;
    struct collision_shape *shape;
    NJS_VECTOR throw;

    if (!record->held) {
        if (work->flags & PLAYER_WORK_HOLDING) {
            work->flags &= ~PLAYER_WORK_HOLDING;
            return TRUE;
        }
        return FALSE;
    }
    if (record->anim.next == ANIM_THROW_AT_TOUCH &&
        record->anim.frame >= THROW_AT_TOUCH_FRAME &&
        (target = collision_first_with_id(task, SHAPE_ID_FACED)) != NULL) {
        /* @hack spelling: held assigned inside the test, where a statement
         * of its own before it would do */
        if ((held = record->held->work) != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            aim = target->work;
            shape = aim->collision->shapes;
            throw.x = aim->pos.x + shape->center.x - held->pos.x;
            throw.y = aim->pos.y + shape->center.y - held->pos.y;
            throw.z = aim->pos.z + shape->center.z - held->pos.z;
            throw.x = njSqrt((throw.x * throw.x) + (throw.z * throw.z));
            throw.z = 0.0F;
            njUnitVector(&throw);
            throw.x *= THROW_SPEED;
            throw.y *= THROW_SPEED;
            turn_to_world(work, &throw);
            TASK_MOTION_OF(record->held)->spd = throw;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        voice_play_priority_2(VOICE_THROW);
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        return TRUE;
    } else if (record->anim.next == ANIM_THROW_AT_TOUCH &&
               record->anim.frame >= THROW_AT_TOUCH_FRAME) {
        held = record->held->work;
        if (held != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            throw.x = THROW_AHEAD_X;
            throw.y = THROW_AHEAD_Y;
            throw.z = 0.0F;
            turn_to_world(work, &throw);
            TASK_MOTION_OF(record->held)->spd = throw;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        voice_play_priority_2(VOICE_THROW);
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        return TRUE;
    }
    return FALSE;
}

void tails_drop_held(struct task_work *work, struct player_motion *motion,
                     struct player *record)
{
    struct task_work *held;
    struct collision_work *collision;

    if (!record->held) {
        return;
    }
    held = record->held->work;
    /* @hack spelling: the test of the latest touch in an else after the
     * return, with the collision record assigned inside it, where a test of
     * its own after the return would do */
    if (player_action_flags(work, record) & ACTION_FLAG_200) {
        return;
    } else if ((collision = work->collision)->hit &&
               held == collision->hit->owner->work) {
        return;
    }
    player_release_held(work, motion, record);
    if (record->held) {
        return;
    }
    if (work->flags & PLAYER_WORK_GROUNDED) {
        player_stand(work, motion, record);
    } else {
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FALL;
    }
}

/* set the held task's pos by his hands, where player_action_flags gives the
 * task ACTION_FLAG_200. it reads the hands' places as his draw last kept them.
 * the task must have a work, unchecked */
static void tails_place_held(struct task_work *work, struct player *record)
{
    struct task_work *held;
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    if (!record->held) {
        return;
    }
    if (!(player_action_flags(work, record) & ACTION_FLAG_200)) {
        return;
    }
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
        dir.x = record->unk_284.x + record->unk_290.x;
        dir.y = record->unk_284.y + record->unk_290.y;
        dir.z = record->unk_284.z + record->unk_290.z;
        if (njScalor(&dir) == 0.0F) {
            dir.x = dir.y = dir.z = 0.0F;
        } else {
            njUnitVector(&dir);
        }
        pos.x += dir.x * HOLD_OUT;
        pos.y += dir.y * HOLD_OUT;
        pos.z += dir.z * HOLD_OUT;
    }
    held->pos = pos;
}

Bool tails_test_tail_attack(struct task_work *work, struct player *record,
                            struct player *tails)
{
    if ((player_pad_records[record->slot].press & pad_action_buttons) &&
        !(work->flags & PLAYER_WORK_ON_PATH)) {
        work->mode = PLAYER_MODE_TAILS_TAIL_ATTACK;
        record->anim.next = ANIM_TAIL_ATTACK;
        record->anim.after = ANIM_STAND;
        tails->by_character.tails.state |= TAILS_ATTACKING;
        sound_play(SOUND_TAIL_ATTACK, NULL, 0, 0);
        voice_play_priority_2(VOICE_TAIL_ATTACK);
        return TRUE;
    }
    return FALSE;
}

void tails_fly_start(struct task_work *work, struct player_motion *motion,
                     struct player *record, struct player *tails)
{
    record->spd.y = 0.0F;
    work->flags &= ~REQUEST_CLEARS;
    work->mode = PLAYER_MODE_TAILS_FLY;
    record->anim.next = ANIM_FLY;
    tails->by_character.tails.state |= TAILS_FLYING;
    voice_play_priority_2(VOICE_FLY);
}

void tails_fly(struct task_work *work, struct player_motion *motion,
               struct player *record, struct player *tails)
{
    Float strength;
    Angle heading;
    NJS_VECTOR acc;
    Float height;
    Angle turn;
    int step;

    step = FLAP_STEP;
    step += step * tails->by_character.tails.climb / 2;
    if ((tails->by_character.tails.flap += step) >= FLAP_PERIOD) {
        tails->by_character.tails.flap = 0;
        sound_play_at_for(SOUND_FLAP, work, FLAP_PRIORITY, FLAP_VOLUME,
                          FLAP_DURATION, &work->pos);
    }
    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    acc = motion->acc;
    acc.x += world_gravity.x * record->consts.gravity;
    acc.y += world_gravity.y * record->consts.gravity;
    acc.z += world_gravity.z * record->consts.gravity;
    height = work->pos.y - record->ground.below_y;
    if (record->spd.y < LIFT_FAST_FALL && height >= LIFT_LOW &&
        height < LIFT_HIGH) {
        acc.y += (LIFT_FAST_FALL - record->spd.y) * LIFT_SHARE;
    } else if (record->spd.y < LIFT_SLOW_FALL && height > 0.0F &&
               height < LIFT_LOW) {
        acc.y += (LIFT_SLOW_FALL - record->spd.y) * LIFT_SHARE;
    }
    tails->by_character.tails.climb = 0;
    if (pad_jump_held[record->slot]) {
        acc.y = 0.0F;
    } else if (pad_action_held[record->slot]) {
        acc.y = 0.0F;
    } else if (record->spd.y < GLIDE_FALL) {
        acc.y += (GLIDE_FALL - record->spd.y) * GLIDE_SHARE;
    }
    turn_from_world(work, &acc);
    acc.x += record->consts.air_resist * record->spd.x;
    acc.y += record->consts.resist.y * record->spd.y;
    acc.z += record->consts.resist.z * record->spd.z;
    if (player_read_stick(work, record, &heading, &strength) == 0) {
        strength = 0.0F;
        motion->aim_y = work->ang.y;
    } else {
        turn = angle_distance(work->ang.y, heading);
        if (record->spd.x > record->consts.running_speed &&
            turn > STEER_BRAKE_TURN) {
            strength = record->consts.air_brake * strength;
        } else if (turn > STEER_TURN) {
            motion->aim_y = heading;
            strength = 0.0F;
        } else {
            motion->aim_y = heading;
            strength *= STEER_SHARE;
        }
        player_turn_toward(work, motion, record, motion->aim_y);
    }
    acc.x += strength;
    record->accel = acc;
}

/* SET_ATTACK sets the attack of work's first shape to level, and SET_GUARD its
 * guard */
/* @hack redundant: the collision record kept in a volatile local, assigned
 * inside the expression. it gives the frame a store of the record that
 * nothing reads */
#define SET_ATTACK(work, level)                                                \
    {                                                                          \
        struct collision_work *volatile collision_;                            \
        struct collision_shape *shape_ =                                       \
            (collision_ = (work)->collision)->shapes;                          \
        shape_->levels = (shape_->levels & ~SHAPE_ATTACK) |                    \
                         (SHAPE_ATTACK_LEVEL(level) & SHAPE_ATTACK);           \
    }
#define SET_GUARD(work, level)                                                 \
    {                                                                          \
        struct collision_work *volatile collision_;                            \
        struct collision_shape *shape_ =                                       \
            (collision_ = (work)->collision)->shapes;                          \
        shape_->levels = (shape_->levels & ~SHAPE_GUARD) |                     \
                         (SHAPE_GUARD_LEVEL(level) & SHAPE_GUARD);             \
    }

void tails_set_shapes(struct task_work *work, struct player *record,
                      struct player *tails)
{
    int i;

    work->collision->shapes[SHAPE_BODY].radius = BODY_RADIUS;
    SET_ATTACK(work, 0);
    SET_GUARD(work, 0);
    for (i = SHAPE_BODY + 1; i < SHAPES; i++) {
        work->collision->shapes[i].flags |= SHAPE_OFF;
    }
    if (work->flags & PLAYER_WORK_BALL) {
        SET_ATTACK(work, BALL_LEVEL);
        SET_GUARD(work, BALL_LEVEL);
    }
    switch (work->mode) {
    case PLAYER_MODE_TAILS_TAIL_ATTACK:
        work->collision->shapes[SHAPE_TIP_PLUS_Z].flags &= ~SHAPE_OFF;
        work->collision->shapes[SHAPE_TIP_MINUS_Z].flags &= ~SHAPE_OFF;
        work->collision->shapes[SHAPE_TIP_PLUS_Z].center =
            tails->by_character.tails.tip_plus_z;
        work->collision->shapes[SHAPE_TIP_MINUS_Z].center =
            tails->by_character.tails.tip_minus_z;
        break;
    case PLAYER_MODE_TAILS_FLY:
        work->collision->shapes[SHAPE_BODY].radius = FLY_BODY_RADIUS;
        work->collision->shapes[SHAPE_ABOVE].flags &= ~SHAPE_OFF;
        work->collision->shapes[SHAPE_ABOVE].center.x =
            work->pos.x - (njCos(work->ang.y) * ABOVE_BACK);
        work->collision->shapes[SHAPE_ABOVE].center.y =
            work->pos.y + ABOVE_HEIGHT;
        work->collision->shapes[SHAPE_ABOVE].center.z =
            work->pos.z - (njSin(work->ang.y) * ABOVE_BACK);
        break;
    }
}

Bool fun_8c08dc00(int slot)
{
    struct player *record = player_tasks[slot]->extra.p;

    if (record == NULL) {
        return FALSE;
    }
    record->by_character.tails.unk_30b |= TAILS_30B_UNK_1;
    return TRUE;
}

Bool fun_8c08dc24(int slot)
{
    struct player *record = player_tasks[slot]->extra.p;

    if (record == NULL) {
        return FALSE;
    }
    record->by_character.tails.unk_30b |= TAILS_30B_UNK_1;
    return TRUE;
}
