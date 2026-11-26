#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "boss_last1/combat_art.h"
#include "boss_last1/arena.h"
#include "boss_last1/body.h"
#include "boss_last1/energy.h"
#include "boss_last1/pose.h"
#include "boss_last1/stage.h"
#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "object/mover.h"
#include "player/player.h"
#include "shared/last_boss_arena.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"
#ifdef NONMATCHING
/* zam_vec3_turn_y, by which this side turns a vector. for a finite vector it
 * gives the shipped bits, as turn.h says */
#include "engine/dc/turn.h"
#endif

#define ENERGY_MOTION_FRAMES 28.0F
enum shot_mode {
    SHOT_CHARGE,
    SHOT_RELEASE,
    SHOT_LEAVE_MOUTH,
    SHOT_ORBIT,
    SHOT_FADE
};
enum volley_mode {
    VOLLEY_IDLE,
    VOLLEY_HOLD,
    VOLLEY_START,
    VOLLEY_WAIT,
    VOLLEY_FINISH
};
/* what starts a volley's shots: its animation's frame, its timer, or the
 * Biolizard's calls of biolizard_energy_charge */
enum volley_trigger { TRIGGER_ANIMATION, TRIGGER_TIMER, TRIGGER_BOSS };
/* a volley's flag: a shot has charged. under TRIGGER_ANIMATION, the next
 * waits for the frame to fall below REARM_FRAME */
#define VOLLEY_CHARGED 0x100
#define REARM_FRAME 1.0F
/* a volley's timer as it finishes, which nothing reads */
#define FINISH_STEPS 180
/* a shot's level. level_heights gives each level's height */
enum level { LEVEL_LOW, LEVEL_HIGH };
/* a NEXT_LEVEL that gives the next shot a random level. nothing sets it */
#define RANDOM_LEVEL 99

/* a shot's scale starts at START_SCALE. it grows by GROWTH a step: to
 * CHARGE_SCALE as it charges, and to FULL_SCALE once released */
#define START_SCALE 0.1F
#define GROWTH 0.01F
#define CHARGE_SCALE 0.35F
#define FULL_SCALE 1.0F
/* a shot charges MOUTH_OFFSET above the volley's from */
#define MOUTH_OFFSET 10.0F
/* a shot's turn a step is measured TURN_RADIUS from the arena's center */
#define TURN_RADIUS 200.0F
/* an orbit's step sets a shot's speed to its kind's orbit speed plus
 * SPEED_CARRY of its own */
#define SPEED_CARRY 0.5F
/* how fast a released shot rises or falls to its goal's height */
#define CLIMB_SPEED 1.5F
/* leaving the mouth, a shot keeps LAUNCH_DRAG of its speed across the ground
 * each step. it orbits once LAUNCH_DISTANCE from the mouth across the
 * ground */
#define LAUNCH_DRAG 0.9F
#define LAUNCH_DISTANCE 55.0F
/* a shot's flag: it has come within REACH of player 0 across the ground.
 * it then no longer matches player 0's distance from the center */
#define SHOT_REACHED_PLAYER 0x200
#define REACH 30.0F
/* an orbiting shot past player 0, round from the head, fades once over
 * PASS_DISTANCE from player 0. it also fades within GOAL_ANGLE of its goal's
 * bearing */
#define PASS_DISTANCE 80.0F
#define GOAL_ANGLE 0x800
#define FADE_STEP 0.1F
/* a shot hurts player 0 within HIT_DISTANCE. it pushes the player along its
 * velocity at KNOCKBACK, and up at KNOCKBACK_RISE. a shot of LEVEL_HIGH spares
 * a player in a combo's moves or its wait between them */
#define HIT_DISTANCE 39.0F
#define KNOCKBACK 4.0F
#define KNOCKBACK_RISE 1.0F
/* a shot's sound plays for SHOT_SOUND_STEPS at SHOT_VOLUME, less
 * FADE_VOLUME times the fade's depth */
#define SHOT_SOUND_STEPS 10
#define SHOT_VOLUME 60
#define FADE_VOLUME 120
/* the least and the most a fading shot's volume takes: a Sint8's range */
#define LEAST_VOLUME (-128)
#define MOST_VOLUME 127
/* a shot less than CAMERA_ABOVE above camera 0 draws fainter near it across
 * the ground. its alpha is offset by NEAR_FADE within NEAR_CAMERA, and by
 * FAR_FADE within FAR_CAMERA */
#define NEAR_CAMERA 100.0F
#define FAR_CAMERA 130.0F
#define CAMERA_ABOVE 40.0F
#define NEAR_FADE (-0.5F)
#define FAR_FADE (-0.25F)
/* an orbiting shot's shadow lies SHADOW_DROP below it, but no lower than
 * SHADOW_FLOOR */
#define SHADOW_DROP 40.0F
#define SHADOW_FLOOR 10.2F

/* a shot's motion record is the mover that mover_alloc gives it. velocity
 * and acceleration are the mover's spd and acc. spin turns the thunder, and
 * facing is the yaw toward camera 0. the preview's record is TASK_MOTION's,
 * which ends before facing. the preview keeps its yaw in preview_yaw
 * instead */
struct energy_motion {
    NJS_VECTOR velocity, acceleration;
    Uint8 unused_18[12];
    struct angles spin;
    Uint8 unused_30[0x4c];
    Angle facing;
};

/* a shot's own record, in its task's extra */
struct energy_shot {
    Sint8 kind;
    Uint16 age; /* the steps it has lived, which scroll its shadow */
    /* the bearing a step turns at the kind's orbit speed, TURN_RADIUS out.
     * energy_orbit scales it by speed over the kind's orbit speed */
    Angle turn;
    const NJS_POINT3 *mouth;
    Float mouth_offset;
    NJS_POINT3 goal;
    Float speed; /* how fast it orbits */
    /* 1 until it fades. a fade starts it at 0, and each step takes
     * FADE_STEP. the shot dies once it reaches -1 */
    Float fade;
};
/* a volley's order, in its task's extra */
struct volley {
    Sint8 trigger, kind, limit, timing;
    Float speed, trigger_frame, end_frame;
    const NJS_POINT3 *from;
    NJS_POINT3 goal;
    const struct task_work *animation;
};
/* a volley's work counts the shots it has left to charge in its submode.
 * its tick holds its next shot's level */
#define SHOTS_LEFT(work) ((work)->submode)
#define NEXT_LEVEL(work) ((work)->tick)
enum speed_field { LAUNCH_SPEED, ORBIT_SPEED };
/* a kind's settings, which the energy test's menu edits. interval is the
 * steps between a timed volley's shots */
