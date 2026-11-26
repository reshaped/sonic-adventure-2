/* the tuning of the lamps, floor glow and conduit streams that Eggman's and
 * Tails's routes share. on each of the CORE_STRIPS strips, a pulse runs from
 * each end across LAMPS_PER_STRIP lamps. each lamp lights core_strip_delay
 * steps after the one before it. it brightens over core_strip_rise steps, and
 * fades over core_strip_fall */
#ifndef SHARED_CORE_MACHINE_LIGHTS_H
#define SHARED_CORE_MACHINE_LIGHTS_H
#include <ninja.h>
#include <sg_xpt.h>

#define CORE_STRIPS 4
#define LAMPS_PER_STRIP 6

extern Float core_floor_probe_reach, core_floor_falloff;
extern int core_glow_texture;
extern Uint32 core_glow_center_color, core_glow_edge_color;
extern Float core_glow_uv_scale;
extern NJS_CNK_OBJECT **core_strip_lamps[CORE_STRIPS];
extern int core_strip_rise, core_strip_fall, core_strip_delay;
extern Float core_strip_red, core_strip_green, core_strip_blue;
extern Float core_strip_reverse_red, core_strip_reverse_green,
    core_strip_reverse_blue;
extern int core_stream_trails, core_stream_trail_step, core_stream_color_speed;
extern Float core_stream_glow_gain, core_segment_base_light;

#endif
