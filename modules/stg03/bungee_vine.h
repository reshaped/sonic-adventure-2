/* the bungee vine, BANGIETUTA in Green Forest's object list: a vine that hangs
 * high above the ground. a player who catches its tip drops with it, rides it
 * back up and is thrown */
#ifndef STG03_BUNGEE_VINE_H
#define STG03_BUNGEE_VINE_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST bungee_vine_texlist;

/* the object list's step for BANGIETUTA. its first step gives task a record in
 * task->extra, with a copy of the vine's chain. both come from syCalloc,
 * unchecked, and the destructor frees both. where task->placement isn't null,
 * the first step sets the placement's state to 1. the vine then ends with
 * task_kill once the state is 0.
 *
 * task needs a work. the vine's top lies 2600 above pos, and ang.y turns the
 * vine. the chain first hangs from scl.x above the top. after a player rides
 * the vine to its bottom, the chain hangs from at most scl.z above.
 *
 * a player whose first shape comes within 20 of the tip catches it. the catch
 * waits 120 steps on which time moves since the vine last let a player go. a
 * player near the tip in that wait starts it over. the vine starts with the
 * wait done. the player drops with the vine, farther while it holds A, and
 * rides it back up. there a task of arc_jump_create_turned throws the player
 * to a fixed point of the stage. the vine lets go a player who has missed, or
 * once the stage is cleared.
 *
 * the vine moves and draws only while a player lies within 4000 of pos. it
 * moves only on steps on which time moves. a catch during a time stop moves
 * it through that stop. after the throw, the task ends once a step finds time
 * moving and a player near */
void bungee_vine_init(struct task *task);

#endif
