/* the camera's BOSSKLAMATH mode: KLAMATH about a point between two that the
 * caller keeps */
#ifndef CAMERA_BOSSKLAMATH_H
#define CAMERA_BOSSKLAMATH_H
#include <ninja.h>
#include <sg_xpt.h>

/* what a BOSSKLAMATH mode works from */
struct camera_bossklamath {
    /* the ends of the line the mode's point lies on, read each step */
    NJS_POINT3 *from;
    NJS_POINT3 *to;
    Float dist; /* how far from the player the camera stands in x and z */
    Float height; /* how far above the player it stands */
    Sint32 adjust; /* the CAMERA_ADJUST_ number the mode takes */
    /* what each step gives camera_state.unk_04: whether the camera task
     * runs camera_collide */
    Sint8 collides;
    Sint8 trigger_push; /* what each step gives camera_state.trigger_push */
    /* the share of the way from from to to that the mode's point lies, in
     * x and z */
    Float part;
};

/* push the BOSSKLAMATH mode onto camera's stack with a copy of param, and
 * return 1. the mode takes param's adjust. an INIT mode on top comes off
 * first. on a full stack it returns 0 and pushes nothing.
 *
 * each step stands the camera dist from the current camera's player, away
 * from the mode's point. it stands height above the player. the
 * camera aims at the mode's point, taken at the player's height. its target
 * is the point above the player on that line */
int camera_bossklamath_start(int camera, struct camera_bossklamath *param);

#endif
