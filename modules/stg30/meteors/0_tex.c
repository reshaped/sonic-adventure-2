/* The placed meteor in selector slot 0 shown in Final Rush. */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/meteor_art.h"
#include "stg30/meteor_art.h"

const char meteor_miu128_fr002[16] = {
#include "stg30/meteors/0_tex/meteor_miu128_fr002.inc"
};

const char meteor_miu256_fr001[13] = {
#include "stg30/meteors/0_tex/meteor_miu256_fr001.inc"
};

static NJS_TEXNAME textures[2] = {
#include "stg30/meteors/0_tex/textures.inc"
};

NJS_TEXLIST meteor_0_texlist = {
#include "stg30/meteors/0_tex/meteor_0_texlist.inc"
};

static Sint16 meteor_0_polygons[349] = {
#include "stg30/meteors/0_tex/meteor_0_polygons.inc"
};

static Sint32 meteor_0_vertices[195] = {
#include "stg30/meteors/0_tex/meteor_0_vertices.inc"
};

static NJS_CNK_MODEL meteor_0_mesh = {
#include "stg30/meteors/0_tex/meteor_0_mesh.inc"
};

NJS_CNK_OBJECT meteor_0_object = {
#include "stg30/meteors/0_tex/meteor_0_object.inc"
};
