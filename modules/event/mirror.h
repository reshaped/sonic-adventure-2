#ifndef EVENT_MIRROR_H
#define EVENT_MIRROR_H
#include <ninja.h>
#include <sg_xpt.h>

#include "event/scene.h"

#define MIRRORS 32
#define QUAD_CORNERS 4

/* the scene file's count mirrors, a quad each. each quad's plane reflects the
 * entities whose flags hold EVENT_REFLECTED. opacity is each mirror's, in
 * percent */
struct event_mirrors {
    Sint32 count;
    Sint32 opacity[MIRRORS];
    NJS_POINT3 (*quad)[QUAD_CORNERS];
};

/* keys holds count keys of stride bytes each, in rising order of frame. each
 * key opens with its frame, a Sint32. start and end bound an interval, in
 * either order. event_find_keys sets first and last */
struct event_key_span {
    char *keys;
    Sint32 count, stride;
    Float start, end;
    Sint32 first, last;
};

/* set pos to the translation of motion's root at frame, and return TRUE. a
 * frame past the last, or below 0, takes the last. a root with no position
 * keys sets pos to the origin and returns FALSE. a null motion returns FALSE,
 * and leaves pos as it was */
Bool event_motion_position(NJS_MOTION *motion, int frame, NJS_POINT3 *pos);
/* find the keys around the interval. span->first becomes the first key at or
 * after its low end. span->last becomes the first at or after its high end,
 * or the last key. it then returns TRUE.
 *
 * it returns FALSE where the low end lies outside the keys, or where both
 * would name one key. first and last are then -1. a span of one key is the
 * exception. it gets first 0 and last 1 and returns TRUE, whatever the
 * interval. where that key's frame is below 0, it gets the -1s and FALSE */
Bool event_find_keys(struct event_key_span *span);
/* draw object's modifier volumes in motion's pose at frame, or at its last
 * frame past the end */
void event_draw_modifier_motion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                Float frame);
/* find each mirror's center and normal from its quad */
void event_mirrors_init(void);
/* draw entity index of scene in each mirror, as kind says. kind is one that
 * draws, never EVENT_CULLED or EVENT_UNFOGGED. only an entity whose flags
 * hold EVENT_REFLECTED draws */
void event_mirrors_draw(enum event_draw_kind kind, int scene, int index);

/* draw the chunk models that follow translucent, at opacity in percent.
 * event_blend_stop puts back the draw attributes and controls */
void event_blend_start(int opacity);
void event_blend_stop(void);

#endif
