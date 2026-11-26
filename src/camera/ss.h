/* the camera's SS mode: the camera looks at a point between two that the
 * caller keeps, and closes in on a place beyond it */
#ifndef CAMERA_SS_H
#define CAMERA_SS_H
#include <ninja.h>
#include <sg_xpt.h>

/* what an SS mode works from */
struct camera_ss {
    Sint32 adjust; /* the CAMERA_ADJUST_ number the mode takes */
    /* the mode looks part of the way from *from to *to. a part of 0 looks
     * at *from, and 1 at *to. it reads both points each step, so they must
     * outlast the mode */
    NJS_POINT3 *from;
    NJS_POINT3 *to;
    Float part;
};

/* push the SS mode onto camera's stack with param's adjust, and copy param into
 * the mode's work. it returns 1. on a full stack it returns 0 and pushes
 * nothing. camera_ss_set copies param over that work. the SS mode
 * camera_ss_start last pushed for camera must still hold its slot. once that
 * mode is off, camera_ss_set writes into the work of whatever mode holds the
 * slot.
 *
 * each step the camera looks at the mode's point. its goal lies back from the
 * point by the distance from *from to *to. the goal's direction is the current
 * camera's line from its target to its pos. that line is made a unit vector,
 * its y set to 0.5, and made a unit vector again. the camera's pos moves a
 * fifth of the way from its place's pos to the goal */
int camera_ss_start(int camera, struct camera_ss *param);
void camera_ss_set(int camera, struct camera_ss *param);

#endif
