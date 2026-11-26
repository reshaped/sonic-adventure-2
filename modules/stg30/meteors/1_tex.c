/* The placed meteor in selector slot 1 shown in Final Rush. */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/meteor_art.h"
#include "stg30/meteor_art.h"

static NJS_TEXNAME textures[2] = {
#include "stg30/meteors/1_tex/textures.inc"
};

NJS_TEXLIST meteor_1_texlist = {
#include "stg30/meteors/1_tex/meteor_1_texlist.inc"
};

static Sint16 meteor_1_polygons[339] = {
#include "stg30/meteors/1_tex/meteor_1_polygons.inc"
};

static Sint32 meteor_1_vertices[177] = {
#include "stg30/meteors/1_tex/meteor_1_vertices.inc"
};

static NJS_CNK_MODEL meteor_1_mesh = {
#include "stg30/meteors/1_tex/meteor_1_mesh.inc"
};

NJS_CNK_OBJECT meteor_1_object = {
#include "stg30/meteors/1_tex/meteor_1_object.inc"
};
