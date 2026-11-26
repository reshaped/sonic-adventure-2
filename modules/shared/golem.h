/* the Egg Golem's state, sensors, actions and tests of player 0's place,
 * which BOSS_GOLEM and BOSS_GOLEM_E share. the comments below name calls of
 * boss_golem/tactics.h. boss_golem_e/tactics.h declares each with golem_e_ in
 * place of golem_ */
#ifndef SHARED_GOLEM_H
#define SHARED_GOLEM_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

/* the golem's sensors: collision models that ride nodes of its skeleton. each
 * sensor puts a copy of its model in the collision world, from a task of its
 * own. each fight's sensor_create takes the copy from world_pool_alloc and the
 * task from the heap, and tests neither. where the pool has no object left,
 * sensor_create writes the copy through null. the destructor tests each
 * sensor's object. where the heap can't give the task, sensor_create reads the
 * task's work through null. the creator tests its other children */
#define GOLEM_SENSORS 9
/* the sensors other code names, by index, the same in both golems.
 * GOLEM_SENSOR_FOREARM_A rides the parent of GOLEM_SENSOR_HAND_A's node, and
 * GOLEM_SENSOR_FOREARM_B that of GOLEM_SENSOR_HAND_B's */
#define GOLEM_SENSOR_LOW 0
#define GOLEM_SENSOR_HEAD 1
#define GOLEM_SENSOR_FOREARM_A 3
#define GOLEM_SENSOR_HAND_A 4
#define GOLEM_SENSOR_FOREARM_B 6
#define GOLEM_SENSOR_HAND_B 7
#define GOLEM_SENSOR_FULL 8

/* the push of the golem's solid shapes */
#define GOLEM_PUSH_SOLID PUSH_SOLID(PUSH_ALL_BUT_PICKUPS)

struct task;

/* the tests of player 0's place, which boss_golem/tactics.c defines for both
 * fights under these names. each reads player 0's work, which must not be
 * null. none reads task. golem_player_low and golem_turn_to_player read the
 * fight's golem, which must not be null. the arena's center is the world's
 * origin, and a distance from it is measured along x and z */

/* whether player 0 is close: within 82.5 of the arena's center */
Bool golem_player_close(struct task *task);
/* whether player 0 is near: within 165 of the arena's center */
Bool golem_player_near(struct task *task);
/* whether player 0 is low: its pos.y below 160, or below 180 in BOSS_GOLEM_E */
Bool golem_player_low(struct task *task);
/* the heading from the arena's center to player 0, -0x8000 to 0x7fff */
Angle golem_player_heading(void);
/* the short turn from the golem's heading to golem_player_heading, -0x8000
 * to 0x7fff */
Angle golem_turn_to_player(void);

/* the arena's slots, spaced evenly about its center. slot n lies at the
 * heading of n GOLEM_SLOT_TURNs */
#define GOLEM_SLOTS 12
#define GOLEM_SLOT_TURN (FULL_TURN / GOLEM_SLOTS)

/* the golem's actions. each fight's action machine indexes its table of
 * action steps by them */
enum golem_action {
    GOLEM_ACTION_WAIT, /* for the fight to start */
    GOLEM_ACTION_REST, /* rest, then choose an attack */
    GOLEM_ACTION_FACE_PLAYER,
    GOLEM_ACTION_AIM_HAND_A, /* turn hand A over the slot before player 0's */
    GOLEM_ACTION_AIM_HAND_B, /* turn hand B over the slot after player 0's */
    GOLEM_ACTION_STRIKE_A, /* hand A strikes, then hand B the next slot */
    GOLEM_ACTION_STRIKE_B, /* hand B strikes, then hand A the slot before */
    /* the hands come down and hold while player 0 is behind the golem.
     * GOLEM_ACTION_HANDS_DOWN_A is for a negative golem_turn_to_player, and
     * GOLEM_ACTION_HANDS_DOWN_B for a positive one */
    GOLEM_ACTION_HANDS_DOWN_A,
    GOLEM_ACTION_HANDS_DOWN_B,
    /* strike the slot and the two on each side of it. the blows two slots away
     * should shake the pad and the camera from the nearer hand. but hand B's
     * distance takes hand A's place, so a blow that lands shakes from hand A.
     * where neither slot takes a blow, the flag that says a blow landed is
     * unset. its register holds the slot two before the golem's. unless that
     * slot is 0, the golem shakes from hand B */
    GOLEM_ACTION_SLAM,
    /* explode and sink, then clear the stage. the sinking sound's volume is
     * meant to fall with a scale from 1 to 0.6 over the motion. but the scale
     * is an integer, which truncates to 0 past the motion's first frame. the
     * sound then plays at volume 0 */
    GOLEM_ACTION_DIE
};
/* what next_action holds while no action is requested */
#define GOLEM_NO_ACTION (-1)

