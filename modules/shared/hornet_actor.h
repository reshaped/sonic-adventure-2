/* the hornet's code, whose contract hornet.h gives. it holds the hornet's
 * step, draw and end, and its bombs' and lasers'. shared/hornet.c compiles it.
 * so do shared/akahige.c and stg31/akahige.c, which define GREEN_HILL_AKAHIGE.
 * the includer defines the hornet's shapes and pieces first. it defines
 * BOMBS_MAX too, the most bombs one of its hornets carries */
#ifndef SHARED_HORNET_ACTOR_H
#define SHARED_HORNET_ACTOR_H

#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/burst.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "player/walker.h"
#include "shared/blast.h"
#include "shared/enemy_jet.h"
#include "shared/enemy_laser.h"
#include "shared/enemy_light.h"
#include "shared/hornet.h"
#include "shared/hornet_art.h"
#include "shared/spark.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* under GREEN_HILL_AKAHIGE, the code is the akahige's. the akahige is a
 * hornet with models and textures of its own, and only HORNET_3 and HORNET_6.
 * an image that links the akahige links the hornet too. so the akahige's
 * includers name its globals apart, by HORNET_ASSET, BOMB_ASSET, HORNET_STEP
 * and HORNET_UNUSED */
#ifdef GREEN_HILL_AKAHIGE
#define IS_AKAHIGE 1
#define IS_LASER(kind) 0
#define KIND_COUNT 2
#define HORNET_TEXTURE_NAME "E_AKAHIGETEX"
#define DEBUG_KIND_LABEL "-> TYPE(0-1)SHADOW "
#else
#define HORNET_ASSET(name) hornet_##name
#define BOMB_ASSET(name) bomb_##name
#define HORNET_STEP hornet_step
#define HORNET_UNUSED hornet_unused
#define IS_AKAHIGE 0
#define IS_LASER(kind) ((kind) == HORNET_LASER)
#define KIND_COUNT HORNET_KINDS
#define HORNET_TEXTURE_NAME "E_NAMITEX"
#define DEBUG_KIND_LABEL "-> TYPE(0-3)SHADOW "
#endif

/* the hornet's modes, in its work's mode */
enum hornet_mode {
    MODE_INIT,
    MODE_WATCH,
    MODE_ATTACK, /* it holds still, its bombs loose or its lasers fired */
    MODE_DEFEATED,
    MODE_DEAD
};

/* a bomb's modes, in its work's mode */
enum bomb_mode {
    BOMB_CARRIED, /* it rides on the hornet at its MOUNT */
    BOMB_LOOSE, /* it drifts away from the hornet, then waits */
    BOMB_CHASING, /* it flies at the nearest player */
    BOMB_BLAST,
    BOMB_SMALL_BLAST,
    BOMB_SPENT
};

/* the bits of a hornet's work->flags */

/* the hornet strayed past RANGE from home, and turns back toward it. the bit
 * stays until AWAY_STEPS runs out */
#define HORNET_AWAY 0x1000
/* a player is within the reach of home, across x and z */
#define HORNET_ALERT 0x2000
/* the hornet is beaten, and each of its bombs dies at its next step. a
 * carried bomb's burst sets the bit too, which hornet_check_hit then takes as
 * a hit */
#define HORNET_BEATEN 0x4000
/* a loose bomb sets this bit as it starts to chase. hornet_attack then ends
 * the attack */
#define HORNET_BOMB_CHASING 0x8000

/* what a hornet keeps in the fields of its enemy_work that enemy.h leaves to
 * each kind */

#define RANGE(enemy) ((enemy)->unk_098) /* how far it may fly from home */
/* the steps HORNET_AWAY stays once the hornet is back within RANGE. each step
 * within RANGE takes one off, and the step that finds 0 or less clears the
 * bit */
#define AWAY_STEPS(enemy) ((enemy)->unk_04e)
#define BOB_ANGLE(enemy) ((enemy)->unk_1c4)
/* the hornet's acceleration, x along its heading and z across it. nothing
 * sets it, so it stays the 0 that enemy_work_alloc gives. a chasing bomb
 * zeroes its own at each step */
#define ACCEL(enemy) ((enemy)->unk_00c)
#define LASER_LENGTH(enemy) ((enemy)->unk_047)
#define LASER_SPEED(enemy) ((enemy)->unk_094)
/* the steps a laser hornet counts before it fires, and after */
#define INTERVAL(enemy) ((enemy)->unk_048)

/* what a hornet keeps in its work's tick: 1 where it and its bombs cast
 * shadows */
#define CASTS_SHADOW(work) ((work)->tick)
/* a hornet's kind, which it keeps in its work's submode */
#define KIND(work) ((work)->submode)

/* what a bomb keeps in its work and its enemy_work */

/* a bomb's submode holds its index among its hornet's bombs. once the bomb
 * bursts, it holds the blast's kind, for other bombs' bursts to test */
#define BOMB_INDEX(work) ((work)->submode)
#define BLAST_KIND(work) ((work)->submode)
/* where the bomb rides on the hornet, in the hornet's frame */
#define MOUNT(work) ((work)->scl)
#define BLAST_RADIUS(work) ((work)->scl.x)
#define BLAST_FADE(work) ((work)->scl.y)
/* a bomb's frame of BOMB_ASSET(anim) */
#define ANIM_FRAME(enemy) ((enemy)->unk_048)

/* the flags bomb_meet_bounds sets where a bomb meets the stage or leaves its
 * hornet's area */
#define BOMB_AT_BOUNDS (ENEMY_UNDER_CEILING | ENEMY_ON_FLOOR | ENEMY_AT_WALL)

/* a bomb's shapes: its body, then its blast, which starts off */
enum bomb_shape { BODY_SHAPE, BLAST_SHAPE, BOMB_SHAPES };

/* the bombs each kind carries, and a laser hornet's lasers */
#define BOMBS_3 3
#define BOMBS_6 6
#define BOMBS_9 9
#define BOMBS_LASER 3
#define LASERS 3

/* the light the hornet and its bombs draw in */
#define HORNET_LIGHT 8

/* what a hornet adds to scl.y for its RANGE, and to scl.z for its reach */
#define RANGE_BASE 60.0F
/* a hornet whose RANGE lies between -FLY_RANGE_MIN and FLY_RANGE_MIN neither
 * flies nor sends out jets */
#define FLY_RANGE_MIN 10.0F
/* ang.x and ang.z hold the area along x and along z, in units of AREA_UNIT.
 * the high byte, AREA_OFFSET, offsets the area's center from pos. the low
 * byte, AREA_HALF_WIDTH, is the area's half width */
#define AREA_UNIT 2.0F
#define AREA_OFFSET(ang) ((Float)((ang) >> 8) * AREA_UNIT)
#define AREA_HALF_WIDTH(ang) ((Float)(Uint8)(ang) * AREA_UNIT)
/* the area's top lies above pos by scl.y's fraction, and its bottom below pos
 * by scl.z's. each fraction is taken without its sign, in units of
 * AREA_HEIGHT_UNIT */
#define AREA_HEIGHT_UNIT 200.0F
/* a hornet starts at SPEED along its heading. ACCEL can take it no faster
 * than SPEED_MAX, and no slower than SPEED_MIN */
#define SPEED 0.4F
#define SPEED_MAX 0.4F
#define SPEED_MIN 0.1F
/* a hornet bobs BOB_HEIGHT above and below home.y. BOB_ANGLE moves by
 * BOB_STEP a step */
#define BOB_HEIGHT 2.0F
#define BOB_STEP 0x200
/* AWAY_STEPS starts at AWAY_SECONDS, and up to AWAY_SPREAD more at random,
 * in seconds of STEPS_PER_SECOND steps. it starts again at each step the
 * hornet spends beyond RANGE */
#define AWAY_SECONDS 1.5F
#define AWAY_SPREAD 2.0F
/* the cosine of the widest angle, across x and z, between the hornet's
 * heading and a player ahead */
