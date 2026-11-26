#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/cylinder_tex.h"
#include "stg08/fixture_art.h"

/* the names that the canister's texture lists point at, each starting on a
 * 4-byte boundary */
const char security_miu128_pl001[16] = {
#include "stg08/cylinder_tex/security_miu128_pl001.inc"
};
static const char miu32_ig002[12] = {
#include "stg08/cylinder_tex/miu32_ig002.inc"
};
const char security_miu64_ig011[12] = {
#include "stg08/cylinder_tex/security_miu64_ig011.inc"
};
static const char miu64_ig019[12] = {
#include "stg08/cylinder_tex/miu64_ig019.inc"
};
const char security_miu64_sh001[12] = {
#include "stg08/cylinder_tex/security_miu64_sh001.inc"
};

static NJS_TEXNAME textures[5] = {
#include "stg08/cylinder_tex/textures.inc"
};
NJS_TEXLIST security_cylinder_texlist = {
#include "stg08/cylinder_tex/security_cylinder_texlist.inc"
};

static Sint16 security_cylinder_object_polygons[553] = {
#include "stg08/cylinder_tex/security_cylinder_object_polygons.inc"
};

static Sint32 security_cylinder_object_vertices[431] = {
#include "stg08/cylinder_tex/security_cylinder_object_vertices.inc"
};

static NJS_CNK_MODEL security_cylinder_object_model = {
#include "stg08/cylinder_tex/security_cylinder_object_model.inc"
};

NJS_CNK_OBJECT security_cylinder_object[1] = {
#include "stg08/cylinder_tex/security_cylinder_object.inc"
};
