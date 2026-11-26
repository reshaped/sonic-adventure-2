/* the twining vine, KARAMI in a stage's object list: a vine that hangs from a
 * tree. a player who catches its tip is carried round the trunk, then swung
 * out and thrown */
#ifndef SHARED_TWINING_VINE_H
#define SHARED_TWINING_VINE_H

struct task;

/* the object list's step for KARAMI. its first step gives task a record in
 * task->extra, with a copy of vine_object's chain. both come from syCalloc,
 * unchecked, and the destructor frees both. where task->placement isn't null,
 * each step sets the placement's state to 1.
 *
 * task needs a work. the trunk stands at pos, and the vine's top lies
 * (scl.y + 1) * 285 + 210 above it. the vine hangs scl.x out from the trunk,
 * headed by ang.y. ang.x's low 16 bits are how far it turns round the trunk,
 * and must not be 0. ang.z's bit 0x8000 turns it the other way. ang.z's low 15
 * bits are the most its top tilts as it turns.
 *
 * a player whose first shape comes within 10 of the tip catches it. the catch
 * sets the vine's speed to the player's speed times 2.5, held to 4 to 30.
 * after the vine lets a player go, no catch comes for 30 steps, counting only
 * steps on which time moves. a player near the tip in that wait starts it
 * over. the vine starts with the wait done. the vine turns its player round
 * the trunk, then swings it out by a quarter turn. there it throws the player
 * out and up at scl.z - 3, and runs back to rest. the throw heads out from the
 * trunk only where ang.y is 0. the vine lets go a player who has missed, or
 * once the stage is cleared.
 *
 * while no player lies within 4000 of pos, the draw draws nothing. a step
 * after the first then only sets the placement's state. the vine moves only on
 * steps on which time moves. a catch during a time stop moves it through that
 * stop. once the wait after a throw is done, the task ends with task_kill */
void twining_vine_init(struct task *task);

#endif
