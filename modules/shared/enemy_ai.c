/* the ai, an enemy that a stage's object list names "E AI". it stands or
 * flies, and fires bullets, lasers or capturing bullets at a player in
 * range */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/light.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/capturing_bullet.h"
#include "shared/dust.h"
#include "shared/enemy_ai.h"
#include "shared/enemy_ai_art.h"
#include "shared/enemy_bullet.h"
#include "shared/enemy_jet.h"
#include "shared/enemy_laser.h"
#include "shared/enemy_light.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the ai's work->mode, which skips 4: no path sets it */
enum ai_mode {
    MODE_SETUP = 0,
    MODE_CROUCH = 1, /* a fixed kind waits, crouched, for a player in range */
    MODE_FIRE = 2, /* a standing ai fires at a player in range */
    MODE_FLY = 3, /* a moving kind flies near home until a player is in range */
    MODE_FLY_FIRE = 5, /* a moving kind fires at a player in range */
    MODE_HIDDEN = 6, /* an EX kind waits, undrawn, for a player near it */
    MODE_DROP = 7, /* an EX kind falls to the floor */
    MODE_DEFLECT = 8, /* a shielder's guard has just turned a hit away */
    MODE_ONLY_STAND = 9, /* AI_ONLY_STAND stands until it is hit */
    MODE_DEFEATED = 10, /* a hit has beaten the ai, which its next step kills */
    MODE_DEAD = 11 /* killed by ai_kill */
};

/* the ai's bits in work->flags */
/* the ai lies farther than the range from home, across x and z */
#define AI_AWAY 0x1000
/* a player lies in range, or a walker's lock holds the shielder as its
 * lock_task */
#define AI_ALERT 0x2000

/* the ai's own fields of its enemy_work */
/* the interval between bursts or lasers, in steps. a burst or a laser waits
 * for work->timer to pass it */
#define INTERVAL(enemy) ((enemy)->unk_048)
/* a gun or capturing kind's shots in a burst. work->tick counts them */
#define SHOTS(enemy) ((enemy)->unk_047)
/* a laser kind's or the shielder's laser length, in SHOTS's byte */
#define LASER_LENGTH(enemy) ((enemy)->unk_047)
#define SHOT_SPEED(enemy) ((enemy)->unk_094)
/* the steps left before ai_turn picks a new target */
#define TARGET_STEPS(enemy) ((enemy)->unk_018)
/* what each step adds to a moving kind's spd.x and spd.z. z stays 0, since
 * nothing sets it */
#define ACCEL(enemy) ((enemy)->unk_00c)
#define BOB_ANGLE(enemy) ((enemy)->unk_1c4)
/* the gun's muzzle in the world, as the last draw found it. the muzzle's
 * light shows there */
#define MUZZLE(enemy) ((enemy)->unk_088)

/* the indexes of ai_actions */
#define ACTION_LAND 0
#define ACTION_CROUCH 1
#define ACTION_RISE 2
#define ACTION_STAND 3
/* AI_LASER_FIXED holds ACTION_CHARGE while its laser charges */
#define ACTION_CHARGE 4
#define ACTION_SHOOT 5
#define ACTION_TURN 6
/* the turn of ai_unused_turn_motion. nothing asks for it, so it never
 * plays */
#define ACTION_UNUSED_TURN 7
#define ACTION_FLY 8 /* a moving kind's flight, and an EX kind's fall */
#define ACTION_FLY_CHARGE 9
#define ACTION_FLY_SHOOT 10
/* a shielder fires its laser as ACTION_SHIELDER_OPEN ends. it holds
 * ACTION_SHIELDER_OPEN_HOLD while the laser charges */
#define ACTION_SHIELDER_OPEN 11
#define ACTION_SHIELDER_OPEN_HOLD 12
#define ACTION_SHIELDER_CLOSE 13
#define ACTION_SHIELDER_DEFLECT 14
#define ACTION_SHIELDER_STAND 15
#define ACTION_SHIELDER_TURN 16

/* the range is scl.z + RANGE, once the setup's enemy_set_offset has rounded
 * scl.z toward 0. near_distance2 holds the range squared */
#define RANGE 60.0F
/* the least interval a gun or capturing kind takes */
#define INTERVAL_MIN 60
/* AI_ONLY_STAND's interval, the most INTERVAL holds. no mode of
 * AI_ONLY_STAND reads it */
#define INTERVAL_MAX 0x7fff
/* a fixed kind sets its timer to INTERVAL - FIRST_WAIT as it starts to rise.
 * an EX kind does the same once it has landed */
#define FIRST_WAIT 60
/* a target lies at random in a square about home, whose side is
 * TARGET_SPREAD times the range. a pick sets TARGET_STEPS to
 * TARGET_STEPS_LEAST plus a random part of under TARGET_STEPS_RANDOM */
#define TARGET_SPREAD 0.2F
#define TARGET_STEPS_LEAST 60.0F
#define TARGET_STEPS_RANDOM 180.0F
/* a step moves ACTION_SHOOT's frame by SHOOT_ACTION_SPEED, and
 * ACTION_FLY_SHOOT's by FLY_SHOOT_ACTION_SPEED. a shot leaves in the step whose
 * frame lies from SHOT_FRAME up to SHOT_FRAME_END, or FLY_SHOT_FRAME_END. each
 * shoot action has one such step */
#define SHOOT_ACTION_SPEED 0.15F
#define FLY_SHOOT_ACTION_SPEED 0.16F
#define SHOT_FRAME 1.0F
#define SHOT_FRAME_END (SHOT_FRAME + SHOOT_ACTION_SPEED)
#define FLY_SHOT_FRAME_END (SHOT_FRAME + FLY_SHOOT_ACTION_SPEED)
/* the most a shot turns, in Angle units, from level and from the ai's
 * heading */
#define AIM_PITCH 0x1800
#define AIM_YAW 0x800
/* a standing ai plays its turn action in each step that turns it TURN_MIN
 * Angle units or more */
#define TURN_MIN 32
/* the turn actions' frames at which a foot lands, where the ai plays
 * STEP_SOUND. ACTION_UNUSED_TURN takes ACTION_SHIELDER_TURN's */
#define TURN_FOOT_1 1.0F
#define TURN_FOOT_2 3.0F
#define SHIELDER_TURN_FOOT_1 2.0F
#define SHIELDER_TURN_FOOT_2 6.0F
/* the heights above pos of the center of the ai's first collision shape. a
 * rise lifts the center from CROUCH_HEIGHT to STAND_HEIGHT by RISE_SPEED a
 * step */
#define STAND_HEIGHT 25.0F
#define CROUCH_HEIGHT 10.0F
#define RISE_SPEED 0.4F
/* the muzzle's x in the axes of each model's weapon object */
#define MUZZLE_X (-3.0F)
/* a shielder's deflect lasts DEFLECT_STEPS + 1 steps */
#define DEFLECT_STEPS 6

