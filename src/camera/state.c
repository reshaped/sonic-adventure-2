/* the cameras' state through a step: camera_state completed, each camera's
 * places, and its move against the land. what the cameras keep of each
 * player's past */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/helpers.h"
#include "camera/probe.h"
#include "camera/stack.h"
#include "camera/zone_collision.h"
#include "collision/world.h"
#include "player/player.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/vector.h"

/* camera_collide's probe passes over flat objects and far land */
#define PROBE_SKIP (WORLD_FLAT | LAND_FAR)
#define PROBE_RADIUS 3.0F /* the radius of camera_collide's sphere */
/* a player has stood still where it lies closer than this to its place a
 * step before */
#define STILL_REACH 0.1F
/* a trail takes a new point once its player's raised pos lies farther than
 * this from the last */
#define TRAIL_SPACING 1.0F
/* the steps a player's mode is kept for, a power of 2. MODE_SLOT(steps) is
 * the slot of mode_past that holds the mode at a game_state.steps of steps */
#define MODE_PAST_STEPS 16
#define MODE_SLOT(steps) ((steps) & (MODE_PAST_STEPS - 1))
/* the steps a player's pos and angles are kept for: 256, which a Uint8
 * index wraps at */
#define PLACE_PAST_STEPS 256
/* the players whose pos and angles the cameras keep */
#define PLACE_PAST_PLAYERS 2
/* camera_player_place gives a player's place now, not a step ago, where its
 * mode LEDGE_CLIMB_BACK steps ago was PLAYER_MODE_LEDGE_CLIMB */
#define LEDGE_CLIMB_BACK 3

static void camera_update_standing(int camera);
static void camera_player_past(int player, int back, NJS_POINT3 *pos,
                               struct angles *ang);

/* nothing calls these. none is static, since SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* it does nothing */
void fun_8c0acd98(void);
/* FALSE where camera's standing is 1 and its trigger_pushed FALSE, else
 * TRUE */
Bool fun_8c0ad00e(int camera);
/* NOLINTEND(misc-use-internal-linkage) */

/* each player's mode at each of the last MODE_PAST_STEPS steps */
static struct {
    Sint32 modes[MODE_PAST_STEPS];
} mode_past[CAMERAS];
/* each player's trail: the last CAMERA_TRAIL_POINTS places it passed, each
 * raised by its target_height. each lies more than TRAIL_SPACING from the
 * one before */
static NJS_POINT3 trails[CAMERAS][CAMERA_TRAIL_POINTS];
static Sint32 trail_next[CAMERAS]; /* where a trail's next point goes */
/* the points every trail has taken since the program started */
static Sint32 trail_points_taken;
/* the first two players' pos and angles at each of the last
 * PLACE_PAST_STEPS steps. place_past_latest holds the index of each one's
 * latest */
static NJS_POINT3 place_past_pos[PLACE_PAST_PLAYERS][PLACE_PAST_STEPS];
static struct angles place_past_ang[PLACE_PAST_PLAYERS][PLACE_PAST_STEPS];
static Uint8 place_past_latest[PLACE_PAST_PLAYERS];

