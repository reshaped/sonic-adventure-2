/* the pool cover: its closed, opening and open models, their texture lists and
 * its collision objects. while the cover is shut, ripples a and b play on its
 * water in turn, COVER_RIPPLE_FRAMES steps each */
#ifndef STG37_WATER_COVER_H
#define STG37_WATER_COVER_H
#include <ninja.h>

enum { COVER_RIPPLE_FRAMES = 60 };

extern NJS_OBJECT cover_closed_rim_collision;
extern NJS_OBJECT cover_open_rim_collision;
extern NJS_OBJECT cover_lid_collision;
extern NJS_TEXLIST cover_closed_textures;
extern NJS_CNK_OBJECT cover_closed_object;
extern NJS_TEXLIST cover_open_textures;
extern NJS_CNK_OBJECT cover_open_object;
extern NJS_TEXLIST cover_motion_textures;
extern NJS_CNK_OBJECT cover_motion_object[1];
extern NJS_MOTION cover_motion[1];
extern NJS_TEXLIST cover_ripple_textures;
extern NJS_TEXLIST cover_ripple_textures_b;
extern NJS_CNK_MODEL *cover_ripple_frames_a[COVER_RIPPLE_FRAMES];
extern NJS_CNK_MODEL *cover_ripple_frames_b[COVER_RIPPLE_FRAMES];
extern NJS_CNK_OBJECT cover_ripple_positions_a;
extern NJS_CNK_OBJECT cover_ripple_positions_b;

#endif
