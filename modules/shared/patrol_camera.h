/* the patrol cameras, CAMV and CAMH in Eternal Engine's and Lost Colony's
 * object lists. each is a swaying base, with a head an attack knocks off */
#ifndef SHARED_PATROL_CAMERA_H
#define SHARED_PATROL_CAMERA_H

struct task;

/* the work's mode. CAMERA_BROKEN is also the placement's state once the head
 * is off */
enum patrol_camera_mode { CAMERA_PATROLLING, CAMERA_BROKEN };

/* the object list's steps for CAMV and CAMH. unless placement_try_leave(task)
 * returns TRUE, each gives task the camera's step, draw and destructor, and a
 * sphere. it also gives task a child at the head, which an attack can hurt.
 * it makes the child even where the camera starts without its head.
 *
 * task needs a work. pos places the camera, and ang.y turns it. the head
 * faces ang.y, or half a turn from it where bit 0 of ang.x is set. CAMV bobs
 * up to 20 above and below where it starts. its bob's cycle advances scl.x
 * degrees a step. CAMH sways along its own x axis, up to scl.x either side of
 * where it starts. its sway's cycle advances ang.z Angle units a step.
 *
 * once an attack hurts the child, the head falls off and the camera scores
 * 20 points. the base then stays still, and CAMH stops colliding. a camera
 * whose placement's state isn't 0 starts without its head. one without its
 * head as its task ends sets the state to CAMERA_BROKEN */
void vertical_camera_init(struct task *task);
void horizontal_camera_init(struct task *task);

#endif
