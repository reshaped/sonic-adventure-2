/* The placed meteor in selector slot 3 shown in Final Rush. */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/meteor_art.h"
#include "stg30/meteor_art.h"

static NJS_TEXNAME textures[2] = {
#include "stg30/meteors/3_tex/textures.inc"
};

NJS_TEXLIST meteor_3_texlist = {
#include "stg30/meteors/3_tex/meteor_3_texlist.inc"
};

static Sint16 meteor_3_polygons[545] = {
#include "stg30/meteors/3_tex/meteor_3_polygons.inc"
};

static Sint32 meteor_3_vertices[315] = {
#include "stg30/meteors/3_tex/meteor_3_vertices.inc"
};

static NJS_CNK_MODEL meteor_3_mesh = {
#include "stg30/meteors/3_tex/meteor_3_mesh.inc"
};

NJS_CNK_OBJECT meteor_3_object = {
#include "stg30/meteors/3_tex/meteor_3_object.inc"
};
