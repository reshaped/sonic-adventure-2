/* the wall warp, WARP and WARP_N in Dry Lagoon's object list: a painted wall
 * of two kinds. a Knuckles or a Rouge in player 0's slot who digs into it
 * comes out somewhere else */
#ifndef STG18_WALL_WARP_H
#define STG18_WALL_WARP_H
#include <ninja.h>

struct task;

/* the wall's texture lists for texture_load_shared, a kind's at its index,
 * ended by null */
extern NJS_TEXLIST *lagoon_wall_warp_texlists[];

/* the object list's step for WARP and WARP_N. unless placement_try_leave(task)
 * returns TRUE, it gives task the wall's own step, draw and destructor.
 *
 * task needs a work. its pos places the wall, and its ang.y turns it. the low
 * 4 bits of ang.x, modulo 3, pick the wall's size: 60, 50 or 40. 60 is the
 * model's own size. the next 4 bits, modulo 2, pick the kind: BRIGHT or DARK.
 * scl is where player 0 comes out, and ang.z the angle it comes out at, about
 * y. the call keeps only the low 5 bits of ang.x, and sets ang.z to 0.
 *
 * the call gives task a collision record of one box. at a size of 60, the box
 * is 20 wide, high and deep. its center lies 10 below pos. at another size,
 * the box and its offset scale by size / 60. the box pushes nothing, and
 * resists the push of players, enemies and objects. the heap must have room
 * for the record, unchecked.
 *
 * a dig starts once any player touches the box while player 0 digs. a player
 * digs in PLAYER_MODE_HUNTER_WALL_DIG, PLAYER_MODE_HUNTER_WALL_DIG_OUT or
 * PLAYER_MODE_UNK_43, at frame 10 or later of its animation. the dig sets
 * game_state's pause_disabled to 1 and its idle_dim to 0, and the screen fades
 * out. 74 steps after the dig's step, player 0 and camera 0 move to scl. the
 * screen then fades in. player 0 takes PLAYER_REQUEST_RELEASE, and both players
 * take the light at light_camera's index. 41 steps after the move, the warp
 * sets pause_disabled to 0 and idle_dim to 1. player 0's work and record must
 * not be null, unchecked */
void lagoon_wall_warp_init(struct task *task);

#endif
