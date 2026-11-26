/* the test that pushes the camera's GAKUGAKU mode: whether a camera has
 * settled in place while its player stands still */
#ifndef CAMERA_GAKUGAKU_H
#define CAMERA_GAKUGAKU_H

/* the places of each camera that camera_settle_step keeps */
#define CAMERA_SETTLE_PLACES 10
/* camera_settle_step finds a camera settled only while world_gravity's y is
 * at most CAMERA_SETTLE_GRAVITY: while gravity points straight down */
#define CAMERA_SETTLE_GRAVITY (-0.999F)
/* the delay camera_delay_settle_all gives each camera, in calls of
 * camera_settle_step */
#define CAMERA_SETTLE_DELAY 180

/* keep the pos of camera_views[camera] in place of the oldest of the
 * CAMERA_SETTLE_PLACES places kept for camera. it returns 1 where the pos
 * lies in the box the other places span, faces included. it returns 0 where
 * the pos lies outside, or where camera_stacks[camera].standing is 0. a
 * place never kept is the origin, and nothing clears the places.
 *
 * it keeps nothing and returns 0 while a camera motion plays, and while
 * world_gravity's y is above CAMERA_SETTLE_GRAVITY. it does the same while
 * camera's delay is above 0, and counts the delay down by 1. where the
 * current camera's player is in PLAYER_MODE_HUNTER_CLIMB, it keeps nothing,
 * returns 0, and gives the current camera a delay of CAMERA_SETTLE_DELAY. the
 * test reads only the mode's number, which is PLAYER_MODE_SONIC_BOUNCE for a
 * Sonic */
int camera_settle_step(int camera);
/* set camera's delay to calls */
void camera_delay_settle(int camera, int calls);
/* set every camera's delay to CAMERA_SETTLE_DELAY */
void camera_delay_settle_all(void);

#endif
