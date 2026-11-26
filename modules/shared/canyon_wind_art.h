/* the two dust rings of Wild Canyon's wind. each is a model with 15-frame
 * motions of its position and its vertices. no code draws them, but the
 * stages load their textures through canyon_wind_texlists */
#ifndef SHARED_CANYON_WIND_ART_H
#define SHARED_CANYON_WIND_ART_H

#include <ninja.h>

extern NJS_TEXLIST *canyon_wind_texlists[];
extern NJS_CNK_OBJECT canyon_wind_lower_object;
extern NJS_MOTION canyon_wind_lower_position_motion,
    canyon_wind_lower_shape_motion;
extern NJS_CNK_OBJECT canyon_wind_upper_object;
extern NJS_MOTION canyon_wind_upper_position_motion,
    canyon_wind_upper_shape_motion;

#endif
