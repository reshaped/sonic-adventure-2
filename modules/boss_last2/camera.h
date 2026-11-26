/* the Finalhazard's camera: camera 0's BOSSPOINT mode, and the motions of
 * the intro, a hit and the defeat */
#ifndef BOSS_LAST2_CAMERA_H
#define BOSS_LAST2_CAMERA_H
#include <ninja.h>
#include <sg_xpt.h>

/* a pointer may be null only where a comment says so */

/* the motions finalhazard_camera_motion plays */
enum finalhazard_camera_kind {
    FINALHAZARD_CAMERA_INTRO,
    FINALHAZARD_CAMERA_HIT,
    FINALHAZARD_CAMERA_DEFEAT
};

/* finalhazard_camera_motion's flag that holds the camera on the motion's
 * last frame */
#define FINALHAZARD_CAMERA_HOLD 1
/* play camera 0's motion of kind about finalhazard_center, at speed frames a
 * step. where camera 0's top mode lies above CAMERA_MOTION_SLOT, that mode
 * comes off first. without FINALHAZARD_CAMERA_HOLD in flags, the mode below
 * the motion blends back in at its end */
void finalhazard_camera_motion(Sint8 kind, Sint8 flags, Float speed);
/* start camera 0's top mode blending in again, by CAMERA_ADJUST_RELATIVE2 */
void finalhazard_camera_blend(void);
/* set *out to *in mirrored through the plane z = 0. a mirror nearer the
 * plane than 100 moves off it. its z becomes -120 for player 0, or 100 for
 * any other player */
void finalhazard_camera_mirror(const NJS_POINT3 *in, NJS_POINT3 *out,
                               Sint8 player);
/* push camera 0's BOSSPOINT mode, which keeps *target beyond the player.
 * target must outlive the mode */
void finalhazard_camera_target(NJS_POINT3 *target);
/* have camera 0's BOSSPOINT mode follow player */
void finalhazard_camera_follow(Sint8 player);
#endif