struct energy_speed {
    Sint16 interval;
    Sint16 unused_interval;
    Float value[2];
};

/* the energy test's menu: two titles from TITLE_ROW, then a row for each
 * setting from BIOLIZARD_MENU_SETTING_ROW */
enum setting_row { ROW_KIND, ROW_INTERVAL, ROW_LAUNCH, ROW_ORBIT, ROWS };
/* a row debug_row never takes, so the menu's tests against it always hold */
#define NO_ROW (-1)
#define TITLE_ROW 5
#define SETTING_LINE(row)                                                      \
    NJM_LOCATION(BIOLIZARD_MENU_COLUMN, (row) + BIOLIZARD_MENU_SETTING_ROW)
/* the menu changes a speed by SPEED_STEP, up to MOST_SPEED, and an interval
 * up to MOST_INTERVAL */
#define SPEED_STEP 0.01F
#define MOST_SPEED 5.0F
#define MOST_INTERVAL 300

/* the preview's menu, on pad PREVIEW_PAD. it prints two titles from
 * PREVIEW_TITLE_ROW, then a row for each turn from TURN_ROW. a row holds
 * the turn's name at LABEL_COLUMN, then each part's value from
 * BIOLIZARD_MENU_COLUMN, PART_WIDTH apart. a fine change is FINE_TURN, and a
 * coarse one COARSE_TURN */
#define PREVIEW_PAD 3
#define PREVIEW_TITLE_ROW 4
#define TURN_ROW 6
#define LABEL_COLUMN 14
#define PART_WIDTH 7
#define FINE_TURN 16
#define COARSE_TURN (QUARTER_TURN / 4)
enum part { PART_BOWL, PART_THUNDER, PARTS }; /* a shot's parts */
/* the columns of part_turns. the first three hold a part's turn a step about
 * x, y and z. the rest tilt the axes it turns about. turn_names names each */
enum turn { TURN_X, TURN_Y, TURN_Z, TILT_X, TILT_Y, TILT_Z, TURNS };
/* the preview menu's cursor, by these indexes */
enum cursor { CURSOR_PART, CURSOR_TURN };
static const char interval_title[16] = "SHOOT INTERVAL";
static const char select_title[16] = "SELECT TYPE Y";
static const char slow_name[8] = "SLOW";
static const char normal_name[8] = "NORMAL";
static const char fast_name[8] = "FAST";
static const char speed_format[12] = "SPEED %s";
static const char interval_format[8] = "SAME %d";
static const char launch_format[16] = "FIRSTSPD  %.2f";
static const char travel_format[12] = "SPD  %.2f";
static const char test_title[12] = "GRV TEST";
static const char parts_title[16] = "BOWL   THUNDER";
static const char label_format[4] = "%s";
static const char angle_format[4] = "%+x";
static const NJS_POINT3 preview_origin = {300, 45, -120};
static const char name_rotx[8] = "ROTX";
static const char name_roty[8] = "ROTY";
static const char name_rotz[8] = "ROTZ";
static const char name_zikux[8] = "ZIKUX";
static const char name_zikuy[8] = "ZIKUY";
static const char name_zikuz[6] = "ZIKUZ";
/* shadow_scroll's uvs. the first two words count the entries and the keys.
 * each entry gives an index in the shadow mesh's polygon list and a count of
 * vertices. a u and a v follow for each vertex */
static Sint16 shadow_uvs[104] = {
#include "boss_last1/energy/shadow_uvs.inc"
};
static struct texture_anim shadow_scroll = {
#include "boss_last1/energy/shadow_scroll.inc"
};
static struct energy_speed energy_speeds[BIOLIZARD_ENERGY_KINDS] = {
    {88, 80, {4, 0.9F}}, {88, 80, {4.5F, 1.25F}}, {88, 80, {5, 1.75F}}};
static Float motion_rate = 1.6F;
static Float unused_floats[9] = {1, 1, 1, 1};
/* each part's turns, by enum turn. the third row, and the bowl's tilt, go
 * unread */
static Angle part_turns[3][TURNS] = {
    {0},
    {0xb10, 0, -(QUARTER_TURN / 4), (QUARTER_TURN / 4), 0, 0},
    {0x800, 0x300, 0x800}};
static Angle preview_yaw = 0;
static const char *turn_names[] = {name_rotx,  name_roty,  name_rotz,
                                   name_zikux, name_zikuy, name_zikuz};
static Float level_heights[] = {-5, 47};
/* the energy test's selected row, and the kind it shows */
static Sint8 debug_row = 0, debug_kind = 0;
static Bool preview_menu_shown = FALSE;
static Sint8 preview_cursor[2] = {0, 0};

static void energy_integrate(struct task *task, Float drag)
{
    struct task_work *work;
    struct energy_motion *motion;
    NJS_POINT3 *pos;
    NJS_VECTOR *velocity;
    NJS_VECTOR *acceleration;
    NJS_VECTOR previous;
    NJS_VECTOR direction;
    Float length;

    work = task->work;
    motion = task->motion.p;
    pos = &work->pos;
    velocity = &motion->velocity;
    acceleration = &motion->acceleration;
    /* @hack redundant: direction, scaled to the velocity's length and never
     * read, and the velocity's copy written back unchanged */
    previous = *velocity;
    length = njScalor(&previous);
    direction = *acceleration;
    njUnitVector(&direction);
    direction.x *= length;
    direction.y *= length;
    direction.z *= length;
    *velocity = previous;
    velocity->x += acceleration->x;
    velocity->y += acceleration->y;
    velocity->z += acceleration->z;
    pos->x += velocity->x;
    pos->y += velocity->y;
    pos->z += velocity->z;
    acceleration->x = 0.0F;
    acceleration->y = 0.0F;
    acceleration->z = 0.0F;
    if (drag != 0.0F) {
        velocity->x *= drag;
        velocity->z *= drag;
    }
}

static Angle energy_turn_per_step(struct task *task, Float scale)
{
    struct task_work *work;
    NJS_POINT3 pos;
    NJS_POINT3 next;
    NJS_VECTOR tangent;

    work = task->work;
    pos = work->pos;
    last_boss_along(&work->pos, TURN_RADIUS, &pos);
    last_boss_tangent_vector(&pos, &tangent);
    tangent.x *= scale;
    tangent.y *= scale;
    tangent.z *= scale;
    vector_sum(&pos, &tangent, &next);
    return last_boss_angle_difference(&pos, &next);
}

