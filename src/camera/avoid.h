/* the camera's wall avoidance: a sight test, and a search beside a player
 * for a place in sight */
#ifndef CAMERA_AVOID_H
#define CAMERA_AVOID_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

/* the count of turns in the search's table */
#define AVOID_TRIES 60

/* a search's states. CLEAR leaves the camera where its mode put it. RETURN
 * starts the camera back there, and RETURNING takes it there. SEARCH starts
 * a search, and SEARCHING tries its places. HOLDING keeps the camera at the
 * place the search found. the states that search or hold, SEARCH to
 * HOLDING, are numbered above the rest */
#define AVOID_CLEAR 0
#define AVOID_RETURN 1
#define AVOID_RETURNING 2
#define AVOID_SEARCH 3
#define AVOID_SEARCHING 4
#define AVOID_HOLDING 5

/* a search for a place in sight of a player. it tries places radius from
 * the player's pos, along ang turned by each of the table's turns */
struct camera_avoid {
    Sint32 state; /* an AVOID_ state */
    NJS_POINT3 start_pos; /* the camera's pos where the search started */
    /* the camera's pos less the player's target. a search keeps the camera
     * there, and a hold moves it there */
    NJS_VECTOR offset;
    /* the direction the table's turns start from, as look_vector_xy reads
     * angles. ang.z is 0 */
    struct angles ang;
    Sint32 next_try; /* the index of the turn camera_avoid_try tries next */
    Sint32 steps; /* the steps since the camera started back or to hold */
    Sint32 cleared; /* 1 once a hold has found its place in sight */
    Float radius; /* how far from the player's pos the search's places lie */
};

/* set *pos to player's target: its work's pos, raised by its record's
 * target_height. FOLLOW, CARMEL and the searches look at it. player's slot
 * must hold a player */
void camera_player_target(int player, NJS_POINT3 *pos);

/* 1 where the land hides player from pos, else 0. the line runs to pos from
 * a point 1 behind the player's target, along its heading. it passes over
 * each of the world's objects with a bit of WORLD_FLAT. player's slot must
 * hold a player */
int camera_sight_blocked(int player, NJS_POINT3 *pos);

/* try the search's place at next_try. it lies radius from player's pos,
 * along look_vector_xy of ang plus the turn at next_try. where the place is
 * in sight of player, *out gets it, and it returns 1. otherwise next_try
 * moves on, from the last turn back to the first, and it returns 0 */
int camera_avoid_try(int player, struct camera_avoid *avoid, NJS_POINT3 *out);

#endif
