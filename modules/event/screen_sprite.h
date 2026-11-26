#ifndef EVENT_SCREEN_SPRITE_H
#define EVENT_SCREEN_SPRITE_H
#include <ninja.h>
#include <sg_xpt.h>

/* the event frames a screen sprite fades in or out over */
#define EVENT_SPRITE_FADE_FRAMES 30
/* the screen's center, which a sprite's offset counts from, in pixels */
#define EVENT_SCREEN_CENTER_X 320
#define EVENT_SCREEN_CENTER_Y 240
/* the far edge of a sprite's texture, in an NJS_TEXANIM's UVs */
#define EVENT_UV_LAST 255
/* the priority a screen sprite draws at */
#define EVENT_SPRITE_PRIORITY (-2.0F)

struct event_screen_point {
    Sint16 x, y;
};

/* a sprite of the scene file's texture, drawn over the scene from event
 * frame first_frame for lifetime frames. offset places its top left corner,
 * from the screen's center. a nonzero fade_in fades it in over its first
 * EVENT_SPRITE_FADE_FRAMES. a nonzero fade_out fades it out over its last.
 * color tints its first draw, and each step then sets it white, at the
 * alpha of its fades */
struct event_screen_sprite {
    Sint32 first_frame;
    Sint8 fade_in, fade_out;
    Sint16 texture;
    Sint32 lifetime;
    struct event_screen_point offset;
    NJS_POINT2 scale;
    NJS_ARGB color;
};

/* start a task that draws a copy of sprite, and ends once its lifetime has
 * passed */
void event_screen_sprite_start(struct event_screen_sprite *sprite);

#endif
