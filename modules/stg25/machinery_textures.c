#include <ninja.h>
#include <sg_xpt.h>

#include "shared/machinery_assets.h"

/* a sized array pads its name, so the name after it starts on a 4-byte
 * boundary */
const char machinery_texture_128a[20] = {
#include "stg25/machinery_textures/machinery_texture_128a.inc"
};
const char machinery_texture_64a[20] = {
#include "stg25/machinery_textures/machinery_texture_64a.inc"
};
const char machinery_texture_64b[18] = {
#include "stg25/machinery_textures/machinery_texture_64b.inc"
};

static NJS_TEXNAME textures[3] = {
#include "stg25/machinery_textures/textures.inc"
};
NJS_TEXLIST machinery_curve_texlist = {
#include "stg25/machinery_textures/machinery_curve_texlist.inc"
};

static Sint16 machinery_curve_polygons[661] = {
#include "stg25/machinery_textures/machinery_curve_polygons.inc"
};

static Sint32 machinery_curve_vertices[563] = {
#include "stg25/machinery_textures/machinery_curve_vertices.inc"
};

NJS_CNK_MODEL machinery_curve_model = {
#include "stg25/machinery_textures/machinery_curve_model.inc"
};

static NJS_CNK_OBJECT machinery_curve_object = {
#include "stg25/machinery_textures/machinery_curve_object.inc"
};
