/* the weight, BUNCHIN in a stage's object list: a block that waits above the
 * ground. it drops, bounces where it lands and rises back */
#ifndef SHARED_WEIGHT_H
#define SHARED_WEIGHT_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* what drops a weight. the names are the object's own, and SONIC in one
 * means player 0 or player 1 */
enum weight_type {
    WEIGHT_UPDOWN, /* drop each time the weight is back at its height */
    /* drop on a player in the weight's column. the column reaches 20 past
     * the weight's sides in x and z, and 1000 above and below pos. the
     * player's flags must hold PLAYER_WORK_ON_GROUND or PLAYER_WORK_ON_SHAPE.
     * while any player is in the column, a rising weight drops again and a
     * landed one stays down */
    WEIGHT_DOWN_ON_SONIC,
    /* drop as WEIGHT_DOWN_ON_SONIC does while the weight's switch is off. a
     * landed weight rises only once the switch is on. while the switch is on,
     * the weight rises to its height and waits there */
    WEIGHT_DOWN_ON_SONIC_SW,
    /* behave as WEIGHT_UPDOWN, but wait at the weight's height while its
     * switch is on */
    WEIGHT_UPDOWN_SW
};
#define WEIGHT_TYPE_BITS 0x3 /* the bits of ang.x that hold the weight_type */
/* in ang.x: the weight lifts no player. without the flag, a falling weight
 * lifts onto its top each player its bottom touches. it lifts only a player
 * within 10 past its sides, and within 20 of pos in y. the lifted player takes
 * the x and z it had a step before */
#define WEIGHT_NORMAL 0x100
/* the bits of ang.z that hold the id of a _SW weight's switch */
#define WEIGHT_SWITCH_ID_BITS 0xff
/* in ang.z: the shadow under the weight reaches 200 lower */
#define WEIGHT_LONG_SHADOW 0x1000

/* the texture lists the weights and their dust draw with, for
 * texture_load_shared, ended by null */
extern NJS_TEXLIST *weight_texlists[];

/* build the handle on the weights' tops at buf, and return buf past the
 * build. nothing is built where the handle is built already, or where its
 * texture list isn't loaded. once the handle is built, every weight draws the
 * build with njDirectDrawObject */
Uint8 *weight_build(Uint8 *buf);
/* the object list's step for BUNCHIN. unless placement_try_leave(task) returns
 * TRUE, it puts the weight's record in task->motion. it gives task the weight's
 * step, draw and destructor, and a collision record. where the heap has no room
 * for the weight's record, weight_init stays task's step to try again.
 *
 * task needs a work and TASK_FORCE's records, and no record in its motion. the
 * heap must have room for the collision record, unchecked. player_work[0] must
 * not be null while a WEIGHT_DOWN_ON_SONIC or WEIGHT_DOWN_ON_SONIC_SW weight
 * waits, unchecked too.
 *
 * weight_init clamps scl.x and scl.z between 0 and 300. the weight is then 20
 * high. it is 40 * (scl.x + 1) wide in x. it is 40 * (scl.z + 1) deep in z.
 * ang.y turns it. its bottom waits |scl.y| + 30 above pos, and falls to pos.
 *
 * weight_init clears every bit of ang.x but WEIGHT_TYPE_BITS and WEIGHT_NORMAL.
 * it clears every bit of ang.z but WEIGHT_SWITCH_ID_BITS and
 * WEIGHT_LONG_SHADOW. the collision checks still read the bits it keeps as
 * angles. those angles turn the centers of the weight's shapes about x and z.
 *
 * the weight puts a copy of its box in the collision world, for the players to
 * stand on. each step sets speed.y of TASK_FORCE's first record to how far the
 * top rose since the step before. while game_state.time_stopped isn't 0, the
 * weight neither moves nor hurts */
void weight_init(struct task *task);

#endif
