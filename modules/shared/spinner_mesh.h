#ifndef SHARED_SPINNER_MESH_H
#define SHARED_SPINNER_MESH_H
#include <ninja.h>
#include <sg_xpt.h>

static Sint16 spinner_object_child_sibling_polygons[25] = {
#include "shared/spinner_mesh/spinner_object_child_sibling_polygons.inc"
};

static Sint32 spinner_object_child_sibling_vertices[19] = {
#include "shared/spinner_mesh/spinner_object_child_sibling_vertices.inc"
};

static NJS_CNK_MODEL spinner_object_child_sibling_model = {
#include "shared/spinner_mesh/spinner_object_child_sibling_model.inc"
};

static NJS_CNK_OBJECT spinner_object_child_sibling = {
#include "shared/spinner_mesh/spinner_object_child_sibling.inc"
};

static Sint16 spinner_object_child_polygons[429] = {
#include "shared/spinner_mesh/spinner_object_child_polygons.inc"
};

#endif
