/* Death Chamber's ground warp, WARP2 in the stage's object list. it is the
 * wall warp of stg25/wall_warp.h, laid on the ground. a Knuckles or a Rouge
 * in player 0's slot digs or dives into it, and comes out elsewhere */
#ifndef STG25_GROUND_WARP_H
#define STG25_GROUND_WARP_H
#include <ninja.h>

struct task;

/* the ground warp's texture lists for texture_load_shared, ended by null.
 * its one list is the wall warp's */
extern NJS_TEXLIST *chamber_ground_warp_texlists[];

/* the object list's step for WARP2. it works as chamber_wall_warp_init does,
 * but for four things. the draw turns the model by minus a quarter turn about
 * x, after ang.y. the shape's center lies at the work's pos. a player digs in
 * PLAYER_MODE_HUNTER_DIG, PLAYER_MODE_HUNTER_DIG_OUT, PLAYER_MODE_UNK_3F or
 * PLAYER_MODE_HUNTER_DIVE, at frame 0.5 or later of its animation. the move
 * leaves the players' lights as they are */
void chamber_ground_warp_init(struct task *task);

#endif
