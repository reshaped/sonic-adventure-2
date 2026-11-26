/* King Boom Boo, the task BossBogyExec. in the dark he floats on his circle
 * about the arena's center. he throws fireballs, breathes fire and bites. a
 * ghost of his holder carries the sandglass. once the ghost has lost it and
 * its sand runs, the sunlight comes. it shrinks him to a shadow on the floor
 * and the wall. player 0 digs him out of the shadow onto the ground. there
 * each hit takes one of his health. as the sand runs low he fades. the dark
 * then comes back with a new ghost to carry the sandglass */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "boss_bigbogy/arena.h"
#include "boss_bigbogy/boss.h"
#include "boss_bigbogy/model_art.h"
#include "boss_bigbogy/motion_art.h"
#include "boss_bigbogy/camera.h"
#include "boss_bigbogy/fire.h"
#include "boss_bigbogy/glow.h"
#include "boss_bigbogy/holder.h"
#include "boss_bigbogy/hourglass.h"
#include "boss_bigbogy/parts.h"
#include "boss_bigbogy/settings.h"
#include "boss_bigbogy/stage.h"
#include "boss_bigbogy/sway.h"
#include "camera/bossklamath.h"
#include "camera/bosspoint.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "system/util.h"
#include "camera/bossinit.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "draw/motion.h"
#include "object/mover.h"
#include "player/player.h"
#include "shared/boss.h"
#include "shared/boss_title.h"
#include "shared/sandglass.h"
#include "shared/sandglass_registry.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/vector.h"
#include "vmu/vibration.h"

/* in his work's flags: he goes round his circle the other way.
 * bigbogy_home_tangent's heading then takes a half turn more */
#define BIGBOGY_REVERSED 0x100
/* in his work's flags: a swing turns ang.y toward its center plus its swing.
 * without the flag, it turns toward the center less the swing. a swing flips
 * the flag as ang.y arrives */
#define BIGBOGY_SWING_PLUS 0x200
/* in his work's flags: the draw adds FADE(mover) to the body's material
 * alpha */
#define BIGBOGY_FADING 0x400
/* in his work's flags: bigbogy_move steers him toward mover->target, not
 * along his circle */
#define BIGBOGY_AIM_AT_TARGET 0x800
/* in his work's flags: the breath has left once for the holder's dash or the
 * sunlight. it doesn't leave again until the next throw clears the flag */
#define BIGBOGY_BREATH_BROKEN 0x2000

/* the fireballs left to throw, in his boss record, a struct boss_record of
 * shared/boss.h. his health is the record's power, which starts at
 * FULL_HEALTH */
#define FIREBALLS(record) ((record)->unk_01)
#define FULL_HEALTH 4

/* fields of the boss's mover that hold his own state */
/* the heading along his circle at his pos, as bigbogy_home_tangent last gave
 * it */
#define TANGENT(mover) ((mover)->unk_7c)
/* the phase of the shadow's throb while player 0 digs into it */
#define THROB(mover) ((mover)->sway_x)
/* the most he moves in a step across the ground */
#define TOP_SPEED(mover) ((mover)->unk_c0)
/* the steps left of his taunt after he hits player 0 */
#define TAUNT_STEPS(mover) ((mover)->hold_turn)
/* his circle's radius: his distance from the center as bigbogy_create
 * measures it */
#define CIRCLE_RADIUS(mover) ((mover)->wall_bounce)
/* the frame of bigbogy_shadow_motion that the shadow draws */
#define SHADOW_FRAME(mover) ((mover)->friction)
/* what BIGBOGY_FADING adds to the body's material alpha. it falls from 0
 * toward -1 as he fades */
#define FADE(mover) ((mover)->radius)
/* how fast each axis of his scale changes as he burns at his defeat */
#define SCALE_SPEED(mover) ((mover)->last_pos)
/* his turn about x as he flips in the recoil on the ground */
#define FLIP(mover) ((mover)->sight_angle)

/* how bigbogy_move moves the boss, in bigbogy_state's movement. MOVE_SUNLIGHT
 * and MOVE_RESTORE only let him fall. the shadow and the defeat move him
 * themselves */
enum bigbogy_movement {
    MOVE_FLY, /* along his circle, or toward mover->target */
    MOVE_SUNLIGHT,
    MOVE_SHADOW,
    MOVE_GROUND,
    MOVE_RESTORE,
    MOVE_DEFEAT
};

/* the attack resume_attack goes back to, in bigbogy_state's attack. the
 * shadow keeps its route there too, as an enum shadow_path */
enum bigbogy_attack {
    ATTACK_START = 1,
    ATTACK_CIRCLE,
    ATTACK_THROW, /* or the breath, once no fireballs are left */
    ATTACK_BREATH
};

/* the shadow's route, in bigbogy_state's attack */
enum shadow_path {
    SHADOW_FLOOR = 6,
    SHADOW_WALL,
    SHADOW_TO_WALL,
    SHADOW_CLIMB,
    SHADOW_DESCEND,
    SHADOW_TO_FLOOR /* out from the wall's foot to his circle */
};

/* the bite's submodes. he lunges at player 0, falls back along his circle, then
 * brakes */
enum bite_phase { BITE_SETUP, BITE_LUNGE, BITE_FALL_BACK, BITE_BRAKE };
/* the shadow's submodes. it grows, then waits for player 0 within
 * FLEE_RANGE. it flees to mover->target, then runs on until it nears the next
 * target or player 0 draws away. PURSUIT_DUG lasts while player 0 digs into
 * it, and PURSUIT_FADE fades it */
enum pursuit_phase {
    PURSUIT_SETUP,
    PURSUIT_GROW,
    PURSUIT_WAIT,
    PURSUIT_FLEE,
    PURSUIT_RUN_ON,
    PURSUIT_DUG,
    PURSUIT_FADE
};
/* the submodes on the ground. he lands, runs to mover->target, then runs
 * about his circle. a hit there throws him into GROUND_RECOIL, and he then
 * flees about his circle. a hit counts in GROUND_RUN and GROUND_FLEE.
 * GROUND_FADE fades him */
enum ground_phase {
    GROUND_SETUP,
    GROUND_LAND,
    GROUND_TO_TARGET,
    GROUND_RUN,
    GROUND_FLEE,
    GROUND_RECOIL,
    GROUND_FADE
};

/* the entries of bigbogy_motions */
enum bigbogy_motion {
    MOTION_IDLE = 0,
    MOTION_WIND_UP = 3, /* links to MOTION_WOUND */
    MOTION_WOUND = 4,
    MOTION_THROW = 5,
    MOTION_INHALE = 6, /* links to MOTION_BREATH */
    MOTION_BREATH = 7,
    MOTION_RECOIL_HOLD = 8, /* the recoil's first frame */
    MOTION_RECOIL = 9,
    MOTION_START_RISE = 11,
    MOTION_START = 12,
    MOTION_WALK = 13,
    MOTION_RUN = 14,
    MOTION_RESTORE = 15,
    MOTION_BITE = 16,
    MOTION_DUG_OUT = 17, /* links to MOTION_WALK */
    MOTION_TO_CIRCLE = 18, /* links to MOTION_CIRCLE */
    MOTION_CIRCLE = 19,
    MOTION_TAUNT = 20,
    MOTION_BREATH_HOLD = 21,
    MOTION_FOLD = 22
};

/* bigbogy_shapes' shapes. the first ATTACK_SHAPES are the body in the air.
 * three of them follow the hands and the mouth. GROUND_SHAPE and
 * GROUND_PUSH_SHAPE are the body on the ground. a hit can land on
 * GROUND_SHAPE while its guard is 0 */
#define HAND_0_SHAPE 5
#define HAND_1_SHAPE 6
#define MOUTH_SHAPE 7
#define ATTACK_SHAPES 8
#define GROUND_SHAPE 8
#define GROUND_PUSH_SHAPE 9
#define SHADOW_SHAPE 10
#define BIGBOGY_SHAPES 13

/* the sounds of bigbogy_sounds, by when they play */
#define HURT_SOUND 0
#define DEFEAT_SOUND 1
#define RESTORE_SOUND 8
#define BITE_SOUND 10
#define INHALE_SOUND 13
#define EXHALE_SOUND 14
#define FLOAT_SOUND 16
#define SHADOW_START_SOUND 17
#define SHADOW_SOUND 18
#define RUN_SOUND 19
/* a sound keyed by his task plays at KEYED_PRIORITY, and one keyed by no
 * task at UNKEYED_PRIORITY */
#define KEYED_PRIORITY 1
#define UNKEYED_PRIORITY 0
/* each step that plays FLOAT_SOUND keeps it playing FLOAT_SOUND_STEPS + 1 steps
 * on */
#define FLOAT_SOUND_STEPS 5
/* the bite's snap lasts BITE_STEPS + 1 steps at BITE_VOLUME. as he circles,
 * it plays at CHATTER_VOLUME each CHATTER_PERIOD steps, and by
 * CHATTER_CHANCE each CHATTER_TRY_PERIOD steps */
#define BITE_STEPS 60
#define BITE_VOLUME 100
#define CHATTER_VOLUME 30
#define CHATTER_PERIOD 15
#define CHATTER_TRY_PERIOD 7
#define CHATTER_CHANCE 0.2F
/* the inhale and the exhale play BREATHING_STEPS + 1 steps at a time, at
 * BREATHING_VOLUME. the exhale then fades by EXHALE_FADE a step over
 * EXHALE_FADE_STEPS */
#define BREATHING_STEPS 5
#define BREATHING_VOLUME 20
#define EXHALE_FADE 2
#define EXHALE_FADE_STEPS 30
#define EXHALE_VOLUME(steps_left)                                              \
    (((steps_left) * EXHALE_FADE) -                                            \
     ((EXHALE_FADE_STEPS * EXHALE_FADE) - BREATHING_VOLUME))
#define RESTORE_SOUND_STEPS 180
#define RESTORE_VOLUME 120
#define SHADOW_START_VOLUME 30
#define RUN_VOLUME 10
#define HURT_VOLUME 120
/* at his defeat DEFEAT_SOUND plays each DEFEAT_SOUND_PERIOD steps, at a
 * random DEFEAT_VOLUME to DEFEAT_VOLUME + DEFEAT_VOLUME_SPREAD. as he burns
 * it plays each BURN_SOUND_PERIOD steps, at a random 0 to
 * BURN_VOLUME_SPREAD */
#define DEFEAT_SOUND_PERIOD 35
#define DEFEAT_VOLUME 50
#define DEFEAT_VOLUME_SPREAD 60.0F
#define BURN_SOUND_PERIOD 20
#define BURN_VOLUME_SPREAD 100.0F

/* the pad's rumbles: their power, frequency and time, as vibration_start
 * takes them. the breath's fades over BREATH_LENGTH, as
 * vibration_start_fading takes it. at his defeat the pad rumbles each
 * DEFEAT_RUMBLE_PERIOD steps */
#define BREATH_POWER 2
#define BREATH_FREQ 10
#define BREATH_LENGTH 5
#define BITE_POWER 3
#define BITE_FREQ 5
#define DIG_POWER 1
#define DIG_FREQ 6
#define DIG_TIME 1
#define HIT_POWER 2
#define HIT_FREQ 30
#define HIT_TIME 1
#define DEFEAT_POWER 2
#define DEFEAT_FREQ 15
#define DEFEAT_TIME 1
#define DEFEAT_RUMBLE_PERIOD 25
#define BURN_POWER 4
#define BURN_FREQ 35
#define BURN_TIME 3

/* the voices, by when they play */
#define THROW_VOICE 0x880
#define SUNLIGHT_VOICE 0x87f
#define RESTORE_VOICE 0x882
#define START_VOICE 0x883
#define GROUNDED_VOICE 0x884
#define HURT_VOICE 0x885
#define DEFEAT_VOICE 0x886
#define SUNLIGHT_VOICE_2 0x889
#define HIT_PLAYER_VOICE 0x88a
#define HIT_PLAYER_VOICE_2 0x88b
#define INHALE_VOICE 0x88c
#define BREATH_VOICE 0x88d
#define INTRO_VOICE 0x88e

/* the land blocks STAGE_LAND_MASK shows in the dark and in the sunlight */
#define DARK_LAND 0x2
#define SUNLIT_LAND 0x4
/* LIGHT_LAND's ambient in the dark and in the sunlight, and its change in a
 * step between them */
#define DARK_AMBIENT 0.6F
#define SUNLIT_AMBIENT 1.0F
#define AMBIENT_STEP 0.02F

#define BIGBOGY_LEVEL 2 /* task_create's level */
#define COLLISION_RADIUS 400.0F
/* the gravity he starts with, which the start's tuning replaces */
#define INTRO_GRAVITY (-0.02F)
/* player 0 starts on his circle this far ahead of him, the way he goes */
#define PLAYER_LEAD 0x5000
/* the messages the messenger at the intro's end may show */
#define MESSENGER_MESSAGES 3

/* the cameras look FOCUS_HEIGHT above his pos as he flies, and
 * INTRO_FOCUS_HEIGHT at the intro's start. on the ground they look
 * LOW_FOCUS_HEIGHT above him, and at his defeat DEFEAT_FOCUS_HEIGHT */
#define FOCUS_HEIGHT 40.0F
#define INTRO_FOCUS_HEIGHT 60.0F
#define LOW_FOCUS_HEIGHT 7.0F
#define DEFEAT_FOCUS_HEIGHT 12.0F
/* GHOST_FOCUS_RANGE is a distance in x and z. in the breath the cameras look at
 * the floor while the ghost lies within it of player 0. they look
 * LOW_FOCUS_HEIGHT above the floor, and SHADOW_FOCUS_DEPTH below it at the
 * shadow on it */
#define GHOST_FOCUS_RANGE 300.0F
#define SHADOW_FOCUS_DEPTH 10.0F
/* how far below the mouth the mouth's shape lies */
#define MOUTH_SHAPE_DROP 10.0F

/* he falls no further than FLY_DEPTH below the floor, or than INTRO_FLOOR
 * above it in the intro */
#define FLY_DEPTH 4.0F
#define INTRO_FLOOR 20.0F
/* match_player_height holds him no higher than FLY_CEILING above the floor,
 * or TAUNT_CEILING in the taunt */
#define FLY_CEILING 40.0F
#define TAUNT_CEILING 20.0F
/* where player 0 stands more than PLAYER_CLEARANCE above the floor,
 * match_player_height sets home.y PLAYER_CLEARANCE below player 0. he then
 * climbs CLIMB_STEP a step toward home.y */
#define PLAYER_CLEARANCE 100.0F
#define CLIMB_STEP 2.0F
#define HOVER_SLACK 4.0F
/* the lift and its random part as he idles and in the intro */
#define IDLE_LIFT 0.1F
#define IDLE_LIFT_VARIATION 0.02F

/* what a step keeps of a speed or an acceleration that drags, and of a speed
 * that brakes. below STOP_SPEED in an axis, a braking speed stops */
#define DRAG 0.96F
#define BRAKE 0.5F
#define STOP_SPEED 0.1F

/* each step, sway_y moves by SWAY_Y_STEP. sway_x moves by SWAY_X_STEP plus a
 * random 0 to SWAY_X_SPREAD, and sway_z alike */
#define SWAY_X_STEP 256.0F
#define SWAY_X_SPREAD 256.0F
#define SWAY_Y_STEP 0x200
#define SWAY_Z_STEP 512.0F
#define SWAY_Z_SPREAD 336.0F
/* the draw turns the body about a point PIVOT_HEIGHT above his pos. the bob
 * moves it up to BOB_HEIGHT up or down. the sways turn it up to
 * SWAY_X_DEGREES and SWAY_Z_DEGREES */
#define PIVOT_HEIGHT 55.0F
#define BOB_HEIGHT 0.9F
#define SWAY_X_DEGREES 3.0F
#define SWAY_Z_DEGREES 5.5F
/* how far a swing raises ang.z in a step */
#define ROLL_STEP 0x100
/* how far the breath eases each sway's phase toward 0 in a step */
#define SWAY_EASE_STEP 0x100

/* how far he turns his facing in a step as he flies. face_player turns it
 * toward player 0 no farther than FACE_LIMIT from TANGENT(mover) */
#define TURN_STEP 0x100
#define FACE_LIMIT 0x3000
/* his turn in a step as he watches player 0 in the dash, the sunlight and the
 * defeat */
#define WATCH_STEP 0x200
/* player 0 within REACH in x and z is within his reach. it must also lie
 * within a quarter turn of his facing */
#define REACH 200.0F
/* he flies at TOP_SPEED while player 0 lies within FULL_SPEED_TURN of his
 * facing. otherwise he flies at TURNED_SPEED of it */
#define FULL_SPEED_TURN 0x2aaa
#define TURNED_SPEED 0.5F

/* where player 0 touches him outside the inner wall, player 0 takes a launch
 * at LAUNCH_SPEED. it locks the stick for LAUNCH_LOCK steps. below
 * LAUNCH_LOW_Y, the launch rises at LAUNCH_RISE too */
#define LAUNCH_SPEED 3.0F
#define LAUNCH_RISE 2.0F
#define LAUNCH_LOW_Y (-25.0F)
#define LAUNCH_LOCK 60

/* the radius of the arena's inner wall, which the shadow climbs */
#define WALL_RADIUS 130.0F
/* the shadow flees once player 0 comes within FLEE_RANGE. on the wall it can
 * stay while player 0 stands within STAY_RANGE in x and z */
#define FLEE_RANGE 70.0F
#define STAY_RANGE 20.0F
/* the shadow grows by SHADOW_GROWTH a step to SHADOW_SCALE. as player 0
 * digs into it, it throbs by up to THROB_SIZE, THROB_STEP a step */
#define SHADOW_SCALE 1.5F
#define SHADOW_GROWTH 0.025F
#define THROB_SIZE 0.2F
#define THROB_STEP 0x1800
/* SHADOW_FRAME runs through SHADOW_FRAMES, by SHADOW_FRAME_STEP a step, or
 * DIG_FRAME_STEP as player 0 digs */
#define SHADOW_FRAMES 15.0F
#define SHADOW_FRAME_STEP 0.26F
#define DIG_FRAME_STEP 1.5F
/* he starts to fade once the sand has fewer than FADE_STEPS steps left. the
 * dark comes back by GONE_STEPS at the latest */
#define FADE_STEPS 120.0F
#define GONE_STEPS 60.0F
#define BURN_SCALE_HIGH 0.45F
#define BURN_SCALE_LOW 0.2F
#define BURN_SPEED 0.025F
#define BURN_SPREAD 0.0125F

#define BIGBOGY_WALL_POINTS 8

/* the boss's state, in his boss record's own block */
struct bigbogy_state {
    Sint8 daylight; /* set_daylight's last enum daylight */
    Sint8 movement; /* an enum bigbogy_movement */
    Sint8 attack; /* an enum bigbogy_attack, or the shadow's enum shadow_path */
    Uint8 unused_03;
    Sint8 prior_submode; /* the submode the last hit on the ground broke */
    Uint8 unused_05[3];
    struct task *holder;
    struct task *glass;
    struct task *glow;
    struct task *flame; /* the fireball in hand or the breath, or null */
    struct task *sway;
    struct task *camera; /* a cut's camera task, or null */
    struct task *parts[2];
    Uint8 unused_28[4];
    Float ambient; /* LIGHT_LAND's ambient */
    /* points evenly about the center, from bigbogy_wall_points. the sunlight's
     * cut first looks at the one nearest player 0. the dark's looks at the one
     * nearest a point ahead of him */
    NJS_POINT3 wall_points[BIGBOGY_WALL_POINTS];
};

/* player 0 as the boss sees it */
struct bigbogy_player {
    /* the size of the short turn from ang.y to the heading toward player 0 */
    Angle turn;
    Float distance;
    Float horizontal; /* the distance in x and z */
    NJS_VECTOR offset; /* player 0's pos less the boss's */
};

/* where the shadow flees player 0 */
enum shadow_evasion {
    EVADE_FLOOR, /* it stays on the floor */
    EVADE_WALL, /* once on the wall, it stays there */
    EVADE_ACROSS /* it crosses between the floor and the wall */
};

