/* the golem fights' camera, a machine of modes. each mode places camera 0 about
 * the golem and player 0 */
#ifndef SHARED_GOLEM_CAMERA_H
#define SHARED_GOLEM_CAMERA_H
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/golem.h"

struct task;

/* the camera's modes. most follow player 0's place. near, close and low are
 * as shared/golem.h's tests have them, and far is not near */
enum golem_camera_mode {
    /* the opening shot: camera 0's BOSSINIT mode about a point above the
     * golem, with player 0's pad off. it lasts until a request */
    GOLEM_CAMERA_INTRO,
    /* player 0 is far: the camera looks at the golem from beyond the player */
    GOLEM_CAMERA_FAR,
    /* player 0 is close: the camera looks at a point between weak_spot_pos and
     * the player. BOSS_GOLEM alone requests it */
    GOLEM_CAMERA_CLOSE,
    /* player 0 is near: camera 0's BOSSPOINT mode keeps camera_point beyond
     * the player */
    GOLEM_CAMERA_POINT,
    /* player 0 is far, and golem_turn_to_player lies beyond 0x6800 either
     * way. the camera looks at a lower point than GOLEM_CAMERA_FAR's, from
     * further back. BOSS_GOLEM alone requests it */
    GOLEM_CAMERA_FAR_TURNED,
    /* GOLEM_CAMERA_POINT from closer in. BOSS_GOLEM alone requests it, from
     * GOLEM_CAMERA_CLOSE while a hit on the weak spot throws player 0. it
     * lasts until the player is low, or is far once the throw ends */
    GOLEM_CAMERA_POINT_CLOSE,
    /* the camera looks at weak_spot_pos from a set direction. no code
     * requests it */
    GOLEM_CAMERA_FIXED,
    /* player 0 is low: the camera looks at a point below the player's
     * height. the point lies along the player's heading from the arena's
     * center */
    GOLEM_CAMERA_LOW,
    /* player 0 is low and hangs: the camera looks at the golem at the player's
     * height */
    GOLEM_CAMERA_HANG,
    GOLEM_CAMERA_MODES
};
/* what camera_next holds while no mode is requested */
#define GOLEM_CAMERA_NONE (-1)

/* task is the golem's. its extra must hold its boss record, whose own block
 * is the struct golem. the modes read task's work and player 0's work and
 * record, which must not be null. mode must lie from GOLEM_CAMERA_INTRO to
 * GOLEM_CAMERA_HANG, unchecked */

/* make mode current at once and start it, dropping any request. camera_steps
 * counts from 0 again */
void golem_camera_set(struct task *task, int mode);
/* run the current mode's step, and add 1 to camera_steps. the step may
 * request the mode that follows. where a mode is requested, the call first
 * makes it current, with camera_steps at 0. before it starts the mode, a
 * BOSSPOINT mode on top of camera 0's stack comes off. the mode then on top
 * takes CAMERA_ADJUST_RELATIVE3 */
void golem_camera_step(struct task *task);
/* request mode, which the next golem_camera_step makes current. a later
 * request replaces it */
void golem_camera_request(struct task *task, int mode);

#endif
