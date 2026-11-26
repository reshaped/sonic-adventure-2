/* City Escape's sound objects, SE_PATCAR and SE_KAZE in its object list: a
 * patrol car's sound and a wind */
#ifndef STG13_STAGE_SOUNDS_H
#define STG13_STAGE_SOUNDS_H

struct task;

/* the object list's step for SE_PATCAR, a patrol car. unless
 * placement_try_leave(task) returns TRUE, it gives task the car's own step,
 * draw and destructor. each step makes the same test. task needs a work, whose
 * pos places the car. each step plays sound 12 of the stage's list there,
 * keyed by the work. the sound's pitch rises as the first camera nears the
 * car, and falls as the camera draws away. each step keeps the camera's
 * distance in the work's scl.x, and moves the one before to scl.y */
void patrol_car_sound_init(struct task *task);
/* the object list's step for SE_KAZE, a wind in a box. it sets task up as
 * patrol_car_sound_init does. the box is centered on the work's pos and turned
 * by its ang.y. it reaches the work's scl from its center along each axis. the
 * wind is sound 16 of the stage's list. while player 0 lies inside the box,
 * each step plays it by a chance of 3 in 100. it plays at a random point
 * within 265 of the player along each axis */
void wind_sound_init(struct task *task);

#endif