#define AHEAD_COS 0.906F
/* a hornet's sight_angle and turn_speed */
#define SIGHT_ANGLE ((Angle)(160 * ANGLE_PER_DEGREE))
#define TURN_SPEED 0x300
/* a hornet's shadow_size and shadow_ratio */
#define SHADOW_SIZE 12.0F
#define SHADOW_RATIO 1.8F
/* the points for beating each kind, doubled where player_touch_attacks returns
 * TRUE. a laser hornet scores SCORE_3 */
#define SCORE_3 100
#define SCORE_6 200
#define SCORE_9 300
/* the scale a beaten hornet's ball grows to */
#define BALL_SCALE 0.1F
/* the jets' points are in the hornet's frame. the large jet leaves from
 * TAIL_JET_X along x as the hornet watches, and from ATTACK_TAIL_JET_X as it
 * attacks */
#define TAIL_JET_X (-30.0F)
#define ATTACK_TAIL_JET_X (-27.0F)
/* a HORNET_3 or a laser hornet sends out three medium jets from JET_X along x.
 * one leaves from JET_HIGH along y, and two from JET_LOW along y and -JET_Z or
 * JET_Z along z */
#define JET_X (-15.0F)
#define JET_HIGH 14.0F
#define JET_LOW (-7.0F)
#define JET_Z 13.0F
/* each jet moves along the hornet's x at JET_SPEED. it moves at JET_SLOW_SPEED
 * while the hornet attacks or holds HORNET_AWAY or HORNET_ALERT */
#define JET_SPEED (-0.7F)
#define JET_SLOW_SPEED (-0.3F)
/* while a player is near, the hornet shows a light of kind ALERT_LIGHT. the
 * light's point in the hornet's frame is ALERT_LIGHT_X along x and
 * ALERT_LIGHT_Y along y */
#define ALERT_LIGHT 2
#define ALERT_LIGHT_X 2.7F
#define ALERT_LIGHT_Y (-1.5F)

/* a chasing bomb flies at BOMB_SPEED. its pitch turns toward the player by up
 * to PITCH_STEP a step. its heading turns by up to HEADING_STEP */
#define BOMB_SPEED 2.4F
#define PITCH_STEP 0x400
#define HEADING_STEP 0x100
/* a laser's pitch turns from level toward the player by up to LASER_PITCH.
 * its heading turns from the hornet's by up to LASER_HEADING */
#define LASER_PITCH 0x1800
#define LASER_HEADING 0x800
/* the share of its MOUNT a loose bomb drifts by at each step */
#define DRIFT_RATE 0.01F
/* the steps a bomb drifts, stays loose and chases, as its work's timer
 * counts them */
#define DRIFT_STEPS 70
#define LOOSE_STEPS 180
#define CHASE_STEPS 300
/* the radius a bomb's blast starts at, in BLAST_RADIUS and in its shape */
#define BLAST_START 5.0F
/* the blast's growth at each step and the radius it grows to, in
 * BOMB_SMALL_BLAST and in BOMB_BLAST */
#define SMALL_BLAST_STEP 0.04F
#define SMALL_BLAST_END 6.0F
#define BLAST_STEP 1.0F
#define BLAST_END 25.0F
/* once the blast has grown, BLAST_FADE falls from 0 by FADE_STEP at each
 * step. the blast ends below BLAST_FADED */
#define FADE_STEP 0.05F
#define BLAST_FADED (-1.0F)
/* a bursting bomb's turn about y at each step */
#define SPIN 0x100
/* bomb_burst makes a tiny blast where another bomb's large blast lies within
 * NEAR_BLAST. only a blast fewer than FRESH_BLAST steps old counts */
#define NEAR_BLAST 30.0F
#define FRESH_BLAST 15
/* the sparks of a large and of a small blast, and a large blast's puff */
#define BLAST_SPARKS 20
#define SMALL_BLAST_SPARKS 8
#define BLAST_PUFF 15.0F
/* a bomb's shadow_size and shadow_ratio */
#define BOMB_SHADOW_SIZE 5.0F
#define BOMB_SHADOW_RATIO 1.0F
/* a bomb's top and radius, and its bottom's depth, as enemy_meet_stage
 * probes the stage for it */
#define BOMB_PROBE_SIZE 3.0F
/* a bomb's bounces off a wall and a floor, and its friction */
#define BOMB_WALL_BOUNCE 0.7F
#define BOMB_FLOOR_BOUNCE 0.68F
#define BOMB_FRICTION 0.5F

#define HOVER_SOUND 0x4011
#define HOVER_PRIORITY 1
#define HOVER_VOLUME 50
#define HOVER_SOUND_DURATION 80
#define LOOSE_SOUND 0x4012
#define LOOSE_PRIORITY 1
#define LOOSE_VOLUME 10
#define LOOSE_SOUND_DURATION 40
#define CHASE_SOUND 0x4013
#define CHASE_PRIORITY 0
#define CHASE_VOLUME 90
#define BLAST_SOUND 0x4019
#define BLAST_PRIORITY 0
#define BLAST_VOLUME 90

/* the task level of the task that keeps the hornet's textures loaded */
#define TEXTURES_LEVEL 0

/* the box a hornet's bombs burst outside of once let loose, at the hornet's
 * task->extra */
struct hornet_area {
    Sint8 active; /* 0 where there is no area: the bombs meet the stage */
    Float x_max;
    Float x_min;
    Float y_max;
    Float y_min;
    Float z_max;
    Float z_min;
};

/* clang-format off */
static struct collision_shape bomb_shapes[BOMB_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, BODY_LEVELS,
     SHAPE_IGNORE_OBJECTS | SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL | SHAPE_GUARD_LEVEL(3) | SHAPE_ATTACK_LEVEL(2), SHAPE_OFF,
     {0.0F, 0.0F, 0.0F}, BLAST_START, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* nothing reads these BOMBS_MAX vectors */
static NJS_VECTOR dat_hornet_1[BOMBS_MAX] = {
#include "shared/hornet_actor/dat_hornet_1.inc"
};
#define NO_ANIM_STEP 0xffffffff
/* the game_state.steps at which a bomb last applied BOMB_ASSET(anim), or
 * NO_ANIM_STEP before any has. the first bomb drawn in a step applies it for
 * every bomb */
static Uint32 bomb_anim_step = NO_ANIM_STEP;
/* each bomb's MOUNT, by its index among the hornet's bombs */
KEPT static NJS_VECTOR bomb_mounts[BOMBS_MAX] = {
#include "shared/hornet_actor/bomb_mounts.inc"
};
/* each bomb's ang.z, its roll, by the same index */
KEPT static Angle bomb_rolls[BOMBS_MAX] = {
#include "shared/hornet_actor/bomb_rolls.inc"
};
/* the task that keeps the hornet's texture list loaded, named ManTex, or null
 * while none does. it releases the list as it is destroyed */
static struct task *hornet_textures = NULL;
#ifndef GREEN_HILL_AKAHIGE
/* where a laser hornet fires its lasers from, in its frame */
static NJS_POINT3 laser_muzzles[LASERS];
#endif
static void bomb_draw(struct task *task);
static void bomb_draw_burst(struct task *task);
static void bomb_destroy(struct task *task);
static void bomb_step(struct task *task);

/* turn the bomb toward the nearest player, and set spd to BOMB_SPEED along
 * its new pitch and heading */
static void bomb_aim(struct task_work *work, NJS_VECTOR *spd)
{
    NJS_POINT3 target;
    NJS_VECTOR step;
    Float distance;
    /* @hack spelling: dy, a local for the rise to the target, where the
     * difference used in place would do */
    Float dy;
    Angle pitch;
    Angle yaw;

    target = player_work[player_nearest(&work->pos)]->collision->shapes->center;
    distance = njDistanceP2P(&work->pos, &target);
    dy = target.y - work->pos.y;
    if (distance != 0.0F) {
        pitch = asinf(dy / distance) * ANGLE_PER_RADIAN;
        yaw = atan2f(work->pos.z - target.z, target.x - work->pos.x) *
              ANGLE_PER_RADIAN;
    } else {
        yaw = work->ang.y;
        pitch = 0;
    }
    pitch = angle_approach(work->ang.x, pitch, PITCH_STEP);
    yaw = angle_approach(work->ang.y, yaw, HEADING_STEP);
    step.x = BOMB_SPEED;
    step.y = 0.0F;
    step.z = 0.0F;
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, yaw);
    njRotateZ(NULL, pitch);
    njCalcVector(NULL, &step, spd);
    njPopMatrixEx();
    work->ang.x = pitch;
    work->ang.y = yaw;
}

