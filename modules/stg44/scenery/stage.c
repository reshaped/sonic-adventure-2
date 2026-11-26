#include <ninja.h>
#include <sg_xpt.h>

#include "stg44/scenery_shapes.h"

#include "stg44/scenery/stage/text.inc"

static NJS_TEXNAME mad_space_platform_names[7] = {
#include "stg44/scenery/stage/mad_space_platform_names.inc"
};

NJS_TEXLIST mad_space_platform_texlist = {
#include "stg44/scenery/stage/mad_space_platform_texlist.inc"
};

static Sint16 mad_space_platform_body_polygons[669] = {
#include "stg44/scenery/stage/mad_space_platform_body_polygons.inc"
};

static Sint32 mad_space_platform_body_vertices[535] = {
#include "stg44/scenery/stage/mad_space_platform_body_vertices.inc"
};

static NJS_CNK_MODEL mad_space_platform_body_model = {
#include "stg44/scenery/stage/mad_space_platform_body_model.inc"
};

NJS_CNK_OBJECT mad_space_platform_body[1] = {
#include "stg44/scenery/stage/mad_space_platform_body.inc"
};

static NJS_MATERIAL mad_space_platform_collision_materials[1] = {
#include "stg44/scenery/stage/mad_space_platform_collision_materials.inc"
};

static Sint16 mad_space_platform_collision_faces[53] = {
#include "stg44/scenery/stage/mad_space_platform_collision_faces.inc"
};

static NJS_MESHSET mad_space_platform_collision_meshsets[1] = {
#include "stg44/scenery/stage/mad_space_platform_collision_meshsets.inc"
};

static NJS_POINT3 mad_space_platform_collision_points[24] = {
#include "stg44/scenery/stage/mad_space_platform_collision_points.inc"
};

static NJS_VECTOR mad_space_platform_collision_normals[24] = {
#include "stg44/scenery/stage/mad_space_platform_collision_normals.inc"
};

static NJS_MODEL mad_space_platform_collision_model = {
#include "stg44/scenery/stage/mad_space_platform_collision_model.inc"
};

NJS_OBJECT mad_space_platform_collision = {
#include "stg44/scenery/stage/mad_space_platform_collision.inc"
};

static Sint16 mad_space_platform_shadow_polygons[89] = {
#include "stg44/scenery/stage/mad_space_platform_shadow_polygons.inc"
};

struct mad_space_platform_shadow_vertex_list {
    Sint32 head[2];
    NJS_POINT3 vertices[16];
    Sint32 end;
};
static struct mad_space_platform_shadow_vertex_list
    mad_space_platform_shadow_vertices = {
#include "stg44/scenery/stage/mad_space_platform_shadow_vertices.inc"
};

static NJS_CNK_MODEL mad_space_platform_shadow_model = {
#include "stg44/scenery/stage/mad_space_platform_shadow_model.inc"
};

NJS_CNK_OBJECT mad_space_platform_shadow = {
#include "stg44/scenery/stage/mad_space_platform_shadow.inc"
};
