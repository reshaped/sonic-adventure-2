#include <ninja.h>
#include <sg_xpt.h>

#include "shared/spin_wood_tex/text.inc"

#include "shared/spin_crates.h"

static NJS_TEXNAME textures[2] = {
#include "shared/spin_wood_tex/textures.inc"
};
NJS_TEXLIST spin_wood_texlist = {
#include "shared/spin_wood_tex/spin_wood_texlist.inc"
};

static Sint16 spin_wood_polygons[103] = {
#include "shared/spin_wood_tex/spin_wood_polygons.inc"
};

#include "shared/orbital/crate_vertices.h"

NJS_CNK_MODEL spin_wood_model = {
#include "shared/spin_wood_tex/spin_wood_model.inc"
};

NJS_CNK_OBJECT spin_wood_object = {
#include "shared/spin_wood_tex/spin_wood_object.inc"
};
