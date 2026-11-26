/* the E-1000, an enemy with two guns that a stage's object list names
 * "E 1000". it stands where it was set, or flies about it */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/light.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/enemy_1000.h"
#include "shared/enemy_bullet.h"
#include "shared/enemy_jet.h"
#include "shared/enemy_light.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* the E-1000's work->mode */
enum e1000_mode {
    MODE_SETUP,
    MODE_WAIT, /* a fixed kind waits for a player in range */
    MODE_FIRE, /* a fixed kind stands on guard */
    MODE_FLY, /* a moving kind flies while no player is in range */
    MODE_FLY_FIRE, /* a moving kind flies on guard */
    MODE_DEFEATED, /* a hit has beaten the E-1000, which its next step kills */
    MODE_DEAD /* killed by placement_kill */
};

/* the E-1000's bits in work->flags */
/* the E-1000 lies farther than the range from home, in all three axes */
#define E1000_AWAY 0x1000
/* a player lies within the range of home, across x and z */
#define E1000_ALERT 0x2000

/* the E-1000's own fields of its enemy_work */
/* the interval between bursts, in steps. a burst waits for work->timer to
 * pass it */
#define INTERVAL(enemy) ((enemy)->unk_048)
/* the pairs of shots in a burst. work->tick counts them */
#define PAIRS(enemy) ((enemy)->unk_047)
#define SHOT_SPEED(enemy) ((enemy)->unk_094)
/* what each step adds to spd.x and spd.z. it stays 0, since nothing sets it */
#define ACCEL(enemy) ((enemy)->unk_00c)
#define BOB_ANGLE(enemy) ((enemy)->unk_1c4)
/* what the draw adds to the exhaust strip's u and v, in 256ths of the
 * texture. EXHAUST_U stays 0, since nothing sets it */
#define EXHAUST_U(enemy) ((enemy)->unk_04e)
#define EXHAUST_V(enemy) ((enemy)->unk_050)

/* the E-1000's record, in its task's extra */
struct e1000 {
    /* the steps left before e1000_turn picks a new target */
    Sint16 target_steps;
    Uint8 unk_02[2];
    Angle pitch; /* the guns' pitch, within GUN_PITCH_MAX of level */
    /* two points of e1000_object in the world, as the last draw found them */
    NJS_POINT3 unk_08;
    NJS_POINT3 unk_14;
    /* the guns' muzzles in the world, as the last draw found them */
    NJS_POINT3 muzzle_high;
    NJS_POINT3 muzzle_low;
};

/* the indexes of e1000_actions */
#define ACTION_WAIT 0
#define ACTION_TO_STAND 1
#define ACTION_STAND 2
#define ACTION_SHOOT 3
#define ACTION_TO_WAIT 4
/* the flight's actions, from ACTION_FLY on. the draw adds the exhaust to
 * each */
#define ACTION_FLY 5
#define ACTION_FLY_ALERT 6
#define ACTION_FLY_SHOOT 7

/* the index in the exhaust's polygon list of its strip's first u */
#define EXHAUST_FIRST_U 13

/* the range is scl.z + RANGE. near_distance2 holds the range squared */
#define RANGE 60.0F
#define INTERVAL_MIN 60
/* the steps a fixed kind waits for its first burst. it sets its timer to
 * INTERVAL - FIRST_WAIT as it comes on guard */
#define FIRST_WAIT 60
/* a target lies at random in a square about home, whose side is
 * TARGET_SPREAD times the range. a pick sets target_steps to
 * TARGET_STEPS_LEAST plus a random part of under TARGET_STEPS_RANDOM */
#define TARGET_SPREAD 0.2F
#define TARGET_STEPS_LEAST 60.0F
#define TARGET_STEPS_RANDOM 180.0F
/* a step moves ACTION_SHOOT's and ACTION_FLY_SHOOT's frame by
 * SHOOT_ACTION_SPEED. a pair of shots leaves in the step whose frame lies from
 * SHOT_FRAME up to SHOT_FRAME_END. each shoot action has one such step */