static void energy_orbit(struct task *task, NJS_POINT3 *out, Float unused_speed,
                         Float orbit_speed)
{
    struct task_work *work;
    struct energy_shot *shot;
    NJS_POINT3 pos;
    NJS_VECTOR direction;
    Float step_fraction;
    Float y;
    Angle turn;

    work = task->work;
    shot = task->extra.p;
    /* @hack redundant: step_fraction, and the last_boss_turn by it into *out,
     * which the rotation below overwrites */
    step_fraction =
        shot->speed * njInvertSqrt(orbit_speed) * njInvertSqrt(orbit_speed);
    pos = work->pos;
    y = pos.y;
    last_boss_turn(&pos, (Angle)(shot->turn * step_fraction), out);
    turn = shot->turn * shot->speed / orbit_speed;
    last_boss_radius(&pos, &direction);
#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls below,
     * as engine/dc/turn.h says */
    zam_vec3_turn_y(turn, &direction.x, &direction.x);
#else
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, turn);
    njCalcVector(NULL, &direction, &direction);
    njPopMatrix(1);
#endif
    *out = direction;
    out->y = y;
}

static Float energy_height(Sint8 level)
{
    Float height;

    switch (level) {
    case LEVEL_HIGH:
        height = level_heights[LEVEL_HIGH];
        break;
    case LEVEL_LOW:
    default:
        height = level_heights[LEVEL_LOW];
        break;
    }
    return height;
}

static Sint8 energy_next_level(Sint8 level)
{
    Sint32 choice;

    choice = (Sint32)(rand() * (200.0F * RAND_SCALE));

    /* @hack spelling: four returns, one for each low bit of choice with
     * each level, where one expression of the two would do */
    choice &= 1;
    if (choice == 0) {
        if (!level) {
            return LEVEL_LOW;
        } else {
            return LEVEL_HIGH;
        }
    } else {
        if (!level) {
            return LEVEL_HIGH;
        } else {
            return LEVEL_LOW;
        }
    }
}

/* @hack spelling: ENERGY_REACH_HEIGHT a macro, where a static function
 * would do */
#define ENERGY_REACH_HEIGHT(work, motion, shot)                                \
    do {                                                                       \
        (work)->pos.y += (motion)->velocity.y;                                 \
        if ((motion)->velocity.y > 0.0F) {                                     \
            if ((work)->pos.y > (shot)->goal.y) {                              \
                (work)->pos.y = (shot)->goal.y;                                \
                (motion)->velocity.y = 0.0F;                                   \
            }                                                                  \
        } else if ((work)->pos.y < (shot)->goal.y) {                           \
            (work)->pos.y = (shot)->goal.y;                                    \
            (motion)->velocity.y = 0.0F;                                       \
        }                                                                      \
    } while (0)

/* @hack spelling: ENERGY_GROW a macro, where a static function would do */
#define ENERGY_GROW(work, limit)                                               \
    do {                                                                       \
        if ((work)->scl.x < (limit)) {                                         \
            (work)->scl.x += GROWTH;                                           \
            if ((work)->scl.x > (limit)) {                                     \
                (work)->scl.x = (limit);                                       \
            }                                                                  \
        }                                                                      \
    } while (0)

static void energy_charge(struct task *task)
{
    struct task_work *work;
    struct energy_shot *shot;

    work = task->work;
    shot = task->extra.p;
    ENERGY_GROW(work, CHARGE_SCALE);
    work->pos = *shot->mouth;
    work->pos.y += shot->mouth_offset;
}

static void energy_release(struct task *task)
{
    struct task_work *work;
    struct energy_motion *motion;
    struct energy_shot *shot;

    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused;

    shot = task->extra.p;
    work = task->work;
    motion = task->motion.p;
    if (work->pos.y < shot->goal.y) {
        motion->velocity.y = CLIMB_SPEED;
    } else {
        motion->velocity.y = -CLIMB_SPEED;
    }
    work->mode = SHOT_LEAVE_MOUTH;
    work->submode = 0;
}

