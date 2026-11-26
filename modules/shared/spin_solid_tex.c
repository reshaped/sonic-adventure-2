#include <ninja.h>
#include <sg_xpt.h>

#include "shared/spin_solid_tex/text.inc"

#include "shared/spin_crates.h"

static NJS_TEXNAME textures[2] = {
#include "shared/spin_solid_tex/textures.inc"
};
NJS_TEXLIST spin_solid_texlist = {
#include "shared/spin_solid_tex/spin_solid_texlist.inc"
};

static Sint16 spin_solid_polygons[97] = {
#include "shared/spin_solid_tex/spin_solid_polygons.inc"
};

#include "shared/orbital/crate_vertices.h"

NJS_CNK_MODEL spin_solid_model = {
#include "shared/spin_solid_tex/spin_solid_model.inc"
};

NJS_CNK_OBJECT spin_solid_object = {
#include "shared/spin_solid_tex/spin_solid_object.inc"
};
