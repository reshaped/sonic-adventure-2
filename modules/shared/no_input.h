/* the input stop, NOINPCOL in a stage's object list. it is a sphere that takes
 * the jump and action buttons from each player inside it */
#ifndef SHARED_NO_INPUT_H
#define SHARED_NO_INPUT_H

struct task;

/* the object list's step for NOINPCOL. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise task takes the stop's own
 * step, draw and destructor. NoconTask, the one task the stops share, starts
 * in level 0 unless it runs already. task needs a work, whose pos is the
 * sphere's center and whose scl.x is its radius.
 *
 * a stop's step marks each of players 0 and 1 whose latest pos lies inside the
 * sphere. player_get_past gives that pos. each step of NoconTask clears the
 * pad_jump_ and pad_action_ flags of system/pad.h for each marked player. it
 * then drops the marks. NoconTask steps before the stops, so a mark clears the
 * flags on the next step */
void no_input_init(struct task *task);

#endif