static void energy_leave_mouth(struct task *task)
{
    struct task_work *work;
    struct energy_motion *motion;
    struct energy_shot *shot;
    struct energy_speed *speed;
    NJS_VECTOR direction;
    NJS_POINT3 *joints[2];
    NJS_POINT3 *mouth;

    NJS_VECTOR *flat;
    Float delta_z;
    Float delta_x;

    shot = task->extra.p;
    work = task->work;
    motion = task->motion.p;
    speed = &energy_speeds[shot->kind];
    joints[0] = &biolizard_joint_positions[BIOLIZARD_JOINT_FRONT];
    joints[1] = &biolizard_joint_positions[BIOLIZARD_JOINT_MOUTH];
    vector_diff(joints[1], joints[0], &direction);
    direction.y = 0.0F;
    njUnitVector(&direction);
    direction.x *= speed->value[LAUNCH_SPEED];
    direction.y *= speed->value[LAUNCH_SPEED];
    direction.z *= speed->value[LAUNCH_SPEED];
    motion->acceleration = direction;
    ENERGY_GROW(work, FULL_SCALE);
    ENERGY_REACH_HEIGHT(work, motion, shot);
    energy_integrate(task, LAUNCH_DRAG);

    /* @hack spelling: mouth and the deltas set inside njSqrt's argument,
     * where statements before the test would do */
#ifdef NONMATCHING
    /* the plain form the hack tag above names */
    mouth = &biolizard_joint_positions[BIOLIZARD_JOINT_MOUTH];
    delta_x = work->pos.x - mouth->x;
    delta_z = work->pos.z - mouth->z;
    if (njSqrt(delta_x * delta_x + delta_z * delta_z) > LAUNCH_DISTANCE) {
#else
    if (njSqrt(
            (delta_x =
                 work->pos.x -
                 (mouth = &biolizard_joint_positions[BIOLIZARD_JOINT_MOUTH])->x,
             (delta_x * delta_x) + (delta_z = work->pos.z - mouth->z,
                                    delta_z * delta_z))) > LAUNCH_DISTANCE) {
#endif
        shot->turn = energy_turn_per_step(task, speed->value[ORBIT_SPEED]);
        /* @hack redundant: flat, a copy of the velocity with y cleared, which
         * nothing reads */
        flat = &direction;
        *flat = motion->velocity;
        flat->y = 0.0F;
        shot->speed = speed->value[ORBIT_SPEED];
        work->mode = SHOT_ORBIT;
        work->submode = 0;
    }
}

#pragma inline(match_player_radius)
static void match_player_radius(struct task_work *work, Float limit,
                                Float (*distance)(const NJS_POINT3 *,
                                                  const NJS_POINT3 *))
{
    NJS_VECTOR offset;
    Float radius_step;
    Float own_radius;
    Angle bearing;

    radius_step = distance(&player_work[0]->pos, &biolizard_arena_center);
    own_radius = distance(&work->pos, &biolizard_arena_center);
    radius_step -= own_radius;
    if (radius_step > limit) {
        radius_step = limit;
    } else if (radius_step < -limit) {
        radius_step = -limit;
    }
    bearing = last_boss_bearing(&work->pos);
    offset.x = njSin(bearing);
    offset.z = njCos(bearing);
    offset.x *= radius_step;
    /* offset.y is never set, and nothing reads it after this scale */
#ifdef NONMATCHING
    /* leaves offset.y unscaled */
#else
    offset.y *= radius_step;
#endif
    offset.z *= radius_step;
    work->pos.x += offset.x;
    work->pos.z += offset.z;
}

static void energy_follow_ring(struct task *task)
{
    struct task_work *work;
    struct energy_motion *motion;
    struct energy_shot *shot;
    struct energy_speed *speed;
    struct task_work **players;

    /* @hack type: last_players volatile, where a plain pointer would do */
    struct task_work **volatile last_players;
    struct task_work *player;
    NJS_POINT3 *head;

    /* @hack redundant: unused1 and unused2, locals that only size the
     * frame */
    NJS_VECTOR unused1;
    NJS_VECTOR unused2;
    Float delta_x;
    Float delta_z;
    Float ground_distance;
    Float distance_to_player;
    Angle first_angle;
    Angle second_angle;
    Float (*distance)(const NJS_POINT3 *, const NJS_POINT3 *);

    shot = task->extra.p;
    work = task->work;
    motion = task->motion.p;
    speed = &energy_speeds[shot->kind];

    /* @hack redundant: a last_boss_angle_distance whose result nothing
     * reads, with players set inside its argument */
    last_boss_angle_distance(&work->pos, &(*(players = &player_work[0]))->pos);
    /* @hack spelling: player and the deltas set inside njSqrt's argument,
     * where statements before it would do */
#ifdef NONMATCHING
    /* the plain form the hack tag above names */
    player = *players;
    delta_x = player->pos.x - work->pos.x;
    delta_z = player->pos.z - work->pos.z;
    ground_distance = njSqrt(delta_x * delta_x + delta_z * delta_z);
#else
    ground_distance =
        njSqrt((delta_x = (player = *players)->pos.x - work->pos.x,
                (delta_x * delta_x) + (delta_z = player->pos.z - work->pos.z,
                                       delta_z * delta_z)));
#endif
    energy_orbit(task, &work->pos, shot->speed, speed->value[ORBIT_SPEED]);

    /* @hack spelling: vector_distance called through distance, where
     * direct calls would do */
    distance = vector_distance;
    if (!(work->flags & SHOT_REACHED_PLAYER)) {
        match_player_radius(work, speed->value[ORBIT_SPEED], distance);
    }
    shot->speed = speed->value[ORBIT_SPEED] + (shot->speed * SPEED_CARRY);
    if (ground_distance < REACH) {
        work->flags |= SHOT_REACHED_PLAYER;
    }
    ENERGY_REACH_HEIGHT(work, motion, shot);

    /* @hack redundant: an if (0) around a scl.x of 1 */
    if (0) {
        work->scl.x = 1.0F;
    }
    ENERGY_GROW(work, FULL_SCALE);

    first_angle = last_boss_angle_distance(
        (head = &biolizard_joint_positions[BIOLIZARD_JOINT_HEAD]),
        &(*(last_players = &player_work[0]))->pos);
    second_angle = last_boss_angle_distance(head, &work->pos);
    distance_to_player = distance(&work->pos, &(*last_players)->pos);
    if (second_angle > first_angle && distance_to_player > PASS_DISTANCE) {
        shot->fade = 0.0F;
        work->mode = SHOT_FADE;
        work->submode = 0;
    }
    first_angle = last_boss_bearing(&work->pos);
    second_angle = last_boss_bearing(&shot->goal);
    if (angle_distance(first_angle, second_angle) < GOAL_ANGLE) {
        shot->fade = 0.0F;
        work->mode = SHOT_FADE;
        work->submode = 0;
    }
}

static void energy_fade(struct task *task)
{
    struct task_work *work;
    struct energy_shot *shot;
    struct energy_speed *speed;
    struct task_work **players;
    struct task_work *player;

    /* @hack redundant: unused1 and unused2, locals that only size the
     * frame */
    NJS_VECTOR unused1;
    NJS_VECTOR unused2;
    Float delta_x;
    Float delta_z;

    work = task->work;

    /* @hack spelling: shot set inside energy_speeds' index, where a
     * statement before it would do */
    speed = &energy_speeds[(shot = task->extra.p)->kind];

    /* @hack redundant: a last_boss_angle_distance whose result nothing
     * reads, with players set inside its argument */
    last_boss_angle_distance(&work->pos, &(*(players = &player_work[0]))->pos);
    /* @hack redundant: an njSqrt of the distance to player 0 across the
     * ground, whose result nothing reads */
#ifdef NONMATCHING
#else
    njSqrt((delta_x = (player = *players)->pos.x - work->pos.x,
            (delta_z = player->pos.z - work->pos.z, delta_z * delta_z) +
                (delta_x * delta_x)));

#endif
    /* @hack redundant: an if (0) around a test of task, work and shot */
    if (0) {
        if (task && work && shot) {
        }
    }
    energy_orbit(task, &work->pos, shot->speed, speed->value[ORBIT_SPEED]);
    shot->fade -= FADE_STEP;
    if (shot->fade > -1.0F) {
        return;
    }
    task_kill(task);
}

static void energy_hit_player(struct task *task)
{
    struct task_work *work;
    struct energy_shot *shot;
    struct energy_motion *motion;
    Float distance;
    int mode;
    NJS_VECTOR knockback;

    work = task->work;
    shot = task->extra.p;
    motion = task->motion.p;
    distance = vector_distance(&work->pos, &player_work[0]->pos);
    if (distance < HIT_DISTANCE) {
        if (shot->goal.y == level_heights[LEVEL_HIGH]) {
            mode = player_work[0]->mode;
            if (mode >= PLAYER_MODE_SONIC_COMBO_1 &&
                mode <= PLAYER_MODE_SONIC_COMBO_WAIT) {
                return;
            }
            if (mode >= PLAYER_MODE_SONIC_COMBO_RUNNING_1 &&
                mode <= PLAYER_MODE_SONIC_COMBO_RUNNING_3) {
                return;
            }
        }
        knockback = motion->velocity;
        njUnitVector(&knockback);
        knockback.x *= KNOCKBACK;
        knockback.y = KNOCKBACK_RISE;
        knockback.z *= KNOCKBACK;
        player_set_speed(0, knockback.x, knockback.y, knockback.z);
        player_work[0]->flags |= WORK_HURT;
        biolizard_mark_player_hurt();
    }
}

static void energy_step(struct task *task)
{
    struct task_work *work;
    struct energy_motion *motion;
    struct energy_shot *shot;
    Sint16 volume;

    work = task->work;
    shot = task->extra.p;
    motion = task->motion.p;
    if (game_state.time_stopped) {
        return;
    }
    switch (work->mode) {
    case SHOT_CHARGE:
        energy_charge(task);
        break;
    case SHOT_RELEASE:
        energy_release(task);
        break;
    case SHOT_LEAVE_MOUTH:
        energy_leave_mouth(task);
        break;
    case SHOT_ORBIT:
        energy_follow_ring(task);
        break;
    case SHOT_FADE:
        energy_fade(task);
        break;
    }
    if (!biolizard_player_hurt()) {
        energy_hit_player(task);
    }
    motion->facing = last_boss_target_yaw(task, &camera_views[0]->pos);
    shot->age++;
    work->scl.z += motion_rate;
    if (work->scl.z >= ENERGY_MOTION_FRAMES) {
        work->scl.z -= ENERGY_MOTION_FRAMES;
    }
    work->ang.x += part_turns[PART_BOWL][TURN_X];
    work->ang.y += part_turns[PART_BOWL][TURN_Y];
    work->ang.z += part_turns[PART_BOWL][TURN_Z];
    motion->spin.x += part_turns[PART_THUNDER][TURN_X];
    motion->spin.y += part_turns[PART_THUNDER][TURN_Y];
    motion->spin.z += part_turns[PART_THUNDER][TURN_Z];
    if (shot->fade >= 0.0F) {
        sound_play_at_for(BIOLIZARD_SOUND_ENERGY, task, 1, SHOT_VOLUME,
                          SHOT_SOUND_STEPS, &work->pos);
    } else {
        volume = SHOT_VOLUME + (FADE_VOLUME * shot->fade);
        if (volume < LEAST_VOLUME) {
            volume = LEAST_VOLUME;
        }
        if (volume > MOST_VOLUME) {
            volume = MOST_VOLUME;
        }
        sound_play_at_for(BIOLIZARD_SOUND_ENERGY, task, 1, volume,
                          SHOT_SOUND_STEPS, &work->pos);
    }
}

static void energy_draw(struct task *task)
{
    struct task_work *work;
    struct energy_motion *motion;
    struct energy_shot *shot;
    NJS_VECTOR scl;
    NJS_VECTOR offset;
    NJS_VECTOR shadow;
    /* @hack redundant: glow, a second scale that nothing applies */
    NJS_VECTOR glow;
    Float camera_range;
    Float height;

    work = task->work;
    motion = task->motion.p;
    shot = task->extra.p;

    scl.x = scl.y = scl.z = task->work->scl.x;
    draw_attr_save();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_OFFSET_MATERIAL |
                     NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
    draw_attr_set(0, NJD_FST_UA);
    if (shot->fade >= 0.0F) {
        vector_diff(&work->pos, &camera_views[0]->pos, &offset);
        camera_range = njSqrt((offset.x * offset.x) + (offset.z * offset.z));
        if (offset.y < CAMERA_ABOVE) {
            if (camera_range < NEAR_CAMERA) {
                constant_material_set(NEAR_FADE, 1.0F, 1.0F, 1.0F);
            } else if (camera_range < FAR_CAMERA) {
                constant_material_set(FAR_FADE, 1.0F, 1.0F, 1.0F);
            }
        }
    } else {
        constant_material_set(shot->fade, shot->fade, shot->fade, shot->fade);
    }
    njSetTexture(&biolizard_effect_texlist);
    light_apply(BIOLIZARD_LIGHT);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njPushMatrixEx();
    njRotateY(NULL, motion->facing + QUARTER_TURN);
    njScaleEx(&scl);
    njCnkSimpleDrawMotion(&biolizard_energy_bowl, &biolizard_energy_bowl_motion,
                          work->scl.z);
    matrix_load_parent();
    njRotateZ(NULL, part_turns[PART_THUNDER][TILT_Z]);
    njRotateX(NULL, part_turns[PART_THUNDER][TILT_X]);
    njRotateY(NULL, part_turns[PART_THUNDER][TILT_Y]);
    njRotateZ(NULL, motion->spin.z);
    njRotateX(NULL, motion->spin.x);
    njRotateY(NULL, motion->spin.y);
    njRotateY(NULL, motion->facing);
    njScaleEx(&scl);
    njCnkSimpleDrawMotion(&biolizard_energy_thunder,
                          &biolizard_energy_thunder_motion, work->scl.z);
    njPopMatrixEx();
    njPopMatrixEx();
    if (work->mode == SHOT_ORBIT) {

        offset = task->work->pos;
        offset.y -= SHADOW_DROP;
        if (offset.y < SHADOW_FLOOR) {
            offset.y = SHADOW_FLOOR;
        }
        /* @bug logic: the test at 0x8c504ea8 always holds, since the floor at
         * 0x8c504e98 keeps the shadow at SHADOW_FLOOR or higher. the branch
         * from 0x8c504ec2, which scales the shadow by its height, never
         * runs */
        if (offset.y > 0.0F) {
            shadow.x = shadow.z = work->scl.x * SHADOW_DROP;
            glow.x = glow.z = work->scl.x;
        } else {
            height = offset.y + SHADOW_DROP;
            shadow.x = shadow.z = work->scl.x * height;
            glow.x = glow.z = work->scl.x * height * (1.0F / SHADOW_DROP);
        }
        glow.y = 1.0F;
        shadow.y = 1.0F;
        draw_control_set(
            (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER));
        njPushMatrixEx();
        njTranslateEx(&offset);
        njPushMatrixEx();
        njScaleEx(&shadow);
        njCnkModDrawObject(&shadow_object);
        matrix_load_parent();
        texture_anim_apply(&biolizard_energy_shadow_mesh, &shadow_scroll,
                           shot->age);
        njPopMatrixEx();
        njPopMatrixEx();
        draw_control_clear(
            (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER));
    }
    light_apply(light_after_draw);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}

static void energy_destroy(struct task *task)
{
}

static struct task *shot_create(struct task *parent, Sint8 timer, Sint8 level)
{
    struct task *task;
    struct task_work *work;
    struct energy_shot *shot;
    struct volley order;
    struct energy_motion *motion;
    struct volley *to;
    struct volley *from;
    struct task *owner;
    Float y;

    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, energy_step, parent);
    if (!task) {
        return NULL;
    }
    work = task->work;
    shot = syCalloc(1, sizeof(*shot));
    if (!shot) {
        task_kill(task);
        return NULL;
    }
    task->extra.p = shot;
    motion = (struct energy_motion *)mover_alloc(task);
    if (!motion) {
        task_kill(task);
        return NULL;
    }

    /* @hack spelling: order copied through to, owner and from, where
     * order = *(struct volley *)task->parent->extra would do */
    to = &order;
    owner = task->parent;
    from = owner->extra.p;
    *to = *from;
    work->pos = *order.from;
    work->pos.y += MOUTH_OFFSET;
    shot->mouth = order.from;
    shot->mouth_offset = MOUTH_OFFSET;
    shot->goal = order.goal;
    shot->speed = order.speed;
    shot->kind = order.kind;
    shot->fade = 1.0F;

    y = energy_height(level);
    shot->goal.y = y;
    work->tick = timer;

    /* @hack redundant: a test of tick whose branches both set SHOT_CHARGE */
    if (work->tick) {
        work->mode = SHOT_CHARGE;
    } else {
        work->mode = SHOT_CHARGE;
    }
    work->scl.x = START_SCALE;
    work->scl.z = 0.0F;
    task->draw = energy_draw;
    task->destructor = energy_destroy;
    return task;
}

