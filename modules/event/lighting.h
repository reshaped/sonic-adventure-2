/* the event's lights, its screen fades and its fog, as its script sets
 * them */
#ifndef EVENT_LIGHTING_H
#define EVENT_LIGHTING_H
#include <ninja.h>
#include <sg_xpt.h>

struct event_lighting {
    NJS_VECTOR direction;
    Float color[3], ambient_intensity, ambient[3];
};

struct event_fade {
    NJS_COLOR current, target;
};
struct event_fog_cue {
    Sint32 words[5];
};

#endif
