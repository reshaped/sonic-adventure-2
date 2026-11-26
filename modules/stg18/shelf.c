/* SHELF, the stone shelf in Dry Lagoon. */
#include "stg18/shelf.h"

#include <ninja.h>
#include <sg_xpt.h>

#include "stg18/shelf/text.inc"

static NJS_TEXNAME shelf_texlist_names[3] = {
#include "stg18/shelf/shelf_texlist_names.inc"
};

static NJS_TEXLIST shelf_texlist = {
#include "stg18/shelf/shelf_texlist.inc"
};

static Sint16 shelf_polygons[195] = {
#include "stg18/shelf/shelf_polygons.inc"
};

static Sint32 shelf_vertices[115] = {
#include "stg18/shelf/shelf_vertices.inc"
};

static NJS_CNK_MODEL shelf_model = {
#include "stg18/shelf/shelf_model.inc"
};

static NJS_CNK_OBJECT shelf_object = {
#include "stg18/shelf/shelf_object.inc"
};

static NJS_MATERIAL shelf_collision_materials = {
#include "stg18/shelf/shelf_collision_materials.inc"
};

static Uint16 shelf_collision_strips[65] = {
#include "stg18/shelf/shelf_collision_strips.inc"
};

static NJS_MESHSET shelf_collision_faces = {
#include "stg18/shelf/shelf_collision_faces.inc"
};

static NJS_POINT3 shelf_collision_points[22] = {
#include "stg18/shelf/shelf_collision_points.inc"
};

static NJS_VECTOR shelf_collision_normals[22] = {
#include "stg18/shelf/shelf_collision_normals.inc"
};

static NJS_MODEL shelf_collision_model = {
#include "stg18/shelf/shelf_collision_model.inc"
};

static NJS_OBJECT shelf_collision = {
#include "stg18/shelf/shelf_collision.inc"
};

/* NOLINTBEGIN(readability-identifier-naming) */
#define chin_build shelf_build
#define chin_init shelf_init
#define chin_texlists shelf_texlists
#define chin_texlist shelf_texlist
#define chin_model shelf_model
#define chin_object shelf_object
#define chin_collision shelf_collision
/* NOLINTEND(readability-identifier-naming) */
#define CHIN_REACH 60.0F
#define CHIN_TURN_COPY(object, work)                                           \
    ((object)->ang[0] = 0, (object)->ang[1] = (work)->ang.y,                   \
     (object)->ang[2] = 0)
#define CHIN_TURN(work) njRotateY(NULL, (work)->ang.y)

#include "stg18/chin_impl.h" /* IWYU pragma: keep -- the code it compiles here */