void biolizard_energy_charge(struct task *volley_task)
{
    struct task_work *work;
    struct volley *order;
    Sint8 level;

    if (!volley_task) {
        return;
    }
    work = volley_task->work;
    order = volley_task->extra.p;
    if (NEXT_LEVEL(work) == RANDOM_LEVEL) {
        NEXT_LEVEL(work) = energy_next_level(NEXT_LEVEL(work));
    }
    shot_create(volley_task, 1, NEXT_LEVEL(work));

    level = energy_next_level(NEXT_LEVEL(work));
    NEXT_LEVEL(work) = level;
    if (order->limit != BIOLIZARD_ENERGY_NO_LIMIT) {
        SHOTS_LEFT(work)--;
    }
    work->flags |= VOLLEY_CHARGED;
}

static void energy_release_siblings(struct task *task, struct task *head)
{
    struct task_work *work;

    if (!task) {
        return;
    }
    work = task->work;
    if (work->mode == SHOT_CHARGE) {
        work->mode = SHOT_RELEASE;
        work->submode = 0;
    }
    if (task->next != head) {
        energy_release_siblings(task->next, head);
    }
}

void biolizard_energy_release(struct task *volley_task)
{
    struct task *child;

    if (!volley_task) {
        return;
    }
    if (!volley_task->child) {
        return;
    }
    child = volley_task->child;
    energy_release_siblings(child, child);
}