/* a moving kind's spd.x, its speed along its heading, stays within FLY_SPEED
 * of 0. a player's touch sets it to -FLY_SPEED. each step adds ACCEL_SLOW to
 * it below ACCEL_SLOW_BELOW, and ACCEL_FAST at or above */
#define FLY_SPEED 1.0F
#define ACCEL_SLOW_BELOW (-0.5F)
#define ACCEL_SLOW 0.01F
#define ACCEL_FAST 0.1F
/* a moving kind sinks while its bottom lies above the floor. a sink takes
 * SINK_STEP off spd.y a step, down to SINK_MAX. otherwise spd.y scales by
 * FALL_DAMPING a step, and becomes 0 once within FALL_STOP of 0 */
#define SINK_STEP 0.05F
#define SINK_MAX (-2.0F)
#define FALL_DAMPING 0.95F
#define FALL_STOP 0.1F
/* a moving kind bobs BOB_HEIGHT above and below its path. its bob's angle
 * moves BOB_STEP a step */
#define BOB_HEIGHT 1.0F
#define BOB_STEP 0x300
/* an EX kind's fall speeds up by DROP_STEP a step. its spd.y stays from
 * DROP_SLOWEST down to DROP_FASTEST */
#define DROP_STEP 0.1F
#define DROP_SLOWEST (-1.0F)
#define DROP_FASTEST (-3.0F)
/* the ring of dust an EX kind raises as it lands */
#define DUST_RADIUS 8.0F
#define DUST_SCALE 4.0F
#define DUST_PUFFS 8

/* the index in lights of the light the ai draws in */
#define AI_LIGHT 8
/* the muzzle's light, as enemy_light_create's kind. each step under AI_ALERT
 * shows one. MUZZLE_LIGHT_COMING shows instead while the ai drops or
 * rises */
#define MUZZLE_LIGHT 3
#define MUZZLE_LIGHT_COMING 4
/* the points score_add_enemy adds for beating an ai, and SCORE_SHIELDER for
 * a shielder. they double where player_touch_attacks finds Sonic's or
 * Shadow's combo */
#define SCORE 100
#define SCORE_SHIELDER 200
/* the ball a magic hand makes of a beaten ai grows to BALL_SCALE. it draws
 * the ai's model off its center by BALL_OFFSET_Y in y and BALL_OFFSET_Z in
 * z */
#define BALL_SCALE 0.12F
#define BALL_OFFSET_Y (-17.0F)
#define BALL_OFFSET_Z (-5.0F)
/* the shielder's guard throws a player back at THROW_SPEED along the
 * player's own x, and THROW_LIFT up */
#define THROW_SPEED (-2.5F)
#define THROW_LIFT 1.0F
#define SIGHT_DISTANCE 80.0F
#define SIGHT_ANGLE ((Angle)(160 * ANGLE_PER_DEGREE))
#define TURN_SPEED 0x800
#define SHADOW_SIZE 6.0F
#define SHADOW_RATIO 1.5F
/* the texture task's level */
#define TEXTURE_LEVEL 0

#define STEP_SOUND 0x400b
#define STEP_VOLUME (-10)
#define LANDING_VOLUME 100 /* STEP_SOUND's volume as an EX kind lands */
#define ALERT_SOUND 0x400c
#define ALERT_VOLUME 110
#define JET_SOUND 0x401c
#define JET_VOLUME (-15)
#define THROW_SOUND 0x401f
#define THROW_VOLUME 10
#define THROW_VIBRATION_POWER 3
#define THROW_VIBRATION_FREQ 7

/* ai_draw keeps these for ai_find_muzzle. ai_view_inverse is the inverse of
 * the matrix current as the draw starts. after a node's matrix, it takes a
 * point in the node's axes to the world. ai_drawn is the ai the draw draws */
static NJS_MATRIX ai_view_inverse;
static struct enemy_work *ai_drawn;

/* the shapes each kind takes of ai_shapes: the shielder the second SHAPES,
 * and every other kind the first. players, enemies and objects can't push
 * the shielder's */
#define SHAPES 2

static struct collision_shape ai_shapes[2 * SHAPES] = {
#include "shared/enemy_ai/ai_shapes.inc"
};

/* the models enemy_break breaks a beaten ai into, null-ended. every kind
 * breaks into the gun model's pieces, and the right shin's piece comes
 * twice */
static NJS_CNK_MODEL *ai_pieces[16] = {
#include "shared/enemy_ai/ai_pieces.inc"
};

static struct enemy_action ai_actions[17] = {
#include "shared/enemy_ai/ai_actions.inc"
};

/* the task that keeps ai_texlist loaded, or null while none does. its
 * destructor releases the list */
static struct task *ai_texture_task = NULL;

static void ai_destroy(struct task *task);
static void ai_draw(struct task *task);

static void ai_texture_destroy(struct task *task)
{
    njReleaseTexture(&ai_texlist);
    ai_texture_task = NULL;
}

static void ai_texture_step(struct task *task)
{
}

