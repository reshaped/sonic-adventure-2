/* the camera's wall avoidance: a sight test, and a search beside a player
 * for a place in sight */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/avoid.h"
#include "camera/camera.h"
#include "camera/helpers.h"
#include "camera/probe.h"
#include "camera/stack.h"
#include "collision/world.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/vector.h"

/* the radius camera_avoid_step gives its searches */
#define SEARCH_RADIUS 30.0F
/* a hold searches again where the land hides the camera. it waits for more
 * than HOLD_STEPS steps unless the camera has been in sight */
#define HOLD_STEPS 90
/* the steps the camera takes to go back to where its mode puts it */
#define RETURN_STEPS 300
/* a hold moves the camera toward its place CATCH_UP times as far as the
 * player moved */
#define CATCH_UP 2.0F
/* where camera_avoid_step prints its search's state and next_try */
#define PRINT_STATE NJM_LOCATION(1, 20)
#define PRINT_TRY NJM_LOCATION(1, 21)
#define PRINT_DIGITS 2

/* a turn of the search's direction: about x, then about y */
struct avoid_turn {
    Angle x;
    Angle y;
};

/* the search's turns, in the order it tries them. |x| + |y| is 90 degrees
 * in the first group, then 60, 120, 30 and 150. within a group, |x| rises
 * from 0 in steps of 30 */
static struct avoid_turn avoid_turns[AVOID_TRIES] = {
    {DEGREES(0), DEGREES(-90)},    {DEGREES(0), DEGREES(90)},
    {DEGREES(-30), DEGREES(-60)},  {DEGREES(30), DEGREES(-60)},
    {DEGREES(-30), DEGREES(60)},   {DEGREES(30), DEGREES(60)},
    {DEGREES(-60), DEGREES(-30)},  {DEGREES(60), DEGREES(-30)},
    {DEGREES(-60), DEGREES(30)},   {DEGREES(60), DEGREES(30)},
    {DEGREES(-90), DEGREES(0)},    {DEGREES(90), DEGREES(0)},
    {DEGREES(0), DEGREES(-60)},    {DEGREES(0), DEGREES(60)},
    {DEGREES(-30), DEGREES(-30)},  {DEGREES(-30), DEGREES(30)},
    {DEGREES(30), DEGREES(-30)},   {DEGREES(30), DEGREES(30)},
    {DEGREES(-60), DEGREES(0)},    {DEGREES(60), DEGREES(0)},
    {DEGREES(0), DEGREES(-120)},   {DEGREES(0), DEGREES(120)},
    {DEGREES(-30), DEGREES(-90)},  {DEGREES(30), DEGREES(-90)},
    {DEGREES(-30), DEGREES(90)},   {DEGREES(30), DEGREES(90)},
    {DEGREES(-60), DEGREES(-60)},  {DEGREES(60), DEGREES(-60)},
    {DEGREES(-60), DEGREES(60)},   {DEGREES(60), DEGREES(60)},
    {DEGREES(-90), DEGREES(-30)},  {DEGREES(90), DEGREES(-30)},
    {DEGREES(-90), DEGREES(30)},   {DEGREES(90), DEGREES(30)},
    {DEGREES(-120), DEGREES(0)},   {DEGREES(120), DEGREES(0)},
    {DEGREES(0), DEGREES(-30)},    {DEGREES(0), DEGREES(30)},
    {DEGREES(-30), DEGREES(0)},    {DEGREES(30), DEGREES(0)},
    {DEGREES(0), DEGREES(-150)},   {DEGREES(0), DEGREES(150)},
    {DEGREES(-30), DEGREES(-120)}, {DEGREES(30), DEGREES(-120)},
    {DEGREES(-30), DEGREES(120)},  {DEGREES(30), DEGREES(120)},
    {DEGREES(-60), DEGREES(-90)},  {DEGREES(60), DEGREES(-90)},
    {DEGREES(-60), DEGREES(90)},   {DEGREES(60), DEGREES(90)},
    {DEGREES(-90), DEGREES(-60)},  {DEGREES(90), DEGREES(-60)},
    {DEGREES(-90), DEGREES(60)},   {DEGREES(90), DEGREES(60)},
    {DEGREES(-120), DEGREES(-30)}, {DEGREES(120), DEGREES(-30)},
    {DEGREES(-120), DEGREES(30)},  {DEGREES(120), DEGREES(30)},
    {DEGREES(-150), DEGREES(0)},   {DEGREES(150), DEGREES(0)},
};

/* @hack type: the probe's pos, next, skip and radius as data of their own,
 * and its other members as one block after them, where one struct
 * camera_probe would do. SHC places the five in the probe's order, with
 * nothing between them */
#ifdef NONMATCHING
/* this side takes the plain form, one struct camera_probe. GCC needn't
 * place separate data in order, and it drops each one whose address nothing
 * takes */