#define SHOOT_ACTION_SPEED 0.11F
#define SHOT_FRAME 1.0F
#define SHOT_FRAME_END (SHOT_FRAME + SHOOT_ACTION_SPEED)
/* the high gun's shot aims AIM_SPREAD above the center of the player's first
 * collision shape. the low gun's aims AIM_SPREAD below it */
#define AIM_SPREAD 2.5F
/* the most a shot turns, in Angle units, from the guns' pitch and from the
 * E-1000's heading */
#define AIM_PITCH 0x1800
#define AIM_YAW 0x800
/* how far the guns' pitch moves toward the player in a step */
#define GUN_PITCH_STEP 0x200
#define GUN_PITCH_MAX 0x2800
/* each barrel's muzzle, in the axes of the barrel's node */
#define MUZZLE_X 6.1F
#define MUZZLE_Z 0.05F
/* the collision record's radius, which the setup sets over the one
 * collision_init fits */
#define RECORD_RADIUS 22.0F
/* a moving kind's spd.x, its speed along its heading, stays from
 * FLY_SPEED_MIN to FLY_SPEED */
#define FLY_SPEED 1.0F
#define FLY_SPEED_MIN 0.5F
/* a moving kind sinks while its bottom lies above the floor. a sink takes
 * SINK_STEP off spd.y a step, down to SINK_MAX. otherwise spd.y scales by
 * FALL_DAMPING a step, and becomes 0 once within FALL_STOP of 0 */
#define SINK_STEP 0.05F
#define SINK_MAX (-2.0F)
#define FALL_DAMPING 0.95F
#define FALL_STOP 0.1F
/* a moving kind bobs scl.y above and below its path. the setup sets scl.y to
 * BOB_HEIGHT. the bob's angle moves BOB_STEP a step */
#define BOB_HEIGHT 1.0F
#define BOB_STEP 0x300
/* what each step takes off EXHAUST_V */
#define EXHAUST_SCROLL 34
/* the index in lights of the light the E-1000 draws in */
#define E1000_LIGHT 8
/* the points score_add_enemy adds for beating an E-1000. they double where
 * player_touch_attacks finds Sonic's or Shadow's combo */
#define SCORE 200
/* the ball a magic hand makes of a beaten E-1000 grows to BALL_SCALE. it
 * draws the E-1000's model BALL_OFFSET_Y above its center */
#define BALL_SCALE 0.17F
#define BALL_OFFSET_Y (-11.0F)
#define JET_SOUND 0x401c
#define JET_VOLUME (-15)
#define SHAPES 1
#define SIGHT_DISTANCE 60.0F
#define SIGHT_ANGLE ((Angle)(160 * ANGLE_PER_DEGREE))
#define TURN_SPEED 0x800
#define SHADOW_SIZE 6.0F
#define SHADOW_RATIO 1.5F
/* the texture task's level */
#define TEXTURE_LEVEL 0

/* clang-format off */
#include "shared/enemy_1000/text.inc"
#include "shared/e1000_art/body.h"
#include "shared/e1000_art/wait_shoot.h"
#include "shared/e1000_art/flight_motions.h"
#include "shared/e1000_art/stance_motions.h"
#include "shared/e1000_art/exhaust.h"
/* clang-format on */

/* the inverse of the matrix current as e1000_draw starts. after a node's
 * matrix, it takes a point in the node's axes to the world */
static NJS_MATRIX e1000_view_inverse;
/* e1000_flight_node's matrix in the last draw's motion, which the exhaust is
 * drawn by */
static NJS_MATRIX e1000_exhaust_matrix;
static struct e1000 *e1000_drawn; /* the E-1000 that e1000_draw draws */

/* clang-format off */
static struct collision_shape e1000_shapes[SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_UNTURNED | SHAPE_HURTABLE, {0.0F, 10.0F, 0.0F}, 4.0F, 10.0F, 0.0F,
     0, {0, 0, 0}},
};
/* clang-format on */

/* the models enemy_break breaks a beaten E-1000 into, null-ended */
static NJS_CNK_MODEL *e1000_pieces[14] = {
#include "shared/enemy_1000/e1000_pieces.inc"
};

