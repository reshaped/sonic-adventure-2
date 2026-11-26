/* the lower dust ring, then the upper, which stands 650 above it */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/canyon_wind_art.h"

#include "shared/canyon_wind_art/text.inc"

static NJS_TEXNAME canyon_wind_lower_names[1] = {
#include "shared/canyon_wind_art/canyon_wind_lower_names.inc"
};

static NJS_TEXLIST canyon_wind_lower_texlist = {
#include "shared/canyon_wind_art/canyon_wind_lower_texlist.inc"
};

static Sint16 canyon_wind_lower_polygons[91] = {
#include "shared/canyon_wind_art/canyon_wind_lower_polygons.inc"
};

static Sint32 canyon_wind_lower_vertices[99] = {
#include "shared/canyon_wind_art/canyon_wind_lower_vertices.inc"
};

static NJS_CNK_MODEL canyon_wind_lower_model = {
#include "shared/canyon_wind_art/canyon_wind_lower_model.inc"
};

NJS_CNK_OBJECT canyon_wind_lower_object = {
#include "shared/canyon_wind_art/canyon_wind_lower_object.inc"
};

static NJS_MKEY_F canyon_wind_lower_position_keys[15] = {
#include "shared/canyon_wind_art/canyon_wind_lower_position_keys.inc"
};

static NJS_MDATA2 canyon_wind_lower_position_track[1] = {
#include "shared/canyon_wind_art/canyon_wind_lower_position_track.inc"
};

NJS_MOTION canyon_wind_lower_position_motion = {
#include "shared/canyon_wind_art/canyon_wind_lower_position_motion.inc"
};

static NJS_POINT3 canyon_wind_lower_shape_0[24] = {
#include "shared/canyon_wind_art/canyon_wind_lower_shape_0.inc"
};

static NJS_POINT3 canyon_wind_lower_shape_4[24] = {
#include "shared/canyon_wind_art/canyon_wind_lower_shape_4.inc"
};

static NJS_POINT3 canyon_wind_lower_shape_9[24] = {
#include "shared/canyon_wind_art/canyon_wind_lower_shape_9.inc"
};

static NJS_POINT3 canyon_wind_lower_shape_12[24] = {
#include "shared/canyon_wind_art/canyon_wind_lower_shape_12.inc"
};

static NJS_POINT3 canyon_wind_lower_shape_14[24] = {
#include "shared/canyon_wind_art/canyon_wind_lower_shape_14.inc"
};

static NJS_MKEY_P canyon_wind_lower_shape_keys[5] = {
#include "shared/canyon_wind_art/canyon_wind_lower_shape_keys.inc"
};

static NJS_MDATA2 canyon_wind_lower_shape_track[1] = {
#include "shared/canyon_wind_art/canyon_wind_lower_shape_track.inc"
};

NJS_MOTION canyon_wind_lower_shape_motion = {
#include "shared/canyon_wind_art/canyon_wind_lower_shape_motion.inc"
};

static NJS_TEXNAME canyon_wind_upper_names[1] = {
#include "shared/canyon_wind_art/canyon_wind_upper_names.inc"
};

static NJS_TEXLIST canyon_wind_upper_texlist = {
#include "shared/canyon_wind_art/canyon_wind_upper_texlist.inc"
};

static Sint16 canyon_wind_upper_polygons[91] = {
#include "shared/canyon_wind_art/canyon_wind_upper_polygons.inc"
};

static Sint32 canyon_wind_upper_vertices[99] = {
#include "shared/canyon_wind_art/canyon_wind_upper_vertices.inc"
};

static NJS_CNK_MODEL canyon_wind_upper_model = {
#include "shared/canyon_wind_art/canyon_wind_upper_model.inc"
};

NJS_CNK_OBJECT canyon_wind_upper_object = {
#include "shared/canyon_wind_art/canyon_wind_upper_object.inc"
};

static NJS_MKEY_F canyon_wind_upper_position_keys[15] = {
#include "shared/canyon_wind_art/canyon_wind_upper_position_keys.inc"
};

static NJS_MDATA2 canyon_wind_upper_position_track[1] = {
#include "shared/canyon_wind_art/canyon_wind_upper_position_track.inc"
};

NJS_MOTION canyon_wind_upper_position_motion = {
#include "shared/canyon_wind_art/canyon_wind_upper_position_motion.inc"
};

static NJS_POINT3 canyon_wind_upper_shape_0[24] = {
#include "shared/canyon_wind_art/canyon_wind_upper_shape_0.inc"
};

static NJS_POINT3 canyon_wind_upper_shape_9[24] = {
#include "shared/canyon_wind_art/canyon_wind_upper_shape_9.inc"
};

static NJS_POINT3 canyon_wind_upper_shape_14[24] = {
#include "shared/canyon_wind_art/canyon_wind_upper_shape_14.inc"
};

static NJS_MKEY_P canyon_wind_upper_shape_keys[3] = {
#include "shared/canyon_wind_art/canyon_wind_upper_shape_keys.inc"
};

static NJS_MDATA2 canyon_wind_upper_shape_track[1] = {
#include "shared/canyon_wind_art/canyon_wind_upper_shape_track.inc"
};

NJS_MOTION canyon_wind_upper_shape_motion = {
#include "shared/canyon_wind_art/canyon_wind_upper_shape_motion.inc"
};

NJS_TEXLIST *canyon_wind_texlists[3] = {
#include "shared/canyon_wind_art/canyon_wind_texlists.inc"
};
