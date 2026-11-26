#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "boss_last1/body.h"
#include "boss_last1/arena.h"
#include "boss_last1/breath.h"
#include "boss_last1/camera.h"
#include "boss_last1/contacts.h"
#include "boss_last1/energy.h"
#include "boss_last1/lift.h"
#include "boss_last1/model_art.h"
#include "boss_last1/motion_art.h"
#include "boss_last1/pose.h"
#include "boss_last1/rails.h"
#include "boss_last1/sphere.h"
#include "boss_last1/stage.h"
#include "boss_last1/weak_point.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "draw/motion.h"
#include "player/player.h"
#include "shared/boss.h"
#include "shared/boss_title.h"
#include "shared/last_boss_arena.h"
#include "shared/last_boss_defend.h"
#include "shared/last_boss_sphere.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"
#include "vmu/vibration.h"
#define BODY_SHAPES 58
/* the shapes from 0 that can hurt the player. a wave spawns its spheres
 * clear of their obstacles. the shapes past them guard the body from
 * objects */
#define BODY_PARTS 35
/* the first of the two capsules that cross on top of the body. the lifted
 * wave turns both off while it lasts */
#define TOP_SHAPE 3
/* the head's sphere, at BIOLIZARD_JOINT_HEAD_MIDDLE, and the mouth's */
#define HEAD_SHAPE 9
#define MOUTH_SHAPE 10
/* the shapes that follow the neck's upper joints, and the tail's */
#define UPPER_NECK_FIRST 14
#define UPPER_NECK_LAST 20
#define TAIL_FIRST 21
#define TAIL_LAST 26
#define RECORD_RADIUS 400.0F /* the collision record's */
#define BODY_LEVEL 2 /* task_create's level */

/* the power the Biolizard starts with. each hit on the life support takes 1 */
#define BODY_POWER 6
/* where player 0 starts: START_RADIUS from the y axis and START_HEIGHT up */
#define START_RADIUS 265.0F
#define START_HEIGHT 10.0F
/* a leg splashes as the point LEG_REACH below its joint enters the water */
#define LEG_REACH (-56.0F)
/* the stage messages a retry's messenger shows, one for each of the first
 * five retries. a later one shows the fifth */
#define MESSAGES 5
/* the alpha the title loses each step as it fades */
#define TITLE_FADE 0.05F

/* the work's flags. under BODY_REVERSED the Biolizard faces the other way
 * round the arena. it then plays the _R twin of each motion of side_motions.
 * BODY_FIXED_BLEND holds radius_blend still */
#define BODY_REVERSED 0x100
#define BODY_FIXED_BLEND 0x200

/* body->flags. the next body_sense after BODY_CHECK_SIDE is set makes
 * BODY_REVERSED match BODY_PLAYER_CLOCKWISE */
#define BODY_SIDE_REVERSED 1 /* copies BODY_REVERSED, and goes unread */
#define BODY_PLAYER_CLOCKWISE 4
#define BODY_TURNING 8 /* nothing reads it */
#define BODY_CHECK_SIDE 0x10
#include "boss_last1/body/text.inc"

/* body_locate_player's walks, in body->walk. MOTION_WALK_LL + a walk is its
 * entry. _LL and _RR walk head first, and _LR and _RL tail first. a walk
 * ending in L turns the work's yaw up, and one ending in R turns it down */
enum walk { WALK_LL, WALK_LR, WALK_RR, WALK_RL };

/* how body_locate_player changes the walk. WALK_REVERSE sets the new walk at
 * the old one's frame mirrored, and WALK_LINK links to the new walk */
enum walk_change { WALK_KEEP, WALK_REVERSE, WALK_LINK };

/* the angles from the Biolizard's points past which body_locate_player
 * links to a new walk, or reverses the walk */
#define LINK_ANGLE 0x1800
#define REVERSE_ANGLE 0x3800

/* BIOLIZARD_JOINT_FRONT lies FRONT_REACH out along the model's -x */
#define FRONT_REACH 300.0F
/* radius_blend's target is 0 while player 0 lies within BLEND_NEAR of the y
 * axis, and 1 beyond BLEND_FAR. between the two it grows in proportion. each
 * step takes radius_blend BLEND_STEP toward it */
#define BLEND_NEAR 200.0F
#define BLEND_FAR 400.0F
#define BLEND_STEP 0.02F

/* a point above BACK_HEIGHT is on the Biolizard's back or its rails */
#define BACK_HEIGHT 60.0F
/* the contacts after a push that push nothing */
#define PUSH_WAIT 7
/* a push's speed from the tail. the guard flare stands PUSH_SPEED past
 * player 0 from the shape's center */
#define PUSH_SPEED 10.0F
/* STRONG_PUSH pushes from the head, the mouth or the upper neck, or in a
 * current. WEAK_PUSH pushes from any other shape */
#define STRONG_PUSH 7.0F
#define WEAK_PUSH 5.0F
/* a push of player 0 below LOW_HEIGHT lifts it LOW_LIFT more */
#define LOW_HEIGHT 12.0F
#define LOW_LIFT 2.0F

/* the yaw body_turn_per_step turns over each cycle of an entry's frames,
 * about 10 degrees. a walk turns WALK_TURN_SCALE times that */
#define CYCLE_TURN 1820.0F
#define WALK_TURN_SCALE 2
/* the frames of MOTION_APPEAR */
#define APPEAR_FRAMES 270.0F
/* how near the mouth the player sets off BIOLIZARD_SWEEP */
#define SWEEP_REACH 35.0F
/* the sweep's first frames, through which the Biolizard keeps turning */
#define SWEEP_TURN_FRAMES 15.0F
/* BIOLIZARD_PAUSE_ATTACK's wait, in pause_wait */
#define PAUSE_STEPS 120
/* the steps BIOLIZARD_OFFER_RAIL keeps its rail before it waits for the rail
 * to empty */
#define RAIL_STEPS 900
/* BIOLIZARD_OFFER_RAIL's rail follows the first of biolizard_rail_paths, or
 * the second under BODY_REVERSED. the lifted wave's rails follow the
 * WAVE_RAILS paths from WAVE_RAIL_PATH */
#define RAIL_PATH 0
#define REVERSED_RAIL_PATH 1
#define OFFERED_RAILS 1
#define WAVE_RAIL_PATH 2
#define WAVE_RAILS 4
/* a rider of BIOLIZARD_OFFER_RAIL's rail keeps the speed it boards at.
 * boarding adds RIDE_BOOST to its local x and RIDE_LIFT to its y */
#define RIDE_BOOST 2.0F
#define RIDE_LIFT 1.0F
/* BIOLIZARD_OFFER_RAIL turns about where it faces away from a player 0 below
 * BACK_HEIGHT. it turns too where that player lies within FRONT_ANGLE of its
 * front */
#define FRONT_ANGLE 160
/* the glide breathes in two spans of its motion's frames. it also breathes
 * on BREATH_CHANCE percent of its steps, at random */
#define FIRST_BREATH_START 24.0F
#define FIRST_BREATH_END 28.0F
#define SECOND_BREATH_START 47.0F
#define SECOND_BREATH_END 51.0F
#define BREATH_CHANCE 4.0F
/* the puffs of black breath each release of energy sends */
#define BLACK_PUFFS 15
/* the energy attack's volley holds VOLLEY_SHOTS shots, or no limit in the
 * energy test. the volley reads none of VOLLEY_SPEED, VOLLEY_TRIGGER_FRAME
 * and VOLLEY_END_FRAME */
#define VOLLEY_SHOTS 8
#define VOLLEY_SPEED 2.0F
#define VOLLEY_TRIGGER_FRAME 8.0F
#define VOLLEY_END_FRAME 17.0F
/* the energy attack plays its motions at INTERVAL_PACE times its volley's
 * interval */
#define INTERVAL_PACE 0.0125F
/* the Biolizard's wait after a wave, in its work's timer, before its next
 * attack */
#define REST_STEPS 180
/* BIOLIZARD_DAMAGE and BIOLIZARD_DEFEAT throw player 0 LAUNCH_SPEED along
 * its z and LAUNCH_RISE up. each throw locks its stick for LAUNCH_LOCK
 * steps. they throw it again each step while it lies within LAUNCH_RADIUS of
 * the y axis and above LAUNCH_HEIGHT */
#define LAUNCH_SPEED (-10.0F)
#define LAUNCH_RISE 1.0F
#define LAUNCH_LOCK 60
#define LAUNCH_RADIUS 300.0F
#define LAUNCH_HEIGHT 10.0F
/* BIOLIZARD_DEFEAT stands player 0 at STAND_HEIGHT where it falls below */
#define STAND_HEIGHT 2.0F
/* the preview's wait between motions, in the work's timer */
#define PREVIEW_STEPS 180
/* the preview prints a motion's label on the row below the test the menu
 * shows */
#define PREVIEW_LINE                                                           \
    NJM_LOCATION(BIOLIZARD_MENU_COLUMN, BIOLIZARD_MENU_TEST_ROW + 1)

/* the frames of the links to the stand after an attack, and to the sweep */
#define STAND_LINK_FRAMES 30
#define SWEEP_LINK_FRAMES 15
/* the frames of the glide's links to a turn, and back from it */
#define TURN_LINK_FRAMES 10
#define GLIDE_LINK_FRAMES 10

/* a second key for the Biolizard's sounds: the address one byte past its
 * task. a sound keyed by it plays apart from one keyed by the task */
#define SECOND_KEY(task) ((char *)(task) + 1)
/* the priority of a sound the Biolizard keys by its task */
#define SOUND_PRIORITY 1
/* BIOLIZARD_SOUND_UNK_19 plays at WALK_VOLUME every WALK_SOUND_INTERVAL steps
 * of a walk. it plays at GLIDE_VOLUME every GLIDE_SOUND_INTERVAL steps of the
 * glide. each play lasts UNK_19_STEPS */
