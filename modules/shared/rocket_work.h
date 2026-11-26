/* Shared launcher work and tuning constants for ROCKET and STROCKET. */
#ifndef SHARED_ROCKET_WORK_H
#define SHARED_ROCKET_WORK_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the launcher's modes, in its work's mode. ROCKET_WAIT lasts until the
 * launcher may make a rocket. ROCKET_RISE lasts until a player takes hold of
 * the rocket, and ROCKET_AIM until the rocket flies */
enum rocket_mode { ROCKET_WAIT = 1, ROCKET_RISE, ROCKET_AIM };
/* the launcher's submode while its rocket stands upright, and 0 otherwise */
#define ROCKET_UPRIGHT 4
/* in ang.x: the launcher makes a rocket only while its switch is on */
#define ROCKET_ON_SWITCH 0x1
/* the bits of ang.z that hold the switch's id */
#define ROCKET_SWITCH_ID 0x1f

/* a time stop lasts while game_state.time_stopped isn't 0. a catch is the
 * step in which a player takes hold of a risen rocket. CAUGHT_IN_STOP holds
 * game_state.time_stopped from a catch to the first step outside a stop.
 * Final Rush's STROCKET holds 1 there from the catch to the flight */
#define CAUGHT_IN_STOP(work) ((work)->tick)

/* a rocket's modes, in its work's mode */
enum rocket_body_mode {
    BODY_ON_BASE,
    BODY_HELD, /* it holds its rider while the launcher aims it */
    BODY_FLYING
};
/* a rocket's submode is the slot of the player it carries, or NO_RIDER */
#define NO_RIDER (-1)

/* RIDING_STEPS counts a rocket's riding steps: the steps with a rider in
 * which RIDER_CAN_MOVE holds */
#define RIDING_STEPS(task) ((task)->unk_3c.l)

/* the rocket's shapes: its hull, and the grip a player takes hold of */
#define BODY_HULL 0
#define BODY_GRIP 1
#define BODY_SHAPES 2
/* the hull's attack, which it has only once RIDING_STEPS passes
 * HULL_SAFE_STEPS. rocket_body_init takes it away, and so does a step
 * without a rider */
#define HULL_ATTACK SHAPE_ATTACK_LEVEL(2)
#define HULL_SAFE_STEPS 10

/* the launcher's step that makes a rocket puts it WAIT_DROP below the
 * launcher's pos. it moves an upright rocket WAIT_UPRIGHT_X along x as well,
 * turned by the launcher's ang.y. the rocket's own first step, later in the
 * same step of the game, moves it to its start */
#define WAIT_DROP 1.5F
#define WAIT_UPRIGHT_X 10.0F
/* where a rocket starts, from the launcher's pos: START_X along x and
 * START_Y along y. the offset turns by the launcher's ang.y. an upright
 * rocket takes START_UPRIGHT_Y in place of START_Y */
#define START_X (-5.0F)
#define START_Y (-10.0F)
#define START_UPRIGHT_Y (-13.0F)

/* how far a step moves the base's frame, and in the rise the rocket's height
 * and frame. AIM_TURN is the most the aim turns the rocket in a step. Final
 * Rush's STROCKET is faster at each */
#ifdef FINAL_RUSH_ROCKET
#define BASE_FRAME_STEP 0.5F
#define RISE_STEP 0.5F
#define BODY_FRAME_STEP 3.0F
#define AIM_TURN 0x300
#else
#define BASE_FRAME_STEP 0.05F
#define RISE_STEP 0.15F
#define BODY_FRAME_STEP 0.3F
#define AIM_TURN 0x100
#endif
#define BODY_FRAMES 7.0F /* the last frame of the rocket's motion */
#define BASE_FRAMES 1.0F /* the last frame of the base's motion */
/* the aim lasts more than AIM_STEPS steps, and until the rocket faces its
 * place. the base then closes, and the rocket flies */
#define AIM_STEPS 7
/* how far a flying rocket moves toward its place in a riding step. it bursts
 * once nearer than that */
#define FLIGHT_STEP 9.0F
#define BURST_SCALE 15.0F /* the size scale of the rocket's burst */
/* the chance of a puff of thrust at each step of the aim */
#define THRUST_CHANCE 0.6F
/* where the thrust leaves the rocket, on the y axis of the rocket's model */
#define AIM_TAIL_Y (-0.3F)
#define FLIGHT_TAIL_Y (-0.2F)
/* a flying rocket puffs thrust on the steps whose ROCKET_STEPS lacks
 * FLIGHT_PUFF_BIT. a puff's speed is FLIGHT_PUFF_SHARE of the rocket's step.
 * FLIGHT_JITTER is the span of a random change to each part of that speed. a
 * puff's size runs from FLIGHT_SIZE_MIN up to FLIGHT_SIZE_MIN +
 * FLIGHT_SIZE_RANGE */
