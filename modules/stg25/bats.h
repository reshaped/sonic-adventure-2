/* the bats, BAT in Death Chamber's object list: a swarm of bats that hang in
 * a line. a bat flies out over the swarm's pos once player 0 comes near. it
 * flies about there for a while, and then flies home */
#ifndef STG25_BATS_H
#define STG25_BATS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the bats' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *chamber_bat_texlists[];

/* the object list's step for BAT. task needs a work and a placement.
 * camera_task_work and player 0's work must not be null, unchecked. the
 * work's fields mean:
 * - scl is the middle of the line the bats hang along.
 * - ang.x's high byte, in 256ths of a turn, turns each hanging bat about y.
 *   the line runs a quarter turn about y from the hanging bats' z axes.
 *   ang.x's low byte is half the line's length.
 * - ang.y's high byte, in 256ths of a turn, bounds a flying bat's tilt from
 *   straight up. ang.y's low byte is the count of bats.
 * - ang.z % 1000 is the fly distance. a bat flying about keeps from half of
 *   it to the whole of it from pos. its speed flying out and home is in
 *   proportion to the fly distance and to ang.y's high byte.
 *
 * the swarm leaves range once camera_task_work's pos lies far from both pos
 * and scl. far from pos is a squared distance past the placement's
 * leave.range plus the fly distance squared. far from scl is one past
 * leave.range plus ang.x's low byte squared. the call and each step test
 * the range first. out of range, each sets task's step to task_destroy, as
 * placement_try_leave does.
 *
 * each bat is a record from the Shinobi heap. the call leaves out a bat the
 * heap can't give. the swarm's destructor frees them */
void chamber_bats_init(struct task *task);

#endif