/* the boss's settings at each health, bigbogy_levels[power - 1] for his
 * record's power */
struct bigbogy_level {
    /* how long he circles before he throws. at 0 he throws at once */
    Sint8 circle_seconds;
    Sint8 fireballs; /* how many he throws */
    Sint8 breath_seconds;
    Sint8 evasion; /* an enum shadow_evasion */
    Float speed_scale; /* the shadow's TOP_SPEED, in player 0's top speeds */
    Float wall_speed_scale; /* its TOP_SPEED on the wall, in WALL_SPEEDs */
};
#define WALL_SPEED 0.8062F

static struct bigbogy_level bigbogy_levels[FULL_HEALTH];
/* the inverse of the matrix the draw starts from, which takes the view to
 * the world */
static NJS_MATRIX bigbogy_view_to_world;
/* the nodes of the body, in the world, as the last draw placed them */
static NJS_POINT3 bigbogy_hand_0, bigbogy_hand_1;
static NJS_POINT3 bigbogy_root_point;
static NJS_POINT3 bigbogy_part_point_0, bigbogy_part_point_1;
static NJS_POINT3 bigbogy_mouth_point;

static void bigbogy_step(struct task *boss);
static void bigbogy_draw(struct task *boss);

static const char boss_name[] = "BossBogyExec";
/* the point bigbogy_wall_points takes the first wall point's bearing and
 * distance from */
static const NJS_POINT3 first_wall_point = {0.0F, 0.0F, 0.0F};
static const struct angles launch_angles = {0, 0, 0};

/* player 0 nearer than this in x and z draws a bite */
static Float bite_distance = 100.0F;
static struct task *active_boss = NULL;
static Sint8 initial_health = FULL_HEALTH;

#ifdef NONMATCHING
#else
#pragma inline(bigbogy_task)
#endif
static void boss_destroy(struct task *boss);

static void bigbogy_player_relative(struct task *boss,
                                    struct bigbogy_player *out)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    /* @hack redundant: state, the own block read on entry, which nothing
     * uses */
    struct bigbogy_state *state = record->own;
    struct bigbogy_player player;

    player.offset.x = player_work[0]->pos.x - work->pos.x;
    player.offset.y = player_work[0]->pos.y - work->pos.y;
    player.offset.z = player_work[0]->pos.z - work->pos.z;
    {
        /* @hack spelling: x, a copy of offset.x for the distance, where
         * offset.x would do */
        Float x = player.offset.x;
        player.distance = njSqrt((x * x) + (player.offset.y * player.offset.y) +
                                 (player.offset.z * player.offset.z));
    }
    player.horizontal = njSqrt((player.offset.x * player.offset.x) +
                               (player.offset.z * player.offset.z));
    player.turn = mover_player_heading(boss, 0);
    player.turn = angle_distance(player.turn, work->ang.y);
    *out = player;
    (void)state;
}

void bigbogy_player_distance(struct task *boss, Float *distance, Angle *turn)
{
    struct boss_record *record = boss->extra.p;
    /* @hack redundant: state, the own block read on entry, which nothing
     * uses */
    struct bigbogy_state *state = record->own;
    struct bigbogy_player player;

    bigbogy_player_relative(boss, &player);
    if (distance) {
        *distance = player.horizontal;
    }
    if (turn) {
        *turn = player.turn;
    }
    (void)state;
}

Angle bigbogy_home_tangent(struct task *boss)
{
    struct boss_record *record = boss->extra.p;
    /* @hack redundant: state, the own block read on entry, which nothing
     * uses */
    struct bigbogy_state *state = record->own;
    struct task_work *work = boss->work;
    struct mover *mover = boss->motion.p;
    /* @hack redundant: unused, a vector that only sizes the frame */
    NJS_VECTOR unused;
    Angle angle;

    angle = (Angle)(atan2f(work->pos.x - mover->home.x,
                           work->pos.z - mover->home.z) *
                    (Float)ANGLE_PER_RADIAN) +
            QUARTER_TURN;
    angle &= ANGLE_MASK;
    if (work->flags & BIGBOGY_REVERSED) {
        angle += HALF_TURN;
    }
    (void)state;
    (void)unused;
    return (Uint16)angle;
}

/* the mode that resumes attack. it is -1 for an attack it doesn't know, and
 * for ATTACK_THROW while bigbogy_task() is null */
static int resume_mode(Sint8 attack)
{
    struct task *boss;
    struct boss_record *record;
    int mode;

    switch (attack) {
    case ATTACK_START:
        mode = BIGBOGY_START;
        break;
    case ATTACK_CIRCLE:
        mode = BIGBOGY_CIRCLE;
        break;
    case ATTACK_THROW:
        boss = bigbogy_task();
        if (!boss) {
            return -1;
        }
        record = boss->extra.p;
        if (FIREBALLS(record) > 0) {
            mode = BIGBOGY_THROW;
        } else {
            mode = BIGBOGY_BREATH;
        }
        break;
    case ATTACK_BREATH:
        mode = BIGBOGY_BREATH;
        break;
    default:
        return -1;
    }
    return mode;
}

Sint8 bigbogy_reversed(void)
{
    struct task *boss = bigbogy_task();
    struct task_work *work;
    if (!boss) {
        return -1;
    }
    work = boss->work;
    if (work->flags & BIGBOGY_REVERSED) {
        return 1;
    }
    return 0;
}

struct task *bigbogy_task(void)
{
    /* @hack spelling: a test of active_boss, where returning it would do */
    if (active_boss) {
        return active_boss;
    }
    return NULL;
}

int bigbogy_mode(void)
{
    struct task *boss = bigbogy_task();
    if (!boss) {
        return -1;
    }
    return boss->work->mode;
}

Float bigbogy_horizontal_distance(const NJS_POINT3 *from, const NJS_POINT3 *to,
                                  NJS_VECTOR *offset)
{
    NJS_VECTOR delta;
    Float distance;
    vector_diff(to, from, &delta);
    distance = njSqrt((delta.x * delta.x) + (delta.z * delta.z));
    if (offset) {
        *offset = delta;
    }
    return distance;
}

int bigbogy_health(void)
{
    struct task *boss = bigbogy_task();
    struct boss_record *record;

    if (!boss) {
        return -1;
    }
    record = boss->extra.p;
    return record->power;
}

void bigbogy_reverse(struct task *boss)
{
    struct task_work *work = boss->work;
    if (work->flags & BIGBOGY_REVERSED) {
        work->flags &= ~BIGBOGY_REVERSED;
    } else {
        work->flags |= BIGBOGY_REVERSED;
    }
}

/* the land set_daylight shows */
enum daylight { DAYLIGHT_DARK, DAYLIGHT_SUNLIT };

/* show the land of daylight, an enum daylight. any other daylight changes
 * nothing */
static void set_daylight(struct task *boss, Sint8 daylight)
{
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;
    switch (daylight) {
    case DAYLIGHT_DARK:
        STAGE_LAND_MASK(0) |= DARK_LAND;
        STAGE_LAND_MASK(0) &= ~SUNLIT_LAND;
        break;
    case DAYLIGHT_SUNLIT:
        STAGE_LAND_MASK(0) |= SUNLIT_LAND;
        STAGE_LAND_MASK(0) &= ~DARK_LAND;
        break;
    default:
        return;
    }
    state->daylight = daylight;
}

static void focus_above(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    NJS_VECTOR offset;
    offset.x = 0.0F;
    offset.y = FOCUS_HEIGHT;
    offset.z = 0.0F;
    boss_record_set_focus(record, &work->pos, &offset);
}

/* his glow's alpha runs from GLOW_LOW to GLOW_HIGH and back each
 * GLOW_PERIOD steps, and ends at GLOW_LOW */
#define GLOW_LOW 50
#define GLOW_HIGH 255
#define GLOW_PERIOD 240
#ifdef NONMATCHING
/* this side returns whether it made the boss, as boss.h says */
Bool bigbogy_create(NJS_POINT3 *position, Angle heading,
                    const NJS_POINT3 *center)
#else
void bigbogy_create(NJS_POINT3 *position, Angle heading,
                    const NJS_POINT3 *center)
#endif
{
    struct task *boss;
    struct bigbogy_state *state;
    struct task_work *work;
    struct boss_record *record;
    struct mover *mover;

    boss = task_create(TASK_WORK, BIGBOGY_LEVEL, bigbogy_step, boss_name);
#ifdef NONMATCHING
    /* this side returns FALSE where the task or the state fails. the matching
     * side writes record->own through a null record, which
     * boss_record_alloc_own refuses. this side frees the record's parts before
     * the kill */
    if (!boss) {
        return FALSE;
    }
    work = boss->work;
    record = boss_record_create(boss);
    state = boss_record_alloc_own(record, sizeof(*state));
    if (!state) {
        boss_record_free_parts(boss);
        task_kill(boss);
        return FALSE;
    }
#else
    if (!boss) {
        return;
    }
    work = boss->work;
    record = boss_record_create(boss);
    state = syCalloc(1, sizeof(*state));
    if (!state) {
        task_kill(boss);
        return;
    }
    record->own = state;
#endif
    motion_init(record->block, bigbogy_motions);
    mover = boss->motion.p;
    active_boss = boss;
    mover->home = *center;
    mover->floor_y = center->y;
    work->pos = *position;
    work->ang.y = heading;
    work->scl.x = work->scl.y = work->scl.z = 1.0F;
    {
        NJS_VECTOR offset;
        offset.x = work->pos.x - mover->home.x;
        offset.y = 0.0F;
        /* @bug logic: CIRCLE_RADIUS takes pos.x for its z offset, where
         * pos.z was meant: the subtraction at 0x8c500870 takes it. the radius
         * comes out right as the stage spawns him, at a pos.x and a pos.z
         * both 0 */
        offset.z = work->pos.x - mover->home.z;
        CIRCLE_RADIUS(mover) =
            njSqrt((offset.x * offset.x) + (offset.z * offset.z));
    }
    mover_set_gravity(boss, INTRO_GRAVITY);
    work->mode = BIGBOGY_INTRO;
    state->movement = MOVE_FLY;
    state->attack = ATTACK_CIRCLE;
    record->power = initial_health;
    state->holder = bigbogy_holder_create(boss);
    bigbogy_holder_spawn_ghost(state->holder);
    state->glass = bigbogy_sandglass_create(boss, state->holder);
    {
        NJS_POINT3 spawn;
        bigbogy_arena_advance(boss, &work->pos, -PLAYER_LEAD, &spawn);
        player_set_pos(0, spawn.x, spawn.y, spawn.z);
    }
    collision_init(boss, bigbogy_shapes, BIGBOGY_SHAPES, COLLISION_ENEMY);
    boss->work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(boss, COLLISION_RADIUS);
    boss_title_create(boss, &bigbogy_title);
    {
        struct bigbogy_glow glow;
        glow.minimum = GLOW_LOW;
        glow.maximum = GLOW_HIGH;
        glow.restored = GLOW_LOW;
        glow.period = GLOW_PERIOD;
        glow.object = &bigbogy_glow_object;
        state->glow = bigbogy_glow_create(boss, &glow);
    }
    {
        struct bigbogy_part part;
        /* @hack spelling: object, a local each part's object passes through,
         * where the address itself would do */
        NJS_CNK_OBJECT *object;

        object = &bigbogy_part_object_0;
        part.side = 0;
        part.object = object;
        part.boss = work;
        part.boss_submode = &work->submode;
        part.position = &bigbogy_part_point_0;
        part.boss_heading = &work->ang.y;
        state->parts[1] = bigbogy_part_create(boss, &part);
        object = &bigbogy_part_object_1;
        part.side = 1;
        part.object = object;
        part.boss = work;
        part.boss_submode = &work->submode;
        part.position = &bigbogy_part_point_1;
        part.boss_heading = &work->ang.y;
        state->parts[0] = bigbogy_part_create(boss, &part);
    }
    state->sway = bigbogy_sway_create(boss, &bigbogy_sway_model);
    state->ambient = DARK_AMBIENT;
    light_set_ambient(LIGHT_LAND, state->ambient);
    set_daylight(boss, DAYLIGHT_DARK);
    {
        /* @hack spelling: position, a copy of first_wall_point, where its
         * address would do */
        NJS_POINT3 position;
        position = first_wall_point;
        bigbogy_wall_points(&position, BIGBOGY_WALL_POINTS, state->wall_points);
    }
    {
        /* @hack spelling: motion, a local for record->block, where
         * record->block would do */
        struct motion_control *motion = record->block;
        motion_set(motion, MOTION_IDLE);
    }
    pad_disable_player(0);
    boss->draw = bigbogy_draw;
    boss->destructor = boss_destroy;
#ifdef NONMATCHING
    /* this side returns that it made the boss */
    return TRUE;
#endif
}

static void boss_destroy(struct task *boss)
{
    voices_stop();
    boss_record_free_parts(boss);
    active_boss = NULL;
}

/* below home.y, acc.y gains lift and a random 0 to lift_variation. spd.y
 * stops more than HOVER_SLACK below home.y, or at the floor where home.y
 * lies below it. acc.y then drags */
static void hover(struct task *boss, Float lift_variation, Float lift)
{
    struct boss_record *record = boss->extra.p;
    /* @hack redundant: state, the own block read on entry, which nothing
     * uses */
    struct bigbogy_state *state = record->own;
    struct mover *mover = boss->motion.p;
    struct task_work *work = boss->work;

    if (work->pos.y < mover->home.y) {
        mover->acc.y += lift + (rand() * lift_variation * RAND_SCALE);
    }
    if (mover->home.y >= mover->floor_y) {
        if (work->pos.y < mover->home.y - HOVER_SLACK) {
            mover->spd.y = 0.0F;
        }
    } else if (work->pos.y <= mover->floor_y) {
        mover->spd.y = 0.0F;
    }
    mover->acc.y *= DRAG;
    (void)state;
}

/* swing ang.y by step about TANGENT(mover), as BIGBOGY_SWING_PLUS says.
 * while ang.z lies below roll, it gains ROLL_STEP. swing_about_aim swings
 * about aim_y */
static void swing_about_tangent(struct task *boss, Angle roll, Angle swing,
                                Angle step)
{
    struct boss_record *record = boss->extra.p;
    /* @hack redundant: state, the own block read on entry, which nothing
     * uses */
    struct bigbogy_state *state = record->own;
    struct task_work *work = boss->work;
    struct mover *mover = boss->motion.p;
    Angle heading;

    if (work->ang.z < roll) {
        work->ang.z += ROLL_STEP;
    }
    if (work->flags & BIGBOGY_SWING_PLUS) {
        heading = TANGENT(mover) + swing;
    } else {
        heading = -swing + TANGENT(mover);
    }
    heading = (Uint16)heading;
    work->ang.y = angle_approach(work->ang.y, heading, step);
    work->ang.y &= ANGLE_MASK;
    if (work->ang.y == heading) {
        if (work->flags & BIGBOGY_SWING_PLUS) {
            work->flags &= ~BIGBOGY_SWING_PLUS;
        } else {
            work->flags |= BIGBOGY_SWING_PLUS;
        }
    }
    (void)state;
}

static void swing_about_aim(struct task *boss, Angle roll, Angle swing,
                            Angle step)
{
    struct boss_record *record = boss->extra.p;
    /* @hack redundant: state, the own block read on entry, which nothing
     * uses */
    struct bigbogy_state *state = record->own;
    struct task_work *work = boss->work;
    struct mover *mover = boss->motion.p;
    Angle heading;

    if (work->ang.z < roll) {
        work->ang.z += ROLL_STEP;
    }
    if (work->flags & BIGBOGY_SWING_PLUS) {
        heading = mover->aim_y + swing;
    } else {
        heading = -swing + mover->aim_y;
    }
    heading = (Uint16)heading;
    work->ang.y = angle_approach(work->ang.y, heading, step);
    work->ang.y &= ANGLE_MASK;
    if (work->ang.y == heading) {
        if (work->flags & BIGBOGY_SWING_PLUS) {
            work->flags &= ~BIGBOGY_SWING_PLUS;
        } else {
            work->flags |= BIGBOGY_SWING_PLUS;
        }
    }
    (void)state;
}

static void advance_sway(struct task *boss)
{
    struct mover *mover = boss->motion.p;
    mover->sway_x += (rand() * (SWAY_X_SPREAD * RAND_SCALE)) + SWAY_X_STEP;
    mover->sway_y += SWAY_Y_STEP;
    mover->sway_z += (rand() * (SWAY_Z_SPREAD * RAND_SCALE)) + SWAY_Z_STEP;
}

static void ease_sway(struct task *boss, Angle step)
{
    struct mover *mover = boss->motion.p;
    mover->sway_x = angle_approach(mover->sway_x, 0, step);
    mover->sway_y = angle_approach(mover->sway_y, 0, step);
    mover->sway_z = angle_approach(mover->sway_z, 0, step);
}

static void clear_sway(struct task *boss)
{
    struct mover *mover = boss->motion.p;
    mover->sway_x = 0;
    mover->sway_y = 0;
    mover->sway_z = 0;
}

void bigbogy_accelerate(struct task *boss, Float acceleration)
{
    struct boss_record *record = boss->extra.p;
    /* @hack redundant: state, the own block read on entry, which nothing
     * uses */
    struct bigbogy_state *state = record->own;
    struct mover *mover = boss->motion.p;
    mover->acc.x = acceleration;
    mover->acc.z = 0.0F;
    mover->spd.x *= DRAG;
    mover->spd.z *= DRAG;
    (void)state;
}

/* turn ang.y by step toward player 0, and return the turn left. a limit
 * other than 0 holds the aim within limit of TANGENT(mover). face_tangent
 * turns toward TANGENT(mover) itself */
static Angle face_player(struct task *boss, Angle step, Angle limit)
{
    struct task_work *work = boss->work;
    struct mover *mover = boss->motion.p;
    Angle player;
    Angle target;
    Angle side;
    /* @hack spelling: remaining, and an else after the return, where the
     * call's result returned at once would do */
    Angle remaining;

    if (!limit) {
        remaining = mover_turn_to_player_by(boss, step, 0);
        return remaining;
    } else {
        player = mover_player_heading(boss, 0);
        target = angle_distance(TANGENT(mover), player);
        side = angle_difference(TANGENT(mover), player);
        if (target > limit) {
            target = limit;
        }
        if (side > 0) {
            target = TANGENT(mover) + target;
        } else {
            target = TANGENT(mover) - target;
        }
        work->ang.y = angle_approach(work->ang.y, target, step);
        return angle_distance(work->ang.y, target);
    }
}

static Angle face_tangent(struct task *boss, Angle step)
{
    struct mover *mover = boss->motion.p;
    struct task_work *work = boss->work;
    work->ang.y = angle_approach(work->ang.y, TANGENT(mover), step);
    return angle_distance(work->ang.y, TANGENT(mover));
}

static void lower_hover_height(struct task *boss, Float step)
{
    struct boss_record *record = boss->extra.p;
    /* @hack redundant: state, the own block read on entry, which nothing
     * uses */
    struct bigbogy_state *state = record->own;
    struct mover *mover = boss->motion.p;
    if (mover->home.y > mover->floor_y) {
        mover->home.y -= step;
        if (mover->home.y < mover->floor_y) {
            mover->home.y = mover->floor_y;
        }
    }
    (void)state;
}

static void match_player_height(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    /* @hack redundant: state, the own block read on entry, which nothing
     * uses */
    struct bigbogy_state *state = record->own;
    struct mover *mover = boss->motion.p;
    NJS_POINT3 player;

    player = player_work[0]->pos;
    if (player.y > mover->floor_y + PLAYER_CLEARANCE) {
        mover->home.y = player.y + -PLAYER_CLEARANCE;
        if (work->pos.y < mover->home.y) {
            work->pos.y += CLIMB_STEP;
        }
    } else {
        mover->home.y = mover->floor_y;
        /* @bug logic: he never sinks here. the test at 0x8c500f0e wants
         * home.y PLAYER_CLEARANCE above floor_y, where it was just set to
         * floor_y */
        if (mover->home.y > mover->floor_y + PLAYER_CLEARANCE) {
            work->pos.y -= CLIMB_STEP;
        }
    }
    if (work->mode != BIGBOGY_PLAYER_HIT) {
        if (work->pos.y >= mover->floor_y + FLY_CEILING) {
            work->pos.y = mover->floor_y + FLY_CEILING;
        }
    } else if (work->pos.y >= mover->floor_y + TAUNT_CEILING) {
        work->pos.y = mover->floor_y + TAUNT_CEILING;
    }
    (void)state;
}