#define FLIGHT_PUFF_BIT 0x2
#define FLIGHT_PUFF_SHARE 0.5F
#define FLIGHT_JITTER 0.3F
#define FLIGHT_SIZE_MIN 2.0F
#define FLIGHT_SIZE_RANGE 1.5F
/* in flight, the rider's pad shakes once every RUMBLE_STEPS riding steps.
 * RUMBLE_FIRST_POWER is the power of a shake while RIDING_STEPS is below
 * RUMBLE_FIRST_STEPS. RUMBLE_TIME is the shake's time, as vibration_start
 * takes it */
#define RUMBLE_STEPS 19
#define RUMBLE_FIRST_STEPS 2
#define RUMBLE_POWER 1
#define RUMBLE_FIRST_POWER 3
#define RUMBLE_FREQ 60
#define RUMBLE_TIME 1

/* the animations a rider plays. a launcher sends RIDE_HOLD at the catch,
 * and RIDE_FLY as the flight starts. PLAYER_REQUEST_RELEASE comes as the
 * flight ends */
#define RIDE_HOLD 76
#define RIDE_FLY 77

#define SOUND_RISE 0x1005 /* as the launcher makes a rocket */
/* each riding step plays the thrust's sound by sound_play_for, for
 * THRUST_DURATION + 1 sound updates */
#define SOUND_THRUST 0x1019
#define THRUST_PRIORITY 1
#define THRUST_VOLUME 0
#define THRUST_DURATION 30

/* the size that picks the base's WORLD_SIZE_CLASS is the base model's radius
 * plus BASE_MARGIN */
#define BASE_MARGIN 5.0F
/* the launcher's step clears WORLD_ADDED while player_near finds no player
 * within the base model's radius plus BASE_NEAR. world_collect_near_players
 * then passes over the base */
#define BASE_NEAR 32.0F

struct launcher {
    /* the base in the collision world: rocket_base_collision, placed and
     * turned as the base is */
    NJS_OBJECT base;
    Float body_frame; /* the rocket's motion frame, 0 to BODY_FRAMES */
    Float base_frame; /* the base's motion frame, 0 to BASE_FRAMES */
    /* the launcher's rocket until it flies, or null. a flying rocket stays
     * the launcher's child */
    struct task *body;
    /* the count of the launcher's rockets that carry a rider. the
     * launcher's step calls placement_try_leave only while the count is 0 */
    Sint32 riders;
};

#define LAUNCHER(task) ((struct launcher *)(task)->motion.p)
#define BODY_WORK(task) (LAUNCHER(task)->body->work)

#ifdef FINAL_RUSH_ROCKET
#define ROCKET_TRANSLATE(pos) njTranslateV(NULL, (pos))
#define ROCKET_PUSH_BODY() njPushMatrix(NULL)
#define ROCKET_POP() njPopMatrix(1)
#define ROCKET_INIT stage_rocket_init
#define FLIGHT_OVER(distance) ((distance) < FLIGHT_STEP)
/* @hack type: game_state's time_stopped and steps read as
 * game_state_time_stopped and game_state_steps, where game_state.time_stopped
 * and game_state.steps would do */
#define RIDER_CAN_MOVE(work, slot)                                             \
    (CAUGHT_IN_STOP(work) || !game_state_time_stopped)
#define ROCKET_STEPS game_state_steps
#else
#define ROCKET_TRANSLATE(pos) njTranslateEx((pos))
#define ROCKET_PUSH_BODY() njPushMatrixEx()
#define ROCKET_POP() njPopMatrixEx()
#define ROCKET_INIT rocket_init
#define FLIGHT_OVER(distance) ((distance) < FLIGHT_STEP || game_state.cleared)
/* a rider moves outside a time stop, and after a catch in one. it doesn't
 * move while its slot's bit of game_state.time_stopped is set */
#define RIDER_CAN_MOVE(work, slot)                                             \
    ((CAUGHT_IN_STOP(work) || !game_state.time_stopped) &&                     \
     !(game_state.time_stopped & TIME_STOPPED_BIT(slot)))
#define ROCKET_STEPS game_state.steps
#endif

#endif
