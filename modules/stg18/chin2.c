/* CHIN2, the carved wall in Dry Lagoon. */
#include "stg18/chin2.h"

#include <ninja.h>
#include <sg_xpt.h>

#include "stg18/chin2/text.inc"

static NJS_TEXNAME chin2_texlist_names[2] = {
#include "stg18/chin2/chin2_texlist_names.inc"
};

static NJS_TEXLIST chin2_texlist = {
#include "stg18/chin2/chin2_texlist.inc"
};

static Sint16 chin2_polygons[139] = {
#include "stg18/chin2/chin2_polygons.inc"
};

static Sint32 chin2_vertices[99] = {
#include "stg18/chin2/chin2_vertices.inc"
};

static NJS_CNK_MODEL chin2_model = {
#include "stg18/chin2/chin2_model.inc"
};

static NJS_CNK_OBJECT chin2_object = {
#include "stg18/chin2/chin2_object.inc"
};

static NJS_MATERIAL chin2_collision_materials = {
#include "stg18/chin2/chin2_collision_materials.inc"
};

static Uint16 chin2_collision_strips[40] = {
#include "stg18/chin2/chin2_collision_strips.inc"
};

static NJS_MESHSET chin2_collision_faces = {
#include "stg18/chin2/chin2_collision_faces.inc"
};

static NJS_POINT3 chin2_collision_points[10] = {
#include "stg18/chin2/chin2_collision_points.inc"
};

static NJS_VECTOR chin2_collision_normals[10] = {
#include "stg18/chin2/chin2_collision_normals.inc"
};

static NJS_MODEL chin2_collision_model = {
#include "stg18/chin2/chin2_collision_model.inc"
};

static NJS_OBJECT chin2_collision = {
#include "stg18/chin2/chin2_collision.inc"
};

/* NOLINTBEGIN(readability-identifier-naming) */
#define chin_build chin2_build
#define chin_init chin2_init
#define chin_texlists chin2_texlists
#define chin_texlist chin2_texlist
#define chin_model chin2_model
#define chin_object chin2_object
#define chin_collision chin2_collision
/* NOLINTEND(readability-identifier-naming) */
#define CHIN_EVAL_CLEARED EVAL_UNIT_POS_ANG
#define CHIN_REACH 45.0F

#include "stg18/chin_impl.h" /* IWYU pragma: keep -- the code it compiles here */
