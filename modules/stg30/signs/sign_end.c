/* Housing and illuminated face of the first end sign. */

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/sign_end_art.h"
#include "stg30/sign_textures.h"

const char sign_end_miu32_fr005[12] = {
#include "stg30/signs/sign_end/sign_end_miu32_fr005.inc"
};

const char sign_end_miu64_fr001[12] = {
#include "stg30/signs/sign_end/sign_end_miu64_fr001.inc"
};

const char sign_end_miu64_fr004[12] = {
#include "stg30/signs/sign_end/sign_end_miu64_fr004.inc"
};

const char sign_end_miu64_fr007[12] = {
#include "stg30/signs/sign_end/sign_end_miu64_fr007.inc"
};

static NJS_TEXNAME textures[4] = {
#include "stg30/signs/sign_end/textures.inc"
};

NJS_TEXLIST sign_end_texlist = {
#include "stg30/signs/sign_end/sign_end_texlist.inc"
};

static Sint16 sign_end_face_polygons[37] = {
#include "stg30/signs/sign_end/sign_end_face_polygons.inc"
};

static Sint32 sign_end_face_vertices[35] = {
#include "stg30/signs/sign_end/sign_end_face_vertices.inc"
};

static NJS_CNK_MODEL sign_end_face_mesh = {
#include "stg30/signs/sign_end/sign_end_face_mesh.inc"
};

static NJS_CNK_OBJECT sign_end_face = {
#include "stg30/signs/sign_end/sign_end_face.inc"
};

static Sint16 sign_end_polygons[425] = {
#include "stg30/signs/sign_end/sign_end_polygons.inc"
};

static Sint32 sign_end_vertices[339] = {
#include "stg30/signs/sign_end/sign_end_vertices.inc"
};

static NJS_CNK_MODEL sign_end_mesh = {
#include "stg30/signs/sign_end/sign_end_mesh.inc"
};

NJS_CNK_OBJECT sign_end_object = {
#include "stg30/signs/sign_end/sign_end_object.inc"
};