static struct enemy_action e1000_actions[8] = {
#include "shared/enemy_1000/e1000_actions.inc"
};

/* the task that keeps e1000_texlist loaded, or null while none does. its
 * destructor releases the list */
static struct task *e1000_texture_task = NULL;

/* the kinds' titles, by e1000_kind */
/* Preserve the two original SET readout titles. */
static char *e1000_titles[E1000_KINDS] = {
#include "shared/enemy_1000/e1000_titles.inc"
};

static void e1000_destroy(struct task *task);
static void e1000_draw(struct task *task);

static void e1000_texture_destroy(struct task *task)
{
    njReleaseTexture(&e1000_texlist);
    e1000_texture_task = NULL;
}

static void e1000_texture_step(struct task *task)
{
}

static void e1000_setup(struct task *task, struct task_work *work)
{
    struct e1000 *e1000;
    struct enemy_work *enemy;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((e1000 = syCalloc(1, sizeof(struct e1000))) == NULL) {
        return;
    }
    task->extra.p = e1000;
    enemy = enemy_work_alloc_grouped(task);
    work->submode = njRoundOff(work->scl.x);
    if (work->submode >= E1000_KINDS || work->submode < 0) {
        work->submode = E1000_GUN_FIXED;
    }
    collision_init(task, e1000_shapes, SHAPES, COLLISION_ENEMY);
    /* @bug memory: reads the collision record through null where
     * collision_init had no room for it. the setup tests the record (TST
     * R2,R2 at 0x72 of the object's code), and goes on without it.
     * e1000_turn reads its flags unchecked (MOV.W @(4,R0),R0 at 0x35e).
     * e1000_check_hit reads its shapes unchecked (MOV.L @(12,R3),R3 at
     * 0x46e) */
#ifdef NONMATCHING
    /* this side repairs the tag above: without a record, the e1000 ends on
     * its next step, and its placement makes it again while in range */
    if (work->collision == NULL) {
        task->destructor = e1000_destroy;
        task_kill(task);
        return;
    }
#endif
    if (work->collision != NULL) {
        work->collision->radius = RECORD_RADIUS;
    }
    PAIRS(enemy) = work->ang.x;
    INTERVAL(enemy) = work->ang.z;
    if (INTERVAL(enemy) < INTERVAL_MIN) {
        INTERVAL(enemy) = INTERVAL_MIN;
    }
    SHOT_SPEED(enemy) = work->scl.y;
    enemy->motion.object = &e1000_object;
    enemy_bullet_load();
    work->ang.x = 0;
    work->ang.z = 0;
    work->scl.y = BOB_HEIGHT;
    enemy->sight_distance2 = SIGHT_DISTANCE * SIGHT_DISTANCE;
    enemy->sight_angle = SIGHT_ANGLE;
    enemy->near_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->spd.x = FLY_SPEED;
    enemy->turn_speed = TURN_SPEED;
    enemy_light_load_texture();
    enemy_set_home(work, enemy);
    enemy->motion.table = e1000_actions;
    if (work->submode == E1000_GUN_MOVING) {
        work->mode = MODE_FLY;
        enemy->motion.next = ACTION_FLY;
        enemy->bottom = -6.4F;
        sound_play_task_held_at(JET_SOUND, task, 0, JET_VOLUME);
    } else {
        work->mode = MODE_WAIT;
        enemy->motion.next = ACTION_WAIT;
    }
    enemy_motion_step(&enemy->motion);
    enemy->shadow_size = SHADOW_SIZE;
    enemy->shadow_ratio = SHADOW_RATIO;
    /* @hack spelling: the two bits set by two statements, where one |= would
     * do */
    enemy->flags |= ENEMY_BURST;
    enemy->flags |= ENEMY_RELEASES_ANIMAL;
    task->draw = e1000_draw;
    task->destructor = e1000_destroy;
    enemy_jet_load_texture();
    if (e1000_texture_task == NULL) {
        texture_load_pvm(E1000_TEXTURE_ARCHIVE, &e1000_texlist);
        e1000_texture_task = task_create(0, TEXTURE_LEVEL, e1000_texture_step,
                                         E1000_TEXTURE_TASK_NAME);
        e1000_texture_task->destructor = e1000_texture_destroy;
    }
}

