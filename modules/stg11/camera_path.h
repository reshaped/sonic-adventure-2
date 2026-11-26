/* Iron Gate's camera path. it glides a player's camera along a fixed path
 * while the player's pad is off. no entry of the stage's object list names it.
 * missile_shutter_v_init's shutter starts it */
#ifndef STG11_CAMERA_PATH_H
#define STG11_CAMERA_PATH_H

struct task;

/* the step a task takes to run the path. task needs a work, whose submode
 * names the player, 0 or 1. the call kills task where that player's pad is off
 * already. it kills it too where the camera's stack has no room above its
 * top. otherwise it pushes the path's mode, with CAMERA_ADJUST_NONE, onto the
 * player's camera. it then turns the player's pad off.
 *
 * the work's timer then counts down from 161, a step past the path's 160. the
 * task kills itself once the timer reads 0. its destructor takes the mode off
 * the stack and turns the pad back on, whatever ends the task */
void camera_path_init(struct task *task);

#endif
