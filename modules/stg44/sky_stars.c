#include <ninja.h>
#include <sg_xpt.h>

#include "stg44/sky.h"

#include "shared/space_sky_textures.h"
#include "stg44/sky_textures.h"

#include "stg44/sky_stars/text.inc"

static NJS_TEXNAME textures[11] = {
#include "stg44/sky_stars/textures.inc"
};
NJS_TEXLIST background_1_texlist = {
#include "stg44/sky_stars/background_1_texlist.inc"
};

static Sint16 stars_polygons[3679] = {
#include "stg44/sky_stars/stars_polygons.inc"
};

static Sint32 stars_vertices[3471] = {
#include "stg44/sky_stars/stars_vertices.inc"
};

static NJS_CNK_MODEL sky_stars_mesh = {
#include "stg44/sky_stars/sky_stars_mesh.inc"
};

NJS_CNK_OBJECT background_1_object[1] = {
#include "stg44/sky_stars/background_1_object.inc"
};