static void e1000_pick_target(struct task_work *work, struct enemy_work *enemy)
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

static void e1000_turn(struct task_work *work, struct enemy_work *enemy,
                       struct e1000 *e1000)
{
    int player;

    if (game_state.time_stopped) {
        return;
    }
    if (work->flags & E1000_AWAY) {
        enemy_turn_to_home(work, enemy);
    } else if (work->flags & E1000_ALERT) {
        player = player_nearest(&work->pos);
        if (player == 0 || player == 1) {
            enemy_turn_to_player(work, enemy, player);
        }
    } else if (e1000->target_steps-- < 0 ||
               ((work->collision->flags & COLLISION_TOUCHED) &&
                work->collision->hit->kind != COLLISION_PLAYER)) {
        e1000_pick_target(work, enemy);
        e1000->target_steps =
            (rand() * (TARGET_STEPS_RANDOM * RAND_SCALE)) + TARGET_STEPS_LEAST;
    } else {
        work->ang.y =
            angle_approach(work->ang.y, enemy->aim_y, enemy->turn_speed);
    }
}

static void e1000_mark_away(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: the local distance, where enemy_home_distance2's result
     * compared in place would do */
    Float distance;

    if (!game_state.time_stopped) {
        distance = enemy_home_distance2(work, enemy);
        if (distance > (work->scl.z + RANGE) * (work->scl.z + RANGE)) {
            work->flags |= E1000_AWAY;
        } else {
            work->flags &= ~E1000_AWAY;
        }
    }
}

static void e1000_check_hit(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    int score;
    int player;

    if (!enemy_take_hit(work, enemy)) {
        return;
    }
    /* @hack redundant: a test of game_state.time_stopped that guards
     * nothing */
    if (game_state.time_stopped) {
    }
    work->mode = MODE_DEFEATED;
    work->collision->shapes->flags |= SHAPE_OFF;
    work->timer = 0;
    enemy_set_home(work, enemy);
    enemy_break(work, e1000_pieces, &e1000_texlist);
    score = SCORE;
    if (player_touch_attacks(task)) {
        score *= 2;
    }
    score_add_enemy(score);
    enemy_defeat(work, enemy);
    if (enemy->flags & ENEMY_IN_MAGIC_HAND) {
        NJS_VECTOR offset;
        NJS_VECTOR scl;

        player = enemy->shot_by;
        offset.x = 0.0F;
        offset.y = BALL_OFFSET_Y;
        offset.z = 0.0F;
        /* @hack spelling: one assignment to the three, where a statement for
         * each would do */
        scl.x = scl.y = scl.z = BALL_SCALE;
        /* @hack spelling: a switch on the kind with a default arm alone, where
         * the call alone would do */
        switch (work->submode) {
        default:
            enemy_ball_create(player, &e1000_object, &e1000_texlist, &scl,
                              &offset);
            break;
        }
    }
}

static void e1000_mark_player_near_home(struct task_work *work,
                                        struct enemy_work *enemy)
{
    if (enemy_find_player_near_home(work, enemy)) {
        work->flags |= E1000_ALERT;
    } else {
        work->flags &= ~E1000_ALERT;
    }
}

