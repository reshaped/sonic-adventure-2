/* ObjectEggJump, which carries a player along an arc from one point to
 * another */
#ifndef STG43_EGG_JUMP_H
#define STG43_EGG_JUMP_H
#include <ninja.h>

struct task;

/* carry the player in slot along an arc from *from to *to, both copied. let
 * it go at the end, at the arc's speed there. the arc peaks 20 above the
 * higher point, plus 3 tenths of the distance across x and z. player 0's
 * gravity sets the arc's speed and its length in steps. the carried player's
 * own gravity bends its path. return the task, or null where task_create
 * fails.
 *
 * the task requests PLAYER_REQUEST_ANIM of the player as it starts. as it lets
 * the player go, it requests PLAYER_REQUEST_RELEASE and sets the player's
 * stick_lock to 2. it requests the release too where it ends early. it ends
 * early once game_state.cleared is 1 or the player has missed, or where
 * another call destroys it.
 *
 * the task waits to start while game_state.cleared is 1, or while the heap
 * lacks room for its record. it holds still while game_state.time_stopped
 * isn't 0, but not for a stop under way at the call */
struct task *egg_jump_create(int slot, NJS_POINT3 *from, NJS_POINT3 *to);

#endif