static void bomb_fly(struct task_work *work, struct enemy_work *enemy)
{
    if (!game_state.time_stopped) {
        bomb_aim(work, &enemy->spd);
        njAddVector(&work->pos, &enemy->spd);
        ACCEL(enemy).x = 0.0F;
        ACCEL(enemy).y = 0.0F;
        ACCEL(enemy).z = 0.0F;
    }
}

/* move the bomb by DRIFT_RATE of its MOUNT, turned by the hornet's heading */
static void bomb_drift(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *hornet = task->parent->work;
    NJS_VECTOR step;

    if (IS_AKAHIGE || !game_state.time_stopped) {
        njPushMatrixEx();
        step.x = MOUNT(work).x * DRIFT_RATE;
        step.y = MOUNT(work).y * DRIFT_RATE;
        step.z = MOUNT(work).z * DRIFT_RATE;
        njUnitMatrix(NULL);
        njRotateY(NULL, hornet->ang.y);
        njCalcPoint(NULL, &step, &step);
        njAddVector(&work->pos, &step);
        njPopMatrixEx();
    }
}

/* put the bomb at its MOUNT on the hornet, facing the hornet's way */
static void bomb_ride(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *hornet = task->parent->work;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&hornet->pos);
    njRotateY(NULL, hornet->ang.y);
    njCalcPoint(NULL, &MOUNT(work), &work->pos);
    njPopMatrixEx();
    work->ang.y = hornet->ang.y;
}

/* burst the bomb at pos in a large blast. near another bomb's fresh large
 * blast, the blast is tiny. raised picks the blast's _RAISED kind */
static void bomb_burst(struct task *task, Bool raised, NJS_POINT3 *pos)
{
    struct task *other;
    Bool found = FALSE;
    NJS_VECTOR spd;

    other = task->parent->child;
    do {
        /* @hack spelling: the three tests nested, where one test joined by
         * && would do */
        if (other != task && other->draw == bomb_draw_burst &&
            (BLAST_KIND(other->work) == BLAST_LARGE_RAISED ||
             BLAST_KIND(other->work) == BLAST_LARGE)) {
            if (other->work->timer < FRESH_BLAST) {
                if (njDistanceP2P(pos, &other->work->pos) < NEAR_BLAST) {
                    found = TRUE;
                    break;
                }
            }
        }
        other = other->next;
    } while (other != task->parent->child);
    if (!found) {
        if (raised) {
            BLAST_KIND(task->work) = BLAST_LARGE_RAISED;
        } else {
            BLAST_KIND(task->work) = BLAST_LARGE;
        }
        blast_create(BLAST_KIND(task->work), pos);
        spark_throw(BLAST_SPARKS, pos, NULL);
        sound_play_at(BLAST_SOUND, NULL, BLAST_PRIORITY, BLAST_VOLUME, pos);
        spd.x = 0.0F;
        spd.y = 0.0F;
        spd.z = 0.0F;
        burst_create_wobbling(pos, &spd, BLAST_PUFF);
    } else {
        if (raised) {
            BLAST_KIND(task->work) = BLAST_TINY_RAISED;
        } else {
            BLAST_KIND(task->work) = BLAST_TINY;
        }
        blast_create(BLAST_KIND(task->work), pos);
    }
}

/* adjacent bombs among the hornet's children draw as one run. the run's first
 * bomb pushes the matrix and sets the light, and its last puts both back */
static void bomb_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (task->parent->child == task || task->prev->draw != bomb_draw) {
        light_apply(HORNET_LIGHT);
        if (work->mode != BOMB_CARRIED &&
            stage_current_id() == STAGE_LOST_COLONY) {
            njFogDisable();
        }
        njSetTexture(&HORNET_ASSET(texlist));
        njPushMatrixEx();
    }
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    njRotateX(NULL, -work->ang.x);
    njRotateZ(NULL, work->ang.z);
    if (game_state.steps != bomb_anim_step) {
        bomb_anim_step = game_state.steps;
        texture_anim_apply(&BOMB_ASSET(model), &BOMB_ASSET(anim),
                           ANIM_FRAME(enemy));
    }
    njCnkSimpleDrawModel(&BOMB_ASSET(model));
    if (task->parent->child->prev == task || task->next->draw != bomb_draw) {
        njPopMatrixEx();
    } else {
        matrix_load_parent();
    }
    /* @hack spelling: CASTS_SHADOW compared with 1, where the flag alone
     * would do */
    if (CASTS_SHADOW(task->parent->work) == 1) {
        enemy_draw_shadow(work, enemy);
    }
    if (task->parent->child->prev == task || task->next->draw != bomb_draw) {
        if (work->mode != BOMB_CARRIED &&
            stage_current_id() == STAGE_LOST_COLONY) {
            njFogEnable();
        }
        light_apply(light_after_draw);
    }
}

/* burst the bomb where enemy_take_hit finds a hit or a player touches it */
static void bomb_check_hit(struct task *task, struct task_work *work,
                           struct enemy_work *enemy)
{
    struct task *attack;
    struct collision_work *hit;
    /* @hack spelling: collision, a local for the work's record, where
     * work->collision read in place would do */
    struct collision_work *collision = work->collision;
    Bool player_touched = FALSE;

    if (work->mode == BOMB_CARRIED &&
        (attack = collision_next_attack(task)) != NULL &&
        attack->work->id == WORK_ID_ATTACK && enemy->shot_by == -1) {
        work->flags &= ~WORK_HURT;
        enemy->flags &= ~ENEMY_HIT;
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((hit = collision->hit) != NULL && player_slot_of(hit->owner) != -1) {
        player_touched = TRUE;
    }
    /* @hack spelling: player_touched compared with TRUE, where player_touched
     * alone would do */
    if (enemy_take_hit(work, enemy) || player_touched == TRUE) {
        BLAST_RADIUS(work) = BLAST_START;
        BLAST_FADE(work) = 0.0F;
        work->collision->shapes[BLAST_SHAPE].flags &= ~SHAPE_OFF;
        work->collision->shapes[BODY_SHAPE].flags |= SHAPE_OFF;
        if (enemy->lock_task != NULL || collision_next_item_or_enemy(task) ||
            !(work->flags & WORK_HURT) || collision_next_player(task) ||
            collision_next_attack(task)) {
            BLAST_KIND(task->work) = BLAST_SMALL;
            blast_create(BLAST_SMALL, &work->pos);
            spark_throw(SMALL_BLAST_SPARKS, &work->pos, NULL);
            sound_play_at(BLAST_SOUND, NULL, BLAST_PRIORITY, BLAST_VOLUME,
                          &work->pos);
            work->mode = BOMB_SMALL_BLAST;
        } else {
            bomb_burst(task, 0, &work->pos);
            work->mode = BOMB_BLAST;
        }
        work->timer = 0;
        if (enemy->lock_task != NULL) {
            lockon_release(enemy);
        }
    }
}

/* set the bomb's BOMB_AT_BOUNDS flags by its hornet's area, or by the stage
 * where the hornet has none */
static void bomb_meet_bounds(struct task *task, struct task_work *work,
                             struct enemy_work *enemy)
{
    struct hornet_area *area = task->parent->extra.p;