/* how golem_turn turns the golem's heading: toward player 0, toward
 * aim_heading, or not at all. a turn to a hand or to the slam turns toward
 * aim_heading */
enum golem_turn {
    GOLEM_TURN_TO_PLAYER,
    GOLEM_TURN_TO_HAND_A,
    GOLEM_TURN_TO_HAND_B,
    GOLEM_TURN_TO_SLAM,
    GOLEM_TURN_NONE
};
/* a hand, as golem_aim_slot and golem_set_slot take it */
#define GOLEM_HAND_A 0
#define GOLEM_HAND_B 1

/* the indexes of each fight's golem_motions. a move has a run of motions for
 * each of the first three foothold states, in order. each run holds a motion
 * for each of the move's stages, in order */
#define GOLEM_MOTION_OPENING 0
#define GOLEM_MOTION_IDLE 1
#define GOLEM_MOTION_STRIKE_A 2
#define GOLEM_MOTION_STRIKE_B 17
#define GOLEM_MOTION_SLAM 32
#define GOLEM_MOTION_HANDS_DOWN_A 44
#define GOLEM_MOTION_HANDS_DOWN_B 53
#define GOLEM_MOTION_DIE 63
/* the motions in each fight's golem_motions */
#define GOLEM_MOTIONS 64
/* each move's stages, in the order it plays their motions */
enum golem_strike_stage {
    GOLEM_STRIKE_START,
    GOLEM_STRIKE_BLOW,
    GOLEM_STRIKE_RISE,
    GOLEM_STRIKE_SECOND_BLOW,
    GOLEM_STRIKE_SECOND_RISE,
    GOLEM_STRIKE_STAGES
};
enum golem_hands_stage {
    GOLEM_HANDS_LOWER,
    GOLEM_HANDS_HOLD,
    GOLEM_HANDS_RISE,
    GOLEM_HANDS_STAGES
};
enum golem_slam_stage {
    GOLEM_SLAM_START,
    GOLEM_SLAM_BLOWS, /* the hands' blows on the four slots about the slam's */
    GOLEM_SLAM_SLAM, /* the slam on its own slot */
    GOLEM_SLAM_END,
    GOLEM_SLAM_STAGES
};

/* the golem's low power. at it or below, the golem slams in place of a
 * strike. each fight's creator gives it to power_low */
#define GOLEM_POWER_LOW 2.0F

/* the golem's state. each fight's creator allocates it as the own block of
 * the golem's boss record. where a field's comment opens with a fight's name,
 * only that fight sets the field, the creators aside.
 *
 * each creator tests the state's allocation, but not the record's. where the
 * heap can't give the record, the store of own writes through null. where it
 * can't give the state, the creator kills the golem's task. but where it can't
 * give a child, the creator returns null and leaves the task to step,
 * unfinished.
 *
 * the creators share one more defect, which no field here holds. each keeps
 * the golem's distance from home in its mover's wall_bounce. the distance's z
 * takes pos.x less home.z, where pos.z is meant. each stage creates the golem
 * at the origin, whose x and z agree. so the distance comes out right */