static void ai_setup(struct task *task, struct task_work *work)
{
    struct enemy_work *enemy;

#ifdef NONMATCHING
    /* Without an action schedule the first blend never advances. */
    if (ai_actions[ACTION_LAND].blend == 0.0F) {
        placement_kill(task);
        return;
    }
#endif
    work->submode = njRoundOff(work->scl.x);
    if (work->submode >= AI_KINDS || work->submode < 0) {
        work->submode = AI_GUN_FIXED;
    }
    if (work->submode == AI_ONLY_STAND) {
        enemy = enemy_work_alloc_grouped_no_hunt(task);
    } else {
        enemy = enemy_work_alloc_grouped(task);
    }
    if (work->submode == AI_SHIELDER) {
        collision_init(task, &ai_shapes[SHAPES], SHAPES, COLLISION_ENEMY);
    } else {
        collision_init(task, ai_shapes, SHAPES, COLLISION_ENEMY);
    }
    if (work->submode == AI_GUN_FIXED || work->submode == AI_GUN_MOVING ||
        work->submode == AI_GUN_FIXED_EX) {
        SHOTS(enemy) = work->ang.x;
        INTERVAL(enemy) = work->ang.z;
        if (INTERVAL(enemy) < INTERVAL_MIN) {
            INTERVAL(enemy) = INTERVAL_MIN;
        }
        SHOT_SPEED(enemy) = work->scl.y;
        enemy->motion.object = ai_gun_object;
        enemy_bullet_load();
    } else if (work->submode == AI_CAPT_FIXED ||
               work->submode == AI_CAPT_FIXED_EX) {
        SHOTS(enemy) = work->ang.x;
        INTERVAL(enemy) = work->ang.z;
        if (INTERVAL(enemy) < INTERVAL_MIN) {
            INTERVAL(enemy) = INTERVAL_MIN;
        }
        SHOT_SPEED(enemy) = work->scl.y;
        enemy->motion.object = ai_gun_object;
        enemy_bullet_load();
        capturing_bullet_load_texture();
    } else if (work->submode == AI_LASER_FIXED ||
               work->submode == AI_LASER_MOVING ||
               work->submode == AI_SHIELDER ||
               work->submode == AI_LASER_FIXED_EX) {
        LASER_LENGTH(enemy) = work->ang.x;
        INTERVAL(enemy) = work->ang.z;
        SHOT_SPEED(enemy) = work->scl.y;
        enemy_laser_load();
        if (work->submode == AI_SHIELDER) {
            enemy->motion.object = &ai_shielder_object;
        } else {
            enemy->motion.object = &ai_laser_object;
        }
    } else if (work->submode == AI_ONLY_STAND) {
        SHOTS(enemy) = 0;
        INTERVAL(enemy) = INTERVAL_MAX;
        SHOT_SPEED(enemy) = 0.0F;
        enemy->motion.object = ai_gun_object;
    }
    work->ang.x = 0;
    work->ang.z = 0;
    /* nothing in this source reads scl.y after the setup */
    work->scl.y = 2.0F;
    enemy_set_offset(work, enemy);
    enemy->sight_distance2 = SIGHT_DISTANCE * SIGHT_DISTANCE;
    enemy->sight_angle = SIGHT_ANGLE;
    enemy->near_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->spd.x = FLY_SPEED;
    enemy->turn_speed = TURN_SPEED;
    enemy_light_load_texture();
    enemy_set_home(work, enemy);
    enemy->motion.table = ai_actions;
    if (work->submode == AI_GUN_MOVING || work->submode == AI_LASER_MOVING) {
        work->mode = MODE_FLY;
        enemy->motion.next = ACTION_FLY;
        enemy->bottom = -12.0F;
        enemy->friction = 0.9999F;
        sound_play_task_held_at(JET_SOUND, task, 0, JET_VOLUME);
    } else if (work->submode == AI_SHIELDER) {
        work->mode = MODE_FIRE;
        enemy->motion.next = ACTION_SHIELDER_STAND;
        enemy->bottom = -6.0F;
        work->id = WORK_ID_LOCKON_GUARD;
    } else if (work->submode == AI_GUN_FIXED_EX ||
               work->submode == AI_LASER_FIXED_EX ||
               work->submode == AI_CAPT_FIXED_EX) {
        work->mode = MODE_HIDDEN;
        enemy->motion.next = ACTION_FLY;
        enemy->bottom = -6.0F;
        enemy->floor_bounce = 0.0F;
    } else if (work->submode == AI_ONLY_STAND) {
        work->mode = MODE_ONLY_STAND;
        enemy->motion.next = ACTION_STAND;
        enemy->bottom = -6.0F;
        enemy->floor_bounce = 0.0F;
    } else {
        work->mode = MODE_CROUCH;
        enemy->motion.next = ACTION_CROUCH;
        work->collision->shapes->center.y = CROUCH_HEIGHT;
    }
    enemy_motion_step(&enemy->motion);
    enemy->shadow_size = SHADOW_SIZE;
    enemy->shadow_ratio = SHADOW_RATIO;
    /* @hack spelling: the two bits set by two statements, where one |= would
     * do */
    enemy->flags |= ENEMY_LEAVES_CHAOS_DRIVE;
    enemy->flags |= ENEMY_BURST;
    if (placement_unk_20(task)) {
        work->mode = MODE_DEAD;
    } else {
        task->draw = ai_draw;
    }
    task->destructor = ai_destroy;
    enemy_jet_load_texture();
    if (ai_texture_task == NULL) {
        texture_load_pvm("E_AITEX", &ai_texlist);
        ai_texture_task =
            task_create(0, TEXTURE_LEVEL, ai_texture_step, "ManTex");
        ai_texture_task->destructor = ai_texture_destroy;
    }
}

static void ai_pick_target(struct task_work *work, struct enemy_work *enemy)
{
    Float range = njSqrt(enemy->near_distance2);

    enemy->target.x =
        enemy->home.x +
        (TARGET_SPREAD * (range * ((rand() * RAND_SCALE) - 0.5F)));
    enemy->target.y = enemy->home.y;
    enemy->target.z =
        enemy->home.z +
        (TARGET_SPREAD * (range * ((rand() * RAND_SCALE) - 0.5F)));
    enemy->aim_y = (Angle)(atan2f(enemy->target.x - work->pos.x,
                                  enemy->target.z - work->pos.z) *
                           ANGLE_PER_RADIAN) -
                   QUARTER_TURN;
}

/* only a moving kind sets AI_AWAY, so only a moving kind turns toward a
 * target */
static void ai_turn(struct task_work *work, struct enemy_work *enemy)
{
    int player;

    if (game_state.time_stopped) {
        return;
    }
    if (work->flags & AI_AWAY) {
        if (TARGET_STEPS(enemy)-- < 0) {
            ai_pick_target(work, enemy);
            TARGET_STEPS(enemy) =
                (rand() * (TARGET_STEPS_RANDOM * RAND_SCALE)) +
                TARGET_STEPS_LEAST;
        } else {
            work->ang.y =
                angle_approach(work->ang.y, enemy->aim_y, enemy->turn_speed);
        }
    } else if (work->flags & AI_ALERT) {
        player = player_nearest(&work->pos);
        if (player == 0 || player == 1) {
            enemy_turn_to_player(work, enemy, player);
        }
    } else if (work->submode == AI_GUN_MOVING ||
               work->submode == AI_LASER_MOVING) {
        if (TARGET_STEPS(enemy)-- < 0 ||
            ((work->collision->flags & COLLISION_TOUCHED) &&
             work->collision->hit->kind != COLLISION_PLAYER)) {
            ai_pick_target(work, enemy);
            TARGET_STEPS(enemy) =
                (rand() * (TARGET_STEPS_RANDOM * RAND_SCALE)) +
                TARGET_STEPS_LEAST;
        } else {
            work->ang.y =
                angle_approach(work->ang.y, enemy->aim_y, enemy->turn_speed);
        }
    }
}

static void ai_mark_away(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: the local distance, where enemy_home_distance2_flat's
     * result compared in place would do */
    Float distance;

    if (game_state.time_stopped) {
        return;
    }
    distance = enemy_home_distance2_flat(work, enemy);
    if (distance > enemy->near_distance2) {
        work->flags |= AI_AWAY;
    } else {
        work->flags &= ~AI_AWAY;
    }
}

static void ai_check_hit(struct task *task, struct task_work *work,
                         struct enemy_work *enemy)
{
    int score;
    int player;