/* aim spd's x and z at the point of his circle ahead of him. the speed is
 * his speed in x and z plus acc.x, held to limit. steer_along_tangent aims
 * along bigbogy_home_tangent's heading, and holds the speed to TOP_SPEED */
static void steer_along_circle(struct task *boss, Float limit)
{
    struct mover *mover = boss->motion.p;
    NJS_VECTOR *velocity = &mover->spd;
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    /* @hack redundant: state, the own block read on entry, which nothing
     * uses */
    struct bigbogy_state *state = record->own;
    NJS_VECTOR next;
    Float speed;
    Angle heading;

    next = *velocity;
    next.y = 0.0F;
    speed = njScalor(&next) + mover->acc.x;
    if (speed > limit) {
        speed = limit;
    }
    bigbogy_arena_project(boss, &work->pos, &next);
    next.x += speed * njSin(TANGENT(mover));
    next.z += speed * njCos(TANGENT(mover));
    heading = atan2f(next.x - work->pos.x, next.z - work->pos.z) *
              (Float)ANGLE_PER_RADIAN;
    velocity->x = njSin(heading) * speed;
    velocity->z = njCos(heading) * speed;
    (void)state;
}

static void steer_along_tangent(struct task *boss)
{
    struct task_work *work = boss->work;
    struct mover *mover = boss->motion.p;
    NJS_VECTOR *velocity = &mover->spd;
    struct boss_record *record = boss->extra.p;
    /* @hack redundant: state, the own block read on entry, which nothing
     * uses */
    struct bigbogy_state *state = record->own;
    NJS_VECTOR next;
    Float speed;
    Angle heading;

    next = *velocity;
    next.y = 0.0F;
    speed = njScalor(&next) + mover->acc.x;
    if (speed > TOP_SPEED(mover)) {
        speed = TOP_SPEED(mover);
    }
    /* @hack redundant: next projected onto his circle and moved ahead, which
     * nothing reads */
    bigbogy_arena_project(boss, &work->pos, &next);
    next.x += speed * njSin(TANGENT(mover));
    next.z += speed * njCos(TANGENT(mover));
    heading = bigbogy_home_tangent(boss);
    velocity->x = njSin(heading) * speed;
    velocity->z = njCos(heading) * speed;
    (void)state;
}

static Bool player_within_reach(struct task *boss, Float *distance)
{
    struct bigbogy_player player;
    bigbogy_player_relative(boss, &player);
    if (distance) {
        *distance = player.horizontal;
    }
    if (player.horizontal < REACH && player.turn < QUARTER_TURN) {
        return TRUE;
    }
    return FALSE;
}

#define SQUARE(value) ((value) * (value))
#define HORIZONTAL_DISTANCE(first, second)                                     \
    njSqrt(SQUARE((first).x - (second).x) + SQUARE((first).z - (second).z))
/* the snap of his bite, keyed by his task */
#define PLAY_BITE(boss)                                                        \
    do {                                                                       \
        sound_play_for(BITE_SOUND, (boss), KEYED_PRIORITY, 0, BITE_STEPS);     \
        sound_set_volume((boss), BITE_SOUND, BITE_VOLUME);                     \
    } while (0)

static void bigbogy_move(struct task *boss)
{
    struct bigbogy_state *state;
    struct boss_record *record;
    struct task_work *work;
    struct mover *mover;

    record = boss->extra.p;
    state = record->own;
    work = boss->work;
    mover = boss->motion.p;
    switch (state->movement) {
    case MOVE_FLY: {
        NJS_VECTOR *acceleration;
        NJS_VECTOR *velocity;
        NJS_POINT3 *position;
        NJS_VECTOR offset;
        Float speed;
        Float limit;
        Sint8 have_target;
        Sint8 shape;

        TANGENT(mover) = bigbogy_home_tangent(boss);
        acceleration = &mover->acc;
        velocity = &mover->spd;
        position = &work->pos;
        acceleration->y += mover->gravity;
        velocity->y += acceleration->y;
        position->y += velocity->y;
        offset = *velocity;
        offset.y = 0.0F;
        speed = njScalor(&offset) + mover->acc.x;
        {
            struct bigbogy_player player;
            bigbogy_player_relative(boss, &player);
            if (player.turn < FULL_SPEED_TURN) {
                limit = TOP_SPEED(mover);
            } else {
                limit = TOP_SPEED(mover) * TURNED_SPEED;
            }
        }
        if (speed > limit) {
            speed = limit;
        }
        if (!(work->flags & BIGBOGY_AIM_AT_TARGET)) {
            steer_along_circle(boss, limit);
        } else {
            Angle heading;
            heading = atan2f(mover->target.x - work->pos.x,
                             mover->target.z - work->pos.z) *
                      (Float)ANGLE_PER_RADIAN;
            bigbogy_set_speed(velocity, heading, speed);
        }
        position->x += velocity->x;
        position->z += velocity->z;
        acceleration->x = 0.0F;
        acceleration->y = 0.0F;
        acceleration->z = 0.0F;
        if (work->mode != BIGBOGY_INTRO) {
            if (mover->floor_y - FLY_DEPTH > work->pos.y) {
                if (mover->spd.y < 0.0F) {
                    mover->spd.y = 0.0F;
                    work->pos.y = mover->floor_y - FLY_DEPTH;
                }
            }
        } else {
            if (mover->floor_y + INTRO_FLOOR > work->pos.y) {
                if (mover->spd.y < 0.0F) {
                    mover->spd.y = 0.0F;
                    work->pos.y = mover->floor_y + INTRO_FLOOR;
                }
            }
        }
        if (!bigbogy_sandglass_carry(state->glass, state->holder)) {
            /* @hack spelling: the test shape <= ATTACK_SHAPES - 1 and the
             * step shape = shape + 1, where shape < ATTACK_SHAPES and shape++
             * would do */
            for (shape = 0; shape <= ATTACK_SHAPES - 1; shape = shape + 1) {
                collision_set_attack(boss, shape, 0);
            }
            bigbogy_change_mode(boss, BIGBOGY_SUNLIGHT);
        }
        if (collision_next_player(boss)) {
            struct angles turn;
            /* @hack type: heading and difference held as Floats, where Angles
             * would do */
            Float heading;
            Float difference;

            turn = launch_angles;
            /* @bug logic: player 0 touching him within WALL_RADIUS of the
             * center takes no launch. this branch sets offset for one, but
             * only the other launches, at 0x8c50146a */
            if (bigbogy_arena_distance(&player_work[0]->pos, &offset) <
                WALL_RADIUS) {
                if (njScalor(&offset) > 0.0F) {
                    njUnitVector(&offset);
                }
                offset.x *= -LAUNCH_SPEED;
                if (player_work[0]->pos.y < LAUNCH_LOW_Y) {
                    offset.y = LAUNCH_RISE;
                } else {
                    offset.y = 0.0F;
                }
                offset.z *= -LAUNCH_SPEED;
            } else {
                offset.x = LAUNCH_SPEED;
                if (player_work[0]->pos.y < LAUNCH_LOW_Y) {
                    offset.y = LAUNCH_RISE;
                } else {
                    offset.y = 0.0F;
                }
                offset.z = 0.0F;
                heading = mover_player_heading(boss, 0);
                difference = angle_distance(heading, work->ang.y);
                if (difference < QUARTER_TURN) {
                    turn.y = TANGENT(mover);
                } else {
                    turn.y = TANGENT(mover) + HALF_TURN;
                }
                player_launch_speed(0, &offset, &turn, LAUNCH_LOCK);
            }
        } else if (work->mode != BIGBOGY_PLAYER_HIT &&
                   work->mode != BIGBOGY_INTRO &&
                   work->mode != BIGBOGY_BREATH &&
                   player_work[0]->mode == PLAYER_MODE_HURT) {
            if (state->flame) {
                bigbogy_fireball_dismiss(state->flame);
                state->flame = NULL;
            }
            if (work->mode == BIGBOGY_BITE) {
                PLAY_BITE(boss);
            }
            bigbogy_change_mode(boss, BIGBOGY_PLAYER_HIT);
        }
        have_target = 0;
        if (work->mode != BIGBOGY_INTRO && work->mode != BIGBOGY_BREATH) {
            focus_above(boss);
            break;
        }
        if (work->mode == BIGBOGY_BREATH) {
            struct task *ghost = state->holder->child;
            if (ghost) {
                NJS_POINT3 player;
                /* @hack spelling: ghost_at and player_at, pointers through
                 * which offset and player take the two positions, where
                 * plain assignments would do */
                NJS_POINT3 *ghost_at;
                NJS_POINT3 *player_at;

                ghost_at = &offset;
                *ghost_at = ghost->work->pos;
                player_at = &player;
                *player_at = player_work[0]->pos;
                if (HORIZONTAL_DISTANCE(*ghost_at, *player_at) <
                    GHOST_FOCUS_RANGE) {
                    focus_above(boss);
                    record->focus.y = mover->floor_y + LOW_FOCUS_HEIGHT;
                    have_target = 1;
                }
            }
        } else {
            have_target = 1;
        }
        if (!have_target) {
            focus_above(boss);
        }
        break;
    }
    case MOVE_SHADOW:
        record->focus = work->pos;
        if (state->attack == SHADOW_FLOOR) {
            record->focus.y = mover->floor_y - SHADOW_FOCUS_DEPTH;
        }
        break;
    case MOVE_GROUND: {
        NJS_VECTOR velocity;
        NJS_VECTOR acceleration;
        NJS_POINT3 target;
        Float speed;
        Angle heading;

        TANGENT(mover) = bigbogy_home_tangent(boss);
        mover->acc.y += mover->gravity;
        mover->spd.y += mover->acc.y;
        work->pos.y += mover->spd.y;
        if (!(work->flags & BIGBOGY_AIM_AT_TARGET)) {
            steer_along_circle(boss, TOP_SPEED(mover));
        } else {
            velocity = mover->spd;
            acceleration = mover->acc;
            velocity.y = 0.0F;
            acceleration.y = 0.0F;
            speed = njScalor(&velocity) + njScalor(&acceleration);
            if (speed > TOP_SPEED(mover)) {
                speed = TOP_SPEED(mover);
            }
            target = mover->target;
            heading = atan2f(target.x - work->pos.x, target.z - work->pos.z) *
                      (Float)ANGLE_PER_RADIAN;
            bigbogy_set_speed(&mover->spd, heading, speed);
        }
        work->pos.x += mover->spd.x;
        work->pos.z += mover->spd.z;
        /* @hack spelling: the accelerations multiplied by 0.0F, where assigning
         * 0.0F would do for finite ones */
        mover->acc.x *= 0.0F;
        mover->acc.y *= 0.0F;
        mover->acc.z *= 0.0F;
        if (work->pos.y < mover->floor_y) {
            if (mover->spd.y < 0.0F) {
                mover->spd.y = 0.0F;
                work->pos.y = mover->floor_y;
            }
        }
        record->focus = work->pos;
        record->focus.y += LOW_FOCUS_HEIGHT;
        break;
    }
    case MOVE_DEFEAT:
        record->focus = work->pos;
        record->focus.y += DEFEAT_FOCUS_HEIGHT;
        break;
    default:
        mover->acc.y += mover->gravity;
        mover->spd.y += mover->acc.y;
        work->pos.y += mover->spd.y;
        mover->acc.y = 0.0F;
        if (mover->floor_y - FLY_DEPTH > work->pos.y) {
            if (mover->spd.y < 0.0F) {
                mover->spd.y = 0.0F;
                work->pos.y = mover->floor_y - FLY_DEPTH;
            }
        }
        focus_above(boss);
        break;
    }
}

static void bigbogy_collide(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;

    if (state->movement == MOVE_FLY) {
        NJS_POINT3 center;
        collision_set_shape_center(boss, HAND_0_SHAPE, &bigbogy_hand_0);
        collision_set_shape_center(boss, HAND_1_SHAPE, &bigbogy_hand_1);
        center = bigbogy_mouth_point;
        center.y -= MOUTH_SHAPE_DROP;
        collision_set_shape_center(boss, MOUTH_SHAPE, &center);
    }
    if (work->mode) {
#ifdef NONMATCHING
        /* the hack's plain form. a call through a function type that returns
         * int, where Bool is a long, is undefined C */
        collision_register(boss);
#else
        /* @hack type: collision_register called as a function returning
         * int, where its own Bool would do */
        ((int (*)(struct task *))collision_register)(boss);
#endif
    }
}

static void start_circling(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    /* @hack spelling: state read through boss->extra again, where
     * record->own would do */
    struct bigbogy_state *state = ((struct boss_record *)boss->extra.p)->own;
    {
        /* @hack spelling: motion, a local for record->block, where
         * record->block would do */
        struct motion_control *motion = record->block;
        motion_link(motion, MOTION_CIRCLE);
    }
    work->timer = bigbogy_levels[(Sint8)record->power - 1].circle_seconds *
                  STEPS_PER_SECOND;
    state->movement = MOVE_FLY;
    state->attack = ATTACK_CIRCLE;
    bigbogy_change_mode(boss, BIGBOGY_CIRCLE);
}

static void start_fireballs(struct task *boss)
{
    struct boss_record *record = boss->extra.p;
    FIREBALLS(record) = bigbogy_levels[(Sint8)record->power - 1].fireballs;
    bigbogy_change_mode(boss, BIGBOGY_THROW);
}

/* try the holder's dash, and leave the boss's mode for it where it starts.
 * where the holder has no ghost, leave for the sunlight instead. the call
 * returns TRUE where the boss leaves his mode */
static Bool leave_for_holder(struct task *boss)
{
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;
    Sint8 result;

    result = bigbogy_holder_try_dash(state->holder);
    if (result == BIGBOGY_DASH_STARTED) {
        bigbogy_change_mode(boss, BIGBOGY_HOLDER_DASH);
        return TRUE;
    } else if (result == BIGBOGY_NO_GHOST) {
        bigbogy_change_mode(boss, BIGBOGY_SUNLIGHT);
        return TRUE;
    }
    return FALSE;
}

/* go back to state->attack's attack, and return the mode he takes. ATTACK_START
 * and ATTACK_CIRCLE go back to the circle. where the bite's brake goes back to
 * the breath, he takes BIGBOGY_START instead. the result is then still
 * BIGBOGY_BREATH */
static Sint8 resume_attack(struct task *boss)
{
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;
    struct task_work *work = boss->work;
    Sint8 next;

    next = resume_mode(state->attack);
    switch (next) {
    case BIGBOGY_CIRCLE: {
        /* @hack spelling: motion, a local for record->block, where
         * record->block would do */
        struct motion_control *motion = record->block;
        motion_link(motion, MOTION_TO_CIRCLE);
        bigbogy_change_mode(boss, BIGBOGY_CIRCLE);
        next = BIGBOGY_CIRCLE;
        break;
    }
    case BIGBOGY_THROW:
        bigbogy_change_mode(boss, BIGBOGY_THROW);
        break;
    case BIGBOGY_BREATH:
        if (work->submode < BITE_BRAKE) {
            bigbogy_change_mode(boss, BIGBOGY_BREATH);
        } else {
            bigbogy_change_mode(boss, BIGBOGY_START);
        }
        break;
    case BIGBOGY_START: {
        /* @hack spelling: motion, a local for record->block, where
         * record->block would do */
        struct motion_control *motion = record->block;
        motion_link(motion, MOTION_TO_CIRCLE);
        state->movement = MOVE_FLY;
        state->attack = ATTACK_CIRCLE;
        work->timer = bigbogy_levels[(Sint8)record->power - 1].circle_seconds *
                      STEPS_PER_SECOND;
        bigbogy_change_mode(boss, BIGBOGY_CIRCLE);
        next = BIGBOGY_CIRCLE;
        break;
    }
    default:
        bigbogy_change_mode(boss, BIGBOGY_START);
        next = BIGBOGY_START;
        break;
    }
    return next;
}

void bigbogy_change_mode(struct task *boss, Sint8 mode)
{
    struct boss_record *record;
    struct mover *mover;
    struct bigbogy_state *state;
    struct task_work *work;

    record = boss->extra.p;
    state = record->own;
    work = boss->work;
    mover = boss->motion.p;
    {
        /* @hack redundant: next, a copy of mode, and tests of it that do
         * nothing */
        Sint8 next = mode;
        if (state->movement == MOVE_FLY && next == BIGBOGY_THROW) {
            if (FIREBALLS(record) > 0) {
            }
        }
    }
    if (state->flame) {
        task_kill(state->flame);
        state->flame = NULL;
    }
    work->ang.x = 0;
    work->ang.z = 0;
    work->flags &= ~BIGBOGY_AIM_AT_TARGET;
    /* @hack spelling: a test of FLIP before it is cleared, where clearing it
     * would do */
    if (FLIP(mover)) {
        FLIP(mover) = 0;
    }
    work->mode = mode;
    work->submode = 0;
}

static void choose_attack(struct task *boss)
{
    struct boss_record *record = boss->extra.p;
    Uint16 time = bigbogy_levels[(Sint8)record->power - 1].circle_seconds *
                  STEPS_PER_SECOND;
    if (time) {
        start_circling(boss);
    } else {
        start_fireballs(boss);
    }
}

static void bigbogy_idle(struct task *boss)
{
    struct boss_record *record = boss->extra.p;
    struct motion_control *motion = record->block;
    hover(boss, IDLE_LIFT_VARIATION, IDLE_LIFT);
    advance_sway(boss);
    motion_step(motion);
    bigbogy_move(boss);
}

/* the intro lasts INTRO_STEPS + 1 steps. its opening orbits the focus,
 * from his facing less ORBIT_START_TURN about y. the camera closes in
 * from ORBIT_START_DISTANCE to ORBIT_DISTANCE, then turns by ORBIT_TURN. one
 * cutaway to the ghost lasts CUTAWAY_STEPS + 1 steps, and the hold after it
 * CUTAWAY_HOLD + 1 */
#define INTRO_STEPS 360
#define ORBIT_SPEED 1.0F
#define ORBIT_START_TURN 0xa000
#define ORBIT_START_DISTANCE 220.0F
#define ORBIT_DISTANCE 130.0F
#define ORBIT_TURN 0x6000
#define CUTAWAYS 1
#define CUTAWAY_HOLD 20
#define CUTAWAY_STEPS 50
/* the fight's camera_bosspoint settings */
#define FIGHT_DISTANCE 60.0F
#define FIGHT_HEIGHT 0.0F
#define FIGHT_PART 1.0F
static void bigbogy_intro(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;

    switch (work->submode) {
    case 0: {
        struct bigbogy_camera_shots shots;
        struct camera_bossinit orbit;
        NJS_VECTOR offset;

        state->movement = MOVE_FLY;
        work->timer = INTRO_STEPS;
        boss_gauge_create(boss, &record->power);
        orbit.speed = ORBIT_SPEED;
        orbit.ang = work->ang.y - ORBIT_START_TURN;
        offset.y = INTRO_FOCUS_HEIGHT;
        offset.x = offset.z = 0.0F;
        boss_record_set_focus(record, &work->pos, &offset);
        orbit.target = &record->focus;
        orbit.start_dist = ORBIT_START_DISTANCE;
        orbit.dist = ORBIT_DISTANCE;
        orbit.turn = ORBIT_TURN;
        shots.opening = orbit;
        shots.count = CUTAWAYS;
        shots.hold_frames = CUTAWAY_HOLD;
        shots.shot_frames = CUTAWAY_STEPS;
        {
            struct task_work *ghost = state->holder->child->work;
            shots.target = &ghost->pos;
        }
        shots.heading = work->ang.y + HALF_TURN;
        state->camera = bigbogy_camera_shots_create(boss, &shots);
        voice_play_priority_2(INTRO_VOICE);
        work->submode++;
    }
        /* fall through */
    case 1:
        collision_shape_off(boss, GROUND_SHAPE);
        collision_shape_off(boss, GROUND_PUSH_SHAPE);
        collision_shape_off(boss, SHADOW_SHAPE);
        if (work->timer-- == 0) {
            struct camera_bosspoint camera;
            task_kill(state->camera);
            state->camera = NULL;
            camera.target = &record->focus;
            camera.dist = FIGHT_DISTANCE;
            camera.height = FIGHT_HEIGHT;
            camera.part = FIGHT_PART;
            camera.adjust = CAMERA_ADJUST_RELATIVE3;
            camera.collides = 1;
            camera.trigger_push = 1;
            camera.lift = 0.0F;
            camera.level = 0;
            pad_enable_player(0);
            camera_bosspoint_start(0, &camera);
            boss_messenger_summon_capped(0, MESSENGER_MESSAGES, boss);
            bigbogy_change_mode(boss, BIGBOGY_START);
        }
        break;
    }
    sound_play_at_for(FLOAT_SOUND, boss, KEYED_PRIORITY, 0, FLOAT_SOUND_STEPS,
                      &work->pos);
    hover(boss, IDLE_LIFT_VARIATION, IDLE_LIFT);
    advance_sway(boss);
}

