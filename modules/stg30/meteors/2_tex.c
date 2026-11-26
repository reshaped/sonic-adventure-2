/* The placed meteor in selector slot 2 shown in Final Rush. */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/meteor_art.h"
#include "stg30/meteor_art.h"

static NJS_TEXNAME textures[2] = {
#include "stg30/meteors/2_tex/textures.inc"
};

NJS_TEXLIST meteor_2_texlist = {
#include "stg30/meteors/2_tex/meteor_2_texlist.inc"
};

static Sint16 meteor_2_polygons[249] = {
#include "stg30/meteors/2_tex/meteor_2_polygons.inc"
};

static Sint32 meteor_2_vertices[135] = {
#include "stg30/meteors/2_tex/meteor_2_vertices.inc"
};

static NJS_CNK_MODEL meteor_2_mesh = {
#include "stg30/meteors/2_tex/meteor_2_mesh.inc"
};

NJS_CNK_OBJECT meteor_2_object = {
#include "stg30/meteors/2_tex/meteor_2_object.inc"
};
