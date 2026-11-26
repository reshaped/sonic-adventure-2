/* STG23.PRS's texture lists, chunk poses, chunk models and texture names,
 * from the disc. its C lies between pot.c and other data, and its D lies
 * between pot.c and flag.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/ocean_flag_frames.h"
#include "stg21/flag.h"

#include "stg23/hidden_base_flag/text.inc"

struct hidden_base_flag_frames_pose {
    Sint16 polygons[53];
    Sint32 vertices[55];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

static NJS_TEXNAME cloth_material[1] = {
#include "stg23/hidden_base_flag/cloth_material.inc"
};
NJS_TEXLIST hidden_base_flag_cloth = {
#include "stg23/hidden_base_flag/hidden_base_flag_cloth.inc"
};
struct flag_cloth_mesh hidden_base_flag_frames[FLAG_FRAMES] = {
#include "stg23/hidden_base_flag/hidden_base_flag_frames.inc"
};
NJS_CNK_OBJECT hidden_base_flag_root = {
#include "stg23/hidden_base_flag/hidden_base_flag_root.inc"
};
static NJS_TEXNAME flag_support_textures[1] = {
#include "stg23/hidden_base_flag/flag_support_textures.inc"
};
NJS_TEXLIST flag_support_texlist = {
#include "stg23/hidden_base_flag/flag_support_texlist.inc"
};
static Sint16 flag_support_model_polygons[63] = {
#include "stg23/hidden_base_flag/flag_support_model_polygons.inc"
};
static Sint32 flag_support_model_vertices[35] = {
#include "stg23/hidden_base_flag/flag_support_model_vertices.inc"
};
NJS_CNK_MODEL flag_support_model = {
#include "stg23/hidden_base_flag/flag_support_model.inc"
};
NJS_CNK_OBJECT flag_support_root = {
#include "stg23/hidden_base_flag/flag_support_root.inc"
};