    if (!enemy_take_hit(work, enemy)) {
        return;
    }
    /* @hack spelling: an empty arm under game_state.time_stopped, where the
     * chain nested under !game_state.time_stopped would do */
    if (game_state.time_stopped) {
    } else if (enemy->flags & ENEMY_IN_MAGIC_HAND) {
        NJS_VECTOR offset;
        NJS_VECTOR scl;

        player = enemy->shot_by;
        /* @hack spelling: offset's parts set from z to x, where x first would
         * do */
        offset.z = BALL_OFFSET_Z;
        offset.y = BALL_OFFSET_Y;
        offset.x = 0.0F;
        /* @hack spelling: one assignment to the three, where a statement for
         * each would do */
        scl.x = scl.y = scl.z = BALL_SCALE;
        switch (work->submode) {
        case AI_GUN_FIXED:
        case AI_GUN_MOVING:
        case AI_CAPT_FIXED:
        case AI_GUN_FIXED_EX:
        case AI_CAPT_FIXED_EX:
        case AI_ONLY_STAND:
            enemy_ball_create(player, ai_gun_object, &ai_texlist, &scl,
                              &offset);
            break;
        case AI_SHIELDER:
            enemy_ball_create(player, &ai_shielder_object, &ai_texlist, &scl,
                              &offset);
            break;
        case AI_LASER_FIXED:
        case AI_LASER_MOVING:
        case AI_LASER_FIXED_EX:
            enemy_ball_create(player, &ai_laser_object, &ai_texlist, &scl,
                              &offset);
            break;
        }
    } else if (work->id == WORK_ID_LOCKON_GUARD) {
        if (enemy->lock_task != NULL && !collision_touched(task)) {
            enemy->motion.next = ACTION_SHIELDER_DEFLECT;
            work->timer = 0;
            work->tick = 0;
            work->mode = MODE_DEFLECT;
            work->flags &= ~WORK_HURT;
            enemy->flags &= ~ENEMY_HIT;
            return;
            /* @hack spelling: else after the return, where a new if would
             * do */
        } else if (collision_touched(task) &&
                   !player_hit_attacks(work->collision->owner,
                                       work->collision->hit->owner)) {
            player = player_slot_of(work->collision->hit->owner);
            if (player == 0 || player == 1) {
                if (collision_next_player(task)) {
                    NJS_VECTOR spd;
                    struct angles ang;

                    spd.x = THROW_SPEED;
                    spd.y = THROW_LIFT;
                    spd.z = 0.0F;
                    ang.x = 0;
                    ang.y = work->ang.y + (HALF_TURN + QUARTER_TURN);
                    ang.z = 0;
                    player_launch_speed(player, &spd, &ang, 0);
                }
                enemy->motion.next = ACTION_SHIELDER_DEFLECT;
                work->timer = 0;
                work->tick = 0;
                work->mode = MODE_DEFLECT;
                work->flags &= ~WORK_HURT;
                enemy->flags &= ~ENEMY_HIT;
                sound_play_at(THROW_SOUND, NULL, 0, THROW_VOLUME, &work->pos);
                vibration_start(player, THROW_VIBRATION_POWER,
                                THROW_VIBRATION_FREQ, VIBRATION_UNTIMED);
                return;
            }
        }
    }
    work->mode = MODE_DEFEATED;
    work->collision->shapes->flags |= SHAPE_OFF;
    work->timer = 0;
    enemy_set_home(work, enemy);
    enemy_break(work, ai_pieces, &ai_texlist);
    if (work->submode == AI_SHIELDER) {
        score = SCORE_SHIELDER;
    } else {
        score = SCORE;
    }
    if (player_touch_attacks(task)) {
        score *= 2;
    }
    score_add_enemy(score);
    enemy_defeat(work, enemy);
}

static void ai_mark_player_near_home(struct task_work *work,
                                     struct enemy_work *enemy)
{
    if (enemy_find_player_near_home(work, enemy)) {
        work->flags |= AI_ALERT;
        /* @hack spelling: a return after the find, where else would do */
        return;
    }
    if (work->submode == AI_SHIELDER && enemy->lock_task != NULL) {
        work->flags |= AI_ALERT;
    } else {
        work->flags &= ~AI_ALERT;
    }
}

static void ai_mark_player_near_offset(struct task_work *work,
                                       struct enemy_work *enemy)
{
    if (enemy_find_player_near_offset(work, enemy)) {
        work->flags |= AI_ALERT;
        /* @hack spelling: a return after the find, where else would do */
        return;
    }
    if (work->submode == AI_SHIELDER && enemy->lock_task != NULL) {
        work->flags |= AI_ALERT;
    } else {
        work->flags &= ~AI_ALERT;
    }
}

static void ai_move(struct task *task, struct task_work *work,
                    struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    if (enemy->spd.x < ACCEL_SLOW_BELOW) {
        ACCEL(enemy).x = ACCEL_SLOW;
    } else {
        ACCEL(enemy).x = ACCEL_FAST;
    }
    if (collision_next_player(task)) {
        enemy->spd.x = -FLY_SPEED;
    } else {
        enemy->spd.x += ACCEL(enemy).x;
        if (ACCEL(enemy).x <= 0.0F &&
            njAbs(enemy->spd.x) <= njAbs(ACCEL(enemy).x)) {
            enemy->spd.x = 0.0F;
        }
        enemy->spd.z += ACCEL(enemy).z;
    }
    if (enemy->spd.x > FLY_SPEED) {
        enemy->spd.x = FLY_SPEED;
    } else if (enemy->spd.x < -FLY_SPEED) {
        enemy->spd.x = -FLY_SPEED;
    }
    work->pos.x += (enemy->spd.x * njCos(work->ang.y)) +
                   (enemy->spd.z * njSin(work->ang.y));
    work->pos.y += enemy->spd.y;
    work->pos.z += -(enemy->spd.x * njSin(work->ang.y)) +
                   (enemy->spd.z * njCos(work->ang.y));
    /* enemy_meet_stage takes the ai's pos without its bob */
    work->pos.y -= njSin(BOB_ANGLE(enemy)) * BOB_HEIGHT;
    BOB_ANGLE(enemy) += BOB_STEP;
    enemy_meet_stage(work, enemy);
    work->pos.y += njSin(BOB_ANGLE(enemy)) * BOB_HEIGHT;
    if (enemy->probe.pos.y + enemy->bottom >
        enemy->probe.sides[SURFACE_NEG_Y].pos) {
        enemy->spd.y -= SINK_STEP;
        if (enemy->spd.y < SINK_MAX) {
            enemy->spd.y = SINK_MAX;
        }
    } else {
        enemy->spd.y *= FALL_DAMPING;
        if (njAbs(enemy->spd.y) < FALL_STOP) {
            enemy->spd.y = 0.0F;
        }
    }
    enemy_keep_near_home(work, enemy, work->scl.z + RANGE);
}

static void ai_fall(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    work->pos.y += enemy->spd.y;
    enemy_meet_stage(work, enemy);
    if (enemy->motion.action != ACTION_FLY) {
        enemy->spd.y = 0.0F;
        return;
    }
    enemy->spd.y -= DROP_STEP;
    if (enemy->spd.y < DROP_FASTEST) {
        enemy->spd.y = DROP_FASTEST;
        return;
        /* @hack spelling: else after the return, where a new if would do */
    } else if (enemy->spd.y > DROP_SLOWEST) {
        enemy->spd.y = DROP_SLOWEST;
    }
}