#define WALK_SOUND_INTERVAL 140
#define WALK_VOLUME 50
#define GLIDE_SOUND_INTERVAL 215
#define GLIDE_VOLUME 5
#define UNK_19_STEPS 130
/* a push's sound lasts PUSH_SOUND_STEPS at PUSH_VOLUME. a sweep's plays at
 * SWEEP_VOLUME, and a release's at RELEASE_VOLUME */
#define PUSH_SOUND_STEPS 30
#define PUSH_VOLUME 30
#define SWEEP_VOLUME 100
#define RELEASE_VOLUME 60
/* the steps a sound lasts that a mode plays each step */
#define HELD_SOUND_STEPS 5
/* MOTION_APPEAR plays BIOLIZARD_SOUND_UNK_16 between APPEAR_SOUND_FRAME and
 * APPEAR_SOUND_END. at its end BIOLIZARD_SOUND_START plays at START_VOLUME.
 * each lasts APPEAR_SOUND_STEPS, as BIOLIZARD_SOUND_UNK_16 does at a wave's
 * start */
#define APPEAR_SOUND_FRAME 150.0F
#define APPEAR_SOUND_END 160.0F
#define APPEAR_SOUND_STEPS 150
#define START_VOLUME 120

/* a wave's start, BIOLIZARD_DAMAGE and BIOLIZARD_DEFEAT each shake the camera
 * by a _SHAKE for _SHAKE_STEPS. each plays BIOLIZARD_SOUND_SHAKE for
 * _SOUND_STEPS, and rumbles player 0's pad by vibration_start's _POWER,
 * _FREQ and _TIME */
#define WAVE_SHAKE 10.0F
#define WAVE_SHAKE_STEPS 90
#define WAVE_SOUND_STEPS 120
#define WAVE_POWER 2
#define WAVE_FREQ 20
#define WAVE_TIME 4
#define DAMAGE_SHAKE 15.0F
#define DAMAGE_SHAKE_STEPS 60
#define DAMAGE_SOUND_STEPS 100
#define DAMAGE_POWER 3
#define DAMAGE_FREQ 25
#define DAMAGE_TIME 5
#define DEFEAT_SHAKE 50.0F
#define DEFEAT_SHAKE_STEPS 120
#define DEFEAT_SOUND_STEPS 300
#define DEFEAT_POWER 3
#define DEFEAT_FREQ 25
#define DEFEAT_TIME 7
/* a push rumbles the pad too */
#define PUSH_POWER 1
#define PUSH_FREQ 7
#define PUSH_TIME 1
/* BIOLIZARD_DEFEAT plays BIOLIZARD_SOUND_DEFEAT every DEFEAT_SOUND_INTERVAL
 * steps, at DEFEAT_VOLUME */
#define DEFEAT_SOUND_INTERVAL 45
#define DEFEAT_VOLUME 100
/* MOTION_DEAD plays BIOLIZARD_SOUND_HURT in the frame from each _CRY_FRAME.
 * it plays BIOLIZARD_SOUND_IMPACT in the frame from each _IMPACT_FRAME. each
 * play lasts DEAD_SOUND_STEPS. each impact rumbles the pad by IMPACT_POWER,
 * IMPACT_FREQ and IMPACT_TIME */
#define IN_FRAME(frame, start) ((frame) > (start) && (frame) < (start) + 1.0F)
#define FIRST_CRY_FRAME 18.0F
#define SECOND_CRY_FRAME 38.0F
#define THIRD_CRY_FRAME 58.0F
#define FIRST_IMPACT_FRAME 118.0F
#define SECOND_IMPACT_FRAME 135.0F
#define DEAD_SOUND_STEPS 240
#define SECOND_CRY_PITCH 0x2000
#define THIRD_CRY_PITCH 0x3fff
#define FIRST_IMPACT_PITCH 0x2000
#define SECOND_IMPACT_PITCH (-0x3000)
#define SECOND_IMPACT_VOLUME 90
#define IMPACT_POWER 3
#define IMPACT_FREQ 10
#define IMPACT_TIME 1

/* the block boss_record_alloc_own gives the Biolizard, at its record's
 * own */
struct biolizard {
    Sint8 mode; /* a copy of the work's mode, which nothing reads */
    Sint8 previous_mode, walk, attack;
    /* the attack's place in its row of attack_order */
    Sint8 attack_index;
    Uint8 flags;
    Uint16 pause_wait;
    Uint16 push_wait; /* the contacts left before the next push */
    struct task *camera, *attack_task, *rails, *life_support, *barrier;
    NJS_CNK_OBJECT *pose;
    /* how far the pose blends each motion toward its partner, from 0 to 1 */
    Float radius_blend;
    Float speed; /* the motion's speed scale */
    NJS_VECTOR spd; /* the speed BIOLIZARD_OFFER_RAIL keeps for its rider */
};
/* the task's motion record, of which the Biolizard uses only the named
 * fields */
struct body_motion {
    Uint8 unused_00[40];
    Angle turn; /* the yaw a walk turns in a step */
    Uint8 unused_2c[28];
    Angle front_yaw; /* the bearing of the front from the arena's center */
    /* the yaw BIOLIZARD_DAMAGE and BIOLIZARD_DEFEAT throw player 0 along */
    Angle launch_yaw;
    Uint8 unused_50[36];
};

/* the entries of body_motions. MOTION_ENERGY_L_1 to _3 are MOTION_ENERGY_L
 * in three parts, and MOTION_ENERGY_R_1 to _3 are MOTION_ENERGY_R's */
enum body_entry {
    MOTION_APPEAR,
    MOTION_STAND_L,
    MOTION_STAND_R,
    MOTION_WALK_LL,
    MOTION_WALK_LR,
    MOTION_WALK_RR,
    MOTION_WALK_RL,
    MOTION_ENERGY_L,
    MOTION_ENERGY_R,
    MOTION_GLIDE_L,
    MOTION_GLIDE_R,
    MOTION_ATTACK_L,
    MOTION_ATTACK_R,
    MOTION_RAISE_SPHERES,
    MOTION_HOLD_SPHERES,
    MOTION_SEND_SPHERES,
    MOTION_DAMAGE,
    MOTION_L_TO_R,
    MOTION_R_TO_L,
    MOTION_DEAD,
    MOTION_ENERGY_L_1,
    MOTION_ENERGY_L_2,
    MOTION_ENERGY_L_3,
    MOTION_ENERGY_R_1,
    MOTION_ENERGY_R_2,
    MOTION_ENERGY_R_3,
    MOTIONS
};
/* the frames of a walk's cycle */
#define WALK_FRAMES 64.0F

KEPT static const struct motion_table_entry body_motions[MOTIONS] = {
#include "boss_last1/body/body_motions.inc"
};

/* the motions the Biolizard plays either way round, in side_motions: the _L
 * entry, then its _R twin */
enum twin {
    TWIN_STAND,
    TWIN_ENERGY,
    TWIN_GLIDE,
    TWIN_ATTACK,
    TWIN_TURN,
    TWIN_ENERGY_1,
    TWIN_ENERGY_2,
    TWIN_ENERGY_3,
    TWINS
};
static const Sint16 side_motions[TWINS][2] = {
#include "boss_last1/body/side_motions.inc"
};

/* by enum biolizard_camera_point_index. the points on the head, the tail and
 * the front lie 40 above their joints */
static const struct biolizard_camera_point
    camera_points[BIOLIZARD_CAMERA_POINTS] = {
#include "boss_last1/body/camera_points.inc"
};

/* the attacks the Biolizard takes in turn. row n - 1 holds the order at a
 * power of n. NO_ATTACK ends a row, and the order goes back to its start */
#define NO_ATTACK (-1)
/* clang-format off */
static const Sint8 attack_order[BODY_POWER][4] = {
    {BIOLIZARD_ATTACK_LIFTED_WAVE, BIOLIZARD_ATTACK_FAST_ENERGY, NO_ATTACK, NO_ATTACK},
    {BIOLIZARD_ATTACK_FAST_WALK, BIOLIZARD_ATTACK_FAST_ENERGY, BIOLIZARD_ATTACK_WAVE, NO_ATTACK},
    {BIOLIZARD_ATTACK_FAST_WALK, BIOLIZARD_ATTACK_FAST_ENERGY, BIOLIZARD_ATTACK_WAVE, NO_ATTACK},
    {BIOLIZARD_ATTACK_FAST_WALK, BIOLIZARD_ATTACK_ENERGY, BIOLIZARD_ATTACK_RAIL, NO_ATTACK},
    {BIOLIZARD_ATTACK_FAST_WALK, BIOLIZARD_ATTACK_SLOW_ENERGY, BIOLIZARD_ATTACK_RAIL, NO_ATTACK},
    {BIOLIZARD_ATTACK_WALK, BIOLIZARD_ATTACK_RAIL, NO_ATTACK, NO_ATTACK},
};
/* clang-format on */

/* clang-format off */
/* each attack and the mode it runs in. NO_ATTACK ends the table */
/* @hack type: a flat array that body_mode_for_attack steps through by
 * twos, where an array of pairs would do */
static const Sint8 attack_modes[] = {
    BIOLIZARD_ATTACK_WALK,         BIOLIZARD_BEGIN_WALK,
    BIOLIZARD_ATTACK_FAST_WALK,    BIOLIZARD_BEGIN_WALK,
    BIOLIZARD_ATTACK_SLOW_ENERGY,  BIOLIZARD_ENERGY_ATTACK,
    BIOLIZARD_ATTACK_ENERGY,       BIOLIZARD_ENERGY_ATTACK,
    BIOLIZARD_ATTACK_FAST_ENERGY,  BIOLIZARD_ENERGY_ATTACK,
    BIOLIZARD_ATTACK_RAIL,         BIOLIZARD_OFFER_RAIL,
    BIOLIZARD_ATTACK_WAVE,         BIOLIZARD_SPHERE_ATTACK,
    BIOLIZARD_ATTACK_LIFTED_WAVE,  BIOLIZARD_SPHERE_ATTACK,
    NO_ATTACK,                     -1,
};
/* clang-format on */

