/* CHIN3, the carved wall and face in Dry Lagoon. */
#include "stg18/chin3.h"

#include <ninja.h>
#include <sg_xpt.h>

#include "stg18/chin3/text.inc"

static NJS_TEXNAME chin3_texlist_names[3] = {
#include "stg18/chin3/chin3_texlist_names.inc"
};

static NJS_TEXLIST chin3_texlist = {
#include "stg18/chin3/chin3_texlist.inc"
};

static Sint16 chin3_polygons[153] = {
#include "stg18/chin3/chin3_polygons.inc"
};

static Sint32 chin3_vertices[95] = {
#include "stg18/chin3/chin3_vertices.inc"
};

static NJS_CNK_MODEL chin3_model = {
#include "stg18/chin3/chin3_model.inc"
};

static NJS_CNK_OBJECT chin3_object = {
#include "stg18/chin3/chin3_object.inc"
};

static NJS_MATERIAL chin3_collision_materials = {
#include "stg18/chin3/chin3_collision_materials.inc"
};

static Uint16 chin3_collision_strips[43] = {
#include "stg18/chin3/chin3_collision_strips.inc"
};

static NJS_MESHSET chin3_collision_faces = {
#include "stg18/chin3/chin3_collision_faces.inc"
};

static NJS_POINT3 chin3_collision_points[10] = {
#include "stg18/chin3/chin3_collision_points.inc"
};

static NJS_VECTOR chin3_collision_normals[10] = {
#include "stg18/chin3/chin3_collision_normals.inc"
};

static NJS_MODEL chin3_collision_model = {
#include "stg18/chin3/chin3_collision_model.inc"
};

static NJS_OBJECT chin3_collision = {
#include "stg18/chin3/chin3_collision.inc"
};

/* NOLINTBEGIN(readability-identifier-naming) */
#define chin_build chin3_build
#define chin_init chin3_init
#define chin_texlists chin3_texlists
#define chin_texlist chin3_texlist
#define chin_model chin3_model
#define chin_object chin3_object
#define chin_collision chin3_collision
/* NOLINTEND(readability-identifier-naming) */
#define CHIN_REACH 39.0F

#include "stg18/chin_impl.h" /* IWYU pragma: keep -- the code it compiles here */
