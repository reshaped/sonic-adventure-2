/* the area of the screen each camera's view takes, and Ninja's screen and
 * clipping for an area */
#ifndef CAMERA_VIEWPORT_H
#define CAMERA_VIEWPORT_H
#include <ninja.h>

/* an area of the screen, in pixels. corners[0] is its top left pixel, and
 * corners[1] lies one past its bottom right pixel. center and rest_center
 * are the centers VIEWPORT_AREA and VIEWPORT_REST give Ninja's screen */
struct viewport_area {
    NJS_POINT2 corners[2];
    NJS_POINT2 center;
    NJS_POINT2 rest_center;
};

/* the parts of the screen viewport_set_screen takes: the whole of it, the
 * area viewport_set_area kept, and what that area leaves */
#define VIEWPORT_WHOLE 0
#define VIEWPORT_AREA 1
#define VIEWPORT_REST 2

/* keep the whole screen for the area, and set Ninja's screen to it, with no
 * clipping */
void viewport_set_whole(void);
/* keep area for viewport_set_screen. Ninja's screen stays as it is */
void viewport_set_area(const struct viewport_area *area);
/* set Ninja's screen and its clipping to part, a VIEWPORT_ part, unchecked.
 * VIEWPORT_WHOLE gives the screen all of SCREEN_WIDTH by SCREEN_HEIGHT,
 * centered, with no clipping. VIEWPORT_AREA gives it the kept area's size
 * and center. draws then reach only the 32-pixel tiles that hold the area.
 * VIEWPORT_REST centers it at the area's rest_center, and draws reach only
 * the tiles outside those.
 *
 * VIEWPORT_REST's screen is SCREEN_WIDTH wide, but for a narrower area that
 * touches the left edge. that rest runs from the area's last column through the
 * screen's last, both included. its screen is SCREEN_HEIGHT tall, but for a
 * shorter area that touches the top edge. that rest runs from the area's last
 * row through the screen's last */
void viewport_set_screen(int part);

/* give camera its area of the screen: keep it, and set Ninja's screen and
 * clipping to it. a camera takes the whole screen, unclipped, where one
 * camera is in use. it does too where game_state.single_view isn't 0. with
 * four in use, each takes a quarter. with any other count, camera takes its
 * area of the pair game_state.split_layout picks. camera must then be 0 or
 * 1, unchecked */
void viewport_set(int camera);
/* make layout, from 0 to 7, the pair of areas viewport_set gives two
 * cameras. layout is unchecked */
void viewport_select_split(int layout);

#endif