/* the title: THE BIOLIZARD above a bar. each letter shows 25 steps after the
 * one before */
#define TITLE_BAR 15
#define TITLE_LETTERS 13
static struct boss_title_letter title_bar[TITLE_BAR] = {
#include "boss_last1/body/title_bar.inc"
};
static struct boss_title_letter title_letters[TITLE_LETTERS] = {
#include "boss_last1/body/title_letters.inc"
};
static struct boss_title body_title = {
#include "boss_last1/body/body_title.inc"
};
static struct task *body_task = NULL;
/* the mark that one of the Biolizard's attacks has hurt the player */
static Bool player_hurt = FALSE;
Sint8 biolizard_test_mode = BIOLIZARD_TEST_OFF;
static struct collision_shape body_shapes[BODY_SHAPES] = {
#include "boss_last1/body/body_shapes.inc"
};
KEPT static struct last_boss_sphere_obstacle body_obstacles[BODY_SHAPES] = {
#include "boss_last1/body/body_obstacles.inc"
};
/* a walk's motion speed, by its attack */
static Float walk_speed[] = {1, 1.3F};
/* how far from the mouth the breath's puffs slow and rise */
static Float breath_reach = 21;
static Sint8 preview_motion = 0;
#define MOTION_LABELS 19
static const char *motion_names[MOTION_LABELS];
static void biolizard_destroy(struct task *task);
static Sint8 body_player_clockwise(void);
static void body_update_front(struct task *task);
static int body_mode_for_attack(Sint8 attack);
static void body_change_mode(struct task *task, int mode);
static void biolizard_step(struct task *task);
static void biolizard_draw(struct task *task);
#ifdef NONMATCHING
/* this side's way out of a create that fails. it frees the pose and the
 * record's parts, which a kill before the destructor is set leaves held. then
 * it destroys the task */
static struct task *body_start_failed(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct biolizard *body;
    if (record && record->own) {
        body = record->own;
        if (body->pose) {
            biolizard_pose_free();
        }
    }
    if (body_task == task) {
        body_task = NULL;
    }
    boss_record_free_parts(task);
    task_destroy(task);
    return NULL;
}
#endif

struct task *biolizard_create(const NJS_POINT3 *pos, Angle angle)
{
    struct task *task;
    struct task_work *work;
    struct boss_record *record;
    struct biolizard *body;
    struct body_motion *moving;
    NJS_POINT3 start;

#ifdef NONMATCHING
    /* this side makes no Biolizard where MOTION_APPEAR's speed isn't above 0.
     * the appearance times itself by that speed, and player 0's pad stays off
     * until it ends */
    if (body_motions[MOTION_APPEAR].speed <= 0.0F) {
        return NULL;
    }
#endif
    task = task_create(TASK_WORK, BODY_LEVEL, biolizard_step, "LastBoss1Exec");
    if (task == NULL) {
        return 0;
    }
    work = task->work;
    record = boss_record_create_parts(task, FALSE, TRUE);
    /* @bug memory: each failure below kills the task before it has a
     * destructor, and so never frees its record's parts. the kills are at
     * 0x8c500484, 0x8c50049a and 0x8c5004b4 */
    body = boss_record_alloc_own(record, sizeof(*body));
    if (body == NULL) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: body_start_failed frees the
         * record's parts */
        return body_start_failed(task);
#else
        task_kill(task);
        return NULL;
#endif
    }
    moving = syCalloc(sizeof(*moving), 1);
    if (moving == NULL) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged above the first failure:
         * body_start_failed frees the record's parts */
        return body_start_failed(task);
#else
        task_kill(task);
        return NULL;
#endif
    }
    task->motion.p = moving;
    body->pose = biolizard_pose_create(&biolizard_model);
    if (body->pose == NULL) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged above the first failure:
         * body_start_failed frees the record's parts and the pose */
        return body_start_failed(task);
#else
        task_kill(task);
        return NULL;
#endif
    }
    work->pos = *pos;
    work->ang.y = angle;
    work->scl.x = work->scl.y = work->scl.z = 1.0F;
    record->power = BODY_POWER;
    work->mode = BIOLIZARD_APPEAR;
    motion_init(record->block, body_motions);
    collision_init(task, body_shapes, BODY_SHAPES, COLLISION_ENEMY);
    /* @bug memory: reads through null at 0x8c500502 where collision_init
     * couldn't allocate the record */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it ends the create where
     * collision_init made no record */
    if (!task->work->collision) {
        return body_start_failed(task);
    }
#endif
    task->work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, RECORD_RADIUS);
    start.x = njSin(work->ang.y + QUARTER_TURN) * START_RADIUS;
    start.y = START_HEIGHT;
    start.z = njCos(work->ang.y + QUARTER_TURN) * START_RADIUS;
    player_set_pos(0, start.x, start.y, start.z);
    body_task = task;
    body->camera = biolizard_camera_create(task, &work->mode, &record->focus,
                                           camera_points);
    body->life_support = biolizard_life_support_create(
        task, &biolizard_joint_positions[BIOLIZARD_JOINT_LIFE_SUPPORT]);
#ifdef NONMATCHING
    /* ends the startup where the heap can't give the camera or the life
     * support. the camera's calls read it unchecked, and without the life
     * support nothing can hurt the Biolizard */
    if (!body->camera || !body->life_support) {
        return body_start_failed(task);
    }
#endif
    biolizard_track_splash(
        task, &biolizard_joint_positions[BIOLIZARD_JOINT_LEG_A], LEG_REACH);
    biolizard_track_splash(
        task, &biolizard_joint_positions[BIOLIZARD_JOINT_LEG_B], LEG_REACH);
    biolizard_track_swing(task,
                          &biolizard_joint_positions[BIOLIZARD_JOINT_TAIL],
                          &biolizard_joint_positions[BIOLIZARD_JOINT_TAIL_TIP]);
    work->flags &= ~BODY_REVERSED;
    biolizard_test_mode = BIOLIZARD_TEST_OFF;
    player_hurt = FALSE;
    task->draw = biolizard_draw;
    task->destructor = biolizard_destroy;
    return task;
}

static void biolizard_destroy(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;

    if (body->rails) {
        task_kill(body->rails);
        body->rails = NULL;
    }
    if (body->camera) {
        task_kill(body->camera);
        body->camera = NULL;
    }
    biolizard_pose_free();
    boss_record_free_parts(task);
    syFree(task->motion.p);
    task->motion.p = NULL;
    body_task = NULL;
}

static Float body_player_radius_blend(void)
{
    Float dist = last_boss_radius(&player_work[0]->pos, 0);

    if (dist < BLEND_NEAR) {
        return 0.0F;
    } else if (dist > BLEND_FAR) {
        return 1.0F;
    } else {
        return (dist - BLEND_NEAR) * (1.0F / (BLEND_FAR - BLEND_NEAR));
    }
}

static void body_ease_radius(Float *value)
{
    Float target = body_player_radius_blend();

    if (_builtin_fabsf(*value - target) > BLEND_STEP) {
        if (*value - target < 0.0F) {
            *value += BLEND_STEP;
        } else {
            *value -= BLEND_STEP;
        }
    } else {
        *value = target;
    }
}

static void body_sense(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct biolizard *body = record->own;
    struct body_motion *moving = task->motion.p;

    body_update_front(task);
    moving->front_yaw =
        last_boss_bearing(&biolizard_joint_positions[BIOLIZARD_JOINT_FRONT]);
    if (biolizard_life_support_hit(body->life_support) == 1 &&
        work->mode != BIOLIZARD_DAMAGE && work->mode != BIOLIZARD_DEFEAT) {
        if (record->power > 1.0F) {
            work->flags |= WORK_HURT;
            body_change_mode(task, BIOLIZARD_DAMAGE);
        } else {
            body_change_mode(task, BIOLIZARD_DEFEAT);
        }
    }
    if (body_player_clockwise() == 1) {
        body->flags |= BODY_PLAYER_CLOCKWISE;
    } else {
        body->flags &= ~BODY_PLAYER_CLOCKWISE;
    }
    if (body->flags & BODY_CHECK_SIDE) {
        int clockwise = body->flags & BODY_PLAYER_CLOCKWISE;

        if (clockwise) {
            work->flags |= BODY_REVERSED;
            body->flags |= BODY_SIDE_REVERSED;
        } else {
            work->flags &= ~BODY_REVERSED;
            body->flags &= ~BODY_SIDE_REVERSED;
        }
        body->flags &= ~BODY_CHECK_SIDE;
    }
    if (!(work->flags & BODY_FIXED_BLEND)) {
        body_ease_radius(&body->radius_blend);
    }
}

struct task *biolizard_task(void)
{
    /* @hack redundant: body_task tested for null before it is returned,
     * where returning it would do */
    if (body_task == NULL) {
        return 0;
    }
    return body_task;
}

struct task *biolizard_life_support_task(void)
{
    struct task *task = biolizard_task();
    struct boss_record *record;
    struct biolizard *body;

    if (task == NULL) {
        return 0;
    }
    record = task->extra.p;
    if (record == NULL) {
        return 0;
    }
    body = record->own;
    if (body == NULL) {
        return 0;
    }
    return body->life_support;
}

Sint8 biolizard_attack(void)
{
    struct task *task = biolizard_task();

    struct boss_record *record;
    struct biolizard *body;

    if (task == NULL) {
        return -1;
    }
    record = task->extra.p;
    body = record->own;
    return body->attack;
}

static Sint8 body_player_clockwise(void)
{
    int difference = last_boss_angle_difference(
        &biolizard_joint_positions[BIOLIZARD_JOINT_FRONT],
        &player_work[0]->pos);

    if (difference < 0) {
        return 1;
    }
    return 0;
}

