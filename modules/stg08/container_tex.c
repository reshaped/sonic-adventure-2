#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/container_tex/text.inc"
#include "stg08/fixture_art.h"
#include "stg08/solid_props.h"

static NJS_TEXNAME textures[9] = {
#include "stg08/container_tex/textures.inc"
};
NJS_TEXLIST security_container_texlist = {
#include "stg08/container_tex/security_container_texlist.inc"
};

static Sint16 security_container_polygons[549] = {
#include "stg08/container_tex/security_container_polygons.inc"
};

static Sint32 security_container_vertices[459] = {
#include "stg08/container_tex/security_container_vertices.inc"
};

NJS_CNK_MODEL security_container_model = {
#include "stg08/container_tex/security_container_model.inc"
};

static NJS_CNK_OBJECT security_container_object = {
#include "stg08/container_tex/security_container_object.inc"
};

static NJS_MATERIAL security_container_collision_materials[1] = {
#include "stg08/container_tex/security_container_collision_materials.inc"
};

static Sint16 security_container_collision_faces[28] = {
#include "stg08/container_tex/security_container_collision_faces.inc"
};

static NJS_MESHSET security_container_collision_meshsets[1] = {
#include "stg08/container_tex/security_container_collision_meshsets.inc"
};

static NJS_POINT3 security_container_collision_points[12] = {
#include "stg08/container_tex/security_container_collision_points.inc"
};

static NJS_VECTOR security_container_collision_normals[12] = {
#include "stg08/container_tex/security_container_collision_normals.inc"
};

static NJS_MODEL security_container_collision_model = {
#include "stg08/container_tex/security_container_collision_model.inc"
};

NJS_OBJECT security_container_collision = {
#include "stg08/container_tex/security_container_collision.inc"
};