/* in the ai's own axes, x points ahead and y up */
static void ai_point_to_world(struct task_work *work, NJS_POINT3 *pos)
{
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, pos, pos);
    njPopMatrixEx();
}

static void ai_vector_to_world(struct task_work *work, NJS_VECTOR *vec)
{
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, work->ang.y);
    njCalcVector(NULL, vec, vec);
    njPopMatrixEx();
}

/* the three jets of a moving kind, which blow back and down from behind
 * it */
static void ai_send_jets(struct task_work *work)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    if (!game_state.time_stopped) {
        pos.x = -7.0F;
        pos.y = 25.0F;
        pos.z = -0.0F;
        ai_point_to_world(work, &pos);
        spd.x = -1.2F;
        spd.y = -0.3F;
        spd.z = 0.0F;
        ai_vector_to_world(work, &spd);
        enemy_jet_create(NULL, ENEMY_JET_MEDIUM, &pos, &spd);
        pos.x = -6.5F;
        pos.y = 23.0F;
        pos.z = -5.0F;
        ai_point_to_world(work, &pos);
        spd.x = -1.0F;
        spd.y = -0.7F;
        spd.z = -0.5F;
        ai_vector_to_world(work, &spd);
        enemy_jet_create(NULL, ENEMY_JET_MEDIUM, &pos, &spd);
        pos.x = -6.5F;
        pos.y = 23.0F;
        pos.z = 5.0F;
        ai_point_to_world(work, &pos);
        spd.x = -1.0F;
        spd.y = -0.7F;
        spd.z = 0.5F;
        ai_vector_to_world(work, &spd);
        enemy_jet_create(NULL, ENEMY_JET_MEDIUM, &pos, &spd);
    }
}

/* turn spd, a shot's speed along x, toward the nearest player from pos */
static void ai_aim(struct task_work *work, NJS_POINT3 *pos, NJS_VECTOR *spd)
{
    NJS_POINT3 target;
    /* @hack spelling: the locals ax and ay, where angle_approach's results
     * passed in place would do */
    Angle ax;
    Angle ay;
    Angle pitch;
    Angle yaw;
    Float distance;
    /* @hack spelling: the local dy, where the difference used in place would
     * do */
    Float dy;

    target = player_work[player_nearest(&work->pos)]->collision->shapes->center;
    distance = njDistanceP2P(pos, &target);
    dy = target.y - pos->y;
    if (distance != 0.0F) {
        pitch = asinf(dy / distance) * ANGLE_PER_RADIAN;
        yaw = atan2f(pos->z - target.z, target.x - pos->x) * ANGLE_PER_RADIAN;
    } else {
        yaw = work->ang.y;
        pitch = 0;
    }
    ax = angle_approach(0, pitch, AIM_PITCH);
    ay = angle_approach(work->ang.y, yaw, AIM_YAW);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, ay);
    njRotateZ(NULL, ax);
    njCalcVector(NULL, spd, spd);
    njPopMatrixEx();
}

/* a shot leaves from a point fixed in the ai's own axes, not from MUZZLE */
static void ai_shoot_bullet(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    if (work->submode == AI_GUN_MOVING) {
        pos.x = 22.6F;
        pos.y = 23.1F;
        pos.z = 3.0F;
    } else {
        pos.x = 20.0F;
        pos.y = 26.5F;
        pos.z = 4.7F;
    }
    ai_point_to_world(work, &pos);
    spd.x = SHOT_SPEED(enemy);
    spd.y = 0.0F;
    spd.z = 0.0F;
    ai_aim(work, &pos, &spd);
    enemy_bullet_fire(task, ENEMY_BULLET_STOPS, &spd, &pos);
}

static void ai_shoot_laser(struct task *task, struct task_work *work,
                           struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    if (work->submode == AI_LASER_MOVING) {
        pos.x = 20.4F;
        pos.y = 20.4F;
        pos.z = 3.6F;
    } else {
        pos.x = 20.4F;
        pos.y = 22.4F;
        pos.z = 4.8F;
    }
    ai_point_to_world(work, &pos);
    spd.x = SHOT_SPEED(enemy);
    spd.y = 0.0F;
    spd.z = 0.0F;
    ai_aim(work, &pos, &spd);
    enemy_laser_fire(task, LASER_LENGTH(enemy), &spd, &pos);
}

static void ai_shoot_capturing_bullet(struct task *task, struct task_work *work,
                                      struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    pos.x = 20.0F;
    pos.y = 26.5F;
    pos.z = 4.7F;
    ai_point_to_world(work, &pos);
    spd.x = SHOT_SPEED(enemy);
    spd.y = 0.0F;
    spd.z = 0.0F;
    ai_aim(work, &pos, &spd);
    capturing_bullet_fire(task, &spd, &pos);
}