    /* @hack spelling: active compared with 1, where active alone would do */
    if (area->active == 1) {
        enemy->flags &= ~BOMB_AT_BOUNDS;
        if (work->pos.x > area->x_max || work->pos.x < area->x_min) {
            enemy->flags |= ENEMY_AT_WALL;
        }
        if (work->pos.y > area->y_max) {
            enemy->flags |= ENEMY_UNDER_CEILING;
        }
        if (work->pos.y < area->y_min) {
            enemy->flags |= ENEMY_ON_FLOOR;
        }
        if (work->pos.z > area->z_max || work->pos.z < area->z_min) {
            enemy->flags |= ENEMY_AT_WALL;
        }
    } else {
        enemy_meet_stage(work, enemy);
    }
}

static void bomb_carried(struct task *task, struct task_work *work,
                         struct enemy_work *enemy)
{
    bomb_check_hit(task, work, enemy);
    if (work->mode == BOMB_BLAST || work->mode == BOMB_SMALL_BLAST) {
        task->parent->work->flags |= HORNET_BEATEN;
    }
    bomb_ride(task);
    /* @bug logic: overwrites a burst's mode with BOMB_LOOSE where a carried
     * bomb bursts while its hornet is in MODE_ATTACK (the MOV.B at 0x7b4 of
     * the object's code). the bomb then plays LOOSE_SOUND, and draws as an
     * unburst bomb until HORNET_BEATEN kills it at its next step */
    /* @hack spelling: the two tests nested, where one test joined by && would
     * do */
    if (!IS_LASER(KIND(task->parent->work))) {
        if (task->parent->work->mode == MODE_ATTACK) {
            work->timer = 0;
            work->mode = BOMB_LOOSE;
            work->collision->shapes[BODY_SHAPE].flags &= ~SHAPE_IGNORE_OBJECTS;
            sound_play_at_for(LOOSE_SOUND, work, LOOSE_PRIORITY, LOOSE_VOLUME,
                              LOOSE_SOUND_DURATION, &work->pos);
        }
    }
}

static void bomb_loose(struct task *task, struct task_work *work,
                       struct enemy_work *enemy)
{
    bomb_check_hit(task, work, enemy);
    if (work->timer < DRIFT_STEPS) {
        bomb_drift(task);
    }
    bomb_meet_bounds(task, work, enemy);
    /* @bug logic: bursts the bomb a second time where bomb_check_hit has
     * burst it and bomb_meet_bounds then finds it at its bounds. each burst
     * makes a blast. the second burst sets BOMB_BLAST, so the shape grows as
     * a large blast's even after a small one (the BSRs at 0x7ea and 0x844 of
     * the object's code, to bomb_check_hit and to bomb_burst) */
    if (enemy->flags & BOMB_AT_BOUNDS) {
        BLAST_RADIUS(work) = BLAST_START;
        BLAST_FADE(work) = 0.0F;
        work->collision->shapes[BLAST_SHAPE].flags &= ~SHAPE_OFF;
        work->collision->shapes[BODY_SHAPE].flags |= SHAPE_OFF;
        bomb_burst(task, enemy->flags & ENEMY_ON_FLOOR, &work->pos);
        work->mode = BOMB_BLAST;
        work->timer = 0;
    } else if (work->timer > LOOSE_STEPS) {
        work->timer = 0;
        work->mode = BOMB_CHASING;
        task->parent->work->flags |= HORNET_BOMB_CHASING;
        sound_play_at(CHASE_SOUND, NULL, CHASE_PRIORITY, CHASE_VOLUME,
                      &work->pos);
    }
}

static void bomb_chasing(struct task *task, struct task_work *work,
                         struct enemy_work *enemy)
{
    bomb_check_hit(task, work, enemy);
    bomb_fly(work, enemy);
    bomb_meet_bounds(task, work, enemy);
    /* @bug logic: bursts the bomb again where a hit burst it in the step it
     * meets the stage or the area's sides, as bomb_loose does (the BSRs at
     * 0x8a0 and 0x8fe of the object's code, to bomb_check_hit and to
     * bomb_burst) */
    if ((enemy->flags & BOMB_AT_BOUNDS) || work->timer > CHASE_STEPS) {
        BLAST_RADIUS(work) = BLAST_START;
        BLAST_FADE(work) = 0.0F;
        work->collision->shapes[BLAST_SHAPE].flags &= ~SHAPE_OFF;
        work->collision->shapes[BODY_SHAPE].flags |= SHAPE_OFF;
        bomb_burst(task, enemy->flags & ENEMY_ON_FLOOR, &work->pos);
        work->mode = BOMB_BLAST;
        work->timer = 0;
    }
}

/* a bursting bomb's draw, which draws nothing: blast_create's task draws its
 * blast */
static void bomb_draw_burst(struct task *task)
{
}

static void bomb_small_blast(struct task *task, struct task_work *work,
                             struct enemy_work *enemy)
{
    /* @hack spelling: shape, a local for the blast's shape, where the record's
     * shapes[BLAST_SHAPE] read at each use would do */
    struct collision_shape *shape = &work->collision->shapes[BLAST_SHAPE];

    if (game_state.time_stopped) {
        return;
    }
    if (BLAST_RADIUS(work) < SMALL_BLAST_END) {
        BLAST_RADIUS(work) += SMALL_BLAST_STEP;
        shape->radius += SMALL_BLAST_STEP;
        collision_set_radius(task, shape->radius);
    } else {
        BLAST_FADE(work) -= FADE_STEP;
        if (BLAST_FADE(work) < BLAST_FADED) {
            work->mode = BOMB_SPENT;
            work->timer = 0;
        }
    }
    work->ang.y += SPIN;
}

static void bomb_blast(struct task *task, struct task_work *work,
                       struct enemy_work *enemy)
{
    /* @hack spelling: shape, a local for the blast's shape, where the record's
     * shapes[BLAST_SHAPE] read at each use would do */
    struct collision_shape *shape = &work->collision->shapes[BLAST_SHAPE];

    if (game_state.time_stopped) {
        return;
    }
    if (BLAST_RADIUS(work) < BLAST_END) {
        BLAST_RADIUS(work) += BLAST_STEP;
        shape->radius += BLAST_STEP;
        collision_set_radius(task, shape->radius);
    } else {
        BLAST_FADE(work) -= FADE_STEP;
        if (BLAST_FADE(work) < BLAST_FADED) {
            work->mode = BOMB_SPENT;
            work->timer = 0;
        }
    }
    work->ang.y += SPIN;
}

/* outside GREEN_HILL_AKAHIGE, a bomb that a walker's lock holds as it dies
 * marks its hornet's placement used */
static void bomb_destroy(struct task *task)
{
#ifndef GREEN_HILL_AKAHIGE
    struct enemy_work *enemy = task->motion.p;

    /* @hack spelling: the two tests nested, where one test joined by && would
     * do */
    if (enemy->lock_task != NULL) {
        if (task->parent != NULL && task->parent->placement != NULL) {
            placement_set_used(task->parent);
        }
    }
#endif
    enemy_destroy(task);
}

/* a bomb dies with its hornet, and once its blast fades */
static void bomb_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (task->parent == NULL) {
        if (placement_try_leave(task)) {
            return;
        }
    } else if ((task->parent->work->flags & HORNET_BEATEN) ||
               task->parent->step == task_destroy) {
        task_kill(task);
        return;
    }
    switch (work->mode) {
    case BOMB_CARRIED:
        bomb_carried(task, work, enemy);
        if (work->mode == BOMB_BLAST || work->mode == BOMB_SMALL_BLAST) {
            task->draw = bomb_draw_burst;
        }
        break;
    case BOMB_LOOSE:
        bomb_loose(task, work, enemy);
        if (work->mode == BOMB_BLAST || work->mode == BOMB_SMALL_BLAST) {
            task->draw = bomb_draw_burst;
        }
        break;
    case BOMB_CHASING:
        bomb_chasing(task, work, enemy);
        if (work->mode == BOMB_BLAST || work->mode == BOMB_SMALL_BLAST) {
            task->draw = bomb_draw_burst;
        }
        break;
    case BOMB_BLAST:
        bomb_blast(task, work, enemy);
        if (work->mode != BOMB_BLAST) {
            task_kill(task);
            return;
        }
        break;
    case BOMB_SMALL_BLAST:
        bomb_small_blast(task, work, enemy);
        if (work->mode != BOMB_SMALL_BLAST) {
            task_kill(task);
            return;
        }
        break;
    }
    collision_register(task);
    if (!game_state.time_stopped) {
        work->timer++;
        ANIM_FRAME(enemy)++;
    }
    /* @hack redundant: a switch and the two increments after it, which no
     * path reaches. the image holds the code of the switch's two cases and of
     * the increments, but no compare of the mode with either case */
    if (0) {
        switch (work->mode) {
        case BOMB_CARRIED:
            bomb_carried(task, work, enemy);
            break;
        case BOMB_BLAST:
            task_kill(task);
            break;
        }
        work->timer++;
        ANIM_FRAME(enemy)++;
    }
}

