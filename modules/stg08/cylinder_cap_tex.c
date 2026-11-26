#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/cylinder_tex.h"
#include "stg08/fixture_art.h"

static NJS_TEXNAME textures[2] = {
#include "stg08/cylinder_cap_tex/textures.inc"
};
NJS_TEXLIST security_cylinder_cap_texlist = {
#include "stg08/cylinder_cap_tex/security_cylinder_cap_texlist.inc"
};

static Sint16 security_cylinder_cap_polygons[189] = {
#include "stg08/cylinder_cap_tex/security_cylinder_cap_polygons.inc"
};

static Sint32 security_cylinder_cap_vertices[203] = {
#include "stg08/cylinder_cap_tex/security_cylinder_cap_vertices.inc"
};

NJS_CNK_MODEL security_cylinder_cap_model = {
#include "stg08/cylinder_cap_tex/security_cylinder_cap_model.inc"
};

static NJS_CNK_OBJECT security_cylinder_cap_object = {
#include "stg08/cylinder_cap_tex/security_cylinder_cap_object.inc"
};