/* BIOLIZARD_JOINT_FRONT is no joint of the pose */
static void body_update_front(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 point = {-FRONT_REACH, 0.0F, 0.0F};

    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njRotateY(NULL, QUARTER_TURN);
    njScaleEx(&work->scl);
    njCalcPoint(NULL, &point,
                &biolizard_joint_positions[BIOLIZARD_JOINT_FRONT]);
    njPopMatrix(1);
}

Angle biolizard_body_angle(void)
{
    struct task *task = biolizard_task();
    struct body_motion *moving;
    Angle ang;

    if (task == NULL) {
        return 0;
    }
    moving = task->motion.p;
    ang = moving->front_yaw;
    return (Uint16)ang;
}

Angle biolizard_tail_angle(void)
{
    struct task *task = biolizard_task();
    struct body_motion *moving;
    Angle ang;

    if (task == NULL) {
        return 0;
    }
    moving = task->motion.p;
    ang = moving->front_yaw;
    return (Uint16)(ang + HALF_TURN);
}

Sint8 biolizard_reversed(void)
{
    struct task *task = biolizard_task();

    struct task_work *work;

    if (task == NULL) {
        return -1;
    }
    work = task->work;
    if (work->flags & BODY_REVERSED) {
        return 1;
    }
    return 0;
}

Bool biolizard_player_hurt(void)
{
    return player_hurt;
}

void biolizard_mark_player_hurt(void)
{
    player_hurt = TRUE;
}

void biolizard_clear_player_hurt(void)
{
    player_hurt = FALSE;
}

static Bool body_facing_player(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    Uint16 reversed = work->flags & BODY_REVERSED;
    Uint16 clockwise = body->flags & BODY_PLAYER_CLOCKWISE;

    if (!reversed && !clockwise) {
        return 1;
    }
    if (reversed && clockwise) {
        return 1;
    }
    return 0;
}

void biolizard_clear_attacks(void)
{
    struct task *task = biolizard_task();
    struct boss_record *record;
    struct biolizard *body;

    /* @hack spelling: an empty arm for a null task, where a test for a live
     * one would do */
    if (task == NULL) {
    } else {
        record = task->extra.p;
        body = record->own;
        if (body->attack_task) {
            task_kill(body->attack_task);
            body->attack_task = NULL;
        }
        if (body->rails) {
            task_kill(body->rails);
            body->rails = NULL;
        }
    }
}

int biolizard_select_attack(Sint8 attack)
{
    struct task *task = biolizard_task();
    struct boss_record *record;
    struct biolizard *body;

    if (task == NULL) {
        return -1;
    }
    record = task->extra.p;
    body = record->own;
    body->attack = attack;
    body->attack_index = -1;
    return body_mode_for_attack(attack);
}

void biolizard_push_player(const NJS_VECTOR *dir, Float speed)
{
    NJS_VECTOR spd;
    Float y;

    if (dir == 0) {
        /* @hack redundant: ang, here and in the other arm, which nothing
         * reads */
        struct angles ang = {0, 0, 0};

        spd = player_motion[0]->spd;
        last_boss_vector_length(&spd, speed, &spd);
        spd.y = 1.0F;
        ang.y = HALF_TURN;
        player_set_speed(0, spd.x, spd.y, spd.z);
    } else {
        struct angles ang = {0, 0, 0};

        spd = *dir;
        y = spd.y;
        spd.y = 0.0F;
        last_boss_vector_length(&spd, speed, &spd);
        spd.y = y;
        player_set_speed(0, spd.x, spd.y, spd.z);
    }
}

static struct collision_shape *body_hit_shape(struct task *task,
                                              struct collision_hit *hit)
{
    struct collision_shape *info =
        &task->work->collision->shapes[hit->own_shape];

    /* @hack redundant: the shape's address tested for null, which it never
     * is, where returning it would do */
    if (info == 0) {
        return 0;
    }
    return info;
}

static int body_push_direction(struct collision_hit *hit, NJS_POINT3 *pos,
                               NJS_VECTOR *dir, NJS_VECTOR *out)
{
    Float half;
    Angle ang;

    if (!out) {
        /* @bug uninitialized: returns no value for a null out, at
         * 0x8c500c30 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it returns 0 */
        return 0;
#else
        return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
    }
    if (pos->y > BACK_HEIGHT) {
        vector_diff(pos, &biolizard_arena_center, out);
        out->y = 1.0F;
        return 1;
    }
    if (player_records[0]->anim.current == ANIM_IN_CURRENT) {
        out->x = dir->x;
        out->y = 0.0F;
        out->z = dir->z;
        njUnitVector(out);
        out->y = 2.0F;
        return 0;
    }
    half = 0.5F;
    if (hit->own_shape >= TAIL_FIRST && hit->own_shape <= TAIL_LAST) {
        if (body_player_clockwise() == 1) {
            ang = biolizard_body_angle() - QUARTER_TURN;
        } else {
            ang = biolizard_body_angle() + QUARTER_TURN;
        }

        ang = (Uint16)ang;
        out->x = njSin(ang);
        out->y = half;
        out->z = njCos(ang);
        return 1;
    }
    if (hit->own_shape == MOUTH_SHAPE && pos->y < BACK_HEIGHT) {
        if (body_player_clockwise() == 1) {
            ang = biolizard_body_angle() - QUARTER_TURN;
        } else {
            ang = biolizard_body_angle() + QUARTER_TURN;
        }

        ang = (Uint16)ang;
        out->x = njSin(ang);
        out->y = half;
        out->z = njCos(ang);
        return 1;
    }
    *out = *dir;
    njUnitVector(out);
    return 0;
}

static void body_extend_point(NJS_POINT3 *from, NJS_POINT3 *to, Float add,
                              NJS_POINT3 *out)
{
    NJS_VECTOR dir;
    Float dist = vector_distance(from, to);
    Float len;

    vector_diff(to, from, &dir);
    njUnitVector(&dir);
    len = add + dist;
    dir.x *= len;
    dir.y *= len;
    dir.z *= len;
    vector_sum(from, &dir, out);
}

