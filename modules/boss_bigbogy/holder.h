/* the holder: a task under King Boom Boo that moves the ghost carrying his
 * sandglass. the ghost, a GHOST_MAKIKO, is the holder's first child. it
 * bounces at its place behind him on his circle. it faces player 0, or him in
 * BIGBOGY_RESTORE_DARK. as player 0 nears, it can dash to the boss and back.
 * its first shape then guards against every attack */
#ifndef BOSS_BIGBOGY_HOLDER_H
#define BOSS_BIGBOGY_HOLDER_H
#include <sg_xpt.h>
struct task;

/* make the holder as boss's child, in its intro, without a ghost. its step
 * does nothing until bigbogy_holder_spawn_ghost gives it one. the call
 * returns null where an allocation fails */
struct task *bigbogy_holder_create(struct task *boss);
/* give holder a new ghost at its place, and end any dash. outside the intro
 * the holder then follows the boss. holder must have no child, unchecked:
 * this call and the holder take its first child for the ghost */
void bigbogy_holder_spawn_ghost(struct task *holder);
/* start a dash where player 0 lies within an arc of the ghost about the
 * center. the arc widens as the boss's health falls. player 0 must also lie
 * no farther round from the ghost than from the boss. the call returns
 * BIGBOGY_DASH_STARTED where the dash starts, and BIGBOGY_NO_GHOST where
 * holder has no ghost. it returns BIGBOGY_NO_DASH otherwise, as where
 * bigbogy_health() is below 0. bigbogy_health() must not be 0 */
Sint8 bigbogy_holder_try_dash(struct task *holder);
#define BIGBOGY_NO_GHOST (-1)
#define BIGBOGY_NO_DASH 0
#define BIGBOGY_DASH_STARTED 1
/* where the holder follows the boss, zero the ghost's speed across the ground
 * and clear the dash's flag. the call then returns TRUE */
Bool bigbogy_holder_settle(struct task *holder);
/* TRUE from a dash's start to its end or to bigbogy_holder_settle */
Bool bigbogy_holder_is_dashing(struct task *holder);
#endif
