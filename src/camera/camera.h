/* the cameras: each one's place, the current camera, and the state its mode
 * and its adjust give it each step */
#ifndef CAMERA_CAMERA_H
#define CAMERA_CAMERA_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task_work;

/* the most cameras in use at once */
#define CAMERAS 4

/* a camera's place: where it stands and how it looks. dir runs from pos to
 * target, and is dist long. ang's x and y aim the camera along dir, and
 * ang.z is its roll */
struct camera_place {
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR dir;
    NJS_POINT3 target;
    NJS_VECTOR vel; /* pos less the place's pos a step before */
    Float dist;
};
/* each camera's place, camera_stacks[camera].place, or null for a camera
 * camera_start didn't put in use */
extern struct camera_place *camera_views[CAMERAS];
/* the count of cameras in use */
extern Sint32 camera_active_count;
/* the camera whose mode and adjust run. it is the number of that camera's
 * player too */
extern int camera_current_index;

/* camera_state's types, each named for the members a mode sets.
 * camera_state_complete works out the rest from them */
#define CAMERA_POS_TARGET 0
#define CAMERA_POS_ANG_DIST 1
#define CAMERA_POS_DIR 2
#define CAMERA_TARGET_ANG_DIST 3
#define CAMERA_TARGET_DIR 4

/* the current camera's place for this step. its mode sets type and the
 * members type names, and its adjust then blends them */
struct camera_state {
    Sint32 type; /* a CAMERA_POS_ or CAMERA_TARGET_ type */
    /* the camera task runs camera_collide after the adjust only while it
     * isn't 0, and game_state.unk_2a is 0 */
    Sint32 unk_04;
    /* camera_run_mode lets a trigger push a mode only while it isn't 0 */
    Sint32 trigger_push;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR dir;
    NJS_POINT3 target;
    Float dist;
};
extern struct camera_state camera_state;

/* a stick's axis as the camera reads it, times 256. the camera takes a
 * reading within CAMERA_STICK_DEAD_ZONE of 0 for 0 */
#define CAMERA_STICK_READING(axis) ((axis) << 8)
#define CAMERA_STICK_DEAD_ZONE 5000

/* the searches for the nearest camlin node or zone take only one closer
 * than CAMERA_NEAREST_REACH */
#define CAMERA_NEAREST_REACH 10000.0F
/* camlin_draw and camset_draw draw a model only where it lies within
 * CAMERA_MODEL_RANGE of the current camera */
#define CAMERA_MODEL_RANGE 1200.0F

/* the debug draws, which src/draw/debug_draw.c defines. debug_draw_model
 * draws debug_models[model] at pos. a non-null scl scales it, and a
 * non-null ang then turns it about y, then x, then z. debug_draw_mark draws
 * a line size long along each axis, centered on pos */
void debug_draw_model(NJS_POINT3 *pos, const struct angles *ang,
                      NJS_VECTOR *scl, int model);
void debug_draw_mark(NJS_POINT3 *pos, Float size);
/* The box has side 1. The cylinder stands 1 high along y with a radius
 * of 1. The plane is a square of side 2 in the xy plane. The sphere has
 * radius 1. EVENT uses the last model for its camera and light markers. */
#define DEBUG_MODEL_CONE 0
#define DEBUG_MODEL_BOX 1
#define DEBUG_MODEL_CYLINDER 2
#define DEBUG_MODEL_PLANE 3
#define DEBUG_MODEL_SPHERE 4
#define DEBUG_MODEL_RING 5
#define DEBUG_MODEL_MARKER 6
#define DEBUG_MODEL_COUNT 7
/* debug_draw_model's seven models, which the DEBUG_MODEL_ numbers index */
extern NJS_CNK_OBJECT *debug_models[DEBUG_MODEL_COUNT];
/* Chao collision fields draw this mesh directly. */
extern NJS_CNK_OBJECT debug_sphere_object;

/* the camera task's work. each step of the task sets its pos and ang to
 * camera 0's, and its scl to 1. it is null from camera_start until the
 * task's next step, and once the task is destroyed */
extern struct task_work *camera_task_work;
/* the camera task's steps since camera_start. no adjust runs until it is
 * above 0 */
extern Sint32 camera_steps;