static struct camera_probe sight;
#define SIGHT_PROBE (&sight)
#define SIGHT(member) sight.member
#else
static NJS_POINT3 sight_pos;
static NJS_POINT3 sight_next;
static Uint32 sight_skip;
static Float sight_radius;
static Uint8 sight_rest[sizeof(struct camera_probe) - (2 * sizeof(NJS_POINT3)) -
                        sizeof(Uint32) - sizeof(Float)];
/* the probe camera_sight_blocked casts: the data from sight_pos on */
#define SIGHT_PROBE ((struct camera_probe *)&sight_pos)
#define SIGHT(member) sight_##member
#endif

static struct camera_avoid avoids[GAME_PLAYERS];

void camera_player_target(int player, NJS_POINT3 *pos)
{
    *pos = player_work[player]->pos;
    pos->y += player_records[player]->consts.target_height;
}

/* set *pos to player's target as it was a step ago */
static void player_target_before(int player, NJS_POINT3 *pos)
{
    player_get_past(player, 1, pos, NULL);
    pos->y += player_records[player]->consts.target_height;
}

int camera_sight_blocked(int player, NJS_POINT3 *pos)
{
    NJS_VECTOR back;
    struct angles heading;

    camera_player_target(player, &SIGHT(pos));
    SIGHT(next) = *pos;
    heading = player_work[player]->ang;
    heading.x = 0;
    heading.z = 0;
    turn_x_axis(&heading, &back);
    njUnitVector(&back);
    back.y = 0.0F;
    vector_subtract(&SIGHT(pos), &back, &SIGHT(pos));
    SIGHT(radius) = 0.0F;
    SIGHT(skip) = WORLD_FLAT;
    return camera_probe_cast(SIGHT_PROBE);
}

/* 1 where the land hides player from camera_state's pos, else 0. a hidden
 * camera starts a search unless it searches or holds already. a camera in
 * sight that searches or holds starts back */
static int avoid_check(int player, struct camera_avoid *avoid)
{
    /* @hack spelling: the else after a return, where the statements after
     * the if would do */
    if (camera_sight_blocked(player, &camera_state.pos)) {
        if (avoid->state <= AVOID_RETURNING) {
            avoid->state = AVOID_SEARCH;
        }
        return 1;
    } else {
        if (avoid->state >= AVOID_SEARCH) {
            avoid->state = AVOID_RETURN;
        }
        return 0;
    }
}

int camera_avoid_try(int player, struct camera_avoid *avoid, NJS_POINT3 *out)
{
    NJS_POINT3 place;
    NJS_VECTOR dir;
    struct angles ang;

    ang.x = avoid->ang.x + avoid_turns[avoid->next_try].x;
    ang.y = avoid->ang.y + avoid_turns[avoid->next_try].y;
    look_vector_xy(&ang, &dir);
    dir.x *= avoid->radius;
    dir.y *= avoid->radius;
    dir.z *= avoid->radius;
    vector_add(&player_work[player]->pos, &dir, &place);
    /* @hack spelling: the else after a return, where the statements after
     * the if would do */
    if (!camera_sight_blocked(player, &place)) {
        *out = place;
        return 1;
    } else {
        avoid->next_try++;
        if (avoid->next_try >= AVOID_TRIES) {
            avoid->next_try = 0;
        }
        return 0;
    }
}

static void avoid_start_search(int player, struct camera_avoid *avoid)
{
    NJS_POINT3 target;

    player_target_before(player, &target);
    avoid->start_pos = camera_stacks[player].place.pos;
    vector_subtract(&avoid->start_pos, &target, &avoid->offset);
    look_angles(&avoid->offset, &avoid->ang.x, &avoid->ang.y);
    avoid->ang.x *= -1;
    avoid->ang.z = 0;
    avoid->next_try = 0;
    avoid->state = AVOID_SEARCHING;
    avoid->radius = SEARCH_RADIUS;
    avoid->cleared = 0;
}

/* move camera_state's pos toward the goal, offset from the player's target
 * of a step ago. the camera moves from its place CATCH_UP times as far as
 * the target has moved since.
 *
 * it returns 1 where the target moved further than the camera's place lay
 * from the goal. camera_state's pos is then offset from the target as it is
 * now. it returns 0 otherwise */
static int avoid_place(int player, struct camera_avoid *avoid)
{
    NJS_POINT3 target;
    NJS_POINT3 target_before;
    NJS_VECTOR way;
    NJS_VECTOR move;
    Float gap;
    Float moved;
    int there;

    camera_player_target(player, &target);
    player_target_before(player, &target_before);
    vector_subtract(&camera_stacks[player].place.pos, &target_before, &way);
    way.x = avoid->offset.x - way.x;
    way.y = avoid->offset.y - way.y;
    way.z = avoid->offset.z - way.z;
    if (way.x == 0.0F && way.y == 0.0F && way.z == 0.0F) {
        gap = 0.0F;
    } else {
        gap = njUnitVector(&way);
    }
    vector_subtract(&target, &target_before, &move);
    if (move.x == 0.0F && move.y == 0.0F && move.z == 0.0F) {
        moved = 0.0F;
    } else {
        moved = njScalor(&move);
    }
    /* @hack redundant: the arm for a moved below 0, which njScalor never
     * returns, where setting there to moved > gap would do */
    if (moved > gap) {
        there = 1;
    } else if (moved < 0.0F) {
        if (gap < 0.0F) {
            there = 1;
        } else {
            there = 0;
            moved = 0.0F;
        }
    } else {
        there = 0;
    }
    if (there) {
        vector_add(&target, &avoid->offset, &camera_state.pos);
    } else {
        way.x *= moved * CATCH_UP;
        way.y *= moved * CATCH_UP;
        way.z *= moved * CATCH_UP;
        vector_add(&camera_stacks[player].place.pos, &way, &camera_state.pos);
    }
    return there;
}