/* the start waits START_STEPS, then for his speed in x and z to pass
 * START_SPEED of TOP_SPEED */
#define START_STEPS 180
#define START_SPEED 0.9F
static void bigbogy_start(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;
    struct mover *mover = boss->motion.p;
    struct motion_control *motion = record->block;
    Float distance;

    switch (work->submode) {
    case 0: {
        struct bigbogy_tuning *tuning;
        /* @hack spelling: the assignment of tuning inside the product, where
         * a statement of its own would do */
        TOP_SPEED(mover) = player_records[0]->consts.top_speed *
                           (tuning = &opening_tuning)->speed_scale;
        mover->gravity = -tuning->gravity;
        motion_link(motion, MOTION_START_RISE);
        work->timer = START_STEPS;
        state->attack = ATTACK_START;
        voice_play_priority_2(START_VOICE);
        work->submode++;
    }
        /* fall through */
    case 1:
        if (work->timer == 0) {
            NJS_VECTOR velocity;
            velocity.x = mover->spd.x;
            velocity.y = 0.0F;
            velocity.z = mover->spd.z;
            if (njScalor(&velocity) > TOP_SPEED(mover) * START_SPEED) {
                motion_set(motion, MOTION_START);
                work->submode++;
            }
        } else {
            work->timer--;
        }
        break;
    case 2:
        if (motion_ended(motion)) {
            choose_attack(boss);
        }
        break;
    }
    match_player_height(boss);
    {
        struct bigbogy_tuning *tuning = &opening_tuning;
        bigbogy_accelerate(boss, tuning->acceleration);
        hover(boss, tuning->lift_variation, tuning->lift);
    }
    if (player_within_reach(boss, &distance) == FALSE) {
        face_tangent(boss, TURN_STEP);
    } else {
        face_player(boss, TURN_STEP, FACE_LIMIT);
        if (distance < bite_distance) {
            bigbogy_change_mode(boss, BIGBOGY_BITE);
        }
    }
    advance_sway(boss);
    leave_for_holder(boss);
}

static void bigbogy_circle(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    struct mover *mover = boss->motion.p;
    struct motion_control *motion = record->block;
    struct bigbogy_player player;
    Float distance;

    bigbogy_player_relative(boss, &player);
    switch (work->submode) {
    case 0: {
        struct bigbogy_tuning *tuning;
        /* @hack spelling: the assignment of tuning inside the product, where
         * a statement of its own would do */
        TOP_SPEED(mover) = player_records[0]->consts.top_speed *
                           (tuning = &circle_tuning)->speed_scale;
        mover->gravity = -tuning->gravity;
        work->submode++;
    }
        /* fall through */
    case 1: {
        struct bigbogy_tuning *tuning = &circle_tuning;
        hover(boss, tuning->lift_variation, tuning->lift);
        bigbogy_accelerate(boss, tuning->acceleration);
        break;
    }
    }
    if (player_within_reach(boss, &distance) == FALSE) {
        face_tangent(boss, TURN_STEP);
    } else {
        face_player(boss, TURN_STEP, FACE_LIMIT);
        if (distance < bite_distance) {
            bigbogy_change_mode(boss, BIGBOGY_BITE);
        }
    }
    match_player_height(boss);
    advance_sway(boss);
    if (work->timer == 0) {
        start_fireballs(boss);
    } else {
        work->timer--;
    }
    sound_play_at_for(FLOAT_SOUND, boss, KEYED_PRIORITY, 0, FLOAT_SOUND_STEPS,
                      &work->pos);
    if (motion->index == MOTION_CIRCLE) {
        Uint32 frame;
        if ((frame = game_state_steps) % CHATTER_PERIOD == 0 ||
            (frame % CHATTER_TRY_PERIOD == 0 &&
             rand() * RAND_SCALE < CHATTER_CHANCE)) {
            sound_play_at(BITE_SOUND, NULL, UNKEYED_PRIORITY, CHATTER_VOLUME,
                          &work->pos);
        }
    }
    leave_for_holder(boss);
}

/* each throw links into MOTION_WIND_UP over WIND_UP_FRAMES, and lifts him by
 * WIND_UP_LIFT. the ball leaves his hand past MOTION_THROW's frame
 * RELEASE_FRAME */
#define WIND_UP_FRAMES 30
#define WIND_UP_LIFT 1.15F
#define RELEASE_FRAME 11.0F
static void bigbogy_throw(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;
    struct motion_control *motion = record->block;
    struct mover *mover = boss->motion.p;
    /* @hack type: tuning, a pointer to the whole of bigbogy_tuning, where the
     * array itself would do */
    struct bigbogy_tuning(*tuning)[BIGBOGY_TUNINGS] = &bigbogy_tuning;
    Float distance;

    switch (work->submode) {
    case 0:
        TOP_SPEED(mover) = player_records[0]->consts.top_speed *
                           (*tuning)[BIGBOGY_THROW_TUNING].speed_scale;
        state->attack = ATTACK_THROW;
        work->submode++;
        /* fall through */
    case 1:
        motion_link_frames(motion, MOTION_WIND_UP, WIND_UP_FRAMES);
        mover->gravity = -(*tuning)[BIGBOGY_THROW_TUNING].gravity;
        mover->acc.y += WIND_UP_LIFT;
        mover->spd.y = 0.0F;
        work->submode++;
        /* fall through */
    case 2:
        mover->spd.x *= DRAG;
        mover->spd.z *= DRAG;
        bigbogy_accelerate(boss, (*tuning)[BIGBOGY_THROW_TUNING].acceleration);
        if (motion->index == MOTION_WOUND ||
            (motion->flags == MOTION_LINKING &&
             motion->index == MOTION_WIND_UP)) {
            hover(boss, (*tuning)[BIGBOGY_THROW_TUNING].lift_variation,
                  (*tuning)[BIGBOGY_THROW_TUNING].lift);
        } else {
            hover(boss, (*tuning)[BIGBOGY_THROW_LIFT_TUNING].lift_variation,
                  (*tuning)[BIGBOGY_THROW_LIFT_TUNING].lift);
        }
        if (motion_held(motion)) {
            FIREBALLS(record)--;
            state->flame = bigbogy_fireball_create(boss, &bigbogy_hand_1);
            motion_link(motion, MOTION_THROW);
            mover->gravity = -(*tuning)[BIGBOGY_THROWN_TUNING].gravity;
            work->submode++;
        }
        break;
    case 3: {
        struct bigbogy_tuning *setting;
        if (bigbogy_holder_try_dash(state->holder) == BIGBOGY_DASH_STARTED) {
            bigbogy_change_mode(boss, BIGBOGY_HOLDER_DASH);
        }
        setting = &(*tuning)[BIGBOGY_THROWN_TUNING];
        bigbogy_accelerate(boss, setting->acceleration);
        lower_hover_height(boss, setting->lift);
        hover(boss, setting->lift_variation, setting->lift);
        if (state->flame && motion->current.frame > RELEASE_FRAME) {
            bigbogy_fireball_throw(state->flame);
            voice_play_priority_2(THROW_VOICE);
            state->flame = NULL;
        }
        if (motion_held(motion)) {
            if (FIREBALLS(record) > 0) {
                work->submode = 1;
            } else {
                work->flags &= ~BIGBOGY_BREATH_BROKEN;
                bigbogy_change_mode(boss, BIGBOGY_BREATH);
            }
        }
        break;
    }
    }
    match_player_height(boss);
    advance_sway(boss);
    if (player_within_reach(boss, &distance) == FALSE) {
        face_tangent(boss, TURN_STEP);
    } else {
        face_player(boss, TURN_STEP, FACE_LIMIT);
        if (distance < bite_distance) {
            bigbogy_change_mode(boss, BIGBOGY_BITE);
        }
    }
    sound_play_at_for(FLOAT_SOUND, boss, KEYED_PRIORITY, 0, FLOAT_SOUND_STEPS,
                      &work->pos);
    leave_for_holder(boss);
}

static void bigbogy_breathe_fire(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;
    struct mover *mover = boss->motion.p;
    struct motion_control *motion = record->block;

    switch (work->submode) {
    case 0:
        mover->gravity = -bigbogy_tuning[BIGBOGY_BREATH_TUNING].gravity;
        state->attack = ATTACK_BREATH;
        work->submode++;
        /* fall through */
    case 1:
        mover->spd.x *= BRAKE;
        if (_builtin_fabsf(mover->spd.x) < STOP_SPEED) {
            mover->spd.x = 0.0F;
        }
        mover->spd.z *= BRAKE;
        if (_builtin_fabsf(mover->spd.z) < STOP_SPEED) {
            mover->spd.z = 0.0F;
        }
        ease_sway(boss, SWAY_EASE_STEP);
        if (record->power != FULL_HEALTH &&
            !(work->flags & BIGBOGY_BREATH_BROKEN) &&
            leave_for_holder(boss) == TRUE) {
            work->flags |= BIGBOGY_BREATH_BROKEN;
            return;
        }
        sound_play_at_for(FLOAT_SOUND, boss, KEYED_PRIORITY, 0,
                          FLOAT_SOUND_STEPS, &work->pos);
        if (mover->spd.x == 0.0F && mover->spd.z == 0.0F &&
            work->pos.y < mover->floor_y &&
            face_tangent(boss, TURN_STEP) == 0) {
            work->pos.y = mover->floor_y;
            mover->home.y = mover->floor_y;
            clear_sway(boss);
            motion_link(motion, MOTION_INHALE);
            voice_play_priority_2(INHALE_VOICE);
            work->submode++;
        }
        break;
    case 2:
        sound_play_for(INHALE_SOUND, boss, KEYED_PRIORITY, 0, BREATHING_STEPS);
        sound_set_volume(boss, INHALE_SOUND, BREATHING_VOLUME);
        if (motion->index == MOTION_BREATH) {
            state->flame = bigbogy_breath_create(boss, &bigbogy_mouth_point);
            voice_play_priority_2(BREATH_VOICE);
            vibration_start_fading(0, BREATH_POWER, BREATH_FREQ, BREATH_LENGTH);
            work->submode++;
        } else if (record->power != FULL_HEALTH &&
                   !(work->flags & BIGBOGY_BREATH_BROKEN) &&
                   leave_for_holder(boss) == TRUE) {
            work->flags |= BIGBOGY_BREATH_BROKEN;
            return;
        }
        break;
    case 3:
        sound_play_for(EXHALE_SOUND, boss, KEYED_PRIORITY, 0, BREATHING_STEPS);
        sound_set_volume(boss, EXHALE_SOUND, BREATHING_VOLUME);
        if (motion_held(motion)) {
            work->timer =
                bigbogy_levels[(Uint8)(record->power - 1.0F)].breath_seconds *
                STEPS_PER_SECOND;
            motion_link(motion, MOTION_BREATH_HOLD);
            work->tick = EXHALE_FADE_STEPS;
            work->submode++;
        }
        break;
    case 4:
        if (work->tick > 0) {
            sound_play_for(EXHALE_SOUND, boss, KEYED_PRIORITY, 0,
                           BREATHING_STEPS);
            sound_set_volume(boss, EXHALE_SOUND, EXHALE_VOLUME(work->tick));
            work->tick--;
        }
        if (work->timer == 0) {
            task_kill(state->flame);
            state->flame = NULL;
            bigbogy_change_mode(boss, BIGBOGY_START);
            state->attack = ATTACK_START;
        } else {
            work->timer--;
        }
        break;
    }
}

/* the dash. he leaps up at DASH_LEAP, and flies at DASH_ACCELERATION. he heads
 * for the point DASH_AIM_NEAR of the way from the ghost to player 0. where they
 * lie DASH_AIM_RANGE or more apart, he heads DASH_AIM_FAR of the way. within
 * DASH_ARRIVAL of the point he brakes, and watches player 0 by BRAKE_WATCH_STEP
 */
#define DASH_LEAP 5.9F
#define DASH_ACCELERATION 0.15F
#define DASH_AIM_NEAR 0.5F
#define DASH_AIM_FAR 0.8F
#define DASH_AIM_RANGE 25.0F
#define DASH_ARRIVAL 100.0F
#define BRAKE_WATCH_STEP 0x400
/* falling below BOUNCE_HEIGHT above the floor, he bounces up. the bounce is
 * DASH_BOUNCE as he flies and BRAKE_BOUNCE as he brakes. each takes a random 0
 * to BOUNCE_SPREAD more */
#define BOUNCE_HEIGHT 6.0F
#define DASH_BOUNCE 0.8F
#define BRAKE_BOUNCE 0.5F
#define BOUNCE_SPREAD 0.5F
static void bigbogy_holder_dash(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;
    struct mover *mover = boss->motion.p;
    struct motion_control *motion = record->block;
    struct bigbogy_player player;
    /* @hack spelling: center, aim_near, aim_far and spread, locals for
     * constants, where the constants would do. aim_near also stands for BRAKE
     * and BRAKE_BOUNCE, and aim_far for DASH_BOUNCE */
    const NJS_POINT3 *center;
    Float aim_near;
    Float aim_far;
    Float spread;

    bigbogy_player_relative(boss, &player);
    /* @hack redundant: an increment of state that no path reaches */
    if (0) {
        state++;
    }
    spread = BOUNCE_SPREAD * RAND_SCALE;
    aim_far = DASH_AIM_FAR;
    center = &bigbogy_arena_center;
    aim_near = DASH_AIM_NEAR;
    switch (work->submode) {
    case 0: {
        NJS_POINT3 ghost;
        NJS_POINT3 target;
        NJS_VECTOR direction;
        /* @hack redundant: unused, a vector that only sizes the frame */
        NJS_VECTOR unused;
        NJS_VECTOR *delta;
        Angle heading;
        struct bigbogy_tuning *tuning;

        if (!state->holder->child) {
            bigbogy_change_mode(boss, BIGBOGY_SUNLIGHT);
            return;
        }
        mover->spd.y = 0.0F;
        mover->acc.y = DASH_LEAP;
        tuning = &dash_tuning;
        mover->gravity = -tuning->gravity;
        TOP_SPEED(mover) =
            player_records[0]->consts.top_speed * tuning->speed_scale;
        bigbogy_reverse(boss);
        ghost = state->holder->child->work->pos;
        delta = &direction;
        delta->x = player_work[0]->pos.x - ghost.x;
        delta->z = player_work[0]->pos.z - ghost.z;
        if (njSqrt((delta->x * delta->x) + (delta->z * delta->z)) <
            DASH_AIM_RANGE) {
            target.x = ghost.x + (delta->x * aim_near);
            target.z = ghost.z + (delta->z * aim_near);
        } else {
            target.x = ghost.x + (delta->x * aim_far);
            target.z = ghost.z + (delta->z * aim_far);
        }
        /* bigbogy_arena_project copies target.y unset, and no code reads it
         * after */
#ifdef NONMATCHING
        /* this side sets it, so that the copy reads no unset float */
        target.y = ghost.y;
#endif
        bigbogy_arena_project(boss, &target, &target);
        mover->target.x = target.x;
        mover->target.y = mover->home.y;
        mover->target.z = target.z;
        heading = atan2f(mover->target.x - mover->home.x,
                         mover->target.z - mover->home.z) *
                  (Float)ANGLE_PER_RADIAN;
        mover->target_radial_heading = heading;
        motion_link(motion, MOTION_IDLE);
        work->submode++;
        (void)unused;
    }
        /* fall through */
    case 1:
        mover_turn_to_player_by(boss, WATCH_STEP, 0);
        bigbogy_accelerate(boss, DASH_ACCELERATION);
        /* @hack redundant: an increment of aim_far that no path reaches */
        if (0) {
            aim_far++;
        }
        if (work->pos.y < center->y + BOUNCE_HEIGHT) {
            if (mover->spd.y < 0.0F) {
                mover->acc.y += (rand() * spread) + aim_far;
                mover->spd.y = 0.0F;
            }
        }
        if (HORIZONTAL_DISTANCE(mover->target, work->pos) < DASH_ARRIVAL) {
            work->submode++;
        }
        break;
    case 2:
        mover->spd.x *= aim_near;
        if (_builtin_fabsf(mover->spd.x) < STOP_SPEED) {
            mover->spd.x = 0.0F;
        }
        mover->spd.z *= aim_near;
        if (_builtin_fabsf(mover->spd.z) < STOP_SPEED) {
            mover->spd.z = 0.0F;
        }
        mover_turn_to_player_by(boss, BRAKE_WATCH_STEP, 0);
        if (njSqrt((mover->spd.x * mover->spd.x) +
                   (mover->spd.z * mover->spd.z)) < STOP_SPEED) {
            struct bigbogy_tuning *tuning = &circle_tuning;
            mover->gravity = -tuning->gravity;
            hover(boss, tuning->lift_variation, tuning->lift);
        } else if (work->pos.y < center->y + BOUNCE_HEIGHT) {
            if (mover->spd.y < 0.0F) {
                mover->acc.y += (rand() * spread) + aim_near;
                mover->spd.y = 0.0F;
            }
        }
        if (work->pos.y < mover->home.y &&
            bigbogy_holder_settle(state->holder)) {
            resume_attack(boss);
        }
        break;
    }
    {
        Float ceiling = center->y + FLY_CEILING;
        if (work->pos.y > ceiling) {
            /* @hack redundant: an increment of ceiling that no path reaches */
            if (0) {
                ceiling++;
            }
            work->pos.y = ceiling;
        }
    }
    advance_sway(boss);
    if (player.horizontal < bite_distance) {
        bigbogy_change_mode(boss, BIGBOGY_BITE);
    }
    sound_play_at_for(FLOAT_SOUND, boss, KEYED_PRIORITY, 0, FLOAT_SOUND_STEPS,
                      &work->pos);
    if (state->attack == ATTACK_CIRCLE && work->timer > 0) {
        work->timer--;
    }
}

/* the bite snaps between MOTION_BITE's frames SNAP_FIRST and SNAP_LAST. he
 * falls back to the point of his circle EIGHTH_TURN behind, and brakes within
 * FALL_BACK_ARRIVAL of it */
