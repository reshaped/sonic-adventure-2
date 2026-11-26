#ifndef SHARED_GOLEM_FOOTHOLDS_SENSOR_MESH_H
#define SHARED_GOLEM_FOOTHOLDS_SENSOR_MESH_H

#include <ninja.h>

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL golem_foothold_sensor_object_materials[1] = {
#include "shared/golem_footholds/sensor_mesh/golem_foothold_sensor_object_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 golem_foothold_sensor_object_faces[115] = {
#include "shared/golem_footholds/sensor_mesh/golem_foothold_sensor_object_faces.inc"
};

static NJS_MESHSET golem_foothold_sensor_object_meshes[1] = {
#include "shared/golem_footholds/sensor_mesh/golem_foothold_sensor_object_meshes.inc"
};

static NJS_POINT3 golem_foothold_sensor_object_points[36] = {
#include "shared/golem_footholds/sensor_mesh/golem_foothold_sensor_object_points.inc"
};

static NJS_VECTOR golem_foothold_sensor_object_normals[36] = {
#include "shared/golem_footholds/sensor_mesh/golem_foothold_sensor_object_normals.inc"
};

static NJS_MODEL golem_foothold_sensor_object_model = {
#include "shared/golem_footholds/sensor_mesh/golem_foothold_sensor_object_model.inc"
};

#endif
