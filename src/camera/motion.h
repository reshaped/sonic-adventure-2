/* the camera's MOTION mode, which plays a Ninja camera motion on a camera */
#ifndef CAMERA_MOTION_H
#define CAMERA_MOTION_H
#include <ninja.h>
#include <sg_xpt.h>

/* the index of a camera's stack where camera_motion_start puts the MOTION
 * mode */
#define CAMERA_MOTION_SLOT 13

/* a camera motion's flags. with neither HOLD nor LOOP, the motion ends at its
 * last frame */
#define CAMERA_MOTION_HOLD 0x01 /* stay on the last frame */
#define CAMERA_MOTION_LOOP 0x02 /* start again from the first frame */
/* start with CAMERA_ADJUST_RELATIVE1, not CAMERA_ADJUST_NONE */
#define CAMERA_MOTION_START_BLENDS 0x04
/* end by giving the new top CAMERA_ADJUST_RELATIVE1. without END_BLENDS, it
 * takes CAMERA_ADJUST_NONE. camera_state's pos and target then go back to the
 * place the motion started from */
#define CAMERA_MOTION_END_BLENDS 0x08
#define CAMERA_MOTION_FOLLOWS 0x10 /* copy *origin again each step */
#define CAMERA_MOTION_CALLS_DONE 0x20 /* call done, if not null, at the end */
/* turn the target with the camera, about origin. without TURNS, the camera
 * turns alone, about the target */
#define CAMERA_MOTION_TURNS 0x40

/* a camera motion to play */
struct camera_motion {
    Float speed; /* the frames a step advances */
    /* a turn about y, which the motion plays under where it has an origin */
    Angle ang;
    NJS_MOTION *motion;
    /* the point the motion plays about, or null. with one, a frame's
     * position and vector are offsets from it to the camera and to its
     * target. with none, they become camera_state's pos and dir, and its
     * target stays as it was. under FOLLOWS or TURNS each step reads
     * *origin, which must stay valid while the motion plays */
    const NJS_POINT3 *origin;
    Sint32 flags; /* CAMERA_MOTION_ flags */
    void (*done)(void);
};

/* put the MOTION mode at CAMERA_MOTION_SLOT of camera's stack, and return 1.
 * the mode plays a copy of motion from its first frame. the place it starts
 * from is the current camera's, whichever camera is given.
 * game_state.camera_motion_playing is 1 until camera_motion_end. The port
 * returns 0 without changing the camera if the clip has no frames. */
int camera_motion_start(int camera, struct camera_motion *motion);
/* end camera's motion, as its last frame does. where CAMERA_MOTION_SLOT is
 * still camera's top, call done under CALLS_DONE and take the mode off. the new
 * top then takes the adjust END_BLENDS picks. either way, delay
 * camera_settle_step for the current camera. camera must have had a motion
 * started */
void camera_motion_end(int camera);
#ifdef NONMATCHING
/* this side's call for the mini event's skip. move camera's motion ahead to
 * the step before its last, and return the steps it passed. the motion's
 * next step is then its last, and ends it as that step does. it returns 0
 * and moves nothing where the motion has no step to pass: where its next
 * step is its last already, or under HOLD or LOOP, or where camera's top
 * isn't the motion */
int camera_motion_skip(int camera);
#endif

#endif
