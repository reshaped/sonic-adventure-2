#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/fixture_art.h"
#include "stg08/platform_tex/text.inc"
#include "stg08/solid_props.h"

static NJS_TEXNAME textures[6] = {
#include "stg08/platform_tex/textures.inc"
};
NJS_TEXLIST security_platform_texlist = {
#include "stg08/platform_tex/security_platform_texlist.inc"
};

static Sint16 security_platform_polygons[303] = {
#include "stg08/platform_tex/security_platform_polygons.inc"
};

static Sint32 security_platform_vertices[195] = {
#include "stg08/platform_tex/security_platform_vertices.inc"
};

NJS_CNK_MODEL security_platform_model = {
#include "stg08/platform_tex/security_platform_model.inc"
};

static NJS_CNK_OBJECT security_platform_object = {
#include "stg08/platform_tex/security_platform_object.inc"
};

static NJS_MATERIAL security_platform_collision_materials[1] = {
#include "stg08/platform_tex/security_platform_collision_materials.inc"
};

static Sint16 security_platform_collision_faces[21] = {
#include "stg08/platform_tex/security_platform_collision_faces.inc"
};

static NJS_MESHSET security_platform_collision_meshsets[1] = {
#include "stg08/platform_tex/security_platform_collision_meshsets.inc"
};

static NJS_POINT3 security_platform_collision_points[8] = {
#include "stg08/platform_tex/security_platform_collision_points.inc"
};

static NJS_VECTOR security_platform_collision_normals[8] = {
#include "stg08/platform_tex/security_platform_collision_normals.inc"
};

static NJS_MODEL security_platform_collision_model = {
#include "stg08/platform_tex/security_platform_collision_model.inc"
};

NJS_OBJECT security_platform_collision = {
#include "stg08/platform_tex/security_platform_collision.inc"
};