static void volley_start(struct task *task)
{
    struct task_work *work;
    struct volley *order;
    struct energy_speed *speed;
    Sint8 level;

    order = task->extra.p;
    work = task->work;
    speed = &energy_speeds[order->kind];
    if (NEXT_LEVEL(work) == RANDOM_LEVEL) {
        NEXT_LEVEL(work) = energy_next_level(NEXT_LEVEL(work));
    }
    shot_create(task, 1, NEXT_LEVEL(work));
    level = energy_next_level(NEXT_LEVEL(work));
    work->timer = speed->interval;
    NEXT_LEVEL(work) = level;
    if (order->limit != BIOLIZARD_ENERGY_NO_LIMIT) {
        SHOTS_LEFT(work)--;
    }
    work->mode = VOLLEY_WAIT;
    work->flags |= VOLLEY_CHARGED;
}

static void volley_wait(struct task *task)
{
    struct task_work *work;
    struct volley *order;

    work = task->work;
    order = task->extra.p;
    switch (order->trigger) {
    case TRIGGER_TIMER:
        work->timer--;
        if (work->timer == 0) {
            if (order->limit != BIOLIZARD_ENERGY_NO_LIMIT &&
                SHOTS_LEFT(work) == 0) {
                work->mode = VOLLEY_FINISH;
                work->timer = FINISH_STEPS;
            } else {
                work->mode = VOLLEY_START;
            }
        }
        break;
    case TRIGGER_ANIMATION:
        if (order->animation->pos.z > order->trigger_frame &&
            !(work->flags & VOLLEY_CHARGED)) {
            if (order->limit != BIOLIZARD_ENERGY_NO_LIMIT &&
                SHOTS_LEFT(work) == 0) {
                work->mode = VOLLEY_FINISH;
                work->timer = FINISH_STEPS;
            } else {
                work->mode = VOLLEY_START;
            }
        }
        if (order->animation->pos.z < REARM_FRAME) {
            work->flags &= ~VOLLEY_CHARGED;
        }
        break;
    case TRIGGER_BOSS:
        if (order->limit != BIOLIZARD_ENERGY_NO_LIMIT &&
            SHOTS_LEFT(work) == 0) {
            work->mode = VOLLEY_FINISH;
            work->timer = FINISH_STEPS;
        }
        break;
    }
}

static void volley_finish(struct task *task)
{
    struct volley *order;

    order = task->extra.p;
    if (order->trigger != TRIGGER_BOSS && !task->child) {
        task_kill(task);
    }
}

static void volley_step(struct task *task)
{
    struct task_work *work;
    struct volley *order;
    struct energy_speed *speed;

    work = task->work;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.unk_09) {
        return;
    }
    if (biolizard_player_hurt() == 1) {
        return;
    }
    order = task->extra.p;
    speed = &energy_speeds[order->kind];
    order->speed = speed->value[ORBIT_SPEED];
    switch (work->mode) {
    case VOLLEY_IDLE:
    case VOLLEY_HOLD:
        break;
    case VOLLEY_START:
        volley_start(task);
        break;
    case VOLLEY_WAIT:
        volley_wait(task);
        break;
    case VOLLEY_FINISH:
        volley_finish(task);
        break;
    }
}