static int body_contact(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    struct collision_hit *hit;
    struct collision_shape *info;
    NJS_VECTOR dir;
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    Float speed;
    Float power;

    hit = collision_next_player_hit(task);
    if (hit == 0) {
        return 0;
    }

    if (player_work[0]->mode != PLAYER_MODE_MISS &&
        player_work[0]->mode != PLAYER_MODE_HURT) {
        if (body->push_wait > 0) {
            body->push_wait--;
            return 0;
        }
        info = body_hit_shape(task, hit);
        speed = PUSH_SPEED;
        if (info) {
            body_extend_point(&info->center, &player_work[0]->pos, speed, &pos);
            vector_diff(&pos, &info->center, &dir);
            body_push_direction(hit, &pos, &dir, &spd);
            last_boss_defend(&pos, &dir);
            sound_play_for(BIOLIZARD_SOUND_PUSH, task, SOUND_PRIORITY, 0,
                           PUSH_SOUND_STEPS);
            sound_set_volume(task, BIOLIZARD_SOUND_PUSH, PUSH_VOLUME);
        }
        if (player_records[0]->anim.current == ANIM_IN_CURRENT) {
            power = STRONG_PUSH;
            player_work[0]->flags &= ~PLAYER_WORK_ON_PATH;
        } else if (hit->own_shape >= TAIL_FIRST &&
                   hit->own_shape <= TAIL_LAST) {
            power = speed;
        } else if (hit->own_shape == MOUTH_SHAPE ||
                   hit->own_shape == HEAD_SHAPE) {
            power = STRONG_PUSH;
        } else if (hit->own_shape >= UPPER_NECK_FIRST &&
                   hit->own_shape <= UPPER_NECK_LAST) {
            power = STRONG_PUSH;
        } else {
            power = WEAK_PUSH;
        }
        if (player_work[0]->pos.y < LOW_HEIGHT) {
            spd.y += LOW_LIFT;
        }
        biolizard_push_player(&spd, power);
        vibration_start(0, PUSH_POWER, PUSH_FREQ, PUSH_TIME);
        if (last_boss_hit_penetrates(task, hit) == 1) {
            player_hurt = TRUE;
        }
        body->push_wait = PUSH_WAIT;
        return 1;
    }
    /* @bug uninitialized: returns no value while player 0 is in
     * PLAYER_MODE_MISS or PLAYER_MODE_HURT, at 0x8c500df8 and 0x8c500e02 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns 0 */
    return 0;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void body_update_shape(struct task *task, Sint8 index)
{
    struct last_boss_sphere_obstacle *point = &body_obstacles[index];

    if (point->joint < 0) {
        return;
    }
    vector_sum(&biolizard_joint_positions[point->joint], &point->offset,
               &point->position);
    collision_set_shape_center(task, index, &point->position);
}

static void body_register_shapes(struct task *task)
{
    struct task_work *work = task->work;
    Sint8 index;

    for (index = 0; index < BODY_SHAPES; index++) {
        body_update_shape(task, index);
    }
    if (work->mode != BIOLIZARD_APPEAR) {
        collision_register(task);
    }
}

static int body_twin_motion(Uint32 twin)
{
    if (twin >= TWINS) {
        return 0;
    }
    if (biolizard_reversed()) {
        return side_motions[twin][1];
    }
    return side_motions[twin][0];
}

static int body_mode_for_attack(Sint8 attack)
{
    const Sint8 *key;
    const Sint8 *mode;

    for (key = &attack_modes[0], mode = &attack_modes[1]; *key >= 0;
         key += 2, mode += 2) {
        if (attack == *key) {
            return *mode;
        }
    }
    return -1;
}

static int body_next_attack(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    Sint8 step = body->attack_index + 1;
    Sint8 row = (int)record->power - 1;

    if (attack_order[row][step] < 0) {
        step = 0;
    }
    body->attack_index = step;
    body->attack = attack_order[row][step];
    return body_mode_for_attack(body->attack);
}

static int body_first_attack(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    Sint8 row = (int)record->power - 1;

    body->attack_index = 0;
    body->attack = attack_order[row][body->attack_index];
    return body_mode_for_attack(body->attack);
}

/* the yaw a step turns, so that entry index played at scale turns
 * CYCLE_TURN a cycle */
static int body_turn_per_step(struct task *task, Sint8 index, Float scale)
{
    const struct motion_table_entry *entry;
    Float count;
    Float speed;

    if (index >= MOTIONS || index < 0) {
        return 0;
    }
    entry = &body_motions[index];
    count = _builtin_fabsf(entry->start - entry->end) + 1.0F;
    speed = entry->speed;
    return (int)(scale * speed / count * CYCLE_TURN);
}

/* set *new_walk to the walk toward player 0, and return how to change to
 * it. with force, the change is always a link */
static int body_locate_player(struct task *task, Sint8 *new_walk, Bool force)
{
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    Bool head_first = FALSE;
    Bool ahead = FALSE;
    int change;
    Sint8 walk;
    int relative;

    relative = last_boss_angle_difference(
        &biolizard_joint_positions[BIOLIZARD_JOINT_FRONT],
        &player_work[0]->pos);
    if (relative < 0 && relative >= -QUARTER_TURN) {
        walk = WALK_RR;
    } else if (relative >= 0 && relative <= QUARTER_TURN) {
        walk = WALK_LL;
    } else {
        relative = last_boss_angle_difference(
            &biolizard_joint_positions[BIOLIZARD_JOINT_TAIL],
            &player_work[0]->pos);
        if (relative < 0) {
            walk = WALK_LR;
        } else {
            walk = WALK_RL;
        }
    }
    if (body->walk == WALK_RR || body->walk == WALK_LL) {
        head_first = TRUE;
    }
    if (walk == WALK_RR || walk == WALK_LL) {
        ahead = TRUE;
    }
    if (!force) {
        if (body->walk != walk) {
            if (head_first == TRUE && ahead == TRUE) {
                if (last_boss_angle_distance(
                        &biolizard_joint_positions[BIOLIZARD_JOINT_FRONT],
                        &player_work[0]->pos) > LINK_ANGLE) {
                    change = WALK_LINK;
                } else {
                    change = WALK_KEEP;
                }
            } else if (head_first == FALSE && ahead == FALSE) {
                if (last_boss_angle_distance(
                        &biolizard_joint_positions[BIOLIZARD_JOINT_TAIL],
                        &player_work[0]->pos) > LINK_ANGLE) {
                    change = WALK_LINK;
                } else {
                    change = WALK_KEEP;
                }
#ifdef NONMATCHING
                /* the plain form the hack tag names, under which each path sets
                 * change */
            } else {
#else
                /* @hack redundant: the enclosing test of body->walk against
                 * walk repeated, where else alone would do */
                /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
            } else if (body->walk != walk) {
#endif
                if (head_first == TRUE) {
                    if (last_boss_angle_distance(
                            &biolizard_joint_positions[BIOLIZARD_JOINT_HEAD],
                            &player_work[0]->pos) < REVERSE_ANGLE) {
                        change = WALK_KEEP;
                        walk = body->walk;
                    } else {
                        change = WALK_REVERSE;
                    }
                } else {
                    change = WALK_REVERSE;
                }
            }
        } else if (head_first == FALSE) {
            if (last_boss_angle_distance(
                    &biolizard_joint_positions[BIOLIZARD_JOINT_TAIL],
                    &player_work[0]->pos) > REVERSE_ANGLE) {
                change = WALK_REVERSE;
                if (body->walk == WALK_RL) {
                    walk = WALK_RR;
                } else {
                    walk = WALK_LL;
                }
            } else {
                change = WALK_KEEP;
            }
        } else {
            change = WALK_KEEP;
        }
    } else {
        change = WALK_LINK;
    }
    *new_walk = walk;

    return change;
}

static void body_turn_to_player(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct motion_control *motion = record->block;
    struct biolizard *body = record->own;
    struct body_motion *moving = task->motion.p;
    Sint8 walk;
    Sint8 turn;
    Float frame;
    int camera_side;

    if (motion->flags & MOTION_LINKING) {
        body->flags |= BODY_TURNING;
        return;
    }
    body->flags &= ~BODY_TURNING;
    body->flags |= BODY_CHECK_SIDE;
    turn = body_locate_player(task, &walk, FALSE);
    switch (turn) {
    case WALK_LINK:
        motion_link(motion, MOTION_WALK_LL + walk);
        body->walk = walk;
        break;
    case WALK_REVERSE:
        frame = -motion_frame(motion) + WALK_FRAMES;
        if (frame == WALK_FRAMES) {
            frame = 0.0F;
        }
        motion_set(motion, MOTION_WALK_LL + walk);
        motion_set_frame(motion, frame);
        body->walk = walk;
        break;
    case WALK_KEEP:
        if (body->walk == WALK_LL || body->walk == WALK_RL) {
            work->ang.y += moving->turn;
        } else {
            work->ang.y -= moving->turn;
        }
        break;
    }
    if (turn == WALK_LINK || turn == WALK_REVERSE) {
        if (body->walk == WALK_LL || body->walk == WALK_RR) {
            camera_side = BIOLIZARD_CAMERA_HEAD;
        } else {
            camera_side = BIOLIZARD_CAMERA_TAIL;

            /* @hack redundant: a goto to the call that follows, where none
             * would do */
            goto call;
        }
    call:
        biolizard_select_camera_side(body->camera, camera_side);
    }
}

static int body_start_walk(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    struct body_motion *moving = task->motion.p;

    if (body->attack == BIOLIZARD_ATTACK_FAST_WALK) {
        body->speed = walk_speed[BIOLIZARD_ATTACK_FAST_WALK];
    } else {
        body->speed = walk_speed[BIOLIZARD_ATTACK_WALK];
    }
    moving->turn =
        body_turn_per_step(task, MOTION_WALK_LL, body->speed) * WALK_TURN_SCALE;
    return BIOLIZARD_WALK;
}

static void body_change_mode(struct task *task, int mode)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;

    struct biolizard *body = record->own;
    struct motion_control *motion = record->block;
    struct collision_shape *info;
    int next_mode;

    work->flags &= ~BODY_FIXED_BLEND;
    body->flags |= BODY_CHECK_SIDE;

    /* @hack type: mode narrowed to Sint8 in each test, where mode alone would
     * do */
    if ((Sint8)mode == BIOLIZARD_PAUSE_ATTACK) {
        body->previous_mode = work->mode;
        work->mode = mode;
        body->pause_wait = PAUSE_STEPS;
        switch (body->previous_mode) {
        case BIOLIZARD_ENERGY_ATTACK:
            motion_link_frames(motion, body_twin_motion(TWIN_STAND),
                               STAND_LINK_FRAMES);
            return;
        case BIOLIZARD_WALK:
        case BIOLIZARD_SWEEP:
            body->previous_mode = BIOLIZARD_RESUME_WALK;
            break;
        case BIOLIZARD_SPHERE_ATTACK:
            break;
        default:
            break;
        }
        return;
    }
    if ((Sint8)mode == BIOLIZARD_DAMAGE) {
        if (work->mode == BIOLIZARD_SPHERE_ATTACK && body->attack_task) {
            biolizard_sphere_clear(body->attack_task);
            body->attack_task = NULL;
        } else {
            /* @hack redundant: an empty else, where none would do */
        }
    } else {
        player_hurt = FALSE;
    }
    if (work->mode == BIOLIZARD_SPHERE_ATTACK) {
        if (body->barrier) {
            task_kill(body->barrier);
#ifdef NONMATCHING
            /* this side forgets the barrier it kills, so that no later test of
             * body->barrier finds the dying task */
            body->barrier = NULL;
#endif
        }
        info = &work->collision->shapes[TOP_SHAPE];
        info->flags &= ~SHAPE_OFF;
        info++;
        info->flags &= ~SHAPE_OFF;
    }
    if ((Sint8)mode == BIOLIZARD_BEGIN_WALK) {
        next_mode = body_start_walk(task);
        work->timer = BIOLIZARD_WALK_STEPS;
    } else if ((Sint8)mode == BIOLIZARD_RESUME_WALK) {
        next_mode = body_start_walk(task);
    } else {
        next_mode = mode;
    }
    body->previous_mode = work->mode;
    work->mode = next_mode;
    work->submode = 0;
}

static void body_appear(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    struct motion_play *state;

    switch (work->submode) {
    case 0:
        pad_disable_player(0);
        motion_set(motion, MOTION_APPEAR);
        work->timer = 1.0F / body_motions[MOTION_APPEAR].speed * APPEAR_FRAMES;
        boss_gauge_create(task, &record->power);
        boss_title_create(task, &body_title);
        work->submode++;
    case 1:
        /* @hack spelling: the assignment of state inside the test, where a
         * statement of its own would do */
        if ((state = &motion->current)->frame > APPEAR_SOUND_FRAME &&
            state->frame < APPEAR_SOUND_END) {
            sound_play_for(BIOLIZARD_SOUND_UNK_16, task, SOUND_PRIORITY, 0,
                           APPEAR_SOUND_STEPS);
            /* @bug logic: sets the volume of BIOLIZARD_SOUND_START at
             * 0x8c501662, not of BIOLIZARD_SOUND_UNK_16, which plays at
             * 0x8c501656. every other play in body.c sets its own sound's
             * volume */
            sound_set_volume(task, BIOLIZARD_SOUND_START, SOUND_LOUDEST);
        }
        if (work->timer-- == 0) {
            pad_enable_player(0);
            body_change_mode(task, body_first_attack(task));
            boss_messenger_summon_capped(0, MESSAGES, task);
            boss_title_fade(TITLE_FADE);
            /* @hack spelling: the two calls joined by a comma, where two
             * statements would do */
            sound_play_for(BIOLIZARD_SOUND_START, task, SOUND_PRIORITY, 0,
                           APPEAR_SOUND_STEPS),
                sound_set_volume(task, BIOLIZARD_SOUND_START, START_VOLUME);
        }
        break;
    }
}

