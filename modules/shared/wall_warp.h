/* the wall warp, WARP and WARP_N in Wild Canyon's object list: a painted wall
 * of two kinds. a Knuckles or a Rouge who digs into it comes out somewhere
 * else */
#ifndef SHARED_WALL_WARP_H
#define SHARED_WALL_WARP_H
#include <ninja.h>

struct task;

/* the wall's texture lists for texture_load_shared, a kind's at its index,
 * ended by null */
extern NJS_TEXLIST *wall_warp_texlists[];

/* the object list's step for WARP and WARP_N. unless half_try_leave or
 * placement_try_leave returns TRUE for task, it gives task the wall's own step,
 * draw and destructor. it gives task a collision record of one box too. the
 * heap must have room for the record, unchecked.
 *
 * task needs a work. its pos places the wall, and its ang.y turns it. the low 4
 * bits of ang.x, modulo 3, pick the wall's size: 60, 50 or 40. 60 is the
 * model's own size. the next 4 bits, modulo 2, pick the kind: BRIGHT or DARK.
 * scl is where the digger comes out, and ang.z the angle it comes out at, about
 * y. the call keeps only the low 5 bits of ang.x, and sets ang.z to 0.
 *
 * a player digs in PLAYER_MODE_HUNTER_WALL_DIG, PLAYER_MODE_HUNTER_WALL_DIG_OUT
 * or PLAYER_MODE_UNK_43. a dig starts once any player touches the box while
 * player 0 digs. in a two-player game, player 1's dig counts too. where both
 * dig, player 1 is the digger. the screen fades out, the digger and its camera
 * move to scl, and the screen fades back in. from the dig's start to its end,
 * game_state's pause_disabled is 1 and its idle_dim 0. player 0's work and
 * record, and in a two-player game player 1's, must not be null, unchecked */
void wall_warp_init(struct task *task);

#endif
