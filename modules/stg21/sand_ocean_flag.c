/* STG21.PRS's texture lists, chunk models, textured chunk poses and texture
 * names, from the disc. its C lies between stacked_platform.c and other
 * data, and its D lies between stacked_platform.c and flag.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/ocean_flag_frames.h"
#include "stg21/flag.h"

#include "stg21/sand_ocean_flag/text.inc"

struct flag_frames_pose {
    NJS_TEXNAME texture;
    NJS_TEXLIST texlist;
    Sint16 polygons[53];
    Sint32 vertices[55];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

static NJS_TEXNAME flag_support_textures[1] = {
#include "stg21/sand_ocean_flag/flag_support_textures.inc"
};
NJS_TEXLIST flag_support_texlist = {
#include "stg21/sand_ocean_flag/flag_support_texlist.inc"
};
static Sint16 flag_support_model_polygons[63] = {
#include "stg21/sand_ocean_flag/flag_support_model_polygons.inc"
};
static Sint32 flag_support_model_vertices[35] = {
#include "stg21/sand_ocean_flag/flag_support_model_vertices.inc"
};
NJS_CNK_MODEL flag_support_model = {
#include "stg21/sand_ocean_flag/flag_support_model.inc"
};
NJS_CNK_OBJECT flag_support_root = {
#include "stg21/sand_ocean_flag/flag_support_root.inc"
};
struct ocean_flag_frame flag_frames[FLAG_FRAMES] = {
#include "stg21/sand_ocean_flag/flag_frames.inc"
};
