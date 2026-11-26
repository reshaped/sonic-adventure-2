/* Scrolling panel marking a trick on the Final Rush rails. */

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "stg30/sign_textures.h"
#include "stg30/sign_trick.h"

const char trick_sign_miu32_fr006[12] = {
#include "stg30/signs/trick_sign/trick_sign_miu32_fr006.inc"
};

static NJS_TEXNAME textures[1] = {
#include "stg30/signs/trick_sign/textures.inc"
};

NJS_TEXLIST trick_sign_texlist = {
#include "stg30/signs/trick_sign/trick_sign_texlist.inc"
};

static Sint16 trick_sign_polygons[149] = {
#include "stg30/signs/trick_sign/trick_sign_polygons.inc"
};

static Sint32 trick_sign_vertices[131] = {
#include "stg30/signs/trick_sign/trick_sign_vertices.inc"
};

static NJS_CNK_MODEL trick_sign_mesh = {
#include "stg30/signs/trick_sign/trick_sign_mesh.inc"
};

NJS_CNK_OBJECT trick_sign_object = {
#include "stg30/signs/trick_sign/trick_sign_object.inc"
};

/* Polygon strip offsets, vertex counts and base texture coordinates. */
static Sint16 trick_sign_uvs[106] = {
#include "stg30/signs/trick_sign/trick_sign_uvs.inc"
};

struct texture_anim trick_sign_anim = {
#include "stg30/signs/trick_sign/trick_sign_anim.inc"
};
