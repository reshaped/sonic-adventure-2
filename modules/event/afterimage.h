#ifndef EVENT_AFTERIMAGE_H
#define EVENT_AFTERIMAGE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "event/scene.h"

/* the pose an afterimage draws, taken from a scene entity: its kind and its
 * EVENT_ flags. EVENT_SHAPE and EVENT_MULTI_SHAPE draw at event_scene_frame
 * as it runs, and the other motion kinds hold at frame. the caller fills the
 * fields through lifetime. event_afterimage_start sets age, and each step
 * sets alpha */
struct event_afterimage {
    NJS_CNK_OBJECT *object;
    NJS_MOTION *motion;
    NJS_MOTION *shape;
    Float frame;
    enum event_draw_kind kind;
    Uint32 flags;
    Sint32 lifetime;
    Sint32 age;
    Float alpha;
};

/* start a task that draws a copy of pose, fading it out over lifetime
 * frames */
void event_afterimage_start(struct event_afterimage *pose);

#endif