static void e1000_move(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    enemy->spd.x += ACCEL(enemy).x;
    enemy->spd.z += ACCEL(enemy).z;
    if (enemy->spd.x > FLY_SPEED) {
        enemy->spd.x = FLY_SPEED;
    } else if (enemy->spd.x < FLY_SPEED_MIN) {
        enemy->spd.x = FLY_SPEED_MIN;
    }
    work->pos.x += (enemy->spd.x * njCos(work->ang.y)) +
                   (enemy->spd.z * njSin(work->ang.y));
    work->pos.y += enemy->spd.y;
    work->pos.z += -(enemy->spd.x * njSin(work->ang.y)) +
                   (enemy->spd.z * njCos(work->ang.y));
    /* enemy_meet_stage takes the E-1000's pos without its bob */
    work->pos.y -= njSin(BOB_ANGLE(enemy)) * work->scl.y;
    BOB_ANGLE(enemy) += BOB_STEP;
    enemy_meet_stage(work, enemy);
    work->pos.y += njSin(BOB_ANGLE(enemy)) * work->scl.y;
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

/* turn spd, a shot's speed along x, toward the nearest player from pos. high
 * is TRUE for the high gun's shot */
static void e1000_aim(struct task_work *work, struct e1000 *e1000,
                      NJS_POINT3 *pos, NJS_VECTOR *spd, Bool high)
{
    NJS_POINT3 target;
    /* @hack spelling: the locals ax and ay, where angle_approach's results
     * passed in place would do */
    Angle ax;
    Angle ay;
    Angle pitch;
    Angle yaw;
    Float distance;
    Float dy;

    target = player_work[player_nearest(&work->pos)]->collision->shapes->center;
    distance = njDistanceP2P(pos, &target);
    if (high) {
        dy = target.y - pos->y + AIM_SPREAD;
    } else {
        dy = target.y - pos->y - AIM_SPREAD;
    }
    if (distance != 0.0F) {
        pitch = asinf(dy / distance) * ANGLE_PER_RADIAN;
        yaw = atan2f(pos->z - target.z, target.x - pos->x) * ANGLE_PER_RADIAN;
    } else {
        yaw = work->ang.y;
        pitch = 0;
    }
    ax = angle_approach(e1000->pitch, pitch, AIM_PITCH);
    ay = angle_approach(work->ang.y, yaw, AIM_YAW);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, ay);
    njRotateZ(NULL, ax);
    njCalcVector(NULL, spd, spd);
    njPopMatrixEx();
}

static void e1000_shoot(struct task *task, struct task_work *work,
                        struct enemy_work *enemy, Bool high)
{
    struct e1000 *e1000 = task->extra.p;
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    /* @hack spelling: high compared with TRUE, where the test of high alone
     * would do */
    if (high == TRUE) {
        pos = e1000->muzzle_high;
    } else {
        pos = e1000->muzzle_low;
    }
    spd.x = SHOT_SPEED(enemy);
    spd.y = 0.0F;
    spd.z = 0.0F;
    e1000_aim(work, e1000, &pos, &spd, high);
    enemy_bullet_fire(task, ENEMY_BULLET_STOPS, &spd, &pos);
}

static void e1000_pitch_guns(struct task_work *work, struct e1000 *e1000)
{
    NJS_POINT3 target;
    NJS_POINT3 pos;
    Angle aim;
    Float distance;
    Float dy;

    target = player_work[player_nearest(&work->pos)]->collision->shapes->center;
    pos.x = (e1000->muzzle_high.x + e1000->muzzle_low.x) * 0.5F;
    pos.y = (e1000->muzzle_high.y + e1000->muzzle_low.y) * 0.5F;
    pos.z = (e1000->muzzle_high.z + e1000->muzzle_low.z) * 0.5F;
    distance = njDistanceP2P(&pos, &target);
    dy = target.y - pos.y;
    if (distance != 0.0F) {
        aim = asinf(dy / distance) * ANGLE_PER_RADIAN;
    } else if (dy > 0.0F) {
        aim = QUARTER_TURN;
    } else {
        aim = HALF_TURN + QUARTER_TURN;
    }
    e1000->pitch = angle_approach(e1000->pitch, aim, GUN_PITCH_STEP);
    aim = angle_difference(0, e1000->pitch);
    if (aim > GUN_PITCH_MAX) {
        e1000->pitch = GUN_PITCH_MAX;
    } else if (aim < -GUN_PITCH_MAX) {
        e1000->pitch = -GUN_PITCH_MAX;
    }
}

