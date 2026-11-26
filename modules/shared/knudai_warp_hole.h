/* the warp hole a flute stand opens, KDWARPHOLE in a stage's object list. it
 * carries a player from either of its two ends to the other */
#ifndef SHARED_KNUDAI_WARP_HOLE_H
#define SHARED_KNUDAI_WARP_HOLE_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the warp hole, the ring sparkles
 * and its swirls, ended by null. nothing of the warp hole draws with the
 * sparkles' list */
extern NJS_TEXLIST *knudai_warp_hole_texlists[];

/* the object list's step for KDWARPHOLE. task needs a work. its pos places the
 * warp hole's in end, and its scl the out end. its ang.x holds the id of the
 * warp hole's stand. a placement whose state isn't 0 starts the warp hole open.
 * a warp hole that starts shut does nothing where placement_try_leave(task)
 * returns TRUE. otherwise the call takes a record in task->motion, and gives
 * task the warp hole's step and destructor. where the heap can't give the
 * record, task keeps this step to try again.
 *
 * a shut warp hole starts to open once its stand is played. it then sets its
 * placement's state to 1. it ends once it leaves range while shut, and never
 * once it starts to open.
 *
 * an open end catches a player within 15 of it, as player_near measures. the
 * warp hole draws the player into that end, shrinking it. it grows the player
 * back to a scale of 1 at the other end. there it lets the player go with
 * PLAYER_REQUEST_RELEASE. once the stage is cleared, or the player is hurt or
 * has missed, the carry ends early. the player then takes the same scale and
 * release. after a release, neither end catches until no player has been near
 * either for 60 steps.
 *
 * a carried player's slot must keep its player_work and player_records entries
 * until the player is let go. the step reads both unchecked. the destructor
 * gives a carried player a scale of 1, but no PLAYER_REQUEST_RELEASE */
void knudai_warp_hole_init(struct task *task);

#endif
