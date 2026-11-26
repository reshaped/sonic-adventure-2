#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/flag_stand.h"
#include "stg21/stand_textures/text.inc"

static NJS_TEXNAME textures[3] = {
#include "stg21/stand_textures/textures.inc"
};
NJS_TEXLIST flag_stand_texlist = {
#include "stg21/stand_textures/flag_stand_texlist.inc"
};

static Sint16 flag_stand_model_polygons[215] = {
#include "stg21/stand_textures/flag_stand_model_polygons.inc"
};

static Sint32 flag_stand_model_vertices[123] = {
#include "stg21/stand_textures/flag_stand_model_vertices.inc"
};

NJS_CNK_MODEL flag_stand_model = {
#include "stg21/stand_textures/flag_stand_model.inc"
};

NJS_CNK_OBJECT flag_stand_root = {
#include "stg21/stand_textures/flag_stand_root.inc"
};
