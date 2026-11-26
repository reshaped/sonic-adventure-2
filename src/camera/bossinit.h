/* the camera's BOSSINIT mode: a boss's opening shot, which closes in on a
 * point and then turns about it */
#ifndef CAMERA_BOSSINIT_H
#define CAMERA_BOSSINIT_H
#include <ninja.h>
#include <sg_xpt.h>

/* what a BOSSINIT mode works from */
struct camera_bossinit {
    Float speed; /* the frames a step advances */
    /* the camera stands off target along the x axis turned about y by ang */
    Angle ang;
    NJS_POINT3 *target; /* the point the mode looks at, read each step */
    Float start_dist; /* how far from target the camera starts */
    Float dist; /* how far from target the camera closes in to */
    Angle turn; /* the turn about y the camera makes after it closes in */
};

/* the frames at which the mode ends its close, its turn and its stay */
#define CAMERA_BOSSINIT_CLOSE_END 60.0F
#define CAMERA_BOSSINIT_TURN_END 240.0F
#define CAMERA_BOSSINIT_STAY_END 360.0F

/* push the BOSSINIT mode onto camera's stack with a copy of param, and
 * return 1. the mode takes CAMERA_ADJUST_NONE. an INIT mode on top comes off
 * first. on a full stack it returns 0 and pushes nothing.
 *
 * the mode looks at target from target's height. until frame
 * CAMERA_BOSSINIT_CLOSE_END, the camera closes in from start_dist to dist at an
 * even rate. from there until frame CAMERA_BOSSINIT_TURN_END, it turns about
 * target by turn at an even rate. it then stays until CAMERA_BOSSINIT_STAY_END.
 * the first step past that frame calls camera_bossinit_end, and leaves
 * camera_state's pos and target as it found them */
int camera_bossinit_start(int camera, struct camera_bossinit *param);
/* take the mode on top off camera's stack, and give the mode then on top
 * CAMERA_ADJUST_NONE */
void camera_bossinit_end(int camera);

#endif