static void body_walk(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    struct motion_control *motion = record->block;
    int camera_side;

    switch (work->submode) {
    case 0:
        body->radius_blend = body_player_radius_blend();
        body_locate_player(task, &body->walk, TRUE);
        if (body->walk == WALK_LL || body->walk == WALK_RR) {
            camera_side = BIOLIZARD_CAMERA_HEAD;
        } else {
            camera_side = BIOLIZARD_CAMERA_TAIL;

            /* @hack redundant: a goto to the call that follows, where none
             * would do */
            goto call;
        }
    call:
        biolizard_select_camera_side(body->camera, camera_side);
        motion_link(motion, MOTION_WALK_LL + body->walk);
        work->submode++;
    case 1:
        motion_set_speed(motion, body->speed);
        body_turn_to_player(task);
        break;
    }
    if (game_state.steps % WALK_SOUND_INTERVAL == 0) {
        sound_play_at_for(
            BIOLIZARD_SOUND_UNK_19, task, SOUND_PRIORITY, WALK_VOLUME,
            UNK_19_STEPS,
            &biolizard_joint_positions[BIOLIZARD_JOINT_HEAD_MIDDLE]);
    }
    if (work->timer) {
        work->timer--;
    }
    if (biolizard_player_hurt() == 1) {
        body_change_mode(task, BIOLIZARD_PAUSE_ATTACK);
        return;
    }
    if (!(motion->flags & MOTION_LINKING)) {
        if (vector_distance(&biolizard_joint_positions[BIOLIZARD_JOINT_MOUTH],
                            &player_work[0]->pos) < SWEEP_REACH) {
            body_change_mode(task, BIOLIZARD_SWEEP);
        }
        if (work->timer == 0) {
            body_change_mode(task, body_next_attack(task));
        }
    }
}

static void body_sweep(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    struct body_motion *moving = task->motion.p;
    struct motion_control *motion = record->block;

    switch (work->submode) {
    case 0:
        body->radius_blend = body_player_radius_blend();
        motion_link_frames(motion, body_twin_motion(TWIN_ATTACK),
                           SWEEP_LINK_FRAMES);
        sound_play(BIOLIZARD_SOUND_SWEEP, 0, 0, 0);
        sound_set_volume(0, BIOLIZARD_SOUND_SWEEP, SWEEP_VOLUME);
        work->submode++;
    case 1:
        if ((motion->flags & MOTION_LINKING) ||
            motion->index == side_motions[TWIN_ATTACK][1] ||
            (motion->index == side_motions[TWIN_ATTACK][0] &&
             motion->current.frame < SWEEP_TURN_FRAMES)) {
            if (body->walk == WALK_LL || body->walk == WALK_RL) {
                work->ang.y += moving->turn;
            } else {
                work->ang.y -= moving->turn;
            }
        }
        if (motion_held(motion)) {
            body_change_mode(task, BIOLIZARD_RESUME_WALK);
        }
        break;
    }
    if (biolizard_player_hurt() == 1) {
        body_change_mode(task, BIOLIZARD_PAUSE_ATTACK);
        return;
    }
}

static void body_offer_rail(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    struct body_motion *moving = task->motion.p;
    struct motion_control *motion = record->block;
    Float frame;
    int path;
    Bool leave;
    Sint8 done;

    switch (work->submode) {
    case 0:
        motion_link(motion, body_twin_motion(TWIN_GLIDE));
        body->flags |= BODY_TURNING;
        body->spd.x = body->spd.y = body->spd.z = 0.0F;
        work->submode++;
    case 1:
        if (motion->flags & MOTION_LINKING) {
            break;
        }
        body->spd.x = body->spd.y = body->spd.z = 0.0F;
        work->timer = RAIL_STEPS;
        if (work->flags & BODY_REVERSED) {
            path = REVERSED_RAIL_PATH;
        } else {
            path = RAIL_PATH;
        }
        /* @hack type: path narrowed to Sint8 as the index, where path alone
         * would do */
        body->rails =
            biolizard_rails_create(&biolizard_rail_paths[(Sint8)path],
                                   OFFERED_RAILS, work->ang.y + QUARTER_TURN);
        body->flags &= ~BODY_TURNING;
        work->submode++;
        break;
    case 2:
        if (player_work[0]->mode == PLAYER_MODE_SONIC_RAIL) {
            NJS_VECTOR spd;

            if (body->spd.x == 0.0F) {
                spd.x = _builtin_fabsf(player_motion[0]->spd.x) + RIDE_BOOST;
                spd.y = player_motion[0]->spd.y + RIDE_LIFT;
                /* @hack redundant: 0.0F added to spd.z's magnitude */
                spd.z = _builtin_fabsf(player_motion[0]->spd.z) + 0.0F;
                body->spd = spd;
            } else {
                spd = body->spd;
            }
            player_set_local_speed(0, spd.x, spd.y, spd.z);
            sound_play_for(BIOLIZARD_SOUND_GLIDE, task, SOUND_PRIORITY, 0,
                           HELD_SOUND_STEPS);
        } else {
            body->spd.x = body->spd.y = body->spd.z = 0.0F;
            sound_play_at_for(
                BIOLIZARD_SOUND_GLIDE, task, SOUND_PRIORITY, 0,
                HELD_SOUND_STEPS,
                &biolizard_joint_positions[BIOLIZARD_JOINT_HEAD_MIDDLE]);
        }
        frame = motion->current.frame;
        if ((frame > FIRST_BREATH_START && frame < FIRST_BREATH_END) ||
            (frame > SECOND_BREATH_START && frame < SECOND_BREATH_END) ||
            rand() * (100 * RAND_SCALE) < BREATH_CHANCE) {
            NJS_VECTOR dir;

            vector_diff(&biolizard_joint_positions[BIOLIZARD_JOINT_MOUTH],
                        &biolizard_joint_positions[BIOLIZARD_JOINT_HEAD_MIDDLE],
                        &dir);
            njUnitVector(&dir);
            dir.y = 0.0F;
            biolizard_white_breath(
                &biolizard_joint_positions[BIOLIZARD_JOINT_MOUTH], &dir,
                breath_reach);
        }
        if (game_state.steps % GLIDE_SOUND_INTERVAL == 0) {
            sound_play_at_for(
                BIOLIZARD_SOUND_UNK_19, task, SOUND_PRIORITY, GLIDE_VOLUME,
                UNK_19_STEPS,
                &biolizard_joint_positions[BIOLIZARD_JOINT_HEAD_MIDDLE]);
        }
        leave = FALSE;
        if (player_work[0]->pos.y < BACK_HEIGHT) {
            if (!body_facing_player(task)) {
                leave = TRUE;
            } else if (last_boss_angle_distance(
                           &biolizard_joint_positions[BIOLIZARD_JOINT_FRONT],
                           &player_work[0]->pos) < FRONT_ANGLE) {
                leave = TRUE;
            }
        }
        if (leave == TRUE) {
            if (body->rails) {
                task_kill(body->rails);
                body->rails = NULL;
            }
            motion_link_frames(motion, body_twin_motion(TWIN_TURN),
                               TURN_LINK_FRAMES);
            body->flags |= BODY_TURNING;
            work->submode = 3;
        }
        if (work->timer > 0) {
            work->timer--;
        }
        if (work->timer == 0 && biolizard_test_mode != BIOLIZARD_TEST_GLIDE) {
            done = 0;
            if (body->rails) {
                if (!players_on_path(biolizard_active_rail())) {
                    done = 1;
                }
            } else {
                done = 1;
            }
            if (done == 1) {
                if (body->rails) {
                    task_kill(body->rails);
                    body->rails = NULL;
                }
                body_change_mode(task, body_next_attack(task));
            }
        }
        break;
    case 3:
        body->flags |= BODY_TURNING;
        if (motion_ended(motion)) {
            if (motion->index == MOTION_R_TO_L) {
                motion_link_frames(motion, MOTION_GLIDE_L, GLIDE_LINK_FRAMES);
                work->flags &= ~BODY_REVERSED;
                body->flags &= ~BODY_SIDE_REVERSED;
            } else {
                motion_link_frames(motion, MOTION_GLIDE_R, GLIDE_LINK_FRAMES);
                work->flags |= BODY_REVERSED;
                body->flags |= BODY_SIDE_REVERSED;
            }
            body->flags |= BODY_TURNING;
            work->submode = 1;
        }
        break;
    }
}

/* the volley's kind for an energy attack */
static int body_energy_kind(Sint8 attack)
{
    int kind;

    switch (attack) {
    case BIOLIZARD_ATTACK_SLOW_ENERGY:
        kind = BIOLIZARD_ENERGY_SLOW;
        break;
    case BIOLIZARD_ATTACK_ENERGY:
        kind = BIOLIZARD_ENERGY_NORMAL;
        break;
    case BIOLIZARD_ATTACK_FAST_ENERGY:
        kind = BIOLIZARD_ENERGY_FAST;
        break;
        /* @bug uninitialized: returns kind unset for any other attack, at
         * 0x8c501d5a */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: any other attack takes
     * BIOLIZARD_ENERGY_SLOW */
    default:
        return BIOLIZARD_ENERGY_SLOW;
#endif
    }
    return kind;
}

