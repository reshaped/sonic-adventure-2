/* The second end sign uses three materials from the first. */

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/sign_end_art.h"
#include "stg30/sign_textures.h"

const char sign_end_2_miu32_fr004[12] = {
#include "stg30/signs/sign_end_2/sign_end_2_miu32_fr004.inc"
};

static NJS_TEXNAME textures[4] = {
#include "stg30/signs/sign_end_2/textures.inc"
};

NJS_TEXLIST sign_end_2_texlist = {
#include "stg30/signs/sign_end_2/sign_end_2_texlist.inc"
};

static Sint16 sign_end_2_face_polygons[37] = {
#include "stg30/signs/sign_end_2/sign_end_2_face_polygons.inc"
};

static Sint32 sign_end_2_face_vertices[35] = {
#include "stg30/signs/sign_end_2/sign_end_2_face_vertices.inc"
};

static NJS_CNK_MODEL sign_end_2_face_mesh = {
#include "stg30/signs/sign_end_2/sign_end_2_face_mesh.inc"
};

static NJS_CNK_OBJECT sign_end_2_face = {
#include "stg30/signs/sign_end_2/sign_end_2_face.inc"
};

static Sint16 sign_end_2_polygons[425] = {
#include "stg30/signs/sign_end_2/sign_end_2_polygons.inc"
};

static Sint32 sign_end_2_vertices[339] = {
#include "stg30/signs/sign_end_2/sign_end_2_vertices.inc"
};

static NJS_CNK_MODEL sign_end_2_mesh = {
#include "stg30/signs/sign_end_2/sign_end_2_mesh.inc"
};

NJS_CNK_OBJECT sign_end_2_object = {
#include "stg30/signs/sign_end_2/sign_end_2_object.inc"
};