static void bomb_init(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;

    collision_init(task, bomb_shapes, BOMB_SHAPES, COLLISION_ITEM);
    task->work->collision->flags &= ~COLLISION_TARGET;
    enemy = enemy_work_alloc(task);
    enemy->shadow_size = BOMB_SHADOW_SIZE;
    enemy->shadow_ratio = BOMB_SHADOW_RATIO;
    enemy->flags |= ENEMY_LEAVES_CHAOS_DRIVE;
    work->timer = 0;
    work->mode = BOMB_CARRIED;
    enemy->aim_y = work->ang.y;
    enemy->motion.frame = 0.0F;
    enemy->probe_offset.x = 0.0F;
    enemy->probe_offset.y = 0.0F;
    enemy->probe_offset.z = 0.0F;
    enemy->top = BOMB_PROBE_SIZE;
    enemy->radius = BOMB_PROBE_SIZE;
    enemy->bottom = -BOMB_PROBE_SIZE;
    enemy->wall_bounce = BOMB_WALL_BOUNCE;
    enemy->floor_bounce = BOMB_FLOOR_BOUNCE;
    enemy->friction = BOMB_FRICTION;
    enemy->unk_0e4 = 0.7F;
    enemy_set_home(work, enemy);
    task->destructor = bomb_destroy;
    task->draw = bomb_draw;
}

/* make the bombs that task's kind carries, as task's children. a bomb the
 * heap has no room for isn't made */
static void bombs_create(struct task *task)
{
    struct task *bomb;
    /* @hack spelling: kind, a local for the hornet's KIND, where KIND read at
     * each test would do */
    int kind;
    int i;
    int count;

    if (task == NULL) {
        return;
    }
    kind = KIND(task->work);
    if (kind == HORNET_3) {
        count = BOMBS_3;
    } else if (kind == HORNET_6) {
        count = BOMBS_6;
#ifndef GREEN_HILL_AKAHIGE
    } else if (kind == HORNET_9) {
        count = BOMBS_9;
    } else if (kind == HORNET_LASER) {
        count = BOMBS_LASER;
#endif
    } else {
        return;
    }
    for (i = 0; i < count; i++) {
        bomb = task_create_child(TASK_WORK, bomb_step, task);
        if (bomb != NULL) {
            BOMB_INDEX(bomb->work) = i;
            MOUNT(bomb->work) = bomb_mounts[i];
            bomb->work->ang.z = bomb_rolls[i];
            bomb_init(bomb);
        }
    }
}

static void hornet_textures_release(struct task *task)
{
    njReleaseTexture(&HORNET_ASSET(texlist));
    hornet_textures = NULL;
}

static void hornet_textures_step(struct task *task)
{
}

static void hornet_draw(struct task *task);
static void hornet_destroy(struct task *task);

/* give the hornet its kind, range, reach and area from its work's scl and
 * ang. where the heap has no room for the area, the call sets nothing up. the
 * mode then stays MODE_INIT */
static void hornet_init(struct task *task, struct task_work *work)
{
    struct hornet_area *area;
    struct enemy_work *enemy;

    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if (!(area = syCalloc(1, sizeof(struct hornet_area)))) {
        return;
    }
    task->extra.p = area;
    enemy = enemy_work_alloc_grouped(task);
    KIND(work) = njRoundOff(work->scl.x);
    if (KIND(work) >= KIND_COUNT || KIND(work) < 0) {
        KIND(work) = HORNET_3;
    }
    collision_init(task, hornet_shapes, HORNET_SHAPES, COLLISION_ENEMY);
    if (KIND(work) == HORNET_3) {
        enemy->motion.object = &HORNET_ASSET(3_object);
        RANGE(enemy) = work->scl.y + RANGE_BASE;
    } else if (KIND(work) == HORNET_6) {
        enemy->motion.object = &HORNET_ASSET(6_object);
        RANGE(enemy) = work->scl.y + RANGE_BASE;
#ifndef GREEN_HILL_AKAHIGE
    } else if (KIND(work) == HORNET_9) {
        enemy->motion.object = &hornet_9_object;
        RANGE(enemy) = work->scl.y + RANGE_BASE;
    } else if (KIND(work) == HORNET_LASER) {
        LASER_LENGTH(enemy) = work->ang.x;
        INTERVAL(enemy) = work->ang.z;
        LASER_SPEED(enemy) = work->scl.y;
        enemy->motion.object = &laser_hornet_object;
        enemy_laser_load();
        RANGE(enemy) = RANGE_BASE;
#endif
    }
    if (!IS_LASER(KIND(work)) && work->ang.x && work->ang.z) {
        area->active = 1;
        area->x_max = work->pos.x + AREA_OFFSET(work->ang.x) +
                      AREA_HALF_WIDTH(work->ang.x);
        area->x_min = work->pos.x + AREA_OFFSET(work->ang.x) -
                      AREA_HALF_WIDTH(work->ang.x);
        area->z_max = work->pos.z + AREA_OFFSET(work->ang.z) +
                      AREA_HALF_WIDTH(work->ang.z);
        area->z_min = work->pos.z + AREA_OFFSET(work->ang.z) -
                      AREA_HALF_WIDTH(work->ang.z);
        area->y_max =
            work->pos.y + (njAbs(njFraction(work->scl.y)) * AREA_HEIGHT_UNIT);
        area->y_min =
            work->pos.y - (njAbs(njFraction(work->scl.z)) * AREA_HEIGHT_UNIT);
    }
    work->ang.x = 0;
    work->ang.z = 0;
    work->scl.y = BOB_HEIGHT;
    enemy->sight_distance2 =
        (work->scl.z + RANGE_BASE) * (work->scl.z + RANGE_BASE);
    enemy->sight_angle = SIGHT_ANGLE;
    enemy->near_distance2 =
        (work->scl.z + RANGE_BASE) * (work->scl.z + RANGE_BASE);
    enemy->spd.x = SPEED;
    enemy->turn_speed = TURN_SPEED;
    enemy_light_load_texture();
    enemy_set_home(work, enemy);
    work->mode = MODE_WATCH;
    if (njFraction(work->scl.x) == 0.0F) {
        CASTS_SHADOW(work) = 1;
        enemy->shadow_size = SHADOW_SIZE;
        enemy->shadow_ratio = SHADOW_RATIO;
    }
    /* @hack spelling: the two bits set by two statements, where one |= would
     * do */
    enemy->flags |= ENEMY_LEAVES_CHAOS_DRIVE;
    enemy->flags |= ENEMY_BURST;
    task->draw = hornet_draw;
    task->destructor = hornet_destroy;
    enemy_jet_load_texture();
    blast_load_texture();
    if (hornet_textures == NULL) {
        texture_load_pvm(HORNET_TEXTURE_NAME, &HORNET_ASSET(texlist));
        hornet_textures =
            task_create(0, TEXTURES_LEVEL, hornet_textures_step, "ManTex");
        hornet_textures->destructor = hornet_textures_release;
    }
}

