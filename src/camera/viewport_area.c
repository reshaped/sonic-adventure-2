/* the area of the screen that draws go to: keeping one, and Ninja's screen
 * and clipping for it */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/viewport.h"
#include "draw/screen.h"

/* how far Ninja's screen lies from the eye */
#define SCREEN_DIST 500.0F
/* the side of a clipping tile, in pixels */
#define TILE 32.0F

static struct viewport_area whole_screen = {
    {{0.0F, 0.0F}, {SCREEN_WIDTH, SCREEN_HEIGHT}},
    {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
    {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
};

/* the area viewport_set_area keeps: its size, its centers, and its first
 * and last pixels counted in tiles, unrounded */
static struct {
    Float w, h;
    NJS_POINT2 center;
    NJS_POINT2 rest_center;
    NJS_POINT2 tiles[2];
} kept;

void viewport_set_whole(void)
{
    viewport_set_area(&whole_screen);
    viewport_set_screen(VIEWPORT_WHOLE);
}

void viewport_set_area(const struct viewport_area *area)
{
    kept.w = area->corners[1].x - area->corners[0].x;
    kept.h = area->corners[1].y - area->corners[0].y;
    kept.center.x = area->center.x;
    kept.center.y = area->center.y;
    kept.rest_center.x = area->rest_center.x;
    kept.rest_center.y = area->rest_center.y;
    /* @hack spelling: each product with 1 / TILE, where a division by TILE
     * would do */
    kept.tiles[0].x = area->corners[0].x * (1.0F / TILE);
    kept.tiles[0].y = area->corners[0].y * (1.0F / TILE);
    kept.tiles[1].x = (area->corners[1].x - 1.0F) * (1.0F / TILE);
    kept.tiles[1].y = (area->corners[1].y - 1.0F) * (1.0F / TILE);
}

void viewport_set_screen(int part)
{
    NJS_SCREEN screen;
    Sint32 mode;

#ifdef NONMATCHING
    /* GCC can't see that every path sets mode. part is one of the three
     * VIEWPORT_ parts, as the header requires */
    mode = NJD_CLIP_DISABLE;
#endif
    switch (part) {
    case VIEWPORT_WHOLE:
        screen.dist = SCREEN_DIST;
        screen.cx = SCREEN_WIDTH / 2;
        screen.cy = SCREEN_HEIGHT / 2;
        screen.w = SCREEN_WIDTH;
        screen.h = SCREEN_HEIGHT;
        mode = NJD_CLIP_DISABLE;
        break;
    case VIEWPORT_AREA:
        screen.dist = SCREEN_DIST;
        screen.cx = kept.center.x;
        screen.cy = kept.center.y;
        screen.w = kept.w;
        screen.h = kept.h;
        mode = NJD_CLIP_INSIDE;
        break;
    case VIEWPORT_REST:
        screen.dist = SCREEN_DIST;
        screen.cx = kept.rest_center.x;
        screen.cy = kept.rest_center.y;
        /* @bug logic: gives an area that touches the right edge but not the
         * left a rest as wide as the screen, not as wide as what lies left of
         * it. its test,
         * tiles[1].x equal to SCREEN_WIDTH / TILE (FCMP/EQ at 0x8c0b923a),
         * never holds: tiles[1].x is the last pixel's, below that */
        if (kept.w == SCREEN_WIDTH ||
            (kept.tiles[0].x != 0.0F &&
             kept.tiles[1].x != SCREEN_WIDTH / TILE)) {
            screen.w = SCREEN_WIDTH;
        } else if (kept.tiles[0].x == 0.0F) {
            screen.w = SCREEN_WIDTH - (kept.tiles[1].x * TILE);
        } else {
            screen.w = kept.tiles[0].x * TILE;
        }
        /* @bug logic: gives an area that touches the bottom edge but not the
         * top a rest as tall as the screen, not as tall as what lies above
         * it. its test,
         * tiles[1].y equal to SCREEN_HEIGHT / TILE (FCMP/EQ at 0x8c0b92c2),
         * never holds: tiles[1].y is the last pixel's, below that */
        if (kept.h == SCREEN_HEIGHT ||
            (kept.tiles[0].y != 0.0F &&
             kept.tiles[1].y != SCREEN_HEIGHT / TILE)) {
            screen.h = SCREEN_HEIGHT;
        } else if (kept.tiles[0].y == 0.0F) {
            screen.h = SCREEN_HEIGHT - (kept.tiles[1].y * TILE);
        } else {
            screen.h = kept.tiles[0].y * TILE;
        }
        mode = NJD_CLIP_OUTSIDE;
        break;
    }
    njSetScreen(&screen);
    camera_apply_light();
    njUserClipping(mode, kept.tiles);
}