void camera_state_complete(void)
{
    switch (camera_state.type) {
    case CAMERA_POS_TARGET:
        vector_subtract(&camera_state.target, &camera_state.pos,
                        &camera_state.dir);
        camera_state.dist = njScalor(&camera_state.dir);
        look_angles(&camera_state.dir, &camera_state.ang.x,
                    &camera_state.ang.y);
        break;
    case CAMERA_POS_ANG_DIST:
        look_vector_xy(&camera_state.ang, &camera_state.dir);
        vector_scale(&camera_state.dir, &camera_state.dir, camera_state.dist);
        vector_add(&camera_state.pos, &camera_state.dir, &camera_state.target);
        break;
    case CAMERA_POS_DIR:
        look_angles(&camera_state.dir, &camera_state.ang.x,
                    &camera_state.ang.y);
        camera_state.dist = njScalor(&camera_state.dir);
        vector_add(&camera_state.pos, &camera_state.dir, &camera_state.target);
        break;
    case CAMERA_TARGET_ANG_DIST:
        look_vector_xy(&camera_state.ang, &camera_state.dir);
        vector_scale(&camera_state.dir, &camera_state.dir, camera_state.dist);
        vector_subtract(&camera_state.target, &camera_state.dir,
                        &camera_state.pos);
        break;
    case CAMERA_TARGET_DIR:
        look_angles(&camera_state.dir, &camera_state.ang.x,
                    &camera_state.ang.y);
        camera_state.dist = njScalor(&camera_state.dir);
        vector_subtract(&camera_state.target, &camera_state.dir,
                        &camera_state.pos);
        break;
    }
}

void camera_place_from_state(int camera)
{
    struct camera_stack *stack = &camera_stacks[camera];
    struct camera_place *place = &stack->place;

    stack->place_before = stack->place;
    place->pos = camera_state.pos;
    place->ang = camera_state.ang;
    place->dir = camera_state.dir;
    place->target = camera_state.target;
    place->dist = camera_state.dist;
    vector_subtract(&place->pos, &stack->place_before.pos, &place->vel);
}

void camera_keep_unclamped(int camera)
{
    struct camera_stack *stack = &camera_stacks[camera];
    /* @hack spelling: state, a pointer to camera_state, where camera_state
     * written directly would do */
    struct camera_state *state = &camera_state;
    struct camera_place *place;
    NJS_POINT3 before;

    /* @hack spelling: before read ahead of place's store, where place set
     * at its declaration and read through would do */
    before = stack->unclamped.pos;
    place = &stack->unclamped;
    place->pos = state->pos;
    place->ang = state->ang;
    place->dir = state->dir;
    place->target = state->target;
    place->dist = state->dist;
    vector_subtract(&place->pos, &before, &place->vel);
}

void camera_step_view_angle(int camera)
{
    camera_stacks[camera].view = angle_approach(
        camera_stacks[camera].view, camera_stacks[camera].view_target,
        camera_stacks[camera].view_turn);
}

void camera_collide(int camera)
{
    /* @hack spelling: probe set ahead of the call, where setting it after
     * would do */
    struct camera_probe *probe = &camera_stacks[camera].probe;

    camera_zones_collide(camera);
    probe->skip = PROBE_SKIP;
    probe->radius = PROBE_RADIUS;
    probe->pos = camera_stacks[camera].place.pos;
    probe->next = camera_state.pos;
    if (camera_probe_move(probe)) {
        camera_state.pos = probe->hit;
    }
    camera_state.type = CAMERA_POS_TARGET;
}

static void camera_update_standing(int camera)
{
    NJS_POINT3 moved;

    if (camera_no_player(camera)) {
        return;
    }
    player_get_past(camera, 1, &moved, NULL);
    njSubVector(&moved, &player_work[camera]->pos);
    if (njScalor(&moved) < STILL_REACH &&
        (player_work[camera]->flags & PLAYER_WORK_ON_GROUND)) {
        camera_stacks[camera].standing = 1;
    } else {
        camera_stacks[camera].standing = 0;
    }
}

void fun_8c0acd98(void)
{
}

void camera_step_standing(int camera)
{
    camera_update_standing(camera);
}

void camera_trails_clear(void)
{
    int i;
    int j;

    if (camera_no_player(0)) {
        return;
    }
    for (i = 0; i < CAMERAS; i++) {
        for (j = 0; j < CAMERA_TRAIL_POINTS; j++) {
            trails[i][j].x = 0.0F;
            trails[i][j].y = 0.0F;
            trails[i][j].z = 0.0F;
        }
        trail_next[i] = 0;
    }
}