static void hornet_to_world(struct task_work *work, NJS_POINT3 *pos)
{
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, pos, pos);
    njPopMatrixEx();
}

static void hornet_turn_vector(struct task_work *work, NJS_VECTOR *vec)
{
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, work->ang.y);
    njCalcVector(NULL, vec, vec);
    njPopMatrixEx();
}

static void hornet_turn(struct task_work *work, struct enemy_work *enemy)
{
    int player;

    if (game_state.time_stopped) {
        return;
    }
    if (work->flags & HORNET_AWAY) {
        enemy_turn_to_home(work, enemy);
    } else if (work->flags & HORNET_ALERT) {
        player = player_nearest(&work->pos);
        if (player == 0 || player == 1) {
            enemy_turn_to_player(work, enemy, player);
        }
    }
}

static void hornet_bob(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: y, a local for the sine taken before BOB_ANGLE steps,
     * where njSin's result used in place would do */
    Float y;

    if (!game_state.time_stopped) {
        y = njSin(BOB_ANGLE(enemy));
        BOB_ANGLE(enemy) += BOB_STEP;
        work->pos.y = enemy->home.y + (y * BOB_HEIGHT);
    }
}

static void hornet_mark_away(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: distance, a local for enemy_home_distance2's result,
     * where the call compared in place would do */
    Float distance;

    if (!game_state.time_stopped) {
        distance = enemy_home_distance2(work, enemy);
        if (distance > RANGE(enemy) * RANGE(enemy)) {
            work->flags |= HORNET_AWAY;
            AWAY_STEPS(enemy) =
                (((Float)rand() * (AWAY_SPREAD * RAND_SCALE)) + AWAY_SECONDS) *
                STEPS_PER_SECOND;
        } else if (AWAY_STEPS(enemy)-- <= 0) {
            work->flags &= ~HORNET_AWAY;
            AWAY_STEPS(enemy) = 0;
        }
    }
}

static void hornet_check_hit(struct task *task, struct task_work *work,
                             struct enemy_work *enemy)
{
    struct task *attack;
    NJS_VECTOR offset;
    NJS_VECTOR scl;
    int score;
    int player;

    if (!enemy_take_hit(work, enemy) && !(work->flags & HORNET_BEATEN)) {
        return;
    }
    collision_restart_scans();
    if ((attack = collision_next_attack(task)) != NULL &&
        attack->work->id == WORK_ID_ATTACK && enemy->shot_by == -1) {
        work->flags &= ~WORK_HURT;
        enemy->flags &= ~ENEMY_HIT;
        return;
    }
    work->mode = MODE_DEFEATED;
    work->collision->shapes->flags |= SHAPE_OFF;
    work->timer = 0;
    enemy_set_home(work, enemy);
    switch (KIND(work)) {
    case HORNET_3:
        enemy_break(work, hornet_3_pieces, &HORNET_ASSET(texlist));
        break;
    case HORNET_6:
        enemy_break(work, hornet_6_pieces, &HORNET_ASSET(texlist));
        break;
#ifndef GREEN_HILL_AKAHIGE
    case HORNET_9:
        enemy_break(work, hornet_9_pieces, &HORNET_ASSET(texlist));
        break;
    case HORNET_LASER:
        enemy_break(work, laser_hornet_pieces, &HORNET_ASSET(texlist));
        break;
#endif
    }
#ifdef NONMATCHING
    /* this store quiets GCC's warning that score may be used unset. the switch
     * below sets score for every KIND that hornet_init leaves */
    score = SCORE_3;
#endif
    switch (KIND(work)) {
    case HORNET_3:
#ifndef GREEN_HILL_AKAHIGE
    case HORNET_LASER:
#endif
        score = SCORE_3;
        break;
    case HORNET_6:
        score = SCORE_6;
        break;
#ifndef GREEN_HILL_AKAHIGE
    case HORNET_9:
        score = SCORE_9;
        break;
#endif
    }
    if (player_touch_attacks(task)) {
        score *= 2;
    }
    score_add_enemy(score);
    enemy_defeat(work, enemy);
    work->flags |= HORNET_BEATEN;
    if (enemy->flags & ENEMY_IN_MAGIC_HAND) {
        /* @hack spelling: player set to shot_by, where shot_by passed at each
         * use would do */
        player = enemy->shot_by;
        /* @hack redundant: a store to offset.z, which the next statement
         * overwrites */
        offset.z = 15.0F;
        /* @hack spelling: one assignment to y and z, where a statement for
         * each would do */
        offset.y = offset.z = 0.0F;
        /* @hack spelling: one assignment to the three, where a statement for
         * each would do */
        scl.x = scl.y = scl.z = BALL_SCALE;
        /* @bug uninitialized: each case passes enemy_ball_create an offset
         * whose x no path sets. the ball's task keeps that x with the rest of
         * the offset (the JSR at 0x144c of the object's code) */
#ifdef NONMATCHING
        /* this side repairs the tag above: offset.x is 0, as y and z are, so
         * the ball draws the hornet at its center */
        offset.x = 0.0F;
#endif
        switch (KIND(work)) {
        case HORNET_3:
            enemy_ball_create(player, &HORNET_ASSET(3_object),
                              &HORNET_ASSET(texlist), &scl, &offset);
            break;
        case HORNET_6:
            enemy_ball_create(player, &HORNET_ASSET(6_object),
                              &HORNET_ASSET(texlist), &scl, &offset);
            break;
#ifndef GREEN_HILL_AKAHIGE
        case HORNET_9:
            enemy_ball_create(player, &hornet_9_object, &HORNET_ASSET(texlist),
                              &scl, &offset);
            break;
        case HORNET_LASER:
            enemy_ball_create(player, &laser_hornet_object,
                              &HORNET_ASSET(texlist), &scl, &offset);
            break;
#endif
        }
    }
}

static void hornet_mark_player_near_home(struct task_work *work,
                                         struct enemy_work *enemy)
{
    if (enemy_find_player_near_home(work, enemy)) {
        work->flags |= HORNET_ALERT;
    } else {
        work->flags &= ~HORNET_ALERT;
    }
}

static void hornet_move(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    if (njAbs(RANGE(enemy)) < FLY_RANGE_MIN) {
        work->pos.y = enemy->home.y;
    } else {
        enemy->spd.x += ACCEL(enemy).x;
        enemy->spd.z += ACCEL(enemy).z;
        if (enemy->spd.x > SPEED_MAX) {
            enemy->spd.x = SPEED_MAX;
        } else if (enemy->spd.x < SPEED_MIN) {
            enemy->spd.x = SPEED_MIN;
        }
        work->pos.x += (enemy->spd.x * njCos(work->ang.y)) +
                       (enemy->spd.z * njSin(work->ang.y));
        work->pos.y = enemy->home.y;
        work->pos.z += (-enemy->spd.x * njSin(work->ang.y)) +
                       (enemy->spd.z * njCos(work->ang.y));
    }
    enemy_keep_near_home(work, enemy, RANGE(enemy));
}

