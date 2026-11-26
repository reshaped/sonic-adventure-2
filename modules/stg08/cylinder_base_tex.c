#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/cylinder_tex.h"
#include "stg08/fixture_art.h"

static NJS_TEXNAME textures[1] = {
#include "stg08/cylinder_base_tex/textures.inc"
};
NJS_TEXLIST security_cylinder_base_texlist = {
#include "stg08/cylinder_base_tex/security_cylinder_base_texlist.inc"
};

static Sint16 security_cylinder_base_polygons[117] = {
#include "stg08/cylinder_base_tex/security_cylinder_base_polygons.inc"
};

static Sint32 security_cylinder_base_vertices[75] = {
#include "stg08/cylinder_base_tex/security_cylinder_base_vertices.inc"
};

NJS_CNK_MODEL security_cylinder_base_model = {
#include "stg08/cylinder_base_tex/security_cylinder_base_model.inc"
};

static NJS_CNK_OBJECT security_cylinder_base_object = {
#include "stg08/cylinder_base_tex/security_cylinder_base_object.inc"
};
