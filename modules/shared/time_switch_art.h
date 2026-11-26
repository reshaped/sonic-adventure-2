/* the time switch's models and its effect's motion. the draw colors the cap's
 * vertices by the timer's glow. it turns the fast and slow rings at rates of
 * their own */
#ifndef SHARED_TIME_SWITCH_ART_H
#define SHARED_TIME_SWITCH_ART_H

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk.h"

#define TIME_SWITCH_CAP_VERTICES 58
struct time_switch_vertices {
    Sint32 head[2];
    struct chunk_vertex_d8 vertices[TIME_SWITCH_CAP_VERTICES];
    Sint32 end;
};
extern struct time_switch_vertices time_switch_cap_vertices;
extern NJS_CNK_MODEL time_switch_base;
extern NJS_CNK_OBJECT time_switch_cap;
extern NJS_CNK_OBJECT time_switch_fast_ring;
extern NJS_CNK_OBJECT time_switch_ring;
extern NJS_CNK_OBJECT time_switch_slow_ring;
extern NJS_CNK_OBJECT time_switch_effect;
extern NJS_MOTION time_switch_motion[1];

#endif