static void ai_destroy(struct task *task)
{
    enemy_destroy(task);
    sound_stop_key(task);
    /* @hack redundant: the return after sound_stop_key, where the function's
     * end would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void ai_kill(struct task *task)
{
    placement_kill(task);
    if (task->work->submode == AI_ONLY_STAND) {
        placement_set_unk_20(task);
    }
}

static void ai_crouch(struct task *task, struct task_work *work,
                      struct enemy_work *enemy)
{
    ai_mark_player_near_offset(work, enemy);
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->flags & AI_ALERT) {
        enemy->motion.next = ACTION_RISE;
        work->mode = MODE_FIRE;
        work->timer = INTERVAL(enemy) - FIRST_WAIT;
        sound_play_at(ALERT_SOUND, NULL, 0, ALERT_VOLUME, &work->pos);
    }
    ai_check_hit(task, work, enemy);
    /* @hack redundant: the return after ai_check_hit, where the function's
     * end would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void ai_fire(struct task *task, struct task_work *work,
                    struct enemy_work *enemy)
{
    Angle last_heading;

    ai_mark_player_near_offset(work, enemy);
    if (enemy->motion.next == ACTION_RISE) {
        work->collision->shapes->center.y = float_approach(
            work->collision->shapes->center.y, STAND_HEIGHT, RISE_SPEED);
    }
    if (enemy->motion.action == ACTION_TURN ||
        enemy->motion.action == ACTION_STAND ||
        (enemy->motion.action == ACTION_SHOOT &&
         enemy->motion.frame > SHOT_FRAME_END)) {
        last_heading = work->ang.y;
        ai_turn(work, enemy);
        if (enemy->motion.action == ACTION_TURN ||
            enemy->motion.action == ACTION_STAND) {
            if (angle_distance(last_heading, work->ang.y) >= TURN_MIN) {
                enemy->motion.next = ACTION_TURN;
            } else {
                enemy->motion.next = ACTION_STAND;
            }
        }
    } else if (enemy->motion.action == ACTION_SHIELDER_TURN ||
               enemy->motion.action == ACTION_SHIELDER_STAND ||
               enemy->motion.action == ACTION_SHIELDER_CLOSE) {
        last_heading = work->ang.y;
        ai_turn(work, enemy);
        if (enemy->motion.action == ACTION_SHIELDER_TURN ||
            enemy->motion.action == ACTION_SHIELDER_STAND) {
            if (angle_distance(last_heading, work->ang.y) >= TURN_MIN) {
                enemy->motion.next = ACTION_SHIELDER_TURN;
            } else {
                enemy->motion.next = ACTION_SHIELDER_STAND;
            }
        }
    }
    ai_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED || work->mode == MODE_DEFLECT) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    /* the shielder's guard is down while its laser charges and while it
     * closes */
    if (work->submode == AI_SHIELDER) {
        if (enemy->motion.action == ACTION_SHIELDER_CLOSE ||
            (work->flags & ENEMY_LASER_CHARGING)) {
            work->id = WORK_ID_NONE;
            enemy->flags |= ENEMY_UNK_800;
        } else {
            work->id = WORK_ID_LOCKON_GUARD;
            enemy->flags &= ~ENEMY_UNK_800;
        }
    }
    if ((work->submode == AI_GUN_FIXED && SHOTS(enemy) > 0) ||
        work->submode == AI_CAPT_FIXED) {
        if (work->flags & CAPTURING_BULLET_CAUGHT) {
            work->flags &= ~CAPTURING_BULLET_CAUGHT;
            work->timer = 0;
        }
        if (!(work->flags & AI_ALERT)) {
            work->timer = 0;
            work->tick = 0;
            return;
        }
        if (work->timer > INTERVAL(enemy)) {
            if (enemy->motion.action == ACTION_STAND ||
                enemy->motion.action == ACTION_TURN) {
                enemy->motion.next = ACTION_SHOOT;
            } else if (enemy->motion.action == ACTION_SHOOT) {
                if (enemy->motion.frame >= SHOT_FRAME &&
                    enemy->motion.frame < SHOT_FRAME_END) {
                    if (work->submode == AI_GUN_FIXED ||
                        player_work[player_nearest(&work->pos)]->mode ==
                            PLAYER_MODE_CAUGHT) {
                        ai_shoot_bullet(task, work, enemy);
                        work->tick++;
                    } else {
                        ai_shoot_capturing_bullet(task, work, enemy);
                        work->tick = SHOTS(enemy);
                    }
                    if (work->tick >= SHOTS(enemy)) {
                        work->tick = 0;
                        work->timer = 0;
                    }
                }
            }
        }
    } else if (work->submode == AI_LASER_FIXED ||
               work->submode == AI_SHIELDER) {
        if (!(work->flags & AI_ALERT)) {
            work->timer = 0;
            work->flags &= ~ENEMY_LASER_CHARGING;
        } else if (work->flags & ENEMY_LASER_CHARGING) {
            /* the laser sets the bit anew at each step of its charge */
            work->flags &= ~ENEMY_LASER_CHARGING;
        } else if (work->timer > INTERVAL(enemy)) {
            if (enemy->motion.action == ACTION_STAND ||
                enemy->motion.action == ACTION_TURN) {
                enemy->motion.next = ACTION_CHARGE;
                ai_shoot_laser(task, work, enemy);
            } else if (enemy->motion.action == ACTION_SHIELDER_STAND ||
                       enemy->motion.action == ACTION_SHIELDER_TURN) {
                enemy->motion.next = ACTION_SHIELDER_OPEN;
            } else if (enemy->motion.action == ACTION_SHIELDER_OPEN &&
                       (enemy->motion.flags & ENEMY_MOTION_ENDING)) {
                ai_shoot_laser(task, work, enemy);
            } else if (enemy->motion.action == ACTION_SHIELDER_OPEN_HOLD) {
                enemy->motion.next = ACTION_SHIELDER_CLOSE;
                work->timer = 0;
            } else if (enemy->motion.action == ACTION_CHARGE) {
                enemy->motion.next = ACTION_SHOOT;
                work->timer = 0;
            } else {
                /* @hack redundant: the last else and its return, where the
                 * chain's end would do */
                return;
            }
        }
    }
}