void camera_trail_fill(int camera)
{
    int j;

    if (camera_no_player(0)) {
        return;
    }
    for (j = 0; j < CAMERA_TRAIL_POINTS; j++) {
        trails[camera][j] = player_work[camera]->pos;
    }
    trail_next[camera] = 0;
}

void camera_trail_step(int camera)
{
    NJS_VECTOR moved;
    int last;

    if (camera_no_player(camera)) {
        return;
    }
    if (trail_next[camera] == 0) {
        last = CAMERA_TRAIL_POINTS - 1;
    } else {
        last = trail_next[camera] - 1;
    }
    moved.x = player_work[camera]->pos.x - trails[camera][last].x;
    moved.y = player_work[camera]->pos.y - trails[camera][last].y +
              player_records[camera]->consts.target_height;
    moved.z = player_work[camera]->pos.z - trails[camera][last].z;
    if (njScalor(&moved) > TRAIL_SPACING) {
        trail_points_taken++;
        trails[camera][trail_next[camera]].x = player_work[camera]->pos.x;
        trails[camera][trail_next[camera]].y =
            player_work[camera]->pos.y +
            player_records[camera]->consts.target_height;
        trails[camera][trail_next[camera]].z = player_work[camera]->pos.z;
        trail_next[camera]++;
        if (trail_next[camera] >= CAMERA_TRAIL_POINTS) {
            trail_next[camera] = 0;
        }
    }
    mode_past[camera].modes[MODE_SLOT(game_state.steps)] =
        player_work[camera]->mode;
}

NJS_POINT3 *camera_trail_point(int camera, int back)
{
    int i = trail_next[camera] - back;

    if (i < 0) {
        i += CAMERA_TRAIL_POINTS;
    }
    return &trails[camera][i];
}

Sint32 camera_trail_points_taken(void)
{
    return trail_points_taken;
}

Bool fun_8c0ad00e(int camera)
{
    return !camera_stacks[camera].standing ||
           camera_stacks[camera].trigger_pushed;
}

int camera_no_player(int player)
{
    if (player_work[player]) {
        switch (stage_current_id()) {
        case STAGE_CART_1:
        case STAGE_CART_2:
            return 1;
        }
        return 0;
    }
    /* @bug uninitialized: returns no value for an empty slot. BT at
     * 0x8c0ad03c skips to the RTS with R0 as MOV.L at 0x8c0ad032 left it,
     * player_work's address. the callers then read nonzero */
#ifdef NONMATCHING
    /* this side keeps the tag's result: it returns 1, nonzero as
     * player_work's address is */
    return 1;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

Sint32 camera_player_mode_before(int player, int back)
{
    return mode_past[player].modes[MODE_SLOT(game_state.steps - back)];
}

void camera_player_place(int camera, NJS_POINT3 *pos, struct angles *ang)
{
    if (camera_player_mode_before(camera, LEDGE_CLIMB_BACK) ==
        PLAYER_MODE_LEDGE_CLIMB) {
        if (pos) {
            *pos = player_work[camera]->pos;
        }
        if (ang) {
            *ang = player_work[camera]->ang;
        }
    } else {
        camera_player_past(camera, 1, pos, ang);
    }
}

void camera_pasts_step(void)
{
    int i;

    for (i = 0; i < PLACE_PAST_PLAYERS; i++) {
        place_past_latest[i]++;
        if (player_work[i]) {
            place_past_pos[i][place_past_latest[i]] = player_work[i]->pos;
            place_past_ang[i][place_past_latest[i]] = player_work[i]->ang;
        }
    }
}

/* set *pos and *ang to player's pos and angles back steps before the latest
 * camera_pasts_step kept. either may be null */
static void camera_player_past(int player, int back, NJS_POINT3 *pos,
                               struct angles *ang)
{
    int i = (Uint8)(place_past_latest[player] - back);

    if (pos) {
        *pos = place_past_pos[player][i];
    }
    if (ang) {
        *ang = place_past_ang[player][i];
    }
}