#define SNAP_FIRST 8.0F
#define SNAP_LAST 10.0F
#define FALL_BACK_ARRIVAL 50.0F
static void bigbogy_bite(struct task *boss)
{
    struct boss_record *record;
    struct motion_control *motion;
    struct task_work *work;
    struct mover *mover;
    struct bigbogy_player player;
    struct bigbogy_tuning *tuning;

    record = boss->extra.p;
    work = boss->work;
    mover = boss->motion.p;
    motion = record->block;
    match_player_height(boss);
    face_player(boss, TURN_STEP, 0);
    bigbogy_player_relative(boss, &player);
    /* @hack spelling: the assignment of tuning in the switch's comma
     * expression, where a statement of its own would do */
    switch (tuning = bite_tuning, work->submode) {
    case BITE_SETUP:
        TOP_SPEED(mover) =
            player_records[0]->consts.top_speed * tuning->speed_scale;
        mover->gravity = -tuning->gravity;
        clear_sway(boss);
        motion_link(motion, MOTION_BITE);
        work->flags |= BIGBOGY_AIM_AT_TARGET;
        work->submode++;
        /* fall through */
    case BITE_LUNGE:
        bigbogy_accelerate(boss, bite_tuning[0].acceleration);
        mover->target = player_work[0]->pos;
        if (motion->index == MOTION_BITE &&
            motion->current.frame > SNAP_FIRST &&
            motion->current.frame < SNAP_LAST) {
            PLAY_BITE(boss);
            vibration_start(0, BITE_POWER, BITE_FREQ, VIBRATION_UNTIMED);
        }
        if (motion_held(motion)) {
            bigbogy_arena_advance(boss, &work->pos, EIGHTH_TURN,
                                  &mover->target);
            work->submode++;
        }
        break;
    case BITE_FALL_BACK:
        bigbogy_accelerate(boss, tuning->acceleration);
        if (HORIZONTAL_DISTANCE(mover->target, work->pos) < FALL_BACK_ARRIVAL) {
            mover->acc.x = 0.0F;
            work->submode++;
        }
        break;
    case BITE_BRAKE:
        mover->spd.x *= BRAKE;
        if (_builtin_fabsf(mover->spd.x) < STOP_SPEED) {
            mover->spd.x = 0.0F;
        }
        mover->spd.z *= BRAKE;
        if (_builtin_fabsf(mover->spd.z) < STOP_SPEED) {
            mover->spd.z = 0.0F;
        }
        if (mover->spd.x == 0.0F && mover->spd.z == 0.0F) {
            if (player.horizontal > bite_distance) {
                motion_link(motion, MOTION_IDLE);
                resume_attack(boss);
            } else {
                motion_link(motion, MOTION_BITE);
                work->submode = BITE_SETUP;
            }
        }
    }
    sound_play_at_for(FLOAT_SOUND, boss, KEYED_PRIORITY, 0, FLOAT_SOUND_STEPS,
                      &work->pos);
}

/* the taunt lasts at least TAUNT + 1 steps. he rises by TAUNT_LIFT, and glides
 * at TAUNT_ACCELERATION to the point of his circle TAUNT_TURN behind. his speed
 * keeps TAUNT_DRAG a step, and TAUNT_BRAKE within TAUNT_ARRIVAL of the point.
 * with TAUNT_FOLD steps left his parts fold, and from then on the holder can
 * dash */
#define TAUNT 180
#define TAUNT_LIFT 0.1F
#define TAUNT_TURN 0x1800
#define TAUNT_ACCELERATION 0.13F
#define TAUNT_DRAG 0.9F
#define TAUNT_ARRIVAL 5.0F
#define TAUNT_BRAKE 0.8F
#define TAUNT_FOLD 60
static void bigbogy_player_hit(struct task *boss)
{
    struct boss_record *record = boss->extra.p;
    struct task_work *work = boss->work;
    struct bigbogy_state *state = record->own;
    struct mover *mover = boss->motion.p;
    struct motion_control *motion = record->block;

    TANGENT(mover) = bigbogy_home_tangent(boss);
    switch (work->submode) {
    case 0:
        TAUNT_STEPS(mover) = TAUNT;
        work->flags |= BIGBOGY_AIM_AT_TARGET;
        mover->gravity = -hit_tuning.gravity;
        motion_set(motion, MOTION_TAUNT);
        bigbogy_arena_advance(boss, &work->pos, TAUNT_TURN, &mover->target);
        mover->acc.y += TAUNT_LIFT;
        advance_sway(boss);
        voice_play_priority_2(HIT_PLAYER_VOICE);
        voice_play_priority_2(HIT_PLAYER_VOICE_2);
        work->submode++;
        /* fall through */
    case 1: {
        Float distance;
        mover->acc.x = TAUNT_ACCELERATION;
        mover->spd.x *= TAUNT_DRAG;
        mover->spd.z *= TAUNT_DRAG;
        distance = HORIZONTAL_DISTANCE(mover->target, work->pos);
        if (distance < TAUNT_ARRIVAL) {
            work->submode++;
        }
        break;
    }
    case 2:
        mover->spd.x *= TAUNT_BRAKE;
        if (_builtin_fabsf(mover->spd.x) < STOP_SPEED) {
            mover->spd.x = 0.0F;
        }
        mover->spd.z *= TAUNT_BRAKE;
        if (_builtin_fabsf(mover->spd.z) < STOP_SPEED) {
            mover->spd.z = 0.0F;
        }
        break;
    }
    face_tangent(boss, TURN_STEP);
    match_player_height(boss);
    if (TAUNT_STEPS(mover) == TAUNT_FOLD) {
        motion_link(motion, MOTION_FOLD);
        bigbogy_part_fold(state->parts[0]);
        bigbogy_part_fold(state->parts[1]);
    }
    if (TAUNT_STEPS(mover) == 0) {
        if (motion->index == MOTION_IDLE) {
            resume_attack(boss);
        }
    } else {
        TAUNT_STEPS(mover)--;
    }
    sound_play_at_for(FLOAT_SOUND, boss, KEYED_PRIORITY, 0, FLOAT_SOUND_STEPS,
                      &work->pos);
    if (TAUNT_STEPS(mover) < TAUNT_FOLD) {
        leave_for_holder(boss);
    }
}

/* the sunlight's first view looks at the sandglass from EYE_BACK beyond it,
 * away from him. it stands EYE_RISE above the floor. once the sand runs, a cut
 * looks from the wall point nearest player 0 to him. its view starts
 * CUT_START_REACH past that point along his heading toward player 0, and
 * CUT_START_HEIGHT above the floor. the land's light rises for SUNRISE_STEPS +
 * 1 steps. the view then moves over SHRINK_CUT steps to CUT_END_REACH past
 * player 0, CUT_END_RISE above the floor. that point is moved onto his circle
 */
#define EYE_BACK 100.0F
#define EYE_RISE 10.0F
#define CUT_START_REACH 20.0F
#define CUT_START_HEIGHT 80.0F
#define CUT_END_REACH 30.0F
#define CUT_END_RISE 20.0F
#define SUNRISE_STEPS 60
#define SHRINK_CUT 120
/* he shrinks by SHRINK_STEP a step to SHRUNK, and rises by SHRINK_RISE. he
 * swings by SUNLIGHT_SWING either side of his aim at SUNLIGHT_SWING_STEP a
 * step, rolled up to SUNLIGHT_ROLL */
#define SHRUNK 0.3F
#define SHRINK_STEP 0.005F
#define SHRINK_RISE 0.002F
#define SUNLIGHT_ROLL 48
#define SUNLIGHT_SWING 0x1000
#define SUNLIGHT_SWING_STEP 0x400
static void bigbogy_sunlight(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;
    struct mover *mover = boss->motion.p;
    struct motion_control *motion = record->block;
    Angle heading;

    switch (work->submode) {
    case 0: {
        NJS_POINT3 eye;
        NJS_VECTOR offset;
        NJS_POINT3 *glass;
        struct task *player;

        mover->gravity = -circle_tuning.gravity;
        state->movement = MOVE_SUNLIGHT;
        mover->spd.x = mover->spd.y = mover->spd.z = 0.0F;
        glass = &state->glass->work->pos;
        offset.x = glass->x - work->pos.x;
        offset.y = 0.0F;
        offset.z = glass->z - work->pos.z;
        njUnitVector(&offset);
        eye.x = glass->x + (offset.x * EYE_BACK);
        eye.y = bigbogy_arena_center.y + EYE_RISE;
        eye.z = glass->z + (offset.z * EYE_BACK);
        bigbogy_camera_look(glass, &eye, CAMERA_ADJUST_THREE2);
        player = player_tasks[0];
        if (sandglass_blow_strikes(player) && sandglass_blow_stops(player)) {
            player_request(0, PLAYER_REQUEST_RELEASE, 0);
        }
        player_set_speed(0, 0.0F, 0.0F, 0.0F);
        player_set_acc(0, 0.0F, 0.0F, 0.0F);
        pad_disable_player(0);
        clear_sway(boss);
        motion_link(motion, MOTION_RECOIL_HOLD);
        voice_play_priority_2(SUNLIGHT_VOICE);
        voice_play_priority_2(SUNLIGHT_VOICE_2);
        work->submode++;
    }
        /* fall through */
    case 1:
        mover_turn_to_player_by(boss, WATCH_STEP, 0);
        if (state->glass->work->mode == SANDGLASS_MODE_RUNNING) {
            struct bigbogy_camera_path shot;
            NJS_POINT3 wall;
            NJS_VECTOR direction;

            bigbogy_camera_pop(CAMERA_ADJUST_NONE);
            heading = mover_player_heading(boss, 0);
            bigbogy_nearest_wall_point(BIGBOGY_WALL_POINTS, state->wall_points,
                                       &player_work[0]->pos, &wall);
            shot.target_from = wall;
            direction.x = njSin(heading);
            direction.z = njCos(heading);
            shot.pos_from.x = wall.x + (direction.x * CUT_START_REACH);
            shot.pos_from.y = mover->floor_y + CUT_START_HEIGHT;
            shot.pos_from.z = wall.z + (direction.z * CUT_START_REACH);
            shot.pos_to.x =
                player_work[0]->pos.x + (direction.x * CUT_END_REACH);
            shot.pos_to.y = bigbogy_arena_center.y + CUT_END_RISE;
            shot.pos_to.z =
                player_work[0]->pos.z + (direction.z * CUT_END_REACH);
            bigbogy_arena_project(boss, &shot.pos_to, &shot.pos_to);
            shot.target_to = work->pos;
            state->camera = bigbogy_camera_path_create(boss, &shot);
            work->timer = SUNRISE_STEPS;
            work->submode++;
        }
        break;
    case 2:
        mover_turn_to_player_by(boss, WATCH_STEP, 0);
        set_daylight(boss, DAYLIGHT_SUNLIT);
        if (state->ambient < SUNLIT_AMBIENT) {
            state->ambient += AMBIENT_STEP;
            if (state->ambient > SUNLIT_AMBIENT) {
                state->ambient = SUNLIT_AMBIENT;
            }
            light_set_ambient(LIGHT_LAND, state->ambient);
        }
        if (!work->timer--) {
            state->ambient = SUNLIT_AMBIENT;
            light_set_ambient(LIGHT_LAND, SUNLIT_AMBIENT);
            work->timer = SHRINK_CUT;
            mover->aim_y = work->ang.y;
            bigbogy_camera_move(state->camera, work->timer);
            work->submode++;
        }
        break;
    case 3:
        if (work->scl.x > SHRUNK) {
            work->scl.x -= SHRINK_STEP;
        }
        work->scl.y = work->scl.z = work->scl.x;
        work->pos.y += SHRINK_RISE;
        if (!(work->scl.x > SHRUNK)) {
            motion_link(motion, MOTION_RECOIL);
            work->submode++;
        }
        heading = mover_player_heading(boss, 0);
        mover->aim_y = angle_approach(mover->aim_y, heading, WATCH_STEP);
        swing_about_aim(boss, SUNLIGHT_ROLL, SUNLIGHT_SWING,
                        SUNLIGHT_SWING_STEP);
        break;
    case 4:
        heading = mover_player_heading(boss, 0);
        mover->aim_y = angle_approach(mover->aim_y, heading, WATCH_STEP);
        swing_about_aim(boss, SUNLIGHT_ROLL, SUNLIGHT_SWING,
                        SUNLIGHT_SWING_STEP);
        if (motion_held(motion)) {
            work->scl.x = work->scl.y = work->scl.z = SHRUNK;
            task_kill(state->camera);
            state->camera = NULL;
            pad_enable_player(0);
            bigbogy_change_mode(boss, BIGBOGY_PURSUIT);
        }
        break;
    }
}

/* the dark's cut looks from a wall point to RESTORE_FOCUS above him. the wall
 * point is the one nearest the point DARK_AHEAD ahead of him, along his
 * circle's heading. the view starts CUT_START_REACH past the wall point along
 * the heading, and CUT_START_HEIGHT above the floor. where the sand has run out
 * by step RESTORE_STEPS + 1, the view moves over RESTORE_CUT steps to the point
 * ahead. it stays as high. the new ghost then starts GHOST_RAISE above its
 * place, and he grows by GROW_STEP a step to full size. he flies again
 * RESTORE_HOLD + 1 steps later */
#define DARK_AHEAD 120.0F
#define RESTORE_STEPS 120
#define RESTORE_FOCUS 60.0F
#define RESTORE_CUT 60
#define GHOST_RAISE 10.0F
#define GROW_STEP 0.005F
#define RESTORE_HOLD 60
/* the attack his shapes in the air take, which no guard is above */
#define FULL_ATTACK 3
static void bigbogy_restore_dark(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;
    struct mover *mover = boss->motion.p;
    struct motion_control *motion = record->block;
    struct task_work *ghost;
    Sint8 shape;

    switch (work->submode) {
    case 0:
        pad_disable_player(0);
        work->flags &= ~WORK_HURT;
        {
            NJS_POINT3 player;
            NJS_POINT3 spawn;
            player = player_work[0]->pos;
            bigbogy_arena_advance(boss, &player, QUARTER_TURN, &spawn);
            work->pos = spawn;
        }
        TANGENT(mover) = bigbogy_home_tangent(boss);
        work->ang.y = TANGENT(mover);
        work->scl.x = work->scl.y;
        motion_link(motion, MOTION_RESTORE);
        state->movement = MOVE_RESTORE;
        {
            struct bigbogy_camera_path shot;
            NJS_POINT3 ahead;
            NJS_POINT3 wall;
            NJS_VECTOR direction;
            Angle heading;

            heading = bigbogy_home_tangent(boss);
            direction.x = njSin(heading);
            direction.z = njCos(heading);
            ahead.x = work->pos.x + (direction.x * DARK_AHEAD);
            ahead.y = mover->floor_y;
            ahead.z = work->pos.z + (direction.z * DARK_AHEAD);
            bigbogy_nearest_wall_point(BIGBOGY_WALL_POINTS, state->wall_points,
                                       &ahead, &wall);
            shot.target_from = wall;
            shot.pos_from.x = wall.x + (direction.x * CUT_START_REACH);
            shot.pos_from.y = mover->floor_y + CUT_START_HEIGHT;
            shot.pos_from.z = wall.z + (direction.z * CUT_START_REACH);
            shot.pos_to = ahead;
            shot.pos_to.y = mover->floor_y + CUT_START_HEIGHT;
            shot.target_to = work->pos;
            shot.target_to.y += RESTORE_FOCUS;
            state->camera = bigbogy_camera_path_create(boss, &shot);
        }
        set_daylight(boss, DAYLIGHT_DARK);
        work->timer = RESTORE_STEPS;
        work->submode++;
        /* fall through */
    case 1:
        if (state->ambient > DARK_AMBIENT) {
            state->ambient -= AMBIENT_STEP;
            if (state->ambient < DARK_AMBIENT) {
                state->ambient = DARK_AMBIENT;
            }
            light_set_ambient(LIGHT_LAND, state->ambient);
        }
        if (!sandglass_running(BIGBOGY_SANDGLASS_ID)) {
            sound_play_for(RESTORE_SOUND, boss, KEYED_PRIORITY, 0,
                           RESTORE_SOUND_STEPS);
            sound_set_volume(boss, RESTORE_SOUND, RESTORE_VOLUME);
        }
        if (!work->timer-- && !sandglass_running(BIGBOGY_SANDGLASS_ID)) {
            work->flags &= ~BIGBOGY_FADING;
            FADE(mover) = 0.0F;
            state->ambient = DARK_AMBIENT;
            light_set_ambient(LIGHT_LAND, DARK_AMBIENT);
            bigbogy_camera_move(state->camera, RESTORE_CUT);
            voice_play_priority_2(RESTORE_VOICE);
            bigbogy_holder_spawn_ghost(state->holder);
            ghost = state->holder->child->work;
            ghost->pos.y += GHOST_RAISE;
            sandglass_pick_up(state->glass, &ghost->pos, ghost->ang.y);
            work->submode++;
        }
        break;
    case 2:
        if (work->scl.x < 1.0F) {
            work->scl.x += GROW_STEP;
        }
        work->scl.y = work->scl.z = work->scl.x;
        bigbogy_sandglass_carry(state->glass, state->holder);
        if (!(work->scl.x < 1.0F)) {
            work->scl.x = work->scl.y = work->scl.z = 1.0F;
            work->timer = RESTORE_HOLD;
            work->submode++;
        }
        break;
    case 3:
        bigbogy_sandglass_carry(state->glass, state->holder);
        if (work->timer--) {
            break;
        }
        state->movement = MOVE_FLY;
        task_kill(state->camera);
        state->camera = NULL;
        camera_bosspoint_ease_to(0, FIGHT_HEIGHT, FIGHT_DISTANCE, FIGHT_PART);
        pad_enable_player(0);
        for (shape = 0; shape < ATTACK_SHAPES; shape++) {
            collision_shape_on(boss, shape);
            collision_set_attack(boss, shape, FULL_ATTACK);
        }
        collision_shape_off(boss, SHADOW_SHAPE);
        collision_shape_off(boss, GROUND_SHAPE);
        collision_shape_off(boss, GROUND_PUSH_SHAPE);
        bigbogy_change_mode(boss, BIGBOGY_START);
        break;
    }
}

/* heading turned by step toward his bearing, the heading out from the center
 * through him. a step of 0 returns his bearing */
static Angle approach_bearing(struct task *boss, Angle heading, Angle step)
{
    struct task_work *work = boss->work;
    Angle target;

    target = (Angle)(atan2f(bigbogy_arena_center.x - work->pos.x,
                            bigbogy_arena_center.z - work->pos.z) *
                     (Float)ANGLE_PER_RADIAN) +
             HALF_TURN;
    if (!step) {
        return target;
    }
    target = angle_approach(heading, target, step);
    return target;
}

/* accelerate him toward the center, move him, and return the heading toward
 * the center */
static Angle accelerate_inward(struct task *boss, Float acceleration)
{
    struct task_work *work = boss->work;
    struct mover *mover = boss->motion.p;
    NJS_VECTOR direction;
    Angle heading;

    bigbogy_arena_distance(&work->pos, &direction);
    direction.x = -direction.x;
    direction.z = -direction.z;
    heading = atan2f(direction.x, direction.z) * (Float)ANGLE_PER_RADIAN;
    mover->acc.x += njSin(heading) * acceleration;
    mover->acc.z += njCos(heading) * acceleration;
    mover->spd.x += mover->acc.x;
    mover->spd.z += mover->acc.z;
    work->pos.x += mover->spd.x;
    work->pos.z += mover->spd.z;
    return heading;
}

/* @hack spelling: the steps below call four helpers by second names at the
 * same entries, where their own names would do */
#ifdef NONMATCHING
/* the four helpers by their own names. this side's objects don't define the
 * second names, which only symbols.txt places */
#define shadow_tangent bigbogy_home_tangent
#define shadow_change_mode bigbogy_change_mode
#define shadow_reverse bigbogy_reverse
#define shadow_accelerate bigbogy_accelerate
#else
Angle shadow_tangent(struct task *boss);
void shadow_change_mode(struct task *boss, Sint8 mode);
void shadow_reverse(struct task *boss);
void shadow_accelerate(struct task *boss, Float acceleration);
#endif

static struct bigbogy_level bigbogy_levels[FULL_HEALTH] = {
    {10, 8, 10, EVADE_ACROSS, 0.97F, 1.2F},
    {10, 6, 10, EVADE_ACROSS, 0.9F, 0.8F},
    {0, 4, 10, EVADE_WALL, 0.0F, 0.8F},
    {0, 4, 15, EVADE_FLOOR, 0.5F, 0.0F}};

