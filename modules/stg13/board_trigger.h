/* the board trigger, BOARDCOL in City Escape's object list. a player takes a
 * board there, or leaves one */
#ifndef STG13_BOARD_TRIGGER_H
#define STG13_BOARD_TRIGGER_H

struct task;

/* the object list's step for BOARDCOL. unless placement_try_leave(task)
 * returns TRUE, it gives task the trigger's own step and destructor. the
 * trigger draws nothing.
 *
 * task needs a work. the trigger fires once in its task's life. it fires for
 * the first player that player_near finds within scl.x + 10 of pos. where the
 * low bit of ang.x is set, it sends that player PLAYER_REQUEST_BOARD, and
 * otherwise PLAYER_REQUEST_RELEASE */
void board_trigger_init(struct task *task);

#endif
