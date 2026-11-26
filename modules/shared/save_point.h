/* the save point, SAVEPOINT in a stage's object list: a gate between two
 * lamps. the first player to pass the gate restarts there, with the clock at
 * the time of the pass */
#ifndef SHARED_SAVE_POINT_H
#define SHARED_SAVE_POINT_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the save point's texture list: one texture, sikake_01_128, which the gate
 * and both lamps draw with */
extern NJS_TEXLIST save_point_texlist;

/* the object list's step for SAVEPOINT. task needs a work and a TASK_EXTRA
 * record. the save point keeps its own record's address in the extra
 * record's first word. each step does nothing where
 * placement_try_leave(task) returns TRUE. where game_state.mission is
 * MISSION_TIMED, the first step calls placement_kill. otherwise it sets the
 * save point up. that needs the heap's room for the record and two child
 * tasks, unchecked.
 *
 * the work's pos places the gate, and its angles turn it. the low byte of
 * ang.y is the gate's order, not part of its heading. the order counts in a
 * two-player game whose stage_character(0) is PLAYER_SONIC or PLAYER_SHADOW.
 * in any other game it is 0.
 *
 * the first player to touch the gate passes it. the touch must be by the
 * player's own task, not by a child of it. the pass gives the player's pos
 * and ang, with the order, to restart_point_keep. it also gives the player
 * MESSENGER_EVENT_SAVE_POINT. a player with 20 rings or more gets an item
 * too, by that count. a ring item's rings go to game_state.ring_total and
 * snapshot_current->ring_total as well.
 *
 * where task has a placement, the first step calls placement_keep. a save
 * point whose placement has PLACEMENT_TOUCHED at its first step starts passed.
 * the pass sets that flag, and writes through null where task has no
 * placement.
 *
 * player n's pass shows the clock's time at the pass in camera n's draws, for
 * 300 steps. camera_current_index must be 0 or 1, unchecked. a draw takes the
 * readout from the record of the save point that last stepped or drew. that
 * can be another gate's record. in the SHC build it can also be one already
 * freed, which the GCC build's destructor prevents */
void save_point_step(struct task *task);

#endif