/* accelerate him toward aim, the point bigbogy_arena_along finds for his
 * pos, and move him. the result is the heading away from aim */
/* @hack redundant: radius, a parameter nothing reads */
static Angle approach_circle(struct task *boss, Float acceleration,
                             Float radius)
{
    struct mover *mover;
    struct task_work *work;
    NJS_POINT3 aim;
    NJS_VECTOR direction;
    Angle heading;

    mover = boss->motion.p;
    work = boss->work;
    /* @bug logic: aim lies along pos's own direction from the origin, where
     * pos less the center was meant. a shadow west of the center heads
     * across it. 0x8c503878 passes pos's address */
    bigbogy_arena_along(&work->pos, CIRCLE_RADIUS(mover), &aim);
    direction.x = aim.x - work->pos.x;
    direction.y = 0.0F;
    direction.z = aim.z - work->pos.z;
    heading =
        (Angle)(atan2f(direction.x, direction.z) * (Float)ANGLE_PER_RADIAN);
    bigbogy_add_speed(&mover->acc, heading, acceleration);
    mover->spd.x += mover->acc.x;
    mover->spd.z += mover->acc.z;
    work->pos.x += mover->spd.x;
    work->pos.z += mover->spd.z;
    return heading + HALF_TURN;
}

/* beyond FLOOR_FAR of the center, a shadow on the floor runs in by FLOOR_STEP.
 * within FLOOR_NEAR it runs out by FLOOR_STEP, where it keeps to the floor.
 * otherwise it runs away from player 0, by FLOOR_SHORT_STEP or FLOOR_STEP.
 * where player 0 lies farther out and evasion allows, it runs to the wall
 * instead. a route to the wall ends WALL_ENTRY above mover->bottom, which the
 * boss never sets. on the wall, above WALL_MIDDLE, the shadow runs down by
 * WALL_STEP. below it, a shadow held to the wall climbs by WALL_STEP. so does
 * one while player 0 stands below LOW_PLAYER above the floor. any other
 * descends to the floor */
#define FLOOR_FAR 235.0F
#define FLOOR_NEAR 160.0F
#define FLOOR_STEP 70.0F
#define FLOOR_SHORT_STEP 30.0F
#define WALL_ENTRY 70.0F
#define WALL_STEP 50.0F
#define WALL_MIDDLE 70.0F
#define LOW_PLAYER 30.0F
/* pick the shadow's next route from the floor or the wall, and set
 * mover->target at the route's end. the call then returns TRUE. on any other
 * route it returns FALSE and changes nothing. on the wall, player 0 within
 * STAY_RANGE in x and z keeps the shadow where it is. the call then returns
 * FALSE. that holds above WALL_MIDDLE, and below it for a shadow held to the
 * wall */
static Bool choose_shadow_path(struct task *boss)
{
    struct boss_record *record;
    struct task_work *work;
    struct mover *mover;
    struct bigbogy_state *state;
    /* @hack type: floor_evasion, evasion copied into an int, where evasion
     * itself would do */
    int floor_evasion;
    Sint8 evasion;
    Sint8 path;
    Float distance;
    Float next;
    Float height;
    NJS_POINT3 position;
    NJS_VECTOR direction;

    record = boss->extra.p;
    state = record->own;
    mover = boss->motion.p;
    work = boss->work;
    evasion = bigbogy_levels[(Uint8)(record->power - 1.0F)].evasion;
    path = state->attack;
    switch (state->attack) {
    case SHADOW_FLOOR:
        floor_evasion = evasion;
        distance = bigbogy_arena_distance(&work->pos, NULL);
        if (distance > FLOOR_FAR) {
            next = distance - FLOOR_STEP;
        } else if (distance < FLOOR_NEAR) {
            if (floor_evasion == EVADE_FLOOR) {
                next = distance + FLOOR_STEP;
            } else if (bigbogy_arena_distance(&player_work[0]->pos, NULL) >
                       distance) {
                next = mover->bottom + WALL_ENTRY;
                path = SHADOW_TO_WALL;
            } else {
                next = distance + FLOOR_SHORT_STEP;
            }
        } else if (bigbogy_arena_distance(&player_work[0]->pos, NULL) >
                   distance) {
            if (floor_evasion == EVADE_FLOOR) {
                next = distance - FLOOR_SHORT_STEP;
            } else {
                next = mover->bottom + WALL_ENTRY;
                path = SHADOW_TO_WALL;
            }
        } else {
            next = distance + FLOOR_STEP;
        }
        break;
    case SHADOW_WALL:
        height = work->pos.y;
        if (work->pos.y > mover->floor_y + WALL_MIDDLE) {
            next = height - WALL_STEP;
            if (HORIZONTAL_DISTANCE(player_work[0]->pos, work->pos) <
                STAY_RANGE) {
                return FALSE;
            }
        } else if (mover->floor_y + WALL_MIDDLE > work->pos.y) {
            if (evasion == EVADE_WALL) {
                next = height + WALL_STEP;
                if (HORIZONTAL_DISTANCE(player_work[0]->pos, work->pos) <
                    STAY_RANGE) {
                    return FALSE;
                }
            } else if (player_work[0]->pos.y < mover->floor_y + LOW_PLAYER) {
                next = height + WALL_STEP;
            } else {
                next = bigbogy_arena_center.y;
                path = SHADOW_DESCEND;
            }
        } else if (height > player_work[0]->pos.y) {
            next = height + WALL_STEP;
        } else if (evasion == EVADE_WALL) {
            next = height - WALL_STEP;
        } else {
            next = bigbogy_arena_center.y;
            path = SHADOW_DESCEND;
        }
        break;
    default:
        return FALSE;
    }
    state->attack = path;
    position = work->pos;
    if (state->attack == SHADOW_FLOOR || state->attack == SHADOW_DESCEND) {
        bigbogy_arena_distance(&position, &direction);
        bigbogy_arena_along(&direction, next, &position);
        position.y = mover->floor_y;
    } else {
        bigbogy_arena_distance(&position, &direction);
        bigbogy_arena_along(&direction, WALL_RADIUS, &position);
        position.y = next;
    }
    mover->target = position;
    return TRUE;
}

#define DISTANCE(first, second)                                                \
    njSqrt(SQUARE((first).x - (second).x) + SQUARE((first).y - (second).y) +   \
           SQUARE((first).z - (second).z))
#define HEADING(x, z) ((Angle)(atan2f(x, z) * (Float)ANGLE_PER_RADIAN))

/* the shadow's camera_bossklamath settings */
#define SHADOW_VIEW_DISTANCE 60.0F
#define SHADOW_VIEW_HEIGHT 40.0F
#define SHADOW_VIEW_PART 0.3F
/* the shadow sets out for the wall at a health of WALL_HEALTH_LOW or
 * WALL_HEALTH_HIGH. at any other it starts on the floor */
#define WALL_HEALTH_LOW 2.0F
#define WALL_HEALTH_HIGH 3.0F
/* a flight ends within FLOOR_ARRIVAL of its target on the floor, or past
 * FLOOR_OVERRUN. on the wall it ends within WALL_ARRIVAL of the target's
 * height, or past WALL_OVERRUN. a route to the floor ends within
 * CIRCLE_ARRIVAL of his circle */
#define FLOOR_ARRIVAL 8.0F
#define FLOOR_OVERRUN 130.0F
#define WALL_ARRIVAL 5.0F
#define WALL_OVERRUN 55.0F
#define CIRCLE_ARRIVAL 5.0F
/* the shadow then runs on. on the wall it runs on to a radius of RUN_ON_INNER
 * where it lies within RUN_ON_LOW of the center. beyond RUN_ON_HIGH it runs on
 * to RUN_ON_OUTER. it waits again within RUN_ON_ARRIVAL of the point it runs
 * to. it also waits once player 0 lies beyond WALL_FLEE_RANGE on the wall, or
 * FLEE_RANGE on the floor */
#define RUN_ON_LOW 150.0F
#define RUN_ON_INNER 170.0F
#define RUN_ON_HIGH 240.0F
#define RUN_ON_OUTER 220.0F
#define RUN_ON_ARRIVAL 10.0F
#define WALL_FLEE_RANGE 55.0F
/* the shadow stays dug DUG_STEPS after the last dig. it fades by FADE_STEP a
 * step to FADED */
#define DUG_STEPS 30.0F
#define FADE_STEP 0.05F
#define FADED 0.2F
/* he comes out of the shadow at DIG_OUT_SPEED */
#define DIG_OUT_SPEED 5.0F
/* the shadow accelerates by SHADOW_ACCELERATION a step, and turns its ang.z
 * by SHADOW_TURN_STEP. it turns to the wall by BEARING_TURN_STEP. back to his
 * circle it accelerates by RETURN_ACCELERATION, and turns by
 * RETURN_TURN_STEP */
#define SHADOW_ACCELERATION 0.11F
#define SHADOW_TURN_STEP 0x200
#define BEARING_TURN_STEP 0x400
#define RETURN_ACCELERATION 0.09F
#define RETURN_TURN_STEP 0x100
/* the turn about the center for each unit along the wall, with pi as
 * ROUGH_PI */
#define ROUGH_PI 3.14F
#define WALL_TURN_PER_UNIT (FULL_TURN / (2.0F * ROUGH_PI * WALL_RADIUS))
/* the shadow climbs the wall to CLIMB_TOP above the floor, and goes no
 * higher than WALL_TOP */
#define CLIMB_TOP 60.0F
#define WALL_TOP 120.0F
/* the shadow, his form in the sunlight */
static void bigbogy_pursuit(struct task *boss)
{
    struct task_work *work;
    struct mover *mover;
    struct boss_record *record;
    struct bigbogy_state *state;
    Float distance;
    Float limit;
    Float speed;
    Float radius;
    Float top_speed;
    Float climb;
    int arrived;
    int hit;
    Sint8 shape;
    Sint8 player_mode;
    Angle heading;
    Angle step;

    work = boss->work;
    record = boss->extra.p;
    state = record->own;
    mover = boss->motion.p;
    switch (work->submode) {
    case PURSUIT_SETUP: {
        struct camera_bossklamath camera;
        state->movement = MOVE_SHADOW;
        work->pos.y = mover->floor_y;
        mover->gravity = 0.0F;
        mover->spd.x = mover->spd.y = mover->spd.z = 0.0F;
        TANGENT(mover) = shadow_tangent(boss);
        work->ang.y = TANGENT(mover);
        SHADOW_FRAME(mover) = 0.0F;
        for (shape = 0; shape < SHADOW_SHAPE; ++shape) {
            collision_shape_off(boss, shape);
        }
        collision_shape_on(boss, SHADOW_SHAPE);
        work->scl.x = 0.0F;
        camera.from = &record->focus;
        camera.to = (NJS_POINT3 *)&bigbogy_arena_center;
        camera.dist = SHADOW_VIEW_DISTANCE;
        camera.height = SHADOW_VIEW_HEIGHT;
        camera.part = SHADOW_VIEW_PART;
        camera.adjust = CAMERA_ADJUST_RELATIVE3;
        camera.collides = 1;
        camera.trigger_push = 1;
        camera_bossklamath_start(0, &camera);
        camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_THREE3);
        if (record->power == WALL_HEALTH_LOW ||
            record->power == WALL_HEALTH_HIGH) {
            state->attack = SHADOW_TO_WALL;
        } else {
            state->attack = SHADOW_FLOOR;
        }
        sound_play(SHADOW_START_SOUND, NULL, UNKEYED_PRIORITY, 0);
        sound_set_volume(NULL, SHADOW_START_SOUND, SHADOW_START_VOLUME);
        ++work->submode;
    }
        /* fall through */
    case PURSUIT_GROW:
        work->scl.x += SHADOW_GROWTH;
        if (work->scl.x >= SHADOW_SCALE) {
            work->scl.x = SHADOW_SCALE;
            ++work->submode;
        }
        break;
    case PURSUIT_WAIT:
        work->flags &= ~BIGBOGY_AIM_AT_TARGET;
        switch (state->attack) {
        case SHADOW_WALL:
        case SHADOW_CLIMB:
        case SHADOW_DESCEND:
            limit = FLEE_RANGE;
            distance = DISTANCE(player_work[0]->pos, work->pos);
            break;
        default:
            distance = HORIZONTAL_DISTANCE(player_work[0]->pos, work->pos);
            limit = FLEE_RANGE;
        }
        if (distance < limit) {
            if (choose_shadow_path(boss) == TRUE) {
                work->flags |= BIGBOGY_AIM_AT_TARGET;
                work->submode = PURSUIT_FLEE;
            }
        }
        break;
    case PURSUIT_FLEE:
        arrived = 0;
        if (state->attack == SHADOW_FLOOR) {
            distance = HORIZONTAL_DISTANCE(mover->target, work->pos);
            if (distance < FLOOR_ARRIVAL || distance > FLOOR_OVERRUN) {
                arrived = 1;
            }
        } else if (state->attack == SHADOW_WALL) {
            distance = _builtin_fabsf(work->pos.y - mover->target.y);
            if (distance < WALL_ARRIVAL || distance > WALL_OVERRUN) {
                arrived = 1;
            }
        } else if (state->attack == SHADOW_TO_FLOOR) {
            distance = bigbogy_arena_distance(&work->pos, NULL);
            if (distance >= CIRCLE_RADIUS(mover) - CIRCLE_ARRIVAL) {
                arrived = 1;
            }
        }
        if (arrived == 1) {
            NJS_POINT3 position;
            NJS_POINT3 next;
            NJS_VECTOR direction;
            work->flags &= ~BIGBOGY_AIM_AT_TARGET;
            /* @hack redundant: a call of bigbogy_reversed whose result
             * nothing reads */
            bigbogy_reversed();
            bigbogy_arena_advance(boss, &work->pos, -EIGHTH_TURN, &position);
            radius = bigbogy_arena_distance(&work->pos, NULL);
            if (state->attack == SHADOW_WALL) {
                if (radius < RUN_ON_LOW) {
                    radius = RUN_ON_INNER;
                } else if (radius > RUN_ON_HIGH) {
                    radius = RUN_ON_OUTER;
                }
            }
            bigbogy_arena_distance(&position, &direction);
            /* @bug logic: next lies along position's own direction from the
             * origin, where direction, its offset from the center, was meant.
             * 0x8c503f7a passes position's address */
            bigbogy_arena_along(&position, radius, &next);
            mover->target = next;
            work->submode = PURSUIT_RUN_ON;
        }
        break;
    case PURSUIT_RUN_ON:
        distance = HORIZONTAL_DISTANCE(mover->target, work->pos);
        if (distance < RUN_ON_ARRIVAL) {
            work->submode = PURSUIT_WAIT;
        } else {
            switch (state->attack) {
            case SHADOW_WALL:
            case SHADOW_CLIMB:
            case SHADOW_DESCEND:
                limit = WALL_FLEE_RANGE;
                distance = DISTANCE(player_work[0]->pos, work->pos);
                break;
            default:
                distance = HORIZONTAL_DISTANCE(player_work[0]->pos, work->pos);
#ifdef NONMATCHING
#else
                /* @hack redundant: reads and stores that no path reaches */
                if (0) {
                    if (distance) {
                    }
                    if (record) {
                    }
                    if (record) {
                    }
                    if ((Uint32)bigbogy_arena_distance) {
                    }
                    if (SHADOW_FRAME(mover) > 5.0F) {
                    }
                    if (mover->floor_y > 5.0F) {
                    }
                    if (work->flags == BIGBOGY_AIM_AT_TARGET) {
                    }
                    if (work->scl.x > 5.0F) {
                    }
                    if (work->scl.y > 5.0F) {
                    }
                    if (work->scl.z > 5.0F) {
                    }
                    if (work->pos.x > 5.0F) {
                    }
                    if (work->pos.y > 5.0F) {
                    }
                    if (work->pos.z > 5.0F) {
                    }
                    state->attack = SHADOW_FLOOR;
                    state->movement = 6;
                    state->prior_submode = 6;
                    state->daylight = 6;
                }
#endif
                limit = FLEE_RANGE;
            }
            if (distance > limit) {
                work->submode = PURSUIT_WAIT;
            }
        }
        break;
    case PURSUIT_DUG:
        /* @hack type: the timer compared with DUG_STEPS, a Float, where an
         * integer would do */
        if (work->timer++ >= DUG_STEPS) {
            work->flags &= ~BIGBOGY_AIM_AT_TARGET;
            work->submode = PURSUIT_WAIT;
        }
        break;
    case PURSUIT_FADE:
        work->scl.x -= FADE_STEP;
        if (work->scl.x <= FADED ||
            sandglass_time_left(BIGBOGY_SANDGLASS_ID) < GONE_STEPS) {
            work->scl.x = FADED;
            collision_shape_off(boss, GROUND_SHAPE);
            collision_shape_off(boss, GROUND_PUSH_SHAPE);
            collision_shape_off(boss, SHADOW_SHAPE);
            work->ang.x = 0;
            work->ang.z = 0;
            camera_remove_mode(0, camera_top(0));
            TANGENT(mover) = shadow_tangent(boss);
            THROB(mover) = 0;
            shadow_change_mode(boss, BIGBOGY_RESTORE_DARK);
        }
        return;
    }
    TANGENT(mover) = shadow_tangent(boss);
    if (sandglass_time_left(BIGBOGY_SANDGLASS_ID) < FADE_STEPS &&
        work->submode != PURSUIT_FADE) {
        FADE(mover) = 0.0F;
        work->flags |= BIGBOGY_FADING;
        work->scl.x = SHADOW_SCALE;
        work->submode = PURSUIT_FADE;
        return;
    }
    hit = 0;
    if (collision_next_player(boss)) {
        hit = 1;
    }
    if (hit == 1 || work->submode == PURSUIT_DUG) {
        NJS_POINT3 position;
        player_mode = player_work[0]->mode;
        if (state->attack == SHADOW_FLOOR || state->attack == SHADOW_TO_WALL ||
            state->attack == SHADOW_TO_FLOOR) {
            player_records[0]->action = PLAYER_MODE_HUNTER_DIG;
        } else {
            player_records[0]->action = PLAYER_MODE_HUNTER_WALL_DIG;
        }
        /* @hack spelling: the speeds and accelerations multiplied by 0.0F,
         * where assigning 0.0F would do for finite ones */
        mover->spd.x *= 0.0F;
        mover->spd.y *= 0.0F;
        mover->spd.z *= 0.0F;
        mover->acc.x *= 0.0F;
        mover->acc.y *= 0.0F;
        mover->acc.z *= 0.0F;
        SHADOW_FRAME(mover) += DIG_FRAME_STEP;
        THROB(mover) += THROB_STEP;
        work->scl.x = SHADOW_SCALE + (THROB_SIZE * njSin(THROB(mover)));
        vibration_start(0, DIG_POWER, DIG_FREQ, DIG_TIME);
        if (hit == 1) {
            if (work->submode != PURSUIT_DUG) {
                state->prior_submode = work->submode;
            }
            if (work->submode != PURSUIT_FADE) {
                work->submode = PURSUIT_DUG;
            }
            work->timer = 0;
        }
        if (player_mode == PLAYER_MODE_HUNTER_DIG_OUT ||
            player_mode == PLAYER_MODE_HUNTER_WALL_DIG_OUT) {
            switch (state->attack) {
            case SHADOW_FLOOR:
                mover->spd.x = njSin(TANGENT(mover)) * DIG_OUT_SPEED;
                mover->spd.z = njCos(TANGENT(mover)) * DIG_OUT_SPEED;
                bigbogy_arena_advance(boss, &work->pos, -EIGHTH_TURN,
                                      &mover->target);
                break;
            case SHADOW_WALL:
                mover->spd.x = njSin(-work->ang.z) * DIG_OUT_SPEED;
                mover->spd.z = njCos(-work->ang.z) * DIG_OUT_SPEED;
                bigbogy_arena_project(boss, &work->pos, &mover->target);
                break;
            default:
                bigbogy_arena_distance(&work->pos, &position);
                heading = HEADING(position.x, position.z);
                mover->spd.x = njSin(heading) * DIG_OUT_SPEED;
                mover->spd.z = njCos(heading) * DIG_OUT_SPEED;
                bigbogy_arena_project(boss, &work->pos, &mover->target);
                break;
            }
            work->ang.x = 0;
            work->ang.z = 0;
            THROB(mover) = 0;
            shadow_change_mode(boss, BIGBOGY_GROUNDED);
        }
        sound_play_repeating_at(SHADOW_SOUND, boss, KEYED_PRIORITY, 0,
                                digging_sound_period.steps, &work->pos);
    } else {
        NJS_POINT3 position;
        /* @hack redundant: unused, 12 bytes that only size the frame */
        Uint8 unused[12];

        SHADOW_FRAME(mover) += SHADOW_FRAME_STEP;
        TOP_SPEED(mover) =
            bigbogy_levels[(Uint8)(record->power - 1.0F)].speed_scale *
            player_records[0]->consts.top_speed;
        /* @hack redundant: a copy of player 0's pos that nothing reads */
        position = player_work[0]->pos;
        (void)position;
        mover->spd.x *= DRAG;
        mover->spd.z *= DRAG;
        THROB(mover) = 0;
        switch (state->attack) {
        case SHADOW_FLOOR: {
            NJS_VECTOR *velocity = &mover->spd;
            NJS_POINT3 target;
            NJS_VECTOR direction;

            work->ang.x = 0;
            mover->acc.x = SHADOW_ACCELERATION;
            if (!(work->flags & BIGBOGY_AIM_AT_TARGET)) {
                steer_along_tangent(boss);
                work->ang.y = TANGENT(mover);
            } else {
                target = mover->target;
                heading =
                    HEADING(target.x - work->pos.x, target.z - work->pos.z);
                velocity->x += njSin(heading) * SHADOW_ACCELERATION;
                velocity->z += njCos(heading) * SHADOW_ACCELERATION;
                direction = mover->spd;
                direction.y = 0.0F;
                speed = njScalor(&direction);
                if (speed > TOP_SPEED(mover)) {
                    top_speed = TOP_SPEED(mover);
                    njUnitVector(&direction);
                    velocity->x = direction.x * top_speed;
                    velocity->z = direction.z * top_speed;
                }
                work->ang.y = heading;
            }
            work->pos.x += mover->spd.x;
            work->pos.z += mover->spd.z;
            heading = HEADING(mover->spd.x, mover->spd.z) + HALF_TURN;
            work->ang.z =
                angle_approach(work->ang.z, heading, SHADOW_TURN_STEP);
        } break;
        case SHADOW_WALL:
        case SHADOW_DESCEND:
        case SHADOW_CLIMB: {
            NJS_VECTOR *velocity;
            work->ang.x = QUARTER_TURN;
            mover->acc.x = SHADOW_ACCELERATION;
            velocity = &mover->spd;
            TOP_SPEED(mover) =
                bigbogy_levels[(Uint8)(record->power - 1.0F)].wall_speed_scale *
                WALL_SPEED;
            {
                NJS_VECTOR horizontal;
                horizontal = *velocity;
                horizontal.y = 0.0F;
                speed = njScalor(&horizontal) + mover->acc.x;
                if (speed > TOP_SPEED(mover)) {
                    speed = TOP_SPEED(mover);
                }
            }
            climb = bigbogy_wall_speed;
            if (!(work->flags & BIGBOGY_AIM_AT_TARGET)) {
                NJS_VECTOR horizontal;
                /* @hack redundant: unused_orbit, 12 bytes that only size the
                 * frame */
                Uint8 unused_orbit[12];
                NJS_POINT3 previous;
                NJS_POINT3 orbit;

                horizontal = *velocity;
                horizontal.y = 0.0F;
                step = speed * WALL_TURN_PER_UNIT;
                if (work->flags & BIGBOGY_REVERSED) {
                    step = -step;
                }
                heading = HEADING(work->pos.x - mover->home.x,
                                  work->pos.z - mover->home.z);
                previous = work->pos;
                bigbogy_arena_at_angle(heading + step, WALL_RADIUS, &orbit);
                orbit.y = work->pos.y;
                work->pos = orbit;
                velocity->x = work->pos.x - previous.x;
                velocity->z = work->pos.z - previous.z;
                (void)unused_orbit;
            } else {
                if (work->pos.y < mover->target.y) {
                    work->pos.y += climb;
                } else if (work->pos.y > mover->target.y) {
                    work->pos.y -= climb;
                }
            }
            work->ang.y = TANGENT(mover);
            work->ang.z = HEADING(mover->home.x - work->pos.x,
                                  mover->home.z - work->pos.z);
            speed = bigbogy_wall_speed;
            if (state->attack == SHADOW_CLIMB) {
                if (work->pos.y < mover->floor_y + CLIMB_TOP) {
                    work->pos.y += speed;
                } else {
                    state->attack = SHADOW_WALL;
                }
            } else if (state->attack == SHADOW_DESCEND) {
                work->pos.y -= speed;
                if (work->pos.y <= mover->floor_y) {
                    work->pos.y = mover->floor_y;
                    state->attack = SHADOW_TO_FLOOR;
                }
            }
            if (work->pos.y > mover->floor_y + WALL_TOP) {
                work->pos.y = mover->floor_y + WALL_TOP;
            }
        } break;
        case SHADOW_TO_WALL: {
            NJS_VECTOR direction;
            NJS_VECTOR position_on_wall;
            work->ang.x = 0;
            accelerate_inward(boss, SHADOW_ACCELERATION);
            work->ang.z =
                approach_bearing(boss, work->ang.z, BEARING_TURN_STEP);
            distance = bigbogy_arena_distance(&work->pos, &direction);
            if (distance <= WALL_RADIUS) {
                bigbogy_arena_along(&direction, WALL_RADIUS, &position_on_wall);
                work->pos = position_on_wall;
                work->pos.y = mover->floor_y;
                state->attack = SHADOW_CLIMB;
            }
        } break;
        case SHADOW_TO_FLOOR:
            work->ang.x = 0;
            heading = approach_circle(boss, RETURN_ACCELERATION,
                                      CIRCLE_RADIUS(mover));
            work->ang.z =
                angle_approach(work->ang.z, heading, RETURN_TURN_STEP);
            distance = bigbogy_arena_distance(&work->pos, NULL);
            if (distance >= bigbogy_circle_radius) {
                state->attack = SHADOW_FLOOR;
            }
            break;
        }
        /* @hack spelling: the accelerations multiplied by 0.0F, where assigning
         * 0.0F would do for finite ones */
        mover->acc.x *= 0.0F;
        mover->acc.y *= 0.0F;
        mover->acc.z *= 0.0F;
        sound_play_repeating_at(SHADOW_SOUND, boss, KEYED_PRIORITY, 0,
                                shadow_sound_period.steps, &work->pos);
        (void)unused;
    }
    if (SHADOW_FRAME(mover) > SHADOW_FRAMES) {
        SHADOW_FRAME(mover) -= SHADOW_FRAMES;
    }
}