static void ai_hidden(struct task *task, struct task_work *work,
                      struct enemy_work *enemy)
{
    if (enemy_find_player_near(work, enemy)) {
        work->flags |= AI_ALERT;
        work->mode = MODE_DROP;
    } else {
        work->flags &= ~AI_ALERT;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    ai_check_hit(task, work, enemy);
}

static void ai_drop(struct task *task, struct task_work *work,
                    struct enemy_work *enemy)
{
    if (enemy_find_player_near(work, enemy)) {
        work->flags |= AI_ALERT;
    } else {
        work->flags &= ~AI_ALERT;
    }
    ai_turn(work, enemy);
    ai_fall(work, enemy);
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (enemy->motion.action == ACTION_STAND) {
        work->mode = MODE_FIRE;
        if (work->submode == AI_GUN_FIXED_EX) {
            work->submode = AI_GUN_FIXED;
        } else if (work->submode == AI_LASER_FIXED_EX) {
            work->submode = AI_LASER_FIXED;
        } else if (work->submode == AI_CAPT_FIXED_EX) {
            work->submode = AI_CAPT_FIXED;
        }
        work->timer = INTERVAL(enemy) - FIRST_WAIT;
    } else if (enemy->motion.action == ACTION_FLY &&
               (enemy->flags & ENEMY_ON_FLOOR)) {
        enemy->motion.next = ACTION_LAND;
        dust_short_ring(&work->pos, DUST_RADIUS, DUST_SCALE, DUST_PUFFS);
        sound_play_at(STEP_SOUND, NULL, 0, LANDING_VOLUME, &work->pos);
    }
    ai_check_hit(task, work, enemy);
    /* @hack redundant: the return after ai_check_hit, where the function's
     * end would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void ai_fly(struct task *task, struct task_work *work,
                   struct enemy_work *enemy)
{
    ai_mark_away(work, enemy);
    ai_mark_player_near_home(work, enemy);
    ai_turn(work, enemy);
    ai_move(task, work, enemy);
    ai_send_jets(work);
    ai_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED || work->mode == MODE_DEFLECT) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->flags & AI_ALERT) {
        work->timer = 0;
        work->tick = 0;
        work->mode = MODE_FLY_FIRE;
    }
}

static void ai_fly_fire(struct task *task, struct task_work *work,
                        struct enemy_work *enemy)
{
    ai_mark_away(work, enemy);
    ai_mark_player_near_home(work, enemy);
    if (work->submode == AI_GUN_MOVING ||
        (work->submode == AI_LASER_MOVING &&
         !(work->flags & ENEMY_LASER_CHARGING))) {
        ai_turn(work, enemy);
        ai_move(task, work, enemy);
    }
    ai_send_jets(work);
    ai_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED || work->mode == MODE_DEFLECT) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->submode == AI_GUN_MOVING && SHOTS(enemy) > 0) {
        if (!(work->flags & AI_ALERT)) {
            work->timer = 0;
            work->tick = 0;
            work->mode = MODE_FLY;
            return;
        }
        if (work->timer > INTERVAL(enemy)) {
            if (enemy->motion.action == ACTION_FLY) {
                enemy->motion.next = ACTION_FLY_SHOOT;
            } else if (enemy->motion.action == ACTION_FLY_SHOOT) {
                if (enemy->motion.frame >= SHOT_FRAME &&
                    enemy->motion.frame < FLY_SHOT_FRAME_END) {
                    ai_shoot_bullet(task, work, enemy);
                    work->tick++;
                }
                if (work->tick >= SHOTS(enemy)) {
                    work->timer = 0;
                    work->tick = 0;
                }
            }
        }
    } else if (work->submode == AI_LASER_MOVING) {
        if (!(work->flags & AI_ALERT)) {
            work->timer = 0;
            work->flags &= ~ENEMY_LASER_CHARGING;
            work->mode = MODE_FLY;
        } else if (work->flags & ENEMY_LASER_CHARGING) {
            work->flags &= ~ENEMY_LASER_CHARGING;
        } else if (work->timer > INTERVAL(enemy)) {
            if (enemy->motion.action == ACTION_FLY) {
                enemy->motion.next = ACTION_FLY_CHARGE;
                ai_shoot_laser(task, work, enemy);
            } else if (enemy->motion.action == ACTION_FLY_CHARGE) {
                enemy->motion.next = ACTION_FLY_SHOOT;
                work->timer = 0;
            }
        }
    }
}

static void ai_deflect(struct task *task, struct task_work *work,
                       struct enemy_work *enemy)
{
    ai_mark_player_near_offset(work, enemy);
    ai_turn(work, enemy);
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->timer > DEFLECT_STEPS) {
        work->mode = MODE_FIRE;
        work->timer = 0;
    }
    ai_check_hit(task, work, enemy);
    /* @hack redundant: the return after ai_check_hit, where the function's
     * end would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void ai_only_stand(struct task *task, struct task_work *work,
                          struct enemy_work *enemy)
{
    /* @hack redundant: a goto to the statement after it */
    goto check;
check:
    ai_check_hit(task, work, enemy);
    if (work->mode != MODE_DEFEATED && !game_state.time_stopped) {
        work->timer++;
    }
}

/* do nothing. nothing calls fun_enemy_ai_19ee, and no header declares it. it
 * is external only because SHC leaves out a static function that nothing
 * calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void fun_enemy_ai_19ee(void)
{
}

/* ai_draw's motion callback, run with each node's matrix current */
static void ai_find_muzzle(NJS_CNK_OBJECT *object)
{
    NJS_POINT3 pos;

    /* @hack spelling: an arm for each part, where one test of the three
     * would do */
    if (object == &ai_gun_weapon_object) {
        pos.x = MUZZLE_X;
        pos.y = 0.0F;
        pos.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&ai_view_inverse, &pos, &MUZZLE(ai_drawn));
    } else if (object == &ai_laser_weapon_object) {
        pos.x = MUZZLE_X;
        pos.y = 0.0F;
        pos.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&ai_view_inverse, &pos, &MUZZLE(ai_drawn));
    } else if (object == &ai_shielder_weapon_object) {
        pos.x = MUZZLE_X;
        pos.y = 0.0F;
        pos.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&ai_view_inverse, &pos, &MUZZLE(ai_drawn));
    }
}

static void ai_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct angles ang;
    NJS_VECTOR scl;
    NJS_POINT3 pos;
    int kind;

    light_apply(AI_LIGHT);
    if (work->mode != MODE_HIDDEN) {
        ai_drawn = enemy;
        njCnkSetMotionCallback(ai_find_muzzle);
        njGetMatrix(&ai_view_inverse);
        njInvertMatrix(&ai_view_inverse);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
        njSetTexture(&ai_texlist);
        if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
            njCnkSimpleDrawMotion(
                enemy->motion.object,
                enemy->motion.table[enemy->motion.action].motion,
                enemy->motion.frame);
        } else {
            njCnkSimpleDrawMotionLink(enemy->motion.object, &enemy->motion.link,
                                      enemy->motion.frame);
        }
        njPopMatrixEx();
        njCnkSetMotionCallback(NULL);
        enemy_draw_shadow(work, enemy);
        /* a readout of the set file's fields under their own labels, with
         * the range drawn as a cylinder. the image holds no test of kind, so
         * each case takes the kinds its strings describe */
        /* @hack redundant: a readout that no path reaches, which leaves its
         * cases' code and its strings in the object */
        if (0) {
            kind = work->submode;
            switch (kind) {
            case AI_GUN_FIXED:
            case AI_CAPT_FIXED:
            case AI_GUN_FIXED_EX:
            case AI_CAPT_FIXED_EX:
                njPrintC(NJM_LOCATION(17, 16), "-> RAPID ");
                njPrintD(NJM_LOCATION(26, 16), (Uint8)work->ang.x, 3);
                njPrintC(NJM_LOCATION(17, 18), "-> INTERVAL ");
                njPrintD(NJM_LOCATION(29, 18), work->ang.z, 5);
                njPrintC(NJM_LOCATION(34, 18), " FRAMES");
                njPrintC(NJM_LOCATION(17, 19), "-> TYPE( 0 - 9)");
                njPrintC(NJM_LOCATION(17, 20), "-> BULLET SPEED");
                njPrintC(NJM_LOCATION(17, 21), "-> RANGE/SCOPE");
                break;
            case AI_GUN_MOVING:
                njPrintC(NJM_LOCATION(17, 16), "-> RAPID ");
                njPrintD(NJM_LOCATION(26, 16), (Uint8)work->ang.x, 3);
                njPrintC(NJM_LOCATION(17, 18), "-> INTERVAL ");
                njPrintD(NJM_LOCATION(29, 18), work->ang.z, 5);
                njPrintC(NJM_LOCATION(34, 18), " FRAMES");
                njPrintC(NJM_LOCATION(17, 19), "-> TYPE( 0 - 9)");
                njPrintC(NJM_LOCATION(17, 20), "-> BULLET SPEED");
                njPrintC(NJM_LOCATION(17, 21), "-> RANGE");
                break;
            case AI_LASER_FIXED:
            case AI_SHIELDER:
            case AI_LASER_FIXED_EX:
                njPrintC(NJM_LOCATION(17, 16), "-> LASER LENGTH ");
                njPrintD(NJM_LOCATION(33, 16), (Uint8)work->ang.x, 3);
                njPrintC(NJM_LOCATION(17, 18), "-> INTERVAL ");
                njPrintD(NJM_LOCATION(29, 18), work->ang.z, 5);
                njPrintC(NJM_LOCATION(34, 18), " FRAMES");
                njPrintC(NJM_LOCATION(17, 19), "-> TYPE( 0 - 9)");
                njPrintC(NJM_LOCATION(17, 20), "-> LASER SPEED");
                njPrintC(NJM_LOCATION(17, 21), "-> RANGE/SCOPE");
                break;
            case AI_LASER_MOVING:
                njPrintC(NJM_LOCATION(17, 16), "-> LASER LENGTH ");
                njPrintD(NJM_LOCATION(33, 16), (Uint8)work->ang.x, 3);
                njPrintC(NJM_LOCATION(17, 18), "-> INTERVAL ");
                njPrintD(NJM_LOCATION(29, 18), work->ang.z, 5);
                njPrintC(NJM_LOCATION(34, 18), " FRAMES");
                njPrintC(NJM_LOCATION(17, 19), "-> TYPE( 0 - 9)");
                njPrintC(NJM_LOCATION(17, 20), "-> LASER SPEED");
                njPrintC(NJM_LOCATION(17, 21), "-> RANGE");
                break;
            case AI_ONLY_STAND:
                njPrintC(NJM_LOCATION(17, 19), "-> TYPE( 0 - 9)");
                break;
            }
            njPrintC(NJM_LOCATION(17, 17), "-> ANG Y ");
            njPrintH(NJM_LOCATION(26, 17), work->ang.y & 0xff00, 4);
            /* @hack redundant: two stores of kind that no path reaches */
            if (0) {
                work->submode = kind;
                work->mode = kind;
            }
            if (kind != AI_ONLY_STAND) {
                njPrintC(NJM_LOCATION(30, 17), " ID ");
                njPrintD(NJM_LOCATION(34, 17), (Uint8)work->ang.y, 3);
            } else {
                njPrintC(NJM_LOCATION(30, 17), " EME ");
                njPrintD(NJM_LOCATION(35, 17), (Uint8)work->ang.y, 3);
            }
            if (kind == AI_GUN_MOVING || kind == AI_LASER_MOVING) {
                /* @hack spelling: one assignment to the three, where a
                 * statement for each would do */
                ang.x = ang.y = ang.z = 0;
                scl.x = work->scl.z + RANGE;
                scl.z = scl.x;
                scl.y = 1.0F;
                debug_draw_model_unlit(&work->pos, &ang, &scl,
                                       DEBUG_MODEL_CYLINDER);
            } else if (kind != AI_ONLY_STAND) {
                /* the readout restores scl.z, which enemy_set_offset rounds */
                Float scl_z = work->scl.z;

                enemy_set_offset(work, enemy);
                /* @hack spelling: one assignment to the three, where a
                 * statement for each would do */
                ang.x = ang.y = ang.z = 0;
                /* @hack spelling: one assignment to scl.z and scl.x, where a
                 * statement for each would do */
                scl.z = scl.x = njRoundOff(work->scl.z) + RANGE;
                scl.y = 1.0F;
                pos = work->pos;
                njAddVector(&pos, &enemy->offset);
                debug_draw_model_unlit(&pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
                work->scl.z = scl_z;
            }
        }
    }
    light_apply(light_after_draw);
}

