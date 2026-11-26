#ifndef SHARED_SHAFT_MESH_H
#define SHARED_SHAFT_MESH_H
#include <ninja.h>
#include <sg_xpt.h>

static Sint16 shaft_object_child_sibling_sibling_polygons[177] = {
#include "shared/shaft_mesh/shaft_object_child_sibling_sibling_polygons.inc"
};

static Sint32 shaft_object_child_sibling_sibling_vertices[107] = {
#include "shared/shaft_mesh/shaft_object_child_sibling_sibling_vertices.inc"
};

static NJS_CNK_MODEL shaft_object_child_sibling_sibling_model = {
#include "shared/shaft_mesh/shaft_object_child_sibling_sibling_model.inc"
};

static NJS_CNK_OBJECT shaft_object_child_sibling_sibling = {
#include "shared/shaft_mesh/shaft_object_child_sibling_sibling.inc"
};

static Sint16 shaft_object_child_sibling_polygons[121] = {
#include "shared/shaft_mesh/shaft_object_child_sibling_polygons.inc"
};

static Sint32 shaft_object_child_sibling_vertices[79] = {
#include "shared/shaft_mesh/shaft_object_child_sibling_vertices.inc"
};

static NJS_CNK_MODEL shaft_object_child_sibling_model = {
#include "shared/shaft_mesh/shaft_object_child_sibling_model.inc"
};

static NJS_CNK_OBJECT shaft_object_child_sibling = {
#include "shared/shaft_mesh/shaft_object_child_sibling.inc"
};

static Sint16 shaft_object_child_polygons[55] = {
#include "shared/shaft_mesh/shaft_object_child_polygons.inc"
};

static Sint32 shaft_object_child_vertices[51] = {
#include "shared/shaft_mesh/shaft_object_child_vertices.inc"
};

static NJS_CNK_MODEL shaft_object_child_model = {
#include "shared/shaft_mesh/shaft_object_child_model.inc"
};

static NJS_CNK_OBJECT shaft_object_child = {
#include "shared/shaft_mesh/shaft_object_child.inc"
};

#endif