static void volley_destroy(struct task *task)
{
}

struct task *biolizard_energy_create(struct task *parent,
                                     const struct biolizard_energy_setup *setup,
                                     Uint16 *interval)
{
    struct task *task;
    struct task_work *work;
    struct volley *order;
    struct energy_speed *speed;

    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, volley_step, parent);
    if (!task) {
        return NULL;
    }
    work = task->work;
    order = syCalloc(1, sizeof(*order));
    if (!order) {
        task_kill(task);
        return NULL;
    }
    task->extra.p = order;
    /* @hack redundant: the first of two writes of the count to SHOTS_LEFT */
    SHOTS_LEFT(work) = setup->count;
    order->from = setup->from;
    order->goal = *setup->goal;
    order->timing = setup->timing;
    order->limit = setup->count;
    SHOTS_LEFT(work) = order->limit;
    order->speed = setup->speed;
    order->kind = setup->kind;
    if (setup->animation != NULL) {
        order->animation = setup->animation;
        order->trigger_frame = setup->trigger_frame;
        order->end_frame = setup->end_frame;
        order->trigger = TRIGGER_ANIMATION;
    } else if (order->timing < 0) {
        order->trigger = TRIGGER_BOSS;
    } else {
        order->trigger = TRIGGER_TIMER;
    }
    work->mode = VOLLEY_WAIT;
    NEXT_LEVEL(work) = LEVEL_LOW;
    task->destructor = volley_destroy;
    speed = &energy_speeds[order->kind];
    /* @bug logic: a timed volley counts its first wait down from the 0 its
     * timer starts at, at 0x8c505270. the timer wraps, and the first shot
     * comes 65536 steps late */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: a timed volley's first wait
     * starts at its kind's interval */
    if (order->trigger == TRIGGER_TIMER) {
        work->timer = speed->interval;
    }
#endif
    *interval = speed->interval;
    return task;
}

void biolizard_debug_energy(void)
{
    struct energy_speed *speed;
    const char *name;
    Sint8 row;
    Sint8 field;
    Sint8 selected;

    speed = &energy_speeds[debug_kind];
    njPrint(NJM_LOCATION(BIOLIZARD_MENU_COLUMN, TITLE_ROW), interval_title);
    njPrint(NJM_LOCATION(BIOLIZARD_MENU_COLUMN, TITLE_ROW + 1), select_title);
    for (row = 0; row < (Sint8)ROWS; row++) {
        if (row == debug_row) {
            njPrintColor(BIOLIZARD_MENU_SELECTED);
        }
        switch (row) {
        case ROW_KIND:
            switch (debug_kind) {
            case BIOLIZARD_ENERGY_SLOW:
                name = slow_name;
                break;
            case BIOLIZARD_ENERGY_NORMAL:
                name = normal_name;
                break;
            case BIOLIZARD_ENERGY_FAST:
                name = fast_name;
                break;
#ifdef NONMATCHING
            /* this side prints no more rows for a debug_kind that names no
             * kind. the matching side prints name unset */
            default:
                return;
#endif
            }
            njPrint(SETTING_LINE(row), speed_format, name);
            break;
        case ROW_INTERVAL:
            njPrint(SETTING_LINE(row), interval_format, speed->interval);
            break;
        case ROW_LAUNCH:
            njPrint(SETTING_LINE(row), launch_format,
                    speed->value[LAUNCH_SPEED]);
            break;
        case ROW_ORBIT:
            njPrint(SETTING_LINE(row), travel_format,
                    speed->value[ORBIT_SPEED]);
            break;
        }
        if (row == debug_row) {
            njPrintColor(BIOLIZARD_MENU_TEXT);
        }
    }
    if (player_pad_records[0].press & PDD_DGT_TY) {
        debug_row++;
        if (debug_row == ROWS) {
            debug_row = 0;
        }
    }
    if (((player_pads[0]->on & PDD_DGT_TL) ||
         (player_pads[0]->press & PDD_DGT_TL)) &&
        debug_row != NO_ROW) {
        if (player_pads[0]->press & PDD_DGT_TL) {
            if (debug_row == ROW_KIND) {
                if (debug_kind != BIOLIZARD_ENERGY_SLOW) {
                    debug_kind--;
                }
            }
        } else if (player_pads[0]->on & PDD_DGT_TL) {
            if (debug_row == ROW_INTERVAL) {
                if (speed->interval != 0) {
                    speed->interval--;
                }
            } else if (debug_row >= ROW_LAUNCH) {
                selected = debug_row - ROW_LAUNCH;
                /* @bug logic: the test at 0x8c505660 never stops a speed at
                 * 0. the kinds' speeds step past 0 by SPEED_STEP without
                 * landing on it, so a speed falls below 0 for as long as L is
                 * held */
                if (speed->value[selected] != 0.0F) {
                    speed->value[selected] -= SPEED_STEP;
                }
            }
        }
    }
    if (((player_pads[0]->on & PDD_DGT_TR) ||
         (player_pads[0]->press & PDD_DGT_TR)) &&
        debug_row != NO_ROW) {
        if (player_pads[0]->press & PDD_DGT_TR) {
            if (debug_row == ROW_KIND) {
                if (debug_kind < BIOLIZARD_ENERGY_FAST) {
                    debug_kind++;
                }
            }
        } else if (player_pads[0]->on & PDD_DGT_TR) {
            if (debug_row == ROW_INTERVAL) {
                if (speed->interval < MOST_INTERVAL) {
                    speed->interval++;
                }
            } else if (debug_row >= ROW_LAUNCH) {
                field = debug_row - ROW_LAUNCH;
                if (speed->value[field] < MOST_SPEED) {
                    speed->value[field] += SPEED_STEP;
                }
            }
        }
    }
}

Sint8 biolizard_debug_energy_kind(void)
{
    return debug_kind;
}

int biolizard_energy_remaining(struct task *volley_task)
{
    struct task_work *work;
    struct volley *order;

    if (!volley_task) {
        return -1;
    }
    work = volley_task->work;
    order = volley_task->extra.p;
    if (order->limit == BIOLIZARD_ENERGY_NO_LIMIT) {
        return BIOLIZARD_ENERGY_UNLIMITED;
    }
    return SHOTS_LEFT(work);
}

Sint8 biolizard_energy_has_shot(struct task *volley_task)
{
    if (!volley_task) {
        return -1;
    }
    if (!volley_task->child) {
        return 0;
    }
    return 1;
}