#ifndef GREEN_HILL_AKAHIGE
/* turn dir, which must lie along x, toward the nearest player from pos */
static void hornet_aim(struct task_work *work, NJS_POINT3 *pos, NJS_VECTOR *dir)
{
    NJS_POINT3 target;
    Float distance;
    /* @hack spelling: dy, a local for the rise to the target, where the
     * difference used in place would do */
    Float dy;
    Angle pitch;
    Angle yaw;

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
    /* @hack spelling: pitch and yaw reused for the turns' results, where
     * angle_approach's results passed in place would do */
    pitch = angle_approach(0, pitch, LASER_PITCH);
    yaw = angle_approach(work->ang.y, yaw, LASER_HEADING);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, yaw);
    njRotateZ(NULL, pitch);
    njCalcPoint(NULL, dir, dir);
    njPopMatrixEx();
}

static void hornet_fire_lasers(struct task *task, struct task_work *work,
                               struct enemy_work *enemy)
{
    int i;
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    for (i = 0; i < LASERS; i++) {
        pos = laser_muzzles[i];
        hornet_to_world(work, &pos);
        dir.x = LASER_SPEED(enemy);
        dir.y = 0.0F;
        dir.z = 0.0F;
        hornet_aim(work, &pos, &dir);
        enemy_laser_fire(task, LASER_LENGTH(enemy), &dir, &pos);
    }
}

#endif
static void hornet_destroy(struct task *task)
{
    enemy_destroy(task);
    sound_stop_key(task);
    /* @hack redundant: a return at the function's end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

#ifdef GREEN_HILL_AKAHIGE
#define WATCH_TASK actor
#define WATCH_ENEMY actor_enemy
#else
#define WATCH_TASK task
#define WATCH_ENEMY enemy
#endif

static void hornet_watch(struct task *WATCH_TASK, struct task_work *work,
                         struct enemy_work *WATCH_ENEMY)
{
#ifdef GREEN_HILL_AKAHIGE
    /* @hack spelling: task and enemy copied from the parameters actor and
     * actor_enemy, where parameters named task and enemy would do */
    struct task *task = actor;
    struct enemy_work *enemy = actor_enemy;
#endif
    NJS_POINT3 pos;
    NJS_VECTOR dir;
    int player;

    hornet_mark_away(work, enemy);
    hornet_mark_player_near_home(work, enemy);
    hornet_turn(work, enemy);
    if (!(work->flags & HORNET_ALERT) || placement_used(task)) {
        hornet_move(work, enemy);
    }
    hornet_bob(work, enemy);
#ifdef GREEN_HILL_AKAHIGE
    if (njAbs(RANGE(enemy)) >= FLY_RANGE_MIN)
#else
    /* @hack spelling: the empty arm, where the else's arm under the test
     * reversed would do */
    if (njAbs(RANGE(enemy)) < FLY_RANGE_MIN) {
    } else if (!game_state.time_stopped)
#endif
    {
        pos.x = TAIL_JET_X;
        pos.z = 0.0F;
        pos.y = 0.0F;
        hornet_to_world(work, &pos);
        /* @hack spelling: the test negated, where its arms swapped would do */
        dir.x = !(work->flags & (HORNET_AWAY | HORNET_ALERT)) ? JET_SPEED
                                                              : JET_SLOW_SPEED;
        dir.z = 0.0F;
        dir.y = 0.0F;
        hornet_turn_vector(work, &dir);
        enemy_jet_create(NULL, ENEMY_JET_LARGE, &pos, &dir);
        if (KIND(work) == HORNET_3 || IS_LASER(KIND(work))) {
            pos.x = JET_X;
            pos.y = JET_HIGH;
            pos.z = 0.0F;
            hornet_to_world(work, &pos);
            enemy_jet_create(NULL, ENEMY_JET_MEDIUM, &pos, &dir);
            pos.x = JET_X;
            pos.y = JET_LOW;
            pos.z = -JET_Z;
            hornet_to_world(work, &pos);
            enemy_jet_create(NULL, ENEMY_JET_MEDIUM, &pos, &dir);
            pos.x = JET_X;
            pos.y = JET_LOW;
            pos.z = JET_Z;
            hornet_to_world(work, &pos);
            enemy_jet_create(NULL, ENEMY_JET_MEDIUM, &pos, &dir);
        }
    }
    hornet_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
#ifndef GREEN_HILL_AKAHIGE
    if (KIND(work) == HORNET_LASER) {
        if (!(work->flags & HORNET_ALERT)) {
            work->timer = 0;
        } else if (work->flags & ENEMY_LASER_CHARGING) {
            work->flags &= ~ENEMY_LASER_CHARGING;
        } else if (work->timer > INTERVAL(enemy)) {
            work->mode = MODE_ATTACK;
            work->timer = 0;
            hornet_fire_lasers(task, work, enemy);
        }
        return;
        /* @hack spelling: an else after the return, where the test alone would
         * do */
    } else if (placement_used(task))
#else
    if (placement_used(task))
#endif
    {
        return;
    }
#ifdef GREEN_HILL_AKAHIGE
    if (work->flags & HORNET_ALERT)
#else
    if (!(work->flags & HORNET_ALERT)) {
        return;
    }
#endif
    {
        player = player_nearest(&work->pos);
        if (player != -1) {
            pos = player_work[player]->pos;
            njSubVector(&pos, &work->pos);
            pos.y = 0.0F;
            njUnitVector(&pos);
        } else {
            pos.z = 0.0F;
            pos.y = 0.0F;
            pos.x = 0.0F;
        }
        dir.x = 1.0F;
        dir.y = 0.0F;
        dir.z = 0.0F;
        hornet_turn_vector(work, &dir);
        if (njInnerProduct(&pos, &dir) > AHEAD_COS) {
            work->timer = 0;
            work->mode = MODE_ATTACK;
            placement_set_used(task);
        }
    }
}

static void hornet_attack(struct task *task, struct task_work *work,
                          struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    hornet_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        return;
    }
#ifdef GREEN_HILL_AKAHIGE
    if (!game_state.time_stopped) {
        work->timer++;
    }
#else
    if (game_state.time_stopped) {
        return;
    }
    work->timer++;
#endif
    if (njAbs(RANGE(enemy)) >= FLY_RANGE_MIN) {
        pos.x = ATTACK_TAIL_JET_X;
        /* @hack spelling: one assignment to y and z, where a statement for
         * each would do */
        pos.y = pos.z = 0.0F;
        hornet_to_world(work, &pos);
        dir.x = JET_SLOW_SPEED;
        /* @hack spelling: one assignment to y and z, where a statement for
         * each would do */
        dir.y = dir.z = 0.0F;
        hornet_turn_vector(work, &dir);
        enemy_jet_create(NULL, ENEMY_JET_LARGE, &pos, &dir);
        if (KIND(work) == HORNET_3 || IS_LASER(KIND(work))) {
            pos.x = JET_X;
            pos.y = JET_HIGH;
            pos.z = 0.0F;
            hornet_to_world(work, &pos);
            enemy_jet_create(NULL, ENEMY_JET_MEDIUM, &pos, &dir);
            pos.x = JET_X;
            pos.y = JET_LOW;
            pos.z = -JET_Z;
            hornet_to_world(work, &pos);
            enemy_jet_create(NULL, ENEMY_JET_MEDIUM, &pos, &dir);
            pos.x = JET_X;
            pos.y = JET_LOW;
            pos.z = JET_Z;
            hornet_to_world(work, &pos);
            enemy_jet_create(NULL, ENEMY_JET_MEDIUM, &pos, &dir);
        }
    }
#ifndef GREEN_HILL_AKAHIGE
    if (KIND(work) == HORNET_LASER) {
        if (work->flags & ENEMY_LASER_CHARGING) {
            work->flags &= ~ENEMY_LASER_CHARGING;
        } else if (work->timer > INTERVAL(enemy)) {
            work->mode = MODE_WATCH;
            work->timer = 0;
            /* @hack redundant: a return that ends the arm, where the function
             * ends after it anyway */
            return;
        }
        /* @bug logic: never ends the attack where every bomb bursts before one
         * starts to chase, since only a chasing bomb sets
         * HORNET_BOMB_CHASING. the hornet stays in MODE_ATTACK, which never
         * moves it (the TST at 0x1c4a of the object's code) */
    } else if (work->flags & HORNET_BOMB_CHASING)