/* keep the camera at offset from the player's target, and try a place as
 * camera_avoid_try does. a place in sight becomes the offset to hold. after
 * the last turn the state is AVOID_SEARCH, which starts the search over */
static void avoid_search(int player, struct camera_avoid *avoid)
{
    NJS_POINT3 place;
    NJS_VECTOR dir;
    struct angles ang;
    NJS_POINT3 target;

    ang.x = avoid->ang.x + avoid_turns[avoid->next_try].x;
    ang.y = avoid->ang.y + avoid_turns[avoid->next_try].y;
    look_vector_xy(&ang, &dir);
    dir.x *= avoid->radius;
    dir.y *= avoid->radius;
    dir.z *= avoid->radius;
    vector_add(&player_work[player]->pos, &dir, &place);
    camera_player_target(player, &target);
    vector_add(&target, &avoid->offset, &camera_state.pos);
    if (!camera_sight_blocked(player, &place)) {
        /* @hack redundant: the target taken again, as target holds it */
        camera_player_target(player, &target);
        vector_subtract(&place, &target, &avoid->offset);
        avoid->state = AVOID_HOLDING;
        avoid->steps = 0;
    } else {
        avoid->next_try++;
        if (avoid->next_try >= AVOID_TRIES) {
            avoid->state = AVOID_SEARCH;
        }
    }
}

static void avoid_hold(int player, struct camera_avoid *avoid)
{
    avoid_place(player, avoid);
    if (camera_sight_blocked(player, &camera_state.pos)) {
        if (avoid->cleared != 0 || avoid->steps > HOLD_STEPS) {
            avoid->state = AVOID_SEARCH;
        }
    } else {
        avoid->cleared = 1;
    }
}

/* @hack redundant: external linkage, which no other object needs, where
 * static would do */
/* @hack redundant: player, which nothing reads */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void avoid_start_return(int player, struct camera_avoid *avoid)
{
    avoid->steps = 0;
    avoid->state = AVOID_RETURNING;
}

/* move the camera steps / RETURN_STEPS of the way from its place to where
 * the mode put it */
static void avoid_return(int player, struct camera_avoid *avoid)
{
    camera_state.pos.x =
        ((camera_state.pos.x - camera_stacks[player].place.pos.x) *
         (Float)avoid->steps * (1.0F / RETURN_STEPS)) +
        camera_stacks[player].place.pos.x;
    camera_state.pos.y =
        ((camera_state.pos.y - camera_stacks[player].place.pos.y) *
         (Float)avoid->steps * (1.0F / RETURN_STEPS)) +
        camera_stacks[player].place.pos.y;
    camera_state.pos.z =
        ((camera_state.pos.z - camera_stacks[player].place.pos.z) *
         (Float)avoid->steps * (1.0F / RETURN_STEPS)) +
        camera_stacks[player].place.pos.z;
    if (avoid->steps >= RETURN_STEPS) {
        avoid->state = AVOID_CLEAR;
    }
}

/* step player's search, which tests and moves camera_state's pos for
 * player's camera. player must be below GAME_PLAYERS. nothing calls it. it
 * isn't static, since SHC leaves out a static function that nothing calls */
void camera_avoid_step(int player) /* NOLINT(misc-use-internal-linkage) */
{
    struct camera_avoid *avoid = &avoids[player];
    NJS_POINT3 target;

    avoid->steps++;
    avoid_check(player, avoid);
    switch (avoid->state) {
    case AVOID_CLEAR:
        break;
    case AVOID_RETURN:
        avoid_start_return(player, avoid);
        /* fall through */
    case AVOID_RETURNING:
        avoid_return(player, avoid);
        break;
    case AVOID_SEARCH:
        avoid_start_search(player, avoid);
        /* fall through */
    case AVOID_SEARCHING:
        avoid_search(player, avoid);
        camera_player_target(player, &target);
        camera_state.target = target;
        break;
    case AVOID_HOLDING:
        avoid_hold(player, avoid);
        camera_player_target(player, &target);
        camera_state.target = target;
        break;
    /* @hack redundant: a default that only breaks */
    default:
        break;
    }
    njPrintD(PRINT_STATE, avoid->state, PRINT_DIGITS);
    njPrintD(PRINT_TRY, avoid->next_try, PRINT_DIGITS);
}
