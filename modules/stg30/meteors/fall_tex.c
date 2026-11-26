/* The falling meteor shown in Final Rush. */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/meteor_art.h"

static const char meteor_miu64_fr006[12] = {
#include "stg30/meteors/fall_tex/meteor_miu64_fr006.inc"
};

static NJS_TEXNAME textures[1] = {
#include "stg30/meteors/fall_tex/textures.inc"
};
NJS_TEXLIST meteor_fall_texlist = {
#include "stg30/meteors/fall_tex/meteor_fall_texlist.inc"
};

static Sint16 meteor_fall_polygons[255] = {
#include "stg30/meteors/fall_tex/meteor_fall_polygons.inc"
};

static Sint32 meteor_fall_vertices[107] = {
#include "stg30/meteors/fall_tex/meteor_fall_vertices.inc"
};

static NJS_CNK_MODEL meteor_fall_mesh = {
#include "stg30/meteors/fall_tex/meteor_fall_mesh.inc"
};

NJS_CNK_OBJECT meteor_fall_object[1] = {
#include "stg30/meteors/fall_tex/meteor_fall_object.inc"
};