/* on the ground he falls by GROUND_GRAVITY. dug out more than WALL_DIG above
 * the floor, he starts LANDING_RADIUS from the center. he leaps out from the
 * wall at DUG_OUT_LEAP. dug out below LOW_DIG above the floor, he also rises at
 * DUG_OUT_RISE. dug out of the floor, he starts DUG_DEPTH below it and rises at
 * DUG_OUT_RISE */
#define GROUND_GRAVITY 0.05F
#define WALL_DIG 1.0F
#define DUG_OUT_LEAP 6.0F
#define DUG_OUT_RISE 2.0F
#define LOW_DIG 40.0F
#define LANDING_RADIUS 120.0F
#define DUG_DEPTH 20.0F
/* the camera_bosspoint settings on the ground */
#define GROUND_VIEW_HEIGHT 40.0F
#define GROUND_VIEW_DISTANCE 80.0F
#define GROUND_VIEW_PART 0.8F
/* he lands within LAND_STEPS + 1 steps, or once below LANDED_SPEED in x and z.
 * he then runs for at most TO_TARGET_STEPS + 1 steps to the point of his circle
 * EIGHTH_TURN ahead. within TARGET_NEAR of it he turns to his circle, and
 * within TARGET_ARRIVAL he stops there. he then runs at RUN_SPEED of player 0's
 * top speed */
#define LAND_STEPS 18
#define LANDED_SPEED 2.0F
#define TO_TARGET_STEPS 120
#define TARGET_NEAR 20.0F
#define TARGET_ARRIVAL 10.0F
#define RUN_SPEED 0.5F
/* his acceleration and his turn a step, in each phase on the ground. near the
 * point he turns by ARRIVE_TURN_STEP, and as he fades by FLEE_TURN_STEP */
#define TO_TARGET_ACCELERATION 0.15F
#define TO_TARGET_TURN_STEP 0x800
#define ARRIVE_TURN_STEP 0x200
#define RUN_ACCELERATION 0.66F
#define RUN_TURN_STEP 0x400
#define FLEE_ACCELERATION 0.167F
#define FLEE_TURN_STEP 0x800
#define RECOIL_ACCELERATION 0.157F
/* a hit throws him RECOIL_KNOCK across the ground and RECOIL_RISE up. he goes
 * along his circle's heading on player 0's side. he then aims RECOIL_REACH that
 * way, at RECOIL_SPEED of player 0's top speed. the recoil lasts RECOIL_STEPS +
 * 1 steps. until RECOIL_RUN steps are left, he swings about his circle's
 * heading by RECOIL_SWING, RECOIL_SWING_STEP a step. after a hit in GROUND_FLEE
 * he flips about x instead. the flip starts at FLIP_START steps left, and turns
 * him by FLIP_STEP a step. from RECOIL_RUN steps left, it ends by FLIP_END_STEP
 * a step. at RECOIL_RUN steps left MOTION_RUN links in over RUN_LINK_FRAMES */
#define RECOIL_KNOCK 2.0F
#define RECOIL_RISE 1.7F
#define RECOIL_REACH 10.0F
#define RECOIL_SPEED 3.0F
#define RECOIL_STEPS 180
#define RECOIL_RUN 120
#define RECOIL_SWING 0x1800
#define RECOIL_SWING_STEP 0x800
#define FLIP_START 174.0F
#define FLIP_STEP 0xa00
#define FLIP_END_STEP 0x1000
#define RUN_LINK_FRAMES 15
/* he fades by GROUND_FADE_STEP a step */
#define GROUND_FADE_STEP 0.025F
/* a guard no attack is above */
#define FULL_GUARD 3
/* he turns his way about his circle where player 0 stands ahead of him. ahead
 * is within TURN_AWAY_RANGE in x and z, and TURN_AWAY_TURN of his circle's
 * heading */
#define TURN_AWAY_RANGE 150.0F
#define TURN_AWAY_TURN 0x3000
/* the hit that ends his health leaves him DEFEAT_RISE a step up */
#define DEFEAT_RISE 1.0F
/* the boss on the ground */
static void bigbogy_grounded(struct task *boss)
{
    struct task_work *work;
    struct mover *mover;
    struct boss_record *record;
    struct bigbogy_state *state;
    struct motion_control *motion;
    Float distance;
    Angle heading;
    Angle player_heading;
    Sint8 shape;

    work = boss->work;
    record = boss->extra.p;
    state = record->own;
    mover = boss->motion.p;
    motion = record->block;
    switch (work->submode) {
    case GROUND_SETUP: {
        NJS_POINT3 landing;
        NJS_VECTOR direction;
        state->movement = MOVE_GROUND;
        mover->gravity = -GROUND_GRAVITY;
        if (work->pos.y > bigbogy_arena_center.y + WALL_DIG) {
            bigbogy_arena_distance(&work->pos, &direction);
            njUnitVector(&direction);
            mover->spd.x = direction.x * DUG_OUT_LEAP;
            mover->spd.y = 0.0F;
            mover->spd.z = direction.z * DUG_OUT_LEAP;
            if (work->pos.y < mover->floor_y + LOW_DIG) {
                mover->spd.y = DUG_OUT_RISE;
            }
            work->ang.y = approach_bearing(boss, work->ang.y, 0);
            bigbogy_arena_at_radius(&work->pos, LANDING_RADIUS, &landing);
            work->pos.x = landing.x;
            work->pos.z = landing.z;
        } else {
            work->pos.y = mover->floor_y - DUG_DEPTH;
            mover->spd.y = DUG_OUT_RISE;
            work->ang.y = shadow_tangent(boss);
        }
        motion_set(motion, MOTION_DUG_OUT);
        collision_shape_on(boss, GROUND_SHAPE);
        collision_shape_on(boss, GROUND_PUSH_SHAPE);
        collision_shape_off(boss, SHADOW_SHAPE);
        TOP_SPEED(mover) = player_records[0]->consts.top_speed;
        SHADOW_FRAME(mover) = 0.0F;
        work->scl.x = work->scl.y;
        camera_remove_mode(0, camera_top(0));
        camera_bosspoint_ease_to(0, GROUND_VIEW_HEIGHT, GROUND_VIEW_DISTANCE,
                                 GROUND_VIEW_PART);
        work->flags |= BIGBOGY_AIM_AT_TARGET;
        work->timer = LAND_STEPS;
        voice_play_priority_2(GROUNDED_VOICE);
        ++work->submode;
    }
        /* fall through */
    case GROUND_LAND:
        if (!work->timer-- || njSqrt(SQUARE(mover->spd.x) +
                                     SQUARE(mover->spd.z)) < LANDED_SPEED) {
            work->timer = TO_TARGET_STEPS;
            bigbogy_arena_advance(boss, &work->pos, -EIGHTH_TURN,
                                  &mover->target);
            ++work->submode;
        }
        break;
    case GROUND_TO_TARGET:
        shadow_accelerate(boss, TO_TARGET_ACCELERATION);
        distance = HORIZONTAL_DISTANCE(mover->target, work->pos);
        if (distance > TARGET_NEAR) {
            heading = HEADING(mover->target.x - work->pos.x,
                              mover->target.z - work->pos.z);
            work->ang.y =
                angle_approach(work->ang.y, heading, TO_TARGET_TURN_STEP);
            /* @hack redundant: a turn along spd that no path reaches */
            if (0) {
                work->ang.y = HEADING(mover->spd.x, mover->spd.z);
            }
        } else {
            face_tangent(boss, ARRIVE_TURN_STEP);
        }
        if (!work->timer-- || distance < TARGET_ARRIVAL) {
            work->timer = 0;
            collision_set_guard(boss, GROUND_SHAPE, 0);
            work->flags &= ~BIGBOGY_AIM_AT_TARGET;
            TOP_SPEED(mover) = player_records[0]->consts.top_speed * RUN_SPEED;
            ++work->submode;
        }
        break;
    case GROUND_RUN:
        shadow_accelerate(boss, RUN_ACCELERATION);
        face_tangent(boss, RUN_TURN_STEP);
        if (!(motion->flags & MOTION_LINKING) &&
            (motion->index == MOTION_RUN || motion->index == MOTION_WALK)) {
            sound_play_repeating_at(RUN_SOUND, boss, KEYED_PRIORITY, RUN_VOLUME,
                                    grounded_sound_period.steps, &work->pos);
        }
        if (sandglass_time_left(BIGBOGY_SANDGLASS_ID) < FADE_STEPS) {
            FADE(mover) = 0.0F;
            work->flags |= BIGBOGY_FADING;
            work->submode = GROUND_FADE;
        }
        break;
    case GROUND_FLEE:
        shadow_accelerate(boss, FLEE_ACCELERATION);
        face_tangent(boss, FLEE_TURN_STEP);
        if (!(motion->flags & MOTION_LINKING) &&
            (motion->index == MOTION_RUN || motion->index == MOTION_WALK)) {
            sound_play_repeating_at(RUN_SOUND, boss, KEYED_PRIORITY, RUN_VOLUME,
                                    fleeing_sound_period.steps, &work->pos);
        }
        if (sandglass_time_left(BIGBOGY_SANDGLASS_ID) < FADE_STEPS) {
            FADE(mover) = 0.0F;
            work->flags |= BIGBOGY_FADING;
            work->submode = GROUND_FADE;
        }
        break;
    case GROUND_RECOIL:
        if (state->prior_submode != GROUND_FLEE) {
            if (work->timer > RECOIL_RUN) {
                swing_about_tangent(boss, 0, RECOIL_SWING, RECOIL_SWING_STEP);
            } else {
                face_tangent(boss, RUN_TURN_STEP);
                if (!(motion->flags & MOTION_LINKING) &&
                    (motion->index == MOTION_RUN ||
                     motion->index == MOTION_WALK)) {
                    sound_play_repeating_at(
                        RUN_SOUND, boss, KEYED_PRIORITY, RUN_VOLUME,
                        fleeing_sound_period.steps, &work->pos);
                }
            }
        }
        /* @hack type: the timer compared with FLIP_START, a Float, where an
         * integer would do */
        else if (work->timer > RECOIL_RUN && work->timer < FLIP_START) {
            FLIP(mover) -= FLIP_STEP;
            if (FLIP(mover) <= -FULL_TURN) {
                FLIP(mover) += FULL_TURN;
            }
        } else if (work->timer <= RECOIL_RUN) {
            face_tangent(boss, RUN_TURN_STEP);
            if (motion->index == MOTION_RUN || motion->index == MOTION_WALK) {
                sound_play_repeating_at(RUN_SOUND, boss, KEYED_PRIORITY,
                                        RUN_VOLUME, fleeing_sound_period.steps,
                                        &work->pos);
            }
            if (FLIP(mover)) {
                FLIP(mover) -= FLIP_END_STEP;
                if (FLIP(mover) <= -FULL_TURN) {
                    FLIP(mover) = 0;
                }
            }
        }
        shadow_accelerate(boss, RECOIL_ACCELERATION);
        if (work->timer == RECOIL_RUN) {
            motion_link_frames(motion, MOTION_RUN, RUN_LINK_FRAMES);
        }
        if (!work->timer--) {
            FLIP(mover) = 0;
            collision_set_guard(boss, GROUND_SHAPE, 0);
            work->flags &= ~WORK_HURT;
            work->submode = GROUND_FLEE;
        }
        break;
    case GROUND_FADE:
        FADE(mover) -= GROUND_FADE_STEP;
        face_tangent(boss, FLEE_TURN_STEP);
        if (motion->index == MOTION_RUN) {
            sound_play_repeating_at(RUN_SOUND, boss, KEYED_PRIORITY, RUN_VOLUME,
                                    fleeing_sound_period.steps, &work->pos);
        } else if (motion->index == MOTION_WALK) {
            sound_play_repeating_at(RUN_SOUND, boss, KEYED_PRIORITY, RUN_VOLUME,
                                    grounded_sound_period.steps, &work->pos);
        }
        if (FADE(mover) <= -1.0F ||
            sandglass_time_left(BIGBOGY_SANDGLASS_ID) < GONE_STEPS) {
            FADE(mover) = -1.0F;
            collision_shape_off(boss, GROUND_SHAPE);
            collision_shape_off(boss, GROUND_PUSH_SHAPE);
            collision_shape_off(boss, SHADOW_SHAPE);
            collision_set_guard(boss, GROUND_SHAPE, FULL_GUARD);
            work->flags &= ~BIGBOGY_AIM_AT_TARGET;
            shadow_change_mode(boss, BIGBOGY_RESTORE_DARK);
            return;
        }
        break;
    }
    if (work->submode != GROUND_LAND && work->submode != GROUND_TO_TARGET &&
        !(work->flags & BIGBOGY_AIM_AT_TARGET)) {
        struct bigbogy_player player;
        bigbogy_player_relative(boss, &player);
        heading = mover_player_heading(boss, 0);
        if (player.horizontal < TURN_AWAY_RANGE) {
            if (angle_distance(TANGENT(mover), heading) < TURN_AWAY_TURN) {
#ifdef NONMATCHING
#else
                /* @hack redundant: turn_away, a label nothing jumps to */
            turn_away: /* NOLINT(clang-diagnostic-unused-label) */
#endif
                shadow_reverse(boss);
            }
        }
    }
    /* a collision test sets WORK_HURT with each hit, and it stays set only
     * through the recoil */
    if (collision_hurt(boss)) {
        if (work->submode == GROUND_RUN || work->submode == GROUND_FLEE) {
            NJS_VECTOR direction;
            collision_set_guard(boss, GROUND_SHAPE, FULL_GUARD);
            voice_play_priority_2(HURT_VOICE);
            sound_play(HURT_SOUND, NULL, UNKEYED_PRIORITY, 0);
            sound_set_volume(NULL, HURT_SOUND, HURT_VOLUME);
            work->timer = RECOIL_STEPS;
            TOP_SPEED(mover) =
                player_records[0]->consts.top_speed * RECOIL_SPEED;
            motion_set(motion, MOTION_RECOIL_HOLD);
            work->ang.y = TANGENT(mover);
            heading = bigbogy_arena_tangent(NULL, &work->pos);
            player_heading = HEADING(player_work[0]->pos.x - work->pos.x,
                                     player_work[0]->pos.z - work->pos.z);
            if (angle_distance(heading, player_heading) > QUARTER_TURN) {
                heading += HALF_TURN;
                shadow_reverse(boss);
                direction.x = njSin(heading);
                direction.z = njCos(heading);
            } else {
                direction.x = njSin(heading);
                direction.z = njCos(heading);
            }
            mover->target.x = work->pos.x + (direction.x * RECOIL_REACH);
            mover->target.y = work->pos.y;
            mover->target.z = work->pos.z + (direction.z * RECOIL_REACH);
            mover->spd.x = direction.x * RECOIL_KNOCK;
            mover->spd.y = RECOIL_RISE;
            mover->spd.z = direction.z * RECOIL_KNOCK;
            state->prior_submode = work->submode;
            work->flags &= ~BIGBOGY_AIM_AT_TARGET;
            --record->power;
            vibration_start(0, HIT_POWER, HIT_FREQ, HIT_TIME);
            if (record->power == 0.0F) {
                mover->spd.x = mover->spd.z = 0.0F;
                mover->spd.y = DEFEAT_RISE;
                /* @hack spelling: the accelerations multiplied by 0.0F, where
                 * assigning 0.0F would do for finite ones */
                mover->acc.x *= 0.0F;
                mover->acc.y *= 0.0F;
                mover->acc.z *= 0.0F;
                /* @hack type: shape compared as an unsigned int, where its
                 * own type would do */
                for (shape = 0; (unsigned int)shape < BIGBOGY_SHAPES; ++shape) {
                    collision_set_attack(boss, shape, 0);
                    collision_set_guard(boss, shape, FULL_GUARD);
                }
                work->flags &= ~WORK_HURT;
                shadow_change_mode(boss, BIGBOGY_DEFEAT);
                return;
            }
            work->submode = GROUND_RECOIL;
            work->flags |= WORK_HURT;
        } else {
            if (work->flags & WORK_HURT) {
                if (work->submode != GROUND_RECOIL) {
                    work->flags &= ~WORK_HURT;
                }
            }
        }
    } else {
        if (work->flags & WORK_HURT) {
            if (work->submode != GROUND_RECOIL) {
                work->flags &= ~WORK_HURT;
            }
        }
    }
}

