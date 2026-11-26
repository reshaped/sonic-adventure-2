/* White Jungle's bungee vine, BANGIETUTA in the stage's object list: a vine
 * that hangs high above the ground. a player who catches its tip drops with
 * it, bounces on it and is thrown */
#ifndef STG04_BUNGEE_VINE_H
#define STG04_BUNGEE_VINE_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST white_jungle_bungee_vine_texlist;

/* the object list's step for BANGIETUTA. its first step gives task a record in
 * task->extra, with a copy of the vine's chain. both come from syCalloc,
 * unchecked, and the destructor frees both. where task->placement isn't null,
 * the first step sets the placement's state to 1. the vine then ends with
 * task_kill once the state is 0.
 *
 * task needs a work. the vine's top lies 2600 above pos, and ang.y turns the
 * vine. the chain first hangs from scl.x above the top. after a player rides
 * the vine to its bottom, the chain hangs from at most scl.z above.
 * from the vine's fourth bottom on, it hangs from scl.z at the bottom too.
 *
 * a player whose first shape comes within 10 of the tip catches it. the catch
 * waits until the vine has moved 120 steps since it started or threw a
 * player. the player drops and bounces with the vine. the vine throws it by
 * player_fling. a press of A throws it while the vine falls no faster than 3
 * times white_jungle_vine_gravity a step. once the vine has reached its bottom
 * twice since its start, it throws without A too. it then throws on any step
 * where it falls faster than that gravity, or ripples. the throw's speed along
 * x is a tenth of ang.z's low half, as a signed number. its speed up is the
 * vine's speed plus scl.y, and ang.y turns it. the vine lets its player go
 * only by a throw.
 *
 * the vine moves and draws only while a player lies within 4000 of pos */
void white_jungle_bungee_vine_init(struct task *task);

#endif