void ai_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    Float last_frame;

    if (work->mode != MODE_SETUP && placement_try_leave(task)) {
        return;
    }
    if (placement_unk_20(task)) {
        task_kill(task);
        return;
    }
    switch (work->mode) {
    case MODE_SETUP:
        /* @hack spelling: a jump to ai_setup's call after the return, where
         * the call in its case would do */
        goto setup;
    case MODE_CROUCH:
        ai_crouch(task, work, enemy);
        break;
    case MODE_FIRE:
        ai_fire(task, work, enemy);
        break;
    /* @hack spelling: the MODE_HIDDEN and MODE_DROP cases ahead of the
     * MODE_FLY and MODE_FLY_FIRE cases, not in the modes' order */
    case MODE_HIDDEN:
        ai_hidden(task, work, enemy);
        break;
    case MODE_DROP:
        ai_drop(task, work, enemy);
        break;
    case MODE_FLY:
        ai_fly(task, work, enemy);
        break;
    case MODE_FLY_FIRE:
        ai_fly_fire(task, work, enemy);
        break;
    case MODE_DEFLECT:
        ai_deflect(task, work, enemy);
        /* @bug logic: a deflecting shielder's motion plays on while
         * game_state.time_stopped is set, as no other mode's does. its case
         * calls enemy_save_pos and enemy_motion_step untested (JSR at 0x1f68
         * and JMP at 0x1f7c of the object's code). the other modes' calls
         * follow the TST of game_state.time_stopped at 0x1fc4 */
        enemy_save_pos(work, enemy);
        enemy_motion_step(&enemy->motion);
        return;
    case MODE_ONLY_STAND:
        ai_only_stand(task, work, enemy);
        break;
    case MODE_DEFEATED:
        work->mode = MODE_DEAD;
        /* fall through */
    case MODE_DEAD:
        ai_kill(task);
        return;
    }
    /* @hack spelling: the empty arm and the else, where one arm under
     * !game_state.time_stopped would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        last_frame = enemy->motion.frame;
        enemy_motion_step(&enemy->motion);
        if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
            switch (enemy->motion.action) {
            case ACTION_TURN:
                /* @hack redundant: a store of 1.0F that no path reaches */
                if (0) {
                    work->scl.x = 1.0F;
                }
                if ((enemy->motion.frame >= TURN_FOOT_1 &&
                     last_frame < TURN_FOOT_1) ||
                    (enemy->motion.frame >= TURN_FOOT_2 &&
                     last_frame < TURN_FOOT_2)) {
                    sound_play_at(STEP_SOUND, NULL, 0, STEP_VOLUME, &work->pos);
                }
                break;
            case ACTION_UNUSED_TURN:
            case ACTION_SHIELDER_TURN:
                if ((enemy->motion.frame >= SHIELDER_TURN_FOOT_1 &&
                     last_frame < SHIELDER_TURN_FOOT_1) ||
                    (enemy->motion.frame >= SHIELDER_TURN_FOOT_2 &&
                     last_frame < SHIELDER_TURN_FOOT_2)) {
                    sound_play_at(STEP_SOUND, NULL, 0, STEP_VOLUME, &work->pos);
                }
                break;
            }
        }
        if (work->flags & AI_ALERT) {
            if (work->mode == MODE_DROP ||
                enemy->motion.action == ACTION_RISE) {
                enemy_light_create(task, MUZZLE_LIGHT_COMING, &MUZZLE(enemy));
            } else {
                enemy_light_create(task, MUZZLE_LIGHT, &MUZZLE(enemy));
            }
        }
    }
    /* @hack redundant: a store to mode that no path reaches */
    if (0) {
        work->mode = work->mode + work->mode;
    }
    /* @hack redundant: an empty else */
    if (work->mode != MODE_HIDDEN && work->mode != MODE_DEFEATED) {
        collision_register(task);
    } else {
    }
    return;
setup:
    ai_setup(task, work);
}
