/* the wall warp, WARP in Death Chamber's object list: a warp in a wall. a
 * Knuckles or a Rouge in player 0's slot who digs into it comes out somewhere
 * else */
#ifndef STG25_WALL_WARP_H
#define STG25_WALL_WARP_H
#include <ninja.h>

struct task;

/* Both warp tasks draw the same disc. */
extern NJS_TEXLIST chamber_warp_texlist;
extern NJS_CNK_MODEL chamber_warp_model;

/* the warp's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *chamber_wall_warp_texlists[];

/* the object list's step for WARP. task needs a work. its pos places the warp,
 * and its ang.y turns it. |ang.x| modulo 3 picks the warp's size: 60, 50 or 40,
 * where 60 is the model's own. scl is where player 0 comes out, and ang.z the
 * angle it comes out at, about y. the call sets ang.x and ang.z to 0.
 *
 * the call gives task a collision record of one box. at a size of 60, the box
 * is 20 wide, high and deep. its center lies 10 below pos. at another size, the
 * box and its offset scale by size / 60. the box pushes no shape of a player,
 * an enemy, an object or a pickup. it resists the push of players, enemies and
 * objects. the heap must have room for the record, unchecked.
 *
 * a dig starts once any player touches the box while player 0 digs. a player
 * digs in PLAYER_MODE_HUNTER_WALL_DIG, PLAYER_MODE_HUNTER_WALL_DIG_OUT or
 * PLAYER_MODE_UNK_43, at frame 10 or later of its animation. the dig sets
 * game_state's pause_disabled to 1 and its idle_dim to 0, and the screen fades
 * out. 74 steps after the dig's step, player 0 and camera 0 move to scl. the
 * screen then fades in. player 0 takes PLAYER_REQUEST_RELEASE, and both players
 * take the light at light_camera's index. 41 steps after the move, the warp
 * sets pause_disabled to 0 and idle_dim to 1. player 0's work must not be null,
 * unchecked */
void chamber_wall_warp_init(struct task *task);

#endif