static void e1000_destroy(struct task *task)
{
    enemy_destroy(task);
    sound_stop_key(task);
    /* @hack redundant: the return after sound_stop_key, where the function's
     * end would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void e1000_wait(struct task *task, struct task_work *work,
                       struct enemy_work *enemy)
{
    e1000_mark_player_near_home(work, enemy);
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->flags & E1000_ALERT) {
        enemy->motion.next = ACTION_TO_STAND;
        work->mode = MODE_FIRE;
        work->timer = INTERVAL(enemy) - FIRST_WAIT;
    }
    e1000_check_hit(task, work, enemy);
    /* @hack redundant: the return after e1000_check_hit, where the function's
     * end would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void e1000_fire(struct task *task, struct task_work *work,
                       struct enemy_work *enemy)
{
    e1000_mark_player_near_home(work, enemy);
    if (enemy->motion.action == ACTION_STAND) {
        e1000_turn(work, enemy, task->extra.p);
    }
    e1000_pitch_guns(work, task->extra.p);
    e1000_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->submode == E1000_GUN_FIXED && PAIRS(enemy) > 0) {
        if (!(work->flags & E1000_ALERT)) {
            work->timer = 0;
            work->tick = 0;
            enemy->motion.next = ACTION_TO_WAIT;
            work->mode = MODE_WAIT;
            /* @hack redundant: the return before the else, where the else alone
             * would do */
            return;
            /* @hack spelling: the else after the return, where a new if would
             * do */
        } else if (work->timer > INTERVAL(enemy)) {
            if (enemy->motion.action == ACTION_STAND) {
                enemy->motion.next = ACTION_SHOOT;
                /* @hack redundant: the return, which skips a test that can't
                 * hold after it */
                return;
            }
            if (enemy->motion.action == ACTION_SHOOT) {
                if (enemy->motion.frame >= SHOT_FRAME &&
                    enemy->motion.frame < SHOT_FRAME_END) {
                    e1000_shoot(task, work, enemy, TRUE);
                    e1000_shoot(task, work, enemy, FALSE);
                    work->tick++;
                    if (work->tick >= PAIRS(enemy)) {
                        work->tick = 0;
                        work->timer = 0;
                    }
                }
            }
        }
    }
}

/* @hack spelling: the scroll as a function SHC expands, where its statements
 * in place would do */
static void e1000_scroll_exhaust(struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    EXHAUST_V(enemy) -= EXHAUST_SCROLL;
}

static void e1000_fly(struct task *task, struct task_work *work,
                      struct enemy_work *enemy)
{
    e1000_mark_away(work, enemy);
    e1000_mark_player_near_home(work, enemy);
    e1000_turn(work, enemy, task->extra.p);
    e1000_move(work, enemy);
    e1000_scroll_exhaust(enemy);
    e1000_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->flags & E1000_ALERT) {
        work->timer = 0;
        work->tick = 0;
        enemy->motion.next = ACTION_FLY_ALERT;
        work->mode = MODE_FLY_FIRE;
    }
}

static void e1000_fly_fire(struct task *task, struct task_work *work,
                           struct enemy_work *enemy)
{
    e1000_mark_away(work, enemy);
    e1000_mark_player_near_home(work, enemy);
    e1000_turn(work, enemy, task->extra.p);
    e1000_move(work, enemy);
    e1000_pitch_guns(work, task->extra.p);
    e1000_scroll_exhaust(enemy);
    e1000_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (PAIRS(enemy) > 0) {
        if (!(work->flags & E1000_ALERT)) {
            work->timer = 0;
            work->tick = 0;
            work->mode = MODE_FLY;
            enemy->motion.next = ACTION_FLY;
            /* @hack redundant: the return before the else, where the else alone
             * would do */
            return;
            /* @hack spelling: the else after the return, where a new if would
             * do */
        } else if (work->timer > INTERVAL(enemy)) {
            if (enemy->motion.action == ACTION_FLY_ALERT) {
                enemy->motion.next = ACTION_FLY_SHOOT;
                /* @hack redundant: the return, which skips a test that can't
                 * hold after it */
                return;
            }
            if (enemy->motion.action == ACTION_FLY_SHOOT) {
                if (enemy->motion.frame >= SHOT_FRAME &&
                    enemy->motion.frame < SHOT_FRAME_END) {
                    e1000_shoot(task, work, enemy, TRUE);
                    e1000_shoot(task, work, enemy, FALSE);
                    work->tick++;
                }
                if (work->tick >= PAIRS(enemy)) {
                    work->timer = 0;
                    work->tick = 0;
                }
            }
        }
    }
}

