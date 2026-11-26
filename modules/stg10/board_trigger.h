/* the board trigger, BCOL in Metal Harbor's object list. a player takes a
 * board there, or leaves one */
#ifndef STG10_BOARD_TRIGGER_H
#define STG10_BOARD_TRIGGER_H

struct task;

/* the object list's step for BCOL. unless placement_try_leave(task) returns
 * TRUE, it gives task the trigger's own step and destructor. the trigger draws
 * nothing.
 *
 * task needs a work. the trigger fires once, for the first player that
 * player_near finds within scl.x + 10 of pos. where the low bit of ang.x is
 * set, it sends that player PLAYER_REQUEST_BOARD, and its task ends. otherwise
 * it sends PLAYER_REQUEST_RELEASE. either way it ends harbor_missile_task's
 * missile, where there is one. a missile not yet in HARBOR_MISSILE_MODE_END
 * goes there, with its timer at 0 */
void harbor_board_trigger_init(struct task *task);

#endif