static void body_energy_attack(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    struct motion_control *motion = record->block;
    Sint8 puff;

    /* @hack redundant: an if (0) around a test of the task's work, motion,
     * force and extra, which nothing needs */
    if (0) {
        if (task->work && task->motion.p && task->force.p && task->extra.p) {
        }
    }
    switch (work->submode) {
    case 0:
        body->radius_blend = body_player_radius_blend();
        motion_link(motion, body_twin_motion(TWIN_ENERGY_1));
        if (!body->attack_task) {
            struct biolizard_energy_setup volley;
            Uint16 interval;
            Sint8 kind = body_energy_kind(body->attack);

            if (biolizard_test_mode == BIOLIZARD_TEST_ENERGY) {
                volley.count = BIOLIZARD_ENERGY_NO_LIMIT;
                volley.timing = BIOLIZARD_ENERGY_BY_CALL;
                volley.from =
                    &biolizard_joint_positions[BIOLIZARD_JOINT_HEAD_MIDDLE];
                volley.goal = &biolizard_joint_positions[BIOLIZARD_JOINT_TAIL];
                volley.speed = VOLLEY_SPEED;
                volley.animation = NULL;
                volley.trigger_frame = 0.0F;
                volley.end_frame = 0.0F;
            } else {
                volley.count = VOLLEY_SHOTS;
                volley.timing = BIOLIZARD_ENERGY_BY_CALL;
                volley.from =
                    &biolizard_joint_positions[BIOLIZARD_JOINT_HEAD_MIDDLE];
                volley.goal = &biolizard_joint_positions[BIOLIZARD_JOINT_TAIL];
                volley.speed = VOLLEY_SPEED;
                volley.animation = NULL;
                volley.trigger_frame = VOLLEY_TRIGGER_FRAME;
                volley.end_frame = VOLLEY_END_FRAME;
            }
            volley.kind = kind;
            body->attack_task =
                biolizard_energy_create(task, &volley, &interval);
            /* @bug uninitialized: reads interval at 0x8c501eae, which a
             * failed biolizard_energy_create at 0x8c501ea6 leaves unset */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: where
             * biolizard_energy_create fails, it tries again the next step */
            if (!body->attack_task) {
                return;
            }
#endif
            body->speed = interval * INTERVAL_PACE;
            work->submode++;
        } else if (!biolizard_energy_remaining(body->attack_task)) {
            motion_link(motion, body_twin_motion(TWIN_STAND));
            work->submode = 4;
        } else {
            work->submode++;
        }
    case 1:
        motion_set_speed(motion, body->speed);
        if (biolizard_player_hurt() == 1) {
            body_change_mode(task, BIOLIZARD_PAUSE_ATTACK);
            break;
        }
        if (motion_ended(motion)) {
            work->flags &= ~BODY_FIXED_BLEND;
            biolizard_energy_charge(body->attack_task);
            motion_set(motion, motion->index + 1);
            work->submode++;
        }
        break;
    case 2:
        motion_set_speed(motion, body->speed);
        if (motion_ended(motion)) {
            NJS_VECTOR dir;

            work->flags |= BODY_FIXED_BLEND;
            biolizard_energy_release(body->attack_task);
            sound_play(BIOLIZARD_SOUND_RELEASE, 0, 0, 0);
            sound_set_volume(0, BIOLIZARD_SOUND_RELEASE, RELEASE_VOLUME);
            motion_set(motion, motion->index + 1);

            vector_diff(&biolizard_joint_positions[BIOLIZARD_JOINT_MOUTH],
                        &biolizard_joint_positions[BIOLIZARD_JOINT_HEAD_MIDDLE],
                        &dir);
            njUnitVector(&dir);
            dir.y = 0.0F;
            for (puff = 0; puff < BLACK_PUFFS; puff++) {
                biolizard_black_breath(
                    &biolizard_joint_positions[BIOLIZARD_JOINT_MOUTH], &dir,
                    breath_reach);
            }
            work->submode++;
        }
        break;
    case 3:
        motion_set_speed(motion, body->speed);
        if (biolizard_player_hurt() == 1) {
            body_change_mode(task, BIOLIZARD_PAUSE_ATTACK);
            break;
        }
        if (motion_ended(motion)) {
            if (!biolizard_energy_remaining(body->attack_task)) {
                work->flags &= ~BODY_FIXED_BLEND;
                motion_link_frames(motion, body_twin_motion(TWIN_STAND),
                                   STAND_LINK_FRAMES);
                work->submode++;
            } else {
                motion_set(motion, body_twin_motion(TWIN_ENERGY_1));
                work->submode = 1;
            }
        }
        break;
    case 4:
        if (biolizard_energy_has_shot(body->attack_task) != 1) {
            task_kill(body->attack_task);
            body->attack_task = NULL;
            body_change_mode(task, body_next_attack(task));
            work->flags &= ~BODY_FIXED_BLEND;
        }
        break;
    }
}

static void body_sphere_attack(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    struct motion_control *motion = record->block;
    struct collision_shape *info;

    switch (work->submode) {
    case 0:
        if (body->attack_task) {
            work->submode = 4;
            break;
        }
        motion_link(motion, MOTION_RAISE_SPHERES);
        work->submode++;
    case 1:
        if (!(motion->flags & MOTION_LINKING)) {
            biolizard_camera_charge(body->camera);
            pad_disable_player(0);
            work->submode++;
        }
        break;
    case 2:
        if (motion_ended(motion)) {
            if (biolizard_test_mode == BIOLIZARD_TEST_OFF) {
                if (body->attack == BIOLIZARD_ATTACK_LIFTED_WAVE) {
                    body->attack_task =
                        biolizard_sphere_wave(task, TRUE, NULL, 0);
                } else {
                    body->attack_task = biolizard_sphere_wave(
                        task, FALSE, body_obstacles, BODY_PARTS);
                }
            } else {
                body->attack_task =
                    biolizard_preview_spheres(task, body_obstacles, BODY_PARTS);
            }
            biolizard_camera_shake(WAVE_SHAKE, WAVE_SHAKE_STEPS);
            vibration_start(0, WAVE_POWER, WAVE_FREQ, WAVE_TIME);
            sound_play_for(BIOLIZARD_SOUND_UNK_16, task, SOUND_PRIORITY, 0,
                           APPEAR_SOUND_STEPS);
            /* @bug logic: sets the volume of BIOLIZARD_SOUND_START at
             * 0x8c5021f2, not of BIOLIZARD_SOUND_UNK_16, which plays at
             * 0x8c5021e8 */
            sound_set_volume(task, BIOLIZARD_SOUND_START, SOUND_LOUDEST);
            sound_play_for(BIOLIZARD_SOUND_SHAKE, task, SOUND_PRIORITY, 0,
                           WAVE_SOUND_STEPS);
            sound_set_volume(task, BIOLIZARD_SOUND_SHAKE, SOUND_LOUDEST);
            work->submode++;
        }
        break;
    case 3:
        if (motion_held(motion)) {
            motion_link(motion, MOTION_SEND_SPHERES);
            biolizard_sphere_volley(body->attack_task);
            if (body->attack == BIOLIZARD_ATTACK_LIFTED_WAVE) {
                biolizard_lift_create(task);
                body->rails = biolizard_rails_create(
                    &biolizard_rail_paths[WAVE_RAIL_PATH], WAVE_RAILS,
                    work->ang.y + QUARTER_TURN);
                body->barrier = biolizard_barrier_create(task);
                info = &work->collision->shapes[TOP_SHAPE];
                info->flags |= SHAPE_OFF;
                info++;
                info->flags |= SHAPE_OFF;
            }
            biolizard_camera_volley(body->camera, body->attack);
            pad_enable_player(0);
            work->submode++;
        }
        break;
    case 4:
        if (biolizard_player_hurt() == 1) {
            body_change_mode(task, BIOLIZARD_PAUSE_ATTACK);
        }
        if (biolizard_sphere_finished(body->attack_task) == 1) {
            if (body->attack_task) {
                body->attack_task = NULL;
            }
            if (body->rails) {
                task_kill(body->rails);
                body->rails = NULL;
            }
            motion_link(motion, MOTION_STAND_L);
            work->timer = REST_STEPS;
            work->submode++;
        }
        break;
    case 5:
        if (work->timer-- == 0) {
            body_change_mode(task, body_next_attack(task));
        }
        break;
    }
}

static void body_take_damage(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    struct body_motion *moving = task->motion.p;
    struct motion_control *motion = record->block;

    switch (work->submode) {
    case 0:
        if (body->attack_task) {
            task_kill(body->attack_task);
            body->attack_task = NULL;
        }
        if (body->rails) {
            task_kill(body->rails);
            body->rails = NULL;
        }
        record->power--;
        motion_link(motion, MOTION_DAMAGE);
        {
            NJS_VECTOR spd = {0.0F, LAUNCH_RISE, LAUNCH_SPEED};
            struct angles ang;

            ang.x = ang.z = 0;
            if (body_player_clockwise() == 1) {
                ang.y = work->ang.y + HALF_TURN;
            } else {
                ang.y = work->ang.y;
            }
            moving->launch_yaw = ang.y;
            player_launch_speed(0, &spd, &ang, LAUNCH_LOCK);
        }
        pad_disable_player(0);
        biolizard_camera_shake(DAMAGE_SHAKE, DAMAGE_SHAKE_STEPS);
        vibration_start(0, DAMAGE_POWER, DAMAGE_FREQ, DAMAGE_TIME);
        sound_play(BIOLIZARD_SOUND_DAMAGE, 0, 0, 0);
        sound_set_volume(0, BIOLIZARD_SOUND_DAMAGE, SOUND_LOUDEST);
        sound_play_for(BIOLIZARD_SOUND_SHAKE, task, SOUND_PRIORITY, 0,
                       DAMAGE_SOUND_STEPS);
        sound_set_volume(task, BIOLIZARD_SOUND_SHAKE, SOUND_LOUDEST);
        work->submode++;
    case 1:
        if (njSqrt((player_work[0]->pos.x * player_work[0]->pos.x) +
                   (player_work[0]->pos.z * player_work[0]->pos.z)) <
                LAUNCH_RADIUS &&
            player_work[0]->pos.y > LAUNCH_HEIGHT) {
            NJS_VECTOR spd = {0.0F, LAUNCH_RISE, LAUNCH_SPEED};
            struct angles ang;

            ang.x = ang.z = 0;
            ang.y = moving->launch_yaw;
            player_launch_speed(0, &spd, &ang, LAUNCH_LOCK);
        } else {
            pad_enable_player(0);
        }
        sound_play_for(BIOLIZARD_SOUND_HURT, task, SOUND_PRIORITY, 0,
                       HELD_SOUND_STEPS);
        sound_set_volume(task, BIOLIZARD_SOUND_HURT, SOUND_LOUDEST);
        if (motion_ended(motion)) {
            body_change_mode(task, body_first_attack(task));
            work->flags &= ~WORK_HURT;
            biolizard_life_support_reset(body->life_support);
            pad_enable_player(0);
        }
        break;
    }
}

