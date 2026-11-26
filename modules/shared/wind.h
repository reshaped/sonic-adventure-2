/* the wind, WIND in a stage's object list: rising air that lifts the
 * players and carries stones up */
#ifndef SHARED_WIND_H
#define SHARED_WIND_H

struct task;

/* the bit of a wind's ang.x that makes it clear. a clear wind throws up no
 * stones. a column that isn't clear is 20 narrower than scl.x. from HALF_LINE
 * of shared/canyon_util.h up, a cone that isn't clear lifts a player as such
 * a column does */
#define WIND_CLEAR 0x1

/* the object list's step for WIND. unless placement_try_leave(task) returns
 * TRUE, it gives task the wind's step, draw and destructor. the wind starts
 * with no stones. task needs a work and TASK_EXTRA's record. player 0's work
 * must exist, unchecked, and player 1's in a two-player game.
 *
 * the work's pos is the wind's base, and scl.y its height. scl.x is its
 * radius. where scl.z is the same, the wind is a column. otherwise it is a
 * cone, which draws a player toward its tip. the cone's radius falls evenly
 * from scl.x at a world y of 0 to 0 at its tip. the tip lies scl.x * h /
 * (scl.x - scl.z) above the base. h is scl.y, or the height from the base to
 * HALF_LINE where that is less */
void wind_init(struct task *task);

#endif
