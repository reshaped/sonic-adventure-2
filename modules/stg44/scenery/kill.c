#include <ninja.h>
#include <sg_xpt.h>

#include "stg44/scenery_shapes.h"

static NJS_MATERIAL mad_space_kill_object_materials[1] = {
#include "stg44/scenery/kill/mad_space_kill_object_materials.inc"
};

static Sint16 mad_space_kill_object_faces[27] = {
#include "stg44/scenery/kill/mad_space_kill_object_faces.inc"
};

static NJS_MESHSET mad_space_kill_object_meshsets[1] = {
#include "stg44/scenery/kill/mad_space_kill_object_meshsets.inc"
};

static NJS_POINT3 mad_space_kill_object_points[13] = {
#include "stg44/scenery/kill/mad_space_kill_object_points.inc"
};

static NJS_VECTOR mad_space_kill_object_normals[13] = {
#include "stg44/scenery/kill/mad_space_kill_object_normals.inc"
};

static NJS_MODEL mad_space_kill_object_model = {
#include "stg44/scenery/kill/mad_space_kill_object_model.inc"
};

NJS_OBJECT mad_space_kill_object = {
#include "stg44/scenery/kill/mad_space_kill_object.inc"
};