static void body_pause_attack(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;

    if (body->pause_wait-- == 0) {
        body_change_mode(task, body->previous_mode);
        biolizard_clear_player_hurt();
    }
}

static void body_defeat(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;
    struct body_motion *moving = task->motion.p;
    struct motion_control *motion = record->block;
    Float frame;

    switch (work->submode) {
    case 0:
        if (body->attack_task) {
            task_kill(body->attack_task);
            body->attack_task = NULL;
        }
        if (body->rails) {
            task_kill(body->rails);
            body->rails = NULL;
        }
        record->power--;
        motion_link(motion, MOTION_DEAD);
        {
            NJS_VECTOR spd = {0.0F, LAUNCH_RISE, LAUNCH_SPEED};
            struct angles ang;

            ang.x = ang.z = 0;
            if (body_player_clockwise() == 1) {
                ang.y = work->ang.y + HALF_TURN;
            } else {
                ang.y = work->ang.y;
            }
            moving->launch_yaw = ang.y;
            player_launch_speed(0, &spd, &ang, LAUNCH_LOCK);
        }
        pad_disable_player(0);
        biolizard_camera_shake(DEFEAT_SHAKE, DEFEAT_SHAKE_STEPS);
        vibration_start(0, DEFEAT_POWER, DEFEAT_FREQ, DEFEAT_TIME);
        sound_play_for(BIOLIZARD_SOUND_SHAKE, task, SOUND_PRIORITY, 0,
                       DEFEAT_SOUND_STEPS);
        sound_set_volume(task, BIOLIZARD_SOUND_SHAKE, SOUND_LOUDEST);
        work->submode++;
    case 1:
        if (njSqrt((player_work[0]->pos.x * player_work[0]->pos.x) +
                   (player_work[0]->pos.z * player_work[0]->pos.z)) <
                LAUNCH_RADIUS &&
            player_work[0]->pos.y > LAUNCH_HEIGHT) {
            NJS_VECTOR spd = {0.0F, LAUNCH_RISE, LAUNCH_SPEED};
            struct angles ang;

            ang.x = ang.z = 0;
            ang.y = moving->launch_yaw;
            player_launch_speed(0, &spd, &ang, LAUNCH_LOCK);
        }
        if (!(motion->flags & MOTION_LINKING)) {
            pad_enable_player(0);
            biolizard_camera_defeat(body->camera);
            work->submode++;
        }
        break;
    case 2:
        if (game_state.steps % DEFEAT_SOUND_INTERVAL == 0) {
            sound_play(BIOLIZARD_SOUND_DEFEAT, 0, 0, 0);
            /* @bug logic: sets the volume of BIOLIZARD_SOUND_DAMAGE at
             * 0x8c5027fe, not of BIOLIZARD_SOUND_DEFEAT, which plays at
             * 0x8c5027f6 */
            sound_set_volume(0, BIOLIZARD_SOUND_DAMAGE, DEFEAT_VOLUME);
        }
        if (motion->index == MOTION_DEAD) {
            frame = motion->current.frame;
            if (IN_FRAME(frame, FIRST_CRY_FRAME)) {
                sound_play_for(BIOLIZARD_SOUND_HURT, task, SOUND_PRIORITY, 0,
                               DEAD_SOUND_STEPS);
                sound_set_volume(task, BIOLIZARD_SOUND_HURT, SOUND_LOUDEST);
            }
            if (IN_FRAME(frame, SECOND_CRY_FRAME)) {
                sound_play_for(BIOLIZARD_SOUND_HURT, SECOND_KEY(task),
                               SOUND_PRIORITY, 0, DEAD_SOUND_STEPS);
                sound_set_volume(SECOND_KEY(task), BIOLIZARD_SOUND_HURT,
                                 SOUND_LOUDEST);
                sound_set_pitch(SECOND_KEY(task), BIOLIZARD_SOUND_HURT,
                                SECOND_CRY_PITCH);
            }
            if (IN_FRAME(frame, THIRD_CRY_FRAME)) {
                sound_play_for(BIOLIZARD_SOUND_HURT, SECOND_KEY(task),
                               SOUND_PRIORITY, 0, DEAD_SOUND_STEPS);
                sound_set_volume(SECOND_KEY(task), BIOLIZARD_SOUND_HURT,
                                 SOUND_LOUDEST);
                sound_set_pitch(SECOND_KEY(task), BIOLIZARD_SOUND_HURT,
                                THIRD_CRY_PITCH);
            }
            if (IN_FRAME(frame, FIRST_IMPACT_FRAME)) {
                sound_play_for(BIOLIZARD_SOUND_IMPACT, task, SOUND_PRIORITY, 0,
                               DEAD_SOUND_STEPS);
                sound_set_volume(task, BIOLIZARD_SOUND_IMPACT, SOUND_LOUDEST);
                sound_set_pitch(task, BIOLIZARD_SOUND_IMPACT,
                                FIRST_IMPACT_PITCH);
                vibration_start(0, IMPACT_POWER, IMPACT_FREQ, IMPACT_TIME);
            }
            if (IN_FRAME(frame, SECOND_IMPACT_FRAME)) {
                sound_play_for(BIOLIZARD_SOUND_IMPACT, SECOND_KEY(task),
                               SOUND_PRIORITY, 0, DEAD_SOUND_STEPS);
                sound_set_volume(SECOND_KEY(task), BIOLIZARD_SOUND_IMPACT,
                                 SECOND_IMPACT_VOLUME);
                sound_set_pitch(SECOND_KEY(task), BIOLIZARD_SOUND_IMPACT,
                                SECOND_IMPACT_PITCH);
                vibration_start(0, IMPACT_POWER, IMPACT_FREQ, IMPACT_TIME);
            }
        }
        if (motion_ended(motion)) {
            stage_clear(0);
            vibration_stop();
            work->submode++;
        }
        break;
    case 3:
        if (game_state.winner_posed == 1) {
            work->ang.y = 0;
        }
        break;
    }
    if (player_work[0]->submode != PLAYER_REQUEST_ANIM &&
        !(player_work[0]->pos.y > STAND_HEIGHT)) {
        player_request(0, PLAYER_REQUEST_ANIM, ANIM_STAND);
        player_work[0]->pos.y = STAND_HEIGHT;
    }
}

static void body_preview_motion(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;

    if (work->timer == 0) {
        preview_motion++;
        if (preview_motion == MOTIONS) {
            preview_motion = 0;
        }
        motion_link(motion, preview_motion);
        work->timer = PREVIEW_STEPS;
    } else {
        work->timer--;
    }
    /* @bug memory: reads past motion_names at 0x8c502a26 while
     * preview_motion is MOTION_LABELS or more */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it prints the number of a motion
     * past motion_names */
    if (preview_motion >= MOTION_LABELS) {
        njPrint(PREVIEW_LINE, "MOTION %d", preview_motion);
        return;
    }
#endif
    njPrint(PREVIEW_LINE, "%s", motion_names[preview_motion]);
}

static void biolizard_step(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct biolizard *body = record->own;

    body->mode = work->mode;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.mini_event_running) {
        motion_step(record->block);
        return;
    }
    switch (work->mode) {
    case BIOLIZARD_APPEAR:
        body_appear(task);
        break;
    case BIOLIZARD_WALK:
        body_walk(task);
        break;
    case BIOLIZARD_SWEEP:
        body_sweep(task);
        break;
    case BIOLIZARD_OFFER_RAIL:
        body_offer_rail(task);
        break;
    case BIOLIZARD_ENERGY_ATTACK:
        body_energy_attack(task);
        break;
    case BIOLIZARD_SPHERE_ATTACK:
        body_sphere_attack(task);
        break;
    case BIOLIZARD_DAMAGE:
        body_take_damage(task);
        break;
    case BIOLIZARD_PAUSE_ATTACK:
        body_pause_attack(task);
        break;
    case BIOLIZARD_DEFEAT:
        body_defeat(task);
        break;
    case BIOLIZARD_RESTORE_CAMERA:
        biolizard_camera_restore();
        break;
    case BIOLIZARD_PREVIEW_MOTION:
        body_preview_motion(task);
        break;
    }
    motion_step(record->block);
    body_sense(task);
    body_contact(task);
    body_register_shapes(task);
}

static void biolizard_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct biolizard *body;

    /* @hack redundant: body_slot, which holds body's address and is never
     * read */
    struct biolizard **body_slot = &body;
    struct body_motion *moving;
    struct motion_control *motion;

    njPushMatrixEx();
    biolizard_pose_save_matrix();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njRotateY(NULL, QUARTER_TURN);
    njScaleEx(&work->scl);
    njSetTexture(&biolizard_model_texlist);
    light_apply(BIOLIZARD_LIGHT);

    motion = record->block;
    body = record->own;
    biolizard_pose_draw(&biolizard_model, motion, body->radius_blend);
    light_apply(light_after_draw);

    /* @hack redundant: the read of task->motion into moving, which nothing
     * uses */
    moving = task->motion.p;
    (void)moving;
    njPopMatrixEx();
}

/* the preview's labels. up to MTN_IKURA_1 they name body_motions' entries in
 * order. from MTN_DAMAGE on, each names the entry after its own */
KEPT static const char *motion_names[MOTION_LABELS] = {
#include "boss_last1/body/motion_names.inc"
};