/* do nothing. nothing calls fun_enemy_1000_d0e, and no header declares it. it
 * is external only because SHC leaves out a static function that nothing
 * calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void fun_enemy_1000_d0e(void)
{
}

/* e1000_draw's motion callback, run with each node's matrix current */
static void e1000_node_callback(NJS_CNK_OBJECT *object)
{
    NJS_POINT3 pos;

    if (object == &e1000_object) {
        pos.x = 7.5F;
        pos.y = 3.0F;
        pos.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&e1000_view_inverse, &pos, &e1000_drawn->unk_08);
        pos.x = 8.0F;
        pos.y = 3.0F;
        pos.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&e1000_view_inverse, &pos, &e1000_drawn->unk_14);
    } else if (object == e1000_flight_node) {
        njGetMatrix(&e1000_exhaust_matrix);
    } else if (object == &e1000_gun_high_object ||
               object == &e1000_gun_low_object) {
        njRotateZ(NULL, -e1000_drawn->pitch);
    } else if (object == &e1000_barrel_high_object) {
        pos.x = MUZZLE_X;
        pos.y = 0.0F;
        pos.z = MUZZLE_Z;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&e1000_view_inverse, &pos, &e1000_drawn->muzzle_high);
    } else if (object == &e1000_barrel_low_object) {
        pos.x = MUZZLE_X;
        pos.y = 0.0F;
        pos.z = MUZZLE_Z;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&e1000_view_inverse, &pos, &e1000_drawn->muzzle_low);
    }
}