/* the next scale of one axis as he burns. above BURN_SCALE_HIGH it starts to
 * shrink, and below BURN_SCALE_LOW, or from rest, to grow. *speed keeps the
 * rate, from BURN_SPEED - BURN_SPREAD to BURN_SPEED a step */
static Float defeat_scale(Float value, Float *speed)
{
    /* @hack spelling: result, a local for the sum, where returning the sum
     * would do */
    Float result;
    if (value > BURN_SCALE_HIGH) {
        *speed = ((Float)rand() * (BURN_SPREAD * RAND_SCALE)) - BURN_SPEED;
    } else if (value < BURN_SCALE_LOW) {
        *speed = BURN_SPEED - ((Float)rand() * (BURN_SPREAD * RAND_SCALE));
    } else if (*speed == 0.0F) {
        *speed = BURN_SPEED - ((Float)rand() * (BURN_SPREAD * RAND_SCALE));
    }
    result = value + *speed;
    return result;
}

/* at his defeat he slides off at DEFEAT_SLIDE along his circle's heading, and
 * MOTION_RECOIL_HOLD links in over DEFEAT_LINK_FRAMES. his rise slows by
 * DEFEAT_FALL a step. below SPIN_SPEED he spins by DEFEAT_SPIN_STEP a step, and
 * below SLIDE_STOP he stops. he then spins for SPIN_STEPS + 1 steps. he rises
 * by DEFEAT_RISE_STEP a step to DEFEAT_HEIGHT above the floor. he turns to
 * player 0 until within FACING_TURN, and burns BURN_WAIT + 1 steps later */
#define DEFEAT_SLIDE 7.5F
#define DEFEAT_LINK_FRAMES 42
#define DEFEAT_FALL 0.05F
#define SPIN_SPEED 1.5F
#define DEFEAT_SPIN_STEP 0x1000
#define SLIDE_STOP 0.5F
#define SPIN_STEPS 120
#define DEFEAT_RISE_STEP 0.5F
#define DEFEAT_HEIGHT 35.0F
#define FACING_TURN 0x100
#define BURN_WAIT 18
/* as he burns, a flame rises by FLAME_CHANCE a step. it lies within
 * FLAME_SPREAD of him in x and z, and up to FLAME_RISE above his pos. he fades
 * by BURN_FADE_STEP a step, for at least BURN_STEPS. the stage clears
 * CLEAR_STEPS + 1 steps after he has faded */
#define FLAME_CHANCE 0.2F
#define FLAME_SPREAD 20.0F
#define FLAME_RISE 40.0F
#define BURN_FADE_STEP 0.01F
#define BURN_STEPS 120
#define CLEAR_STEPS 60
/* camera_bosspoint's settings at his defeat: the DEFEAT_ ones as he slides, and
 * the BURN_ ones as he burns. CLEAR_VIEW_PART is its part once he has burned
 * out */
#define DEFEAT_VIEW_HEIGHT 25.0F
#define DEFEAT_VIEW_DISTANCE 40.0F
#define DEFEAT_VIEW_PART 0.8F
#define BURN_VIEW_HEIGHT 40.0F
#define BURN_VIEW_DISTANCE 90.0F
#define BURN_VIEW_PART 0.5F
#define CLEAR_VIEW_PART 0.4F
static void bigbogy_defeat(struct task *boss)
{
    struct task_work *work;
    struct mover *mover;
    struct boss_record *record;
    struct bigbogy_state *state;
    struct motion_control *motion;
    Float speed;
    Angle heading;

    record = boss->extra.p;
    work = boss->work;
    state = record->own;
    mover = boss->motion.p;
    motion = record->block;
    switch (work->submode) {
    case 0:
        heading = shadow_tangent(boss);
        mover->spd.x = njSin(heading) * DEFEAT_SLIDE;
        mover->spd.z = njCos(heading) * DEFEAT_SLIDE;
        state->movement = MOVE_DEFEAT;
        mover->gravity = 0.0F;
        motion_link_frames(motion, MOTION_RECOIL_HOLD, DEFEAT_LINK_FRAMES);
        camera_bosspoint_ease_to(0, DEFEAT_VIEW_HEIGHT, DEFEAT_VIEW_DISTANCE,
                                 DEFEAT_VIEW_PART);
        work->flags &= ~WORK_HURT;
        voice_play_priority_2(DEFEAT_VOICE);
        ++work->submode;
        /* fall through */
    case 1:
        work->pos.x += mover->spd.x;
        work->pos.y += mover->spd.y;
        work->pos.z += mover->spd.z;
        mover->spd.x *= DRAG;
        mover->spd.y *= DRAG;
        mover->spd.z *= DRAG;
        if (mover->spd.y > 0.0F) {
            mover->spd.y += -DEFEAT_FALL;
        } else {
            mover->spd.y = 0.0F;
        }
        speed = njSqrt(SQUARE(mover->spd.x) + SQUARE(mover->spd.z));
        if (speed < SPIN_SPEED) {
            work->ang.y = work->ang.y + DEFEAT_SPIN_STEP;
        }
        if (speed < SLIDE_STOP) {
            mover->spd.x = 0.0F;
            mover->spd.y = 0.0F;
            mover->spd.z = 0.0F;
            work->timer = SPIN_STEPS;
            ++work->submode;
        }
        if (game_state.steps % DEFEAT_SOUND_PERIOD == 0) {
            sound_play(DEFEAT_SOUND, NULL, UNKEYED_PRIORITY, 0);
            sound_set_volume(
                NULL, DEFEAT_SOUND,
                (int)((Float)rand() * (DEFEAT_VOLUME_SPREAD * RAND_SCALE)) +
                    DEFEAT_VOLUME);
        }
        if (game_state.steps % DEFEAT_RUMBLE_PERIOD == 0) {
            vibration_start(0, DEFEAT_POWER, DEFEAT_FREQ, DEFEAT_TIME);
        }
        break;
    case 2:
        work->ang.y += DEFEAT_SPIN_STEP;
        if (work->pos.y < bigbogy_arena_center.y + DEFEAT_HEIGHT) {
            work->pos.y += DEFEAT_RISE_STEP;
        } else if (work->pos.y > bigbogy_arena_center.y + DEFEAT_HEIGHT) {
            work->pos.y = bigbogy_arena_center.y + DEFEAT_HEIGHT;
        }
        if (!work->timer--) {
            work->timer = 48;
            ++work->submode;
        }
        if (game_state.steps % DEFEAT_SOUND_PERIOD == 0) {
            sound_play(DEFEAT_SOUND, NULL, UNKEYED_PRIORITY, 0);
            sound_set_volume(
                NULL, DEFEAT_SOUND,
                (int)((Float)rand() * (DEFEAT_VOLUME_SPREAD * RAND_SCALE)) +
                    DEFEAT_VOLUME);
        }
        if (game_state.steps % DEFEAT_RUMBLE_PERIOD == 0) {
            vibration_start(0, DEFEAT_POWER, DEFEAT_FREQ, DEFEAT_TIME);
        }
        break;
    case 3:
        heading = angle_distance(work->ang.y, mover_player_heading(boss, 0));
        if (heading < EIGHTH_TURN) {
            heading = mover_turn_to_player_by(boss, WATCH_STEP, 0);
        } else {
            work->ang.y += DEFEAT_SPIN_STEP;
        }
        if (heading < FACING_TURN) {
            ++work->submode;
            work->timer = BURN_WAIT;
        }
        break;
    case 4:
        if (!work->timer--) {
            work->flags |= BIGBOGY_FADING;
            work->timer = BURN_STEPS;
            FADE(mover) = 0.0F;
            camera_bosspoint_ease_to(0, BURN_VIEW_HEIGHT, BURN_VIEW_DISTANCE,
                                     BURN_VIEW_PART);
            vibration_start(0, BURN_POWER, BURN_FREQ, BURN_TIME);
            ++work->submode;
        }
        break;
    case 5:
        if ((Float)rand() * RAND_SCALE < FLAME_CHANCE) {
            NJS_POINT3 position;
            position = work->pos;
            position.x += FLAME_SPREAD - ((Float)rand() *
                                          ((2.0F * FLAME_SPREAD) * RAND_SCALE));
            position.y +=
                FLAME_RISE - ((Float)rand() * (FLAME_RISE * RAND_SCALE));
            position.z += FLAME_SPREAD - ((Float)rand() *
                                          ((2.0F * FLAME_SPREAD) * RAND_SCALE));
            bigbogy_fire_wall(boss, &position, 1, 1.0F);
        }
        if (game_state.steps % BURN_SOUND_PERIOD == 0) {
            sound_play(DEFEAT_SOUND, NULL, UNKEYED_PRIORITY, 0);
            sound_set_volume(
                NULL, DEFEAT_SOUND,
                (int)((Float)rand() * (BURN_VOLUME_SPREAD * RAND_SCALE)));
        }
        work->scl.x = defeat_scale(work->scl.x, &SCALE_SPEED(mover).x);
        work->scl.y = defeat_scale(work->scl.y, &SCALE_SPEED(mover).y);
        work->scl.z = defeat_scale(work->scl.z, &SCALE_SPEED(mover).z);
        FADE(mover) -= BURN_FADE_STEP;
        if (work->timer > 0) {
            --work->timer;
        }
        if (FADE(mover) < -1.0F) {
            FADE(mover) = -1.0F;
            if (work->timer == 0) {
                work->timer = CLEAR_STEPS;
                camera_bosspoint_ease_to(0, BURN_VIEW_HEIGHT,
                                         BURN_VIEW_DISTANCE, CLEAR_VIEW_PART);
                ++work->submode;
            }
        }
        break;
    case 6:
        if (!work->timer--) {
            stage_clear(0);
        }
        break;
    }
}

static const NJS_POINT3 attachment_origin = {0.0F, 0.0F, 0.0F};

static void bigbogy_step(struct task *boss)
{
    struct task_work *work = boss->work;
    struct boss_record *record = boss->extra.p;
    /* @hack redundant: state, the own block read on entry, which nothing
     * uses */
    struct bigbogy_state *state = record->own;

    if (game_state.time_stopped) {
        return;
    }
    if (game_state.mini_event_running) {
        bigbogy_idle(boss);
        return;
    }
    switch (work->mode) {
    case BIGBOGY_INTRO:
        bigbogy_intro(boss);
        break;
    case BIGBOGY_START:
        bigbogy_start(boss);
        break;
    case BIGBOGY_CIRCLE:
        bigbogy_circle(boss);
        break;
    case BIGBOGY_HOLDER_DASH:
        bigbogy_holder_dash(boss);
        break;
    case BIGBOGY_THROW:
        bigbogy_throw(boss);
        break;
    case BIGBOGY_PLAYER_HIT:
        bigbogy_player_hit(boss);
        break;
    case BIGBOGY_BREATH:
        bigbogy_breathe_fire(boss);
        break;
    case BIGBOGY_SUNLIGHT:
        bigbogy_sunlight(boss);
        break;
    case BIGBOGY_RESTORE_DARK:
        bigbogy_restore_dark(boss);
        break;
    case BIGBOGY_BITE:
        bigbogy_bite(boss);
        break;
    case BIGBOGY_PURSUIT:
        bigbogy_pursuit(boss);
        break;
    case BIGBOGY_GROUNDED:
        bigbogy_grounded(boss);
        break;
    case BIGBOGY_DEFEAT:
        bigbogy_defeat(boss);
        break;
    }
    motion_step(record->block);
    bigbogy_move(boss);
    bigbogy_collide(boss);
}

static void capture_boss_node(NJS_CNK_OBJECT *object)
{
    NJS_POINT3 view;
    NJS_POINT3 origin;
    origin = attachment_origin;
    if (object == &bigbogy_root_node) {
        njCalcPoint(NULL, &origin, &view);
        njCalcPoint(&bigbogy_view_to_world, &view, &bigbogy_root_point);
    }
    if (object == &bigbogy_hand_node_1) {
        njCalcPoint(NULL, &origin, &view);
        njCalcPoint(&bigbogy_view_to_world, &view, &bigbogy_hand_1);
    }
    if (object == &bigbogy_hand_node_0) {
        njCalcPoint(NULL, &origin, &view);
        njCalcPoint(&bigbogy_view_to_world, &view, &bigbogy_hand_0);
    }
    if (object == &bigbogy_part_object_0) {
        njCalcPoint(NULL, &origin, &view);
        njCalcPoint(&bigbogy_view_to_world, &view, &bigbogy_part_point_0);
    }
    if (object == &bigbogy_part_object_1) {
        njCalcPoint(NULL, &origin, &view);
        njCalcPoint(&bigbogy_view_to_world, &view, &bigbogy_part_point_1);
    }
    if (object == &bigbogy_mouth_node) {
        njCalcPoint(NULL, &origin, &view);
        njCalcPoint(&bigbogy_view_to_world, &view, &bigbogy_mouth_point);
    }
}

static void capture_boss_pose(struct task *boss)
{
    /* @hack redundant: the reads of record, state and work, which nothing
     * uses */
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;
    struct task_work *work = boss->work;
    njGetMatrix(&bigbogy_view_to_world);
    njInvertMatrix(&bigbogy_view_to_world);
    njCnkSetMotionCallback(capture_boss_node);
}

static void draw_shadow_nodes(NJS_CNK_OBJECT *object)
{
    njPushMatrix(NULL);
    njCnkMotionTranslate(NULL, object, 0);
    njCnkMotionRotateXYZ(NULL, object, 0);
    njCnkMotionScale(NULL, object, 0);
    njSetCnkNextMotionNode();
    if (object->model) {
        njCnkModDrawModel(object->model);
    }
    if (object->child) {
        draw_shadow_nodes(object->child);
    }
    njPopMatrix(1);
    if (object->sibling) {
        draw_shadow_nodes(object->sibling);
    }
}

/* on the ground, the recoil flashes his body to BIGBOGY_FLASH_LIGHT for the
 * first FLASH_STEPS of each FLASH_PERIOD steps */
#define FLASH_PERIOD 20
#define FLASH_STEPS 10
static void bigbogy_draw(struct task *boss)
{
    struct boss_record *record = boss->extra.p;
    struct bigbogy_state *state = record->own;
    struct task_work *work = boss->work;
    struct motion_control *motion = record->block;
    struct mover *mover = boss->motion.p;
    Angle angle;

    if (state->movement != MOVE_SHADOW) {
        njPushMatrixEx();
        capture_boss_pose(boss);
        njTranslateEx(&work->pos);
        njScaleEx(&work->scl);
        njTranslate(NULL, 0.0F,
                    PIVOT_HEIGHT + (njSin(mover->sway_y) * BOB_HEIGHT), 0.0F);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, work->ang.y);
        if (FLIP(mover)) {
            njRotateX(NULL, FLIP(mover));
        }
        angle =
            (Angle)((njSin(mover->sway_x) * SWAY_X_DEGREES) * ANGLE_PER_DEGREE);
        njRotateX(NULL, angle);
        njTranslate(NULL, 0.0F, -PIVOT_HEIGHT, 0.0F);
        angle =
            (Angle)((njSin(mover->sway_z) * SWAY_Z_DEGREES) * ANGLE_PER_DEGREE);
        njRotateZ(NULL, angle);
        njRotateY(NULL, QUARTER_TURN);
        njSetTexture(&bigbogy_model_texlist);
        if (work->flags & BIGBOGY_FADING) {
            /* @hack spelling: fade, boss->motion read again, where mover
             * would do */
            struct mover *fade = boss->motion.p;
            draw_attr_save();
            draw_control_save();
            draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR |
                             NJD_CONTROL_3D_OFFSET_MATERIAL);
            draw_attr_set(1, NJD_FST_UA);
            constant_material_set(FADE(fade), 0.0F, 0.0F, 0.0F);
        }
        if (work->mode == BIGBOGY_GROUNDED && work->submode == GROUND_RECOIL) {
            if (game_state.steps % FLASH_PERIOD < FLASH_STEPS) {
                light_apply(BIGBOGY_FLASH_LIGHT);
            } else {
                light_apply(BIGBOGY_BODY_LIGHT);
            }
        } else {
            light_apply(BIGBOGY_BODY_LIGHT);
        }
        /* @hack redundant: a second test of movement, whose else no path
         * reaches */
        if (state->movement != MOVE_SHADOW) {
            motion_draw_simple(&bigbogy_body, motion);
        } else {
            njCnkModDrawObject(&bigbogy_shadow);
        }
        light_apply(light_after_draw);
        if (work->flags & BIGBOGY_FADING) {
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_attr_restore();
            draw_control_restore();
        }
        njPopMatrixEx();
        njCnkSetMotionCallback(NULL);
    } else {
        NJS_VECTOR scale;
        njPushMatrixEx();
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        scale.x = scale.y = scale.z = work->scl.x;
        njScaleEx(&scale);
        njSetCnkCurrentMotion(&bigbogy_shadow_motion, SHADOW_FRAME(mover));
        draw_shadow_nodes(&bigbogy_shadow);
        njPopMatrixEx();
    }
    bigbogy_sandglass_shadow(state->glass);
}