static void energy_preview_controls(struct task *task)
{
    Sint8 setting;
    Sint8 part;
    Sint8 column_offset;

    if (preview_menu_shown == FALSE) {
        if (player_pads[PREVIEW_PAD]->press & PDD_DGT_ST) {
            preview_menu_shown = TRUE;
        }
        return;
    }
    njPrint(NJM_LOCATION(BIOLIZARD_MENU_COLUMN, PREVIEW_TITLE_ROW), test_title);
    njPrint(NJM_LOCATION(BIOLIZARD_MENU_COLUMN, PREVIEW_TITLE_ROW + 1),
            parts_title);
    for (setting = 0; setting < (Sint8)TURNS; setting++) {
        njPrint(NJM_LOCATION(LABEL_COLUMN, setting + TURN_ROW), label_format,
                turn_names[setting]);
        for (part = 0; part < (Sint8)PARTS; part++) {
            column_offset = 0;
            if (preview_cursor[CURSOR_PART] == part &&
                preview_cursor[CURSOR_TURN] == setting) {
                njPrintColor(BIOLIZARD_MENU_SELECTED);
            }
            if (part == PART_THUNDER) {
                column_offset = PART_WIDTH;
            }
            njPrint(NJM_LOCATION(column_offset + BIOLIZARD_MENU_COLUMN,
                                 setting + TURN_ROW),
                    angle_format, part_turns[part][setting]);
            if (preview_cursor[CURSOR_PART] == part &&
                preview_cursor[CURSOR_TURN] == setting) {
                njPrintColor(BIOLIZARD_MENU_TEXT);
            }
        }
    }
    if (player_pads[PREVIEW_PAD]->press & PDD_DGT_KD) {
        preview_cursor[CURSOR_TURN]++;
        if (preview_cursor[CURSOR_TURN] == TURNS) {
            preview_cursor[CURSOR_TURN] = 0;
        }
    }
    if (player_pads[PREVIEW_PAD]->press & PDD_DGT_KU) {
        preview_cursor[CURSOR_TURN]--;
        if (preview_cursor[CURSOR_TURN] < 0) {
            preview_cursor[CURSOR_TURN] = TURNS - 1;
        }
    }
    if (player_pads[PREVIEW_PAD]->press & PDD_DGT_KR) {
        preview_cursor[CURSOR_PART] = PART_THUNDER;
    }
    part = preview_cursor[CURSOR_PART];
    setting = preview_cursor[CURSOR_TURN];
    if (player_pads[PREVIEW_PAD]->on & PDD_DGT_TL) {
        part_turns[part][setting] -= FINE_TURN;
    }
    if (player_pads[PREVIEW_PAD]->on & PDD_DGT_TR) {
        part_turns[part][setting] += FINE_TURN;
    }
    if (player_pads[PREVIEW_PAD]->press & PDD_DGT_TA) {
        part_turns[part][setting] = 0;
    }
    if (player_pads[PREVIEW_PAD]->press & PDD_DGT_TX) {
        part_turns[part][setting] -= COARSE_TURN;
    }
    if (player_pads[PREVIEW_PAD]->press & PDD_DGT_TB) {
        part_turns[part][setting] += COARSE_TURN;
    }
    if (player_pads[PREVIEW_PAD]->press & PDD_DGT_KL) {
        preview_cursor[CURSOR_PART] = PART_BOWL;
    }
    if (player_pads[PREVIEW_PAD]->press & PDD_DGT_ST) {
        preview_menu_shown = FALSE;
    }
}

static void energy_preview_step(struct task *task)
{
    struct task_work *work;
    struct energy_motion *motion;

    work = task->work;
    motion = task->motion.p;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.unk_09) {
        return;
    }
    preview_yaw = last_boss_target_yaw(task, &camera_views[0]->pos);
    energy_preview_controls(task);

    /* @hack spelling: motion_rate read as *&motion_rate, where motion_rate
     * would do */
    work->scl.z += *&motion_rate;
    if (work->scl.z >= ENERGY_MOTION_FRAMES) {
        work->scl.z -= ENERGY_MOTION_FRAMES;
    }
    work->ang.x += part_turns[PART_BOWL][TURN_X];
    work->ang.y += part_turns[PART_BOWL][TURN_Y];
    work->ang.z += part_turns[PART_BOWL][TURN_Z];
    motion->spin.x += part_turns[PART_THUNDER][TURN_X];
    motion->spin.y += part_turns[PART_THUNDER][TURN_Y];
    motion->spin.z += part_turns[PART_THUNDER][TURN_Z];
}

static void energy_preview_draw(struct task *task)
{
    struct task_work *work;
    struct energy_motion *motion;

    work = task->work;
    motion = task->motion.p;
    njSetTexture(&biolizard_effect_texlist);
    light_apply(BIOLIZARD_LIGHT);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njPushMatrixEx();
    njRotateY(NULL, preview_yaw + QUARTER_TURN);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCnkSimpleDrawMotion(&biolizard_energy_bowl, &biolizard_energy_bowl_motion,
                          work->scl.z);
    matrix_load_parent();
    njRotateZ(NULL, part_turns[PART_THUNDER][TILT_Z]);
    njRotateX(NULL, part_turns[PART_THUNDER][TILT_X]);
    njRotateY(NULL, part_turns[PART_THUNDER][TILT_Y]);
    njRotateY(NULL, preview_yaw);
    njRotateZ(NULL, motion->spin.z);
    njRotateX(NULL, motion->spin.x);
    njRotateY(NULL, motion->spin.y);
    njCnkSimpleDrawMotion(&biolizard_energy_thunder,
                          &biolizard_energy_thunder_motion, work->scl.z);
    njPopMatrixEx();
    njPopMatrixEx();
    light_apply(light_after_draw);
}

static void energy_preview_destroy(struct task *task)
{
}

struct task *biolizard_energy_preview(struct task *parent)
{
    struct task *task;
    struct task_work *work;
    NJS_POINT3 pos;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK | TASK_MOTION, energy_preview_step,
                                 parent);
        /* @bug memory: reads through null at 0x8c505b9a where
         * task_create_child couldn't allocate the task */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it returns null */
        if (!task) {
            return NULL;
        }
#endif
        work = task->work;
        pos = preview_origin;
        work->pos = pos;
        work->scl.z = 0.0F;
        task->draw = energy_preview_draw;
        task->destructor = energy_preview_destroy;
        return task;
    }
    /* @bug uninitialized: returns no value for a null parent, at
     * 0x8c505bc2 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */
