/* the karts of a race. each racer is a task whose motion holds its struct
 * cart_vehicle */
#ifndef CART_VEHICLE_H
#define CART_VEHICLE_H
#include "camera/probe.h"
#include "cart/pads.h"
#include "system/task.h"

/* the steps back for which a player's kart keeps its velocity, angles and
 * stick */
#define CART_HISTORY_STEPS 60

/* the driver's sounds of cart_driver_sound */
enum { CART_LAUNCH_SOUND, CART_BOOST_SOUND, CART_DRIVER_SOUNDS };

/* where a racer starts. model is its cart_model_id. player is its slot in
 * cart_racers, and the player whose pad drives it where player_controlled is
 * set */
struct cart_start {
    Bool player_controlled;
    Sint32 model, player;
    NJS_POINT3 pos;
    struct angles ang;
};

/* a kart's driving state. previous is its task's work as its step began. where
 * start.player_controlled is clear, the draw then sets previous.pos to the
 * work's. driver is the player whose kart it is. a kart the CPU drives from the
 * start has a driver of -1. a player's kart keeps its driver once the CPU takes
 * it over at the finish. yaw turns the body from the work's ang.y, the way the
 * kart drives */
struct cart_vehicle {
    struct task_work previous;
    Sint32 mode;
    NJS_CNK_OBJECT *object, *second;
    NJS_TEXLIST *textures;
    struct cart_start start;
    Angle yaw;
    Sint32 unk_68, driver;
    Bool grounded;
    Sint32 player;
    NJS_VECTOR velocity, local_velocity;
    Float unk_90;
    NJS_VECTOR push;
    Sint32 steering_timer, drift_stick;
    NJS_VECTOR drift_velocity;
    Sint32 spin;
    /* gate is the last gate the kart crossed, and crossing gates backwards
     * takes it back. lap counts the times gate passed the last gate, and
     * crossing back past the first takes one away. a story race's gate never
     * passes the last, so there lap counts the checkpoints passed instead.
     * place is 1 for the leader. furthest_lap is the most lap has been */
    Sint32 gate, place, lap, furthest_lap;
    Sint32 unk_c8, unk_cc;
    Angle wheel;
    Sint32 boost_rings, boost_steps;
    Uint8 unk_dc[12];
    struct task *chase_target;
    struct angles body_tilt, air_tilt;
    Bool invincible;
    Sint32 flags;
    Bool accelerating, braking;
    struct camera_probe probe;
    Sint32 target_gate, target_lane, last_gate, last_lane;
    Float cpu_speed;
    NJS_POINT3 last_ground;
    Angle cpu_heading;
    Uint8 unk_274[12];
    Bool tunnel;
    Sint32 invincible_steps, airborne_steps;
    Sint32 wall_cooldown, respawn_timer;
};
/* whether each player's kart holds a boost. while cart_story_approach is set,
 * a story race's kart drives in on its own */
extern Bool cart_boost_ready[CART_PLAYERS], cart_story_approach;
/* start a racer's task with a copy of start. each call makes the task's work
 * player_work[0] */
struct task *cart_create_racer(struct cart_start *start);
/* clear the records of each player's kart */
void cart_reset_history(void);
/* set *out to the angles of player's kart steps_back records back.
 * steps_back runs from 1, the latest, to CART_HISTORY_STEPS, unchecked */
void cart_previous_angles(int player, int steps_back, struct angles *out);
Bool cart_is_racer(struct task *task);
void cart_grant_invincibility(struct task *task);
/* push task's kart away from pos, where it hit a car, and slow it. under
 * MISSION_CHAO the hit ends a story race's stage. the stage restarts while
 * player 0 has a life to lose, and quits otherwise */
void cart_hit_traffic(struct task *task, NJS_POINT3 *pos);
/* set task's kart moving at velocity. a CPU kart takes velocity's speed, and
 * turns toward it by up to QUARTER_TURN */
void cart_launch(struct task *task, NJS_VECTOR *velocity);
/* play sound, the driver's sound of task's kart, and return whether it did.
 * only a kart a player drives plays one, and not a CART_MODEL_DOWNLOAD kart. it
 * first holds the MIDI ports' effect level, as cart_leader_tunnel_sound does.
 * that level follows whether cart_racers[0]'s kart is in a tunnel */
Bool cart_driver_sound(struct task *task, int sound);
#endif
