/* CHIN, the raised stone column in Dry Lagoon. */
#include "stg18/chin.h"

#include <ninja.h>
#include <sg_xpt.h>

#include "stg18/chin/text.inc"

static NJS_TEXNAME chin_texlist_names[2] = {
#include "stg18/chin/chin_texlist_names.inc"
};

static NJS_TEXLIST chin_texlist = {
#include "stg18/chin/chin_texlist.inc"
};

static Sint16 chin_polygons[177] = {
#include "stg18/chin/chin_polygons.inc"
};

static Sint32 chin_vertices[163] = {
#include "stg18/chin/chin_vertices.inc"
};

static NJS_CNK_MODEL chin_model = {
#include "stg18/chin/chin_model.inc"
};

static NJS_CNK_OBJECT chin_object = {
#include "stg18/chin/chin_object.inc"
};

static NJS_MATERIAL chin_collision_materials = {
#include "stg18/chin/chin_collision_materials.inc"
};

static Uint16 chin_collision_strips[53] = {
#include "stg18/chin/chin_collision_strips.inc"
};

static NJS_MESHSET chin_collision_faces = {
#include "stg18/chin/chin_collision_faces.inc"
};

static NJS_POINT3 chin_collision_points[12] = {
#include "stg18/chin/chin_collision_points.inc"
};

static NJS_VECTOR chin_collision_normals[12] = {
#include "stg18/chin/chin_collision_normals.inc"
};

static NJS_MODEL chin_collision_model = {
#include "stg18/chin/chin_collision_model.inc"
};

static NJS_OBJECT chin_collision = {
#include "stg18/chin/chin_collision.inc"
};

#include "stg18/chin_impl.h" /* IWYU pragma: keep -- the code it compiles here */