struct golem {
    Bool dark_story; /* TRUE in BOSS_GOLEM_E, the dark story's fight */
    enum golem_action action; /* the current action */
    Sint32 next_action; /* the action requested, or GOLEM_NO_ACTION */
    Sint32 action_mode; /* the current action's own mode, from 0 */
    Sint32 action_steps; /* the steps the current action has run */
    /* the golem's power, which its gauge shows. it starts at power_max. each
     * hit takes some off, and in BOSS_GOLEM_E so does a weak point's break */
    Float power;
    Float power_max; /* 5 in BOSS_GOLEM, 6 in BOSS_GOLEM_E */
    /* GOLEM_POWER_LOW in both fights. at this power or below, a hand aims at
     * the slot nearest player 0. the golem then turns to face that slot, not
     * to put the hand over it */
    Float power_low;
    /* BOSS_GOLEM_E: the hits taken, each on a sensor counting 1 and each on a
     * weak point 3. no hit counts once it reaches 60. each weak point opens
     * at a count of its own */
    Sint32 hits;
    /* the camera's mode, a GOLEM_CAMERA_ value of shared/golem_camera.h. only
     * while it isn't GOLEM_CAMERA_INTRO do the golem's limbs throw rubble.
     * BOSS_GOLEM_E's sensors take hits only then too */
    Sint32 camera_mode;
    Sint32 camera_next; /* the mode requested, or GOLEM_CAMERA_NONE */
    /* the mode before camera_mode. golem_camera_set makes it camera_mode */
    Sint32 camera_last;
    /* the point camera 0's BOSSPOINT mode keeps beyond player 0, in
     * GOLEM_CAMERA_POINT and GOLEM_CAMERA_POINT_CLOSE */
    NJS_POINT3 camera_point;
    /* player 0's heading from the arena's center, as GOLEM_CAMERA_POINT or
     * GOLEM_CAMERA_POINT_CLOSE last took it. camera_point lies across the
     * center, at the opposite heading */
    Angle camera_heading;
    Sint32 turn; /* how golem_turn turns the golem, an enum golem_turn */
    Sint32 unk_44;
    Sint32 camera_steps; /* the steps camera_mode has run */
    /* the steps the current action's mode has waited. in
     * GOLEM_ACTION_FACE_PLAYER and the GOLEM_ACTION_AIM_ actions, golem_hover
     * times the golem's hops in it instead */
    Sint32 timer;
    /* the golem's step clears it before the action runs. where the action sets
     * it, the golem's motion keeps its frame for the step */
    Bool hold_motion;
    /* BOSS_GOLEM: TRUE while a hit on the weak spot throws player 0 */
    Bool throwing;
    Sint32 unk_58; /* BOSS_GOLEM: its throw sets it to 0, as the creators do */
    /* BOSS_GOLEM_E: TRUE from a hit until the golem's flash and every weak
     * point's flash have run out. no hit counts while it is TRUE */
    Bool flashing;
    /* BOSS_GOLEM_E: the steps left of the flash of a hit on a sensor */
    Sint32 flash_steps;
    /* BOSS_GOLEM: the throw's steps so far, out of throw_steps. player 0
     * rises for the first rise_steps of them */
    Sint32 throw_step;
    Sint32 throw_steps;
    Sint32 rise_steps;
    /* BOSS_GOLEM: player 0's speed at the throw's start */
    NJS_VECTOR throw_spd;
    /* the turn about y that the golem's draw gives the model, in place of the
     * work's ang.y */
    Angle heading;
    /* the heading that a turn to a hand or to the slam turns toward */
    Angle aim_heading;
    Sint32 slot; /* the slot the golem's next blow strikes */
    /* the state of the foothold each hand strikes, an enum
     * golem_foothold_state of boss_golem/footholds.h. it picks the motions of
     * the hand's strike */
    Sint32 hand_a_state;
    Sint32 hand_b_state;
    /* each piece of rubble a limb throws adds 1, and each piece that ends
     * takes 1 off. no limb throws while it is above 32. in BOSS_GOLEM_E, a
     * hit's pieces and an opening weak point's take 1 off each but add none.
     * weak_points.c tags the two stores. the count there can fall below the
     * pieces in flight, and below 0. BOSS_GOLEM's creator never sets it, so it
     * starts as the heap left it */
    Sint32 rubble_count;
    Float lift; /* how far above the work's pos the model draws */
    Float lift_speed; /* what a hop adds to lift each step */
    /* the ground below player 0, as GOLEM_CAMERA_FAR or
     * GOLEM_CAMERA_FAR_TURNED last found it while it lay above 100 */
    Float floor_goal;
    /* the height GOLEM_CAMERA_FAR and GOLEM_CAMERA_FAR_TURNED take for the
     * golem's pos. it eases toward floor_goal. it and floor_goal start at the
     * floor's height */
    Float floor_y;
    Uint8 unk_a4[0x10];
    /* each sensor's object in the collision world, from world_pool_alloc */
    NJS_OBJECT *sensors[GOLEM_SENSORS];
    /* FALSE until GOLEM_CAMERA_FAR first starts, and TRUE after. so only a
     * later start of that mode blends camera 0 to its view */
    Bool camera_far_started;
    /* BOSS_GOLEM: the weak spot's pos, as its last step placed it */
    NJS_POINT3 weak_spot_pos;
    /* BOSS_GOLEM: the state of the least sunk foothold, which picks the
     * motions of the hands coming down. BOSS_GOLEM_E's stays
     * FOOTHOLD_WHOLE */
    Sint32 hands_down_state;
    /* the state of the least sunk foothold of the slam's slot and the slots
     * beside it. it picks the slam's motions */
    Sint32 slam_state;
    /* the golem's steps since its start or the end of its last attack */
    Sint32 rest_steps;
    /* while player 0 isn't near, golem_may_attack lets the golem attack only
     * after rest_steps passes it. golem_set_rest_wait sets it each step */
    Sint32 rest_wait;
    Sint32 up_steps; /* the steps since player 0 was last low */
};

#endif
