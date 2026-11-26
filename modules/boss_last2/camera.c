#include <ninja.h>
#include <sg_xpt.h>
#include "boss_last2/arena.h"
#include "boss_last2/camera.h"
#include "boss_last2/motion_art.h"
#include "camera/bosspoint.h"
#include "camera/motion.h"
#include "camera/stack.h"
#include "system/port.h"

/* a mirror within MIRROR_MARGIN of its plane moves off it. its z becomes
 * PLAYER_0_Z for player 0, or PLAYER_1_Z for any other */
#define MIRROR_MARGIN 100.0F
#define PLAYER_0_Z (-120.0F)
#define PLAYER_1_Z 100.0F

/* the motion of each finalhazard_camera_kind */
KEPT static NJS_MOTION *motions[3] = {
#include "boss_last2/camera/motions.inc"
};
/* the BOSSPOINT mode's settings, but its target */
/* @hack type: a one-element array, where a struct alone would do */
static struct camera_bosspoint follow[1] = {
#include "boss_last2/camera/follow.inc"
};

void finalhazard_camera_motion(Sint8 kind, Sint8 flags, Float speed)
{
    struct camera_motion motion;
    motion.origin = &finalhazard_center;
    motion.ang = 0;
    motion.flags = CAMERA_MOTION_TURNS;
    if (flags & FINALHAZARD_CAMERA_HOLD) {
        motion.flags |= CAMERA_MOTION_HOLD;
    } else {
        motion.flags |= CAMERA_MOTION_END_BLENDS;
    }
    motion.done = NULL;
    motion.motion = motions[kind];
    motion.speed = speed;
    if (camera_top(0) > CAMERA_MOTION_SLOT) {
        camera_remove_mode(0, camera_top(0));
    }
    camera_motion_start(0, &motion);
}

void finalhazard_camera_blend(void)
{
    camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_RELATIVE2);
}

void finalhazard_camera_mirror(const NJS_POINT3 *in, NJS_POINT3 *out,
                               Sint8 player)
{
    NJS_POINT3 pos;
    pos = *in;
    pos.z = -pos.z;
    if (njAbs(pos.z) < MIRROR_MARGIN) {
        if (!player) {
            pos.z = PLAYER_0_Z;
        } else {
            pos.z = PLAYER_1_Z;
        }
    }
    *out = pos;
}

void finalhazard_camera_target(NJS_POINT3 *target)
{
    struct camera_bosspoint mode;
    mode = follow[0];
    mode.target = target;
    camera_bosspoint_start(0, &mode);
}

void finalhazard_camera_follow(Sint8 player)
{
    camera_bosspoint_follow(player);
}