#else
    if (work->flags & HORNET_BOMB_CHASING)
#endif
    {
        work->flags &= ~HORNET_BOMB_CHASING;
        work->mode = MODE_WATCH;
        work->timer = 0;
#ifndef GREEN_HILL_AKAHIGE
        /* @hack redundant: a return that ends the arm, where the function ends
         * after it anyway */
        return;
#endif
    }
}

/* nothing calls this empty function, but the image holds it. it is external
 * because SHC leaves out a static function that nothing calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void HORNET_UNUSED(void)
{
}

static void hornet_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    Angle zero;
    int kind;

    light_apply(HORNET_LIGHT);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    njSetTexture(&HORNET_ASSET(texlist));
    njCnkSimpleDrawObject(enemy->motion.object);
    njPopMatrixEx();
    if (CASTS_SHADOW(work)) {
        enemy_draw_shadow(work, enemy);
    }
    /* @hack redundant: a readout that no path reaches. the image holds its
     * strings, its cases' code and the code after its switch. it holds no
     * code of the statements before the switch, but they set kind, which the
     * switch reads, and zero, which the code after it reads */
    if (0) {
#ifdef GREEN_HILL_AKAHIGE
        zero = 0;
        kind = KIND(work);
#else
        kind = KIND(work);
        if (kind >= HORNET_KINDS) {
            kind = HORNET_LASER;
        }
        njPrintD(NJM_LOCATION(29, 19), kind, 1);
        zero = 0;
#endif
        switch (kind) {
        case HORNET_3:
        case HORNET_6:
#ifndef GREEN_HILL_AKAHIGE
        case HORNET_9:
#endif
            njPrintC(NJM_LOCATION(17, 16), "-> AREA X DIFF/X SIZE ");
            njPrintC(NJM_LOCATION(17, 18), "-> AREA Z DIFF/Z SIZE ");
            njPrintC(NJM_LOCATION(17, 20), "-> MOVE RANGE/MAX Y");
            njPrintC(NJM_LOCATION(17, 21), "-> VIEW RANGE/MIN Y");
            break;
#ifndef GREEN_HILL_AKAHIGE
        case HORNET_LASER:
            njPrintC(NJM_LOCATION(17, 16), "-> LASER LENGTH ");
            njPrintD(NJM_LOCATION(33, 16), (Uint8)work->ang.x, 3);
            njPrintC(NJM_LOCATION(17, 18), "-> INTERVAL ");
            njPrintD(NJM_LOCATION(29, 18), work->ang.z, 5);
            njPrintC(NJM_LOCATION(34, 18), " FRAMES");
            njPrintC(NJM_LOCATION(17, 20), "-> LASER SPEED");
            njPrintC(NJM_LOCATION(17, 21), "-> VIEW RANGE");
            break;
#endif
        }
        njPrintC(NJM_LOCATION(17, 17), "-> ANG Y ");
        njPrintH(NJM_LOCATION(26, 17), work->ang.y & 0xff00, 4);
        njPrintC(NJM_LOCATION(30, 17), " EME ");
        njPrintD(NJM_LOCATION(35, 17), (Uint8)work->ang.y, 3);
        njPrintC(NJM_LOCATION(17, 19), DEBUG_KIND_LABEL);
        if (njFraction(work->scl.x) == 0.0F) {
            njPrintC(NJM_LOCATION(36, 19), "ON");
        } else {
            njPrintC(NJM_LOCATION(36, 19), "OFF");
        }
        {
            struct angles ang;
            NJS_VECTOR scl;

            /* @hack spelling: one assignment to the three, where a statement
             * for each would do */
            ang.x = ang.y = ang.z = zero;
            /* @hack spelling: one assignment to scl.z and scl.x, where a
             * statement for each would do */
            scl.z = scl.x = work->scl.z + RANGE_BASE;
            scl.y = 1.0F;
            debug_draw_model_unlit(&work->pos, &ang, &scl,
                                   DEBUG_MODEL_CYLINDER);
            /* @hack spelling: one assignment to the three, where a statement
             * for each would do */
            ang.x = ang.y = ang.z = zero;
            /* @hack spelling: one assignment to scl.z and scl.x, where a
             * statement for each would do */
            scl.z = scl.x = work->scl.y + RANGE_BASE;
            scl.y = 40.0F;
            debug_draw_model_unlit(&work->pos, &ang, &scl,
                                   DEBUG_MODEL_CYLINDER);
        }
        if (work->ang.x && work->ang.z) {
            Float y_max = work->pos.y +
                          (njAbs(njFraction(work->scl.y)) * AREA_HEIGHT_UNIT) +
                          1.0F;
            Float y_min = work->pos.y -
                          (njAbs(njFraction(work->scl.z)) * AREA_HEIGHT_UNIT) -
                          1.0F;
            NJS_VECTOR size;
            NJS_POINT3 center;

            center.x = work->pos.x + AREA_OFFSET(work->ang.x);
            center.y = (y_max + y_min) * 0.5F;
            center.z = work->pos.z + AREA_OFFSET(work->ang.z);
            size.x = (AREA_HALF_WIDTH(work->ang.x) + 5.0F) * 2.0F;
            size.y = y_max - y_min;
            size.z = (AREA_HALF_WIDTH(work->ang.z) + 5.0F) * 2.0F;
            debug_draw_model_unlit(&center, NULL, &size, DEBUG_MODEL_BOX);
        }
    }
    light_apply(light_after_draw);
}

void HORNET_STEP(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    NJS_POINT3 pos;

    if (work->mode != MODE_INIT && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
#ifdef GREEN_HILL_AKAHIGE
        placement_clear_used(task);
#endif
        hornet_init(task, work);
        if (!IS_AKAHIGE && placement_used(task)) {
            return;
        }
        /* @bug logic: makes more bombs at each step where the heap has no room
         * for the area, since hornet_init then leaves the mode at MODE_INIT
         * (the JSR to bombs_create at 0x2000 of the object's code) */
        bombs_create(task);
        return;
    case MODE_WATCH:
        hornet_watch(task, work, enemy);
        break;
    case MODE_ATTACK:
        hornet_attack(task, work, enemy);
        break;
    case MODE_DEFEATED:
        work->mode = MODE_DEAD;
        /* fall through */
    case MODE_DEAD:
        placement_kill(task);
        return;
    /* @hack redundant: a default that only breaks */
    default:
        break;
    }
    /* @hack spelling: the empty arm, where the else's arm under the test
     * reversed would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        if (work->flags & HORNET_ALERT) {
            njPushMatrix(&_nj_unit_matrix_);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            pos.x = ALERT_LIGHT_X;
            pos.y = ALERT_LIGHT_Y;
            pos.z = 0.0F;
            njCalcPoint(NULL, &pos, &pos);
            enemy_light_create(task, ALERT_LIGHT, &pos);
            njPopMatrixEx();
        }
        if (work->mode == MODE_DEFEATED) {
            return;
        }
        sound_start_at_for(HOVER_SOUND, work, HOVER_PRIORITY, HOVER_VOLUME,
                           HOVER_SOUND_DURATION, &work->pos);
    }
    if (work->mode == MODE_DEFEATED) {
        return;
    }
    collision_register(task);
    /* @hack spelling: the test of 0.0F, where 0 would do */
    if (0.0F) {
        /* @hack redundant: a switch that no path reaches, whose one case sets
         * the hornet up anew. the image holds the case's code, but no compare
         * of the mode with the case */
        switch (work->mode) {
#ifdef GREEN_HILL_AKAHIGE
        case MODE_DEAD + 1:
#else
        case MODE_INIT:
#endif
            placement_clear_used(task);
            hornet_init(task, work);
            bombs_create(task);
            break;
        }
    }
}

#endif