/* put count cameras in use, 1 to CAMERAS, and create the camera task where
 * there is none. every stack is zeroed. a camera in use gets its place in
 * camera_views and an angle of view of 70 degrees. its stack takes
 * CAMERA_MODE_CARMEL at index 0. any other camera gets a null view. on a
 * stage below STAGE_STG_END, every stack also takes CAMERA_MODE_INIT at
 * CAMERA_INIT_SLOT. it ends a camera_hold */
void camera_start(int count);
/* destroy the camera task, where there is one, and free the probes' list
 * of faces */
void camera_stop(void);
/* camera_start again, for the cameras in use */
void camera_restart(void);
#ifdef NONMATCHING
/* make the camera task's next draw camera 0's. the task counts its draws
 * from each move of game_state.total_steps, which comes once a frame, and the
 * port's split frame draws two pictures in one. so a loop calls this before
 * and after it draws the first */
void camera_draw_restart(void);
#endif
/* stop camera's shake, and take every mode above index 0 off its stack.
 * then give it the modes camera_start gives a camera in use */
void camera_reset_modes(int camera);
/* keep the camera task from stepping, until camera_release or
 * camera_start. the task still draws. stage is unused */
void camera_hold(int stage);
void camera_release(void);
/* copy camera's place into camera_state: its pos, ang, dir, target and
 * dist. the state's type, unk_04 and trigger_push stay */
void camera_state_from_place(int camera);
/* hand Ninja the light at index light_camera of lights */
void camera_apply_light(void);
/* the camera task's draw calls it last, for the camera it drew. it does
 * nothing */
void camera_draw_end(int camera);

/* work out the members of camera_state that its type doesn't name. dir
 * runs from pos to target and is dist long, and ang's x and y look along
 * it. ang.z is left as it is */
void camera_state_complete(void);
/* keep camera's place as its place_before, then set the place from
 * camera_state */
void camera_place_from_state(int camera);
/* set camera's unclamped place from camera_state, as camera_place_from_state
 * sets its place. the camera task calls it before camera_collide */
void camera_keep_unclamped(int camera);
/* run camera_zones_collide for camera, then move a sphere from camera's
 * place toward camera_state's pos. where the sphere slides along the land,
 * camera_state's pos becomes where it came to rest. camera_state's type
 * becomes CAMERA_POS_TARGET */
void camera_collide(int camera);
/* turn camera's angle of view toward its view_target, by its view_turn */
void camera_step_view_angle(int camera);
/* set camera_stacks[camera].standing to 1 where camera's player is on the
 * ground and moved under STILL_REACH last step. set it to 0 otherwise.
 * state.c defines STILL_REACH as 0.1. standing stays as it is where
 * camera_no_player(camera) isn't 0 */
void camera_step_standing(int camera);

/* 0 where the cameras follow the player in player's slot. it returns 1 for
 * a player on a cart stage, and nonzero for an empty slot */
int camera_no_player(int player);

/* the cameras keep a trail for each player: the last CAMERA_TRAIL_POINTS
 * points it passed. each lies more than 1 from the one before. a point is
 * the player's pos raised by its target_height */
#define CAMERA_TRAIL_POINTS 50
/* zero every trail. it does nothing where camera_no_player(0) isn't 0 */
void camera_trails_clear(void);
/* set each point of camera's trail to its player's pos, not raised. it does
 * nothing where camera_no_player(0) isn't 0 */
void camera_trail_fill(int camera);
/* take a point into camera's trail where its player lies more than 1 from
 * the last. keep the player's mode for camera_player_mode_before. it does
 * nothing where camera_no_player(camera) isn't 0 */
void camera_trail_step(int camera);
/* a point of camera's trail. a back of 1 gives the latest, and
 * CAMERA_TRAIL_POINTS the oldest */
NJS_POINT3 *camera_trail_point(int camera, int back);
/* the total of points all the trails have taken since the program
 * started */
Sint32 camera_trail_points_taken(void);

/* the mode of player's work as camera_trail_step kept it back steps ago,
 * for a back of 0 to 15 */
Sint32 camera_player_mode_before(int player, int back);
/* keep the pos and angles of the first two players for this step. the
 * camera task calls it once a step */
void camera_pasts_step(void);
/* set *pos and *ang to the pos and angles of camera's player that the
 * cameras follow. they are those camera_pasts_step kept a step ago. where
 * the player's mode three steps ago was PLAYER_MODE_LEDGE_CLIMB, they are the
 * player's own now. either pointer may be null. camera must be 0 or 1 */
void camera_player_place(int camera, NJS_POINT3 *pos, struct angles *ang);

#endif
