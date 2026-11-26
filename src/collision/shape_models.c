/* the models of shape_models.h's three shapes, untextured */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/shape_models.h"

/* the sphere's vertices are two poles and nine rings of ten */
static Sint16 sphere_polygons[249] = {
#include "collision/shape_models/sphere_polygons.inc"
};

static Sint32 sphere_vertices[555] = {
#include "collision/shape_models/sphere_vertices.inc"
};

static NJS_CNK_MODEL sphere_model = {
#include "collision/shape_models/sphere_model.inc"
};
NJS_CNK_OBJECT collision_sphere_object = {
#include "collision/shape_models/collision_sphere_object.inc"
};

/* the hemisphere's vertices are four rings of ten, from the equator up, and
 * the pole */
static Sint16 hemisphere_polygons[101] = {
#include "collision/shape_models/hemisphere_polygons.inc"
};

static Sint32 hemisphere_vertices[249] = {
#include "collision/shape_models/hemisphere_vertices.inc"
};

static NJS_CNK_MODEL hemisphere_model = {
#include "collision/shape_models/hemisphere_model.inc"
};
NJS_CNK_OBJECT collision_hemisphere_object = {
#include "collision/shape_models/collision_hemisphere_object.inc"
};

/* the tube's vertices are two rings of ten, which one strip joins */
static Sint16 tube_polygons[33] = {
#include "collision/shape_models/tube_polygons.inc"
};

static Sint32 tube_vertices[123] = {
#include "collision/shape_models/tube_vertices.inc"
};

static NJS_CNK_MODEL tube_model = {
#include "collision/shape_models/tube_model.inc"
};
NJS_CNK_OBJECT collision_tube_object = {
#include "collision/shape_models/collision_tube_object.inc"
};
