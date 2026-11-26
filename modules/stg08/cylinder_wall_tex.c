#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/cylinder_tex.h"
#include "stg08/fixture_art.h"

static NJS_TEXNAME textures[2] = {
#include "stg08/cylinder_wall_tex/textures.inc"
};
NJS_TEXLIST security_cylinder_wall_texlist = {
#include "stg08/cylinder_wall_tex/security_cylinder_wall_texlist.inc"
};

static Sint16 security_cylinder_wall_polygons[95] = {
#include "stg08/cylinder_wall_tex/security_cylinder_wall_polygons.inc"
};

static Sint32 security_cylinder_wall_vertices[55] = {
#include "stg08/cylinder_wall_tex/security_cylinder_wall_vertices.inc"
};

NJS_CNK_MODEL security_cylinder_wall_model = {
#include "stg08/cylinder_wall_tex/security_cylinder_wall_model.inc"
};

static NJS_CNK_OBJECT security_cylinder_wall_object = {
#include "stg08/cylinder_wall_tex/security_cylinder_wall_object.inc"
};