static void e1000_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    Sint16 *uv;
    struct angles ang;
    NJS_VECTOR scl;

    light_apply(E1000_LIGHT);
    /* @hack redundant: a block no path reaches */
    if (0) {
        njPopMatrixEx();
        njPushMatrixEx();
        /* @hack redundant: four tests of task that guard nothing */
        if (task) {
        }
        if (task) {
        }
        if (task) {
        }
        if (task) {
        }
    }
    e1000_drawn = task->extra.p;
    njCnkSetMotionCallback(e1000_node_callback);
    njGetMatrix(&e1000_view_inverse);
    njInvertMatrix(&e1000_view_inverse);
    e1000_flight_node->evalflags |= NJD_EVAL_HIDE;
    if (enemy->motion.action >= ACTION_FLY) {
        /* each of the strip's vertices is an index, a u and a v. the numbers
         * below copy each u and v of e1000_exhaust_polygons. the draw adds
         * EXHAUST_U to each u, and EXHAUST_V to each v */
        uv = &e1000_flight_object->model->plist[EXHAUST_FIRST_U];
        *uv++ = EXHAUST_U(enemy) + 433;
        *uv++ = EXHAUST_V(enemy) + 202;
        uv++;
        *uv++ = EXHAUST_U(enemy) + 512;
        *uv++ = EXHAUST_V(enemy);
        uv++;
        *uv++ = EXHAUST_U(enemy) + 412;
        *uv++ = EXHAUST_V(enemy) + 224;
        uv++;
        *uv++ = EXHAUST_U(enemy) + 362;
        *uv++ = EXHAUST_V(enemy) + 18;
        uv++;
        *uv++ = EXHAUST_U(enemy) + 256;
        *uv++ = EXHAUST_V(enemy) + 255;
        uv++;
        *uv++ = EXHAUST_U(enemy) + 256;
        *uv++ = EXHAUST_V(enemy) + 19;
        uv++;
        *uv++ = EXHAUST_U(enemy) + 100;
        *uv++ = EXHAUST_V(enemy) + 224;
        uv++;
        *uv++ = EXHAUST_U(enemy) + 150;
        *uv++ = EXHAUST_V(enemy) + 18;
        uv++;
        *uv++ = EXHAUST_U(enemy) + 79;
        *uv++ = EXHAUST_V(enemy) + 202;
        uv++;
        *uv++ = EXHAUST_U(enemy);
        *uv = EXHAUST_V(enemy);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    njSetTexture(&e1000_texlist);
    if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
        njCnkSimpleDrawMotion(enemy->motion.object,
                              enemy->motion.table[enemy->motion.action].motion,
                              enemy->motion.frame);
    } else {
        njCnkSimpleDrawMotionLink(enemy->motion.object, &enemy->motion.link,
                                  enemy->motion.frame);
    }
    njPopMatrixEx();
    if (enemy->motion.action >= ACTION_FLY) {
        njPushMatrixEx();
        njSetMatrix(NULL, &e1000_exhaust_matrix);
        njCnkSimpleDrawModel(e1000_flight_object->model);
        njPopMatrixEx();
    }
    njCnkSetMotionCallback(NULL);
    enemy_draw_shadow(work, enemy);
    /* a readout of the set file's fields under their own labels, with the
     * range drawn as a cylinder */
    /* @hack redundant: a readout that no path reaches, which leaves its
     * strings and a stretch of its code in the object */
    if (0) {
        /* @hack redundant: two prints the image holds no code for, in place
         * of the readout's first lines */
        njPrintC(NJM_LOCATION(17, 15), E1000_DEBUG_BURST);
        njPrintD(NJM_LOCATION(26, 15),
                 ((struct e1000 *)task->extra.p)->target_steps, 3);
        /* @hack spelling: the switch of one case */
        switch (work->submode) {
        case E1000_GUN_FIXED:
            njPrintC(NJM_LOCATION(17, 16), E1000_DEBUG_BURST);
            njPrintD(NJM_LOCATION(26, 16), (Uint8)work->ang.x, 3);
            njPrintC(NJM_LOCATION(17, 18), E1000_DEBUG_INTERVAL);
            njPrintD(NJM_LOCATION(29, 18), work->ang.z, 5);
            njPrintC(NJM_LOCATION(34, 18), E1000_DEBUG_FRAMES);
            njPrintC(NJM_LOCATION(17, 19), E1000_DEBUG_KIND);
            njPrintC(NJM_LOCATION(17, 20), E1000_DEBUG_BULLET_SPEED);
            njPrintC(NJM_LOCATION(17, 21), E1000_DEBUG_RANGE);
            njPrintC(NJM_LOCATION(17, 17), E1000_DEBUG_YAW);
            njPrintH(NJM_LOCATION(26, 17), work->ang.y & 0xff00, 4);
            njPrintC(NJM_LOCATION(30, 17), E1000_DEBUG_EMERALD);
            njPrintD(NJM_LOCATION(35, 17), (Uint8)work->ang.y, 3);
            /* @hack spelling: one assignment to the three, and one to scl.z
             * and scl.x, where a statement for each would do */
            ang.x = ang.y = ang.z = 0;
            scl.z = scl.x = work->scl.z + RANGE;
            scl.y = 1.0F;
            debug_draw_model_unlit(&work->pos, &ang, &scl,
                                   DEBUG_MODEL_CYLINDER);
        }
    }
    light_apply(light_after_draw);
}

void e1000_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (work->mode != MODE_SETUP && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_SETUP:
        /* @hack spelling: a jump to the setup's call after the return, where
         * the call in its case would do */
        goto setup;
    case MODE_WAIT:
        e1000_wait(task, work, enemy);
        break;
    case MODE_FIRE:
        e1000_fire(task, work, enemy);
        break;
    case MODE_FLY:
        e1000_fly(task, work, enemy);
        break;
    case MODE_FLY_FIRE:
        e1000_fly_fire(task, work, enemy);
        break;
    case MODE_DEFEATED:
        work->mode = MODE_DEAD;
        /* fall through */
    case MODE_DEAD:
        placement_kill(task);
        /* @hack spelling: a jump to a return, where the return in place would
         * do */
        goto done;
    }
    /* @hack spelling: the empty arm and the else, where one arm under
     * !game_state.time_stopped would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        enemy_motion_step(&enemy->motion);
        /* @hack redundant: a test of E1000_ALERT that guards nothing */
        if (work->flags & E1000_ALERT) {
        }
    }
    collision_register(task);
    return;
done:
    return;
setup:
    /* @hack redundant: a jump to the statement after it */
    goto load;
load:
    e1000_setup(task, work);
}
