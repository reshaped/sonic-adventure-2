#include <ninja.h>
#include <sg_xpt.h>

#include "shared/spin_iron_tex/text.inc"

#include "shared/spin_crates.h"

static NJS_TEXNAME textures[2] = {
#include "shared/spin_iron_tex/textures.inc"
};
NJS_TEXLIST spin_iron_texlist = {
#include "shared/spin_iron_tex/spin_iron_texlist.inc"
};

static Sint16 spin_iron_polygons[103] = {
#include "shared/spin_iron_tex/spin_iron_polygons.inc"
};

#include "shared/orbital/crate_vertices.h"

NJS_CNK_MODEL spin_iron_model = {
#include "shared/spin_iron_tex/spin_iron_model.inc"
};

NJS_CNK_OBJECT spin_iron_object = {
#include "shared/spin_iron_tex/spin_iron_object.inc"
};
