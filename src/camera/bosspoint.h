/* the camera's BOSSPOINT mode: the camera stands behind a player, on the far
 * side from a point the caller keeps, such as a boss */
#ifndef CAMERA_BOSSPOINT_H
#define CAMERA_BOSSPOINT_H
#include <ninja.h>
#include <sg_xpt.h>

/* what a BOSSPOINT mode works from */
struct camera_bosspoint {
    /* the point the camera keeps beyond the player, read each step */
    NJS_POINT3 *target;
    Float dist; /* how far from the player the camera stands */
    Float height; /* how much higher it stands while level is 0 */
    Sint32 adjust; /* the CAMERA_ADJUST_ number the mode takes */
    /* what each step gives camera_state.unk_04: whether the camera task
     * runs camera_collide */
    Sint8 collides;
    Sint8 trigger_push; /* what each step gives camera_state.trigger_push */
    /* the camera looks at the point part * blend / CAMERA_BOSSPOINT_BLEND of
     * the way from the player to target */
    Float part;
    /* that point rises by lift * (1 - blend / CAMERA_BOSSPOINT_BLEND) */
    Float lift;
    /* while level is 0, each step moves blend 1 toward
     * CAMERA_BOSSPOINT_BLEND. otherwise each moves it 1 toward 0 */
    Sint32 level;
    /* what the mode moves height, dist and part toward, as x, y and z. each
     * step moves them a tenth of the way */
    NJS_VECTOR goal;
    Sint32 blend; /* 0 to CAMERA_BOSSPOINT_BLEND */
};
#define CAMERA_BOSSPOINT_BLEND 30

/* push the BOSSPOINT mode onto camera's stack with a copy of param, and
 * return 1. the mode takes param's adjust, and follows the current camera's
 * player. an INIT mode on top comes off first. on a full stack it returns 0
 * and pushes nothing. the copy's goal starts at its height, dist and part,
 * and its blend at CAMERA_BOSSPOINT_BLEND */
int camera_bosspoint_start(int camera, struct camera_bosspoint *param);

/* the calls below change the copy camera_bosspoint_start last made for a
 * camera. that camera's mode must still hold the slot it was pushed into.
 * once the mode has come off, the copy lies in the work of the slot's next
 * mode */

/* set the goal of camera's copy to height, dist and part */
void camera_bosspoint_ease_to(int camera, Float height, Float dist, Float part);
/* set the level of camera's copy */
void camera_bosspoint_set_level(int camera, int level);
/* have camera 0's copy follow player */
void camera_bosspoint_follow(int player);

#endif
