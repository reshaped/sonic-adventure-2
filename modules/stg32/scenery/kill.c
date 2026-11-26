#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/scenery_shapes.h"

static NJS_MATERIAL meteor_herd_kill_object_materials[1] = {
#include "stg32/scenery/kill/meteor_herd_kill_object_materials.inc"
};

static Sint16 meteor_herd_kill_object_faces[9] = {
#include "stg32/scenery/kill/meteor_herd_kill_object_faces.inc"
};

static NJS_MESHSET meteor_herd_kill_object_meshsets[1] = {
#include "stg32/scenery/kill/meteor_herd_kill_object_meshsets.inc"
};

static NJS_POINT3 meteor_herd_kill_object_points[8] = {
#include "stg32/scenery/kill/meteor_herd_kill_object_points.inc"
};

static NJS_VECTOR meteor_herd_kill_object_normals[8] = {
#include "stg32/scenery/kill/meteor_herd_kill_object_normals.inc"
};

static NJS_MODEL meteor_herd_kill_object_model = {
#include "stg32/scenery/kill/meteor_herd_kill_object_model.inc"
};

NJS_OBJECT meteor_herd_kill_object = {
#include "stg32/scenery/kill/meteor_herd_kill_object.inc"
};
