/* the mech bosses' camera: two views for camera 0's BOSSPOINT mode, and a
 * shake. Big Foot, Hot Shot and Flying Dog use them */
#ifndef SHARED_MECH_BOSS_CAMERA_H
#define SHARED_MECH_BOSS_CAMERA_H
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/stack.h"

struct task;

/* the near view each boss's opening ends in. camera 0's BOSSPOINT mode takes
 * this dist, height and part, and a lift of MECH_BOSS_CAMERA_LIFT.
 * mech_boss_camera_near eases the first three back to it */
#define MECH_BOSS_CAMERA_DIST 30.0F
#define MECH_BOSS_CAMERA_HEIGHT 15.0F
#define MECH_BOSS_CAMERA_PART 0.5F
#define MECH_BOSS_CAMERA_LIFT 10.0F

/* the kinds of mech_boss_camera_shake. the distance is player 0's from pos */
enum mech_boss_shake {
    MECH_BOSS_SHAKE_HEAVY, /* 6 steps, at a size of the distance / 200 */
    MECH_BOSS_SHAKE_LIGHT /* 3 steps, at a size of the distance / 400 */
};

/* a camera_step for a mode whose work holds a pointer to a point. it puts the
 * camera 1.5 times as far from the world's origin as the point, and 20 higher.
 * the camera looks at player 0's pos. no code in the game's images names this
 * step */
void mech_boss_camera_step(struct camera_stack *stack,
                           struct camera_slot *slot);
/* ease camera 0's BOSSPOINT mode to height 100, dist 100 and part 0.2. then set
 * its level to 0. task and unused aren't read */
void mech_boss_camera_far(struct task *task, int unused);
/* ease camera 0's BOSSPOINT mode back to the near view's height, dist and
 * part. unused isn't read. both calls change the mode as camera/bosspoint.h
 * says */
void mech_boss_camera_near(int unused);
/* shake camera 0 by kind, at a size that grows with player 0's distance from
 * pos. kind must be one of enum mech_boss_shake, unchecked. any other leaves
 * the shake's steps and size unset. player_work[0] must not be null */
void mech_boss_camera_shake(NJS_POINT3 *pos, Sint8 kind);

#endif
