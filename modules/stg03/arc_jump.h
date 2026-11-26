/* the arc jump, the task ObjectSonicJump: the iron bar's throw without the
 * bar. it carries a player somersaulting along an arc from one point to
 * another. it gives the player's camera a mode that follows the player */
#ifndef STG03_ARC_JUMP_H
#define STG03_ARC_JUMP_H
#include <ninja.h>

struct task;

/* create the arc jump's task, ObjectSonicJump, at task level 2. the task
 * carries the player in slot along an arc from from to to, which it copies.
 * it returns the task, or null where task_create fails. player 0 and the
 * player in slot must exist, unchecked.
 *
 * the task's first step works the arc out. while game_state.cleared is 1, or
 * the heap can't give the task its record, the step does nothing. the next
 * step tries again. the arc tops out 20 above the higher of from and to, plus
 * 0.3 of their ground distance. player 0's gravity sets its speeds.
 *
 * each step after that puts the player on the arc. the player somersaults
 * through the first two thirds, playing ANIM_BAR_THROW_BACK and then
 * ANIM_BAR_FLY_BACK. it plays ANIM_BAR_LAND through the last third. the
 * player's camera takes a mode that follows the player. past the arc's end,
 * the task lets the player go by PLAYER_REQUEST_RELEASE. the player leaves at
 * the arc's speed, with a stick_lock of 2. the next step ends the task.
 *
 * the task ends too where game_state.cleared is 1, or the player is in
 * PLAYER_MODE_HURT or has PLAYER_MISSED. the destructor lets go a player the
 * task still carries, and takes the camera's mode off. the arc holds still
 * through a time stop, but for one under way as the task was created */
struct task *arc_jump_create_turned(int slot, NJS_POINT3 *from, NJS_POINT3 *to);

#endif
