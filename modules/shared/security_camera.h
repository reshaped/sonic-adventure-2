/* the security camera, CAM in Prison Lane's object list. its head turns to
 * watch a near player, until an attack knocks it off. stg11/security_camera.c
 * compiles this source again for Iron Gate */
#ifndef SHARED_SECURITY_CAMERA_H
#define SHARED_SECURITY_CAMERA_H
#include <ninja.h>

struct task;

/* the camera's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *security_camera_texlists[];

/* the object list's step for CAM. unless placement_try_leave(task) returns
 * TRUE, it gives task the camera's step, draw and destructor, and a collision
 * record. without room for the record, the camera goes on without one. it
 * also gives task a child at the head, which an attack can hurt. it makes
 * the child even where the camera starts without its head.
 *
 * task needs a work. pos places the camera, and ang.y turns it. the camera
 * keeps the head's heading in the work's timer. until the head falls, it
 * turns toward a near player by up to 128 Angle units a step. a player within
 * 300 of pos, as player_near measures, is near. where both players are near,
 * slot 0's player counts.
 *
 * once an attack hurts the child, the head falls off and the camera scores
 * 20 points. the camera's record goes unregistered on the step the head
 * falls. a camera whose placement's state isn't 0 starts without its head.
 * one that has lost its head as its task ends sets the state to 1 */
void security_camera_init(struct task *task);

#endif
