/* debug_draw_model's seven models, which camera.h describes. each has one
 * material, a diffuse color blended additively */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"

static Sint16 cone_polygons[39] = {
#include "draw/debug_models/cone_polygons.inc"
};

static Sint32 cone_vertices[63] = {
#include "draw/debug_models/cone_vertices.inc"
};

static NJS_CNK_MODEL cone_model = {
#include "draw/debug_models/cone_model.inc"
};

static NJS_CNK_OBJECT cone = {
#include "draw/debug_models/cone.inc"
};

static Sint16 box_polygons[29] = {
#include "draw/debug_models/box_polygons.inc"
};

static Sint32 box_vertices[51] = {
#include "draw/debug_models/box_vertices.inc"
};

static NJS_CNK_MODEL box_model = {
#include "draw/debug_models/box_model.inc"
};

static NJS_CNK_OBJECT box = {
#include "draw/debug_models/box.inc"
};

static Sint16 cylinder_polygons[59] = {
#include "draw/debug_models/cylinder_polygons.inc"
};

static Sint32 cylinder_vertices[111] = {
#include "draw/debug_models/cylinder_vertices.inc"
};

static NJS_CNK_MODEL cylinder_model = {
#include "draw/debug_models/cylinder_model.inc"
};

static NJS_CNK_OBJECT cylinder = {
#include "draw/debug_models/cylinder.inc"
};

static Sint16 plane_polygons[13] = {
#include "draw/debug_models/plane_polygons.inc"
};

static Sint32 plane_vertices[27] = {
#include "draw/debug_models/plane_vertices.inc"
};

static NJS_CNK_MODEL plane_model = {
#include "draw/debug_models/plane_model.inc"
};

static NJS_CNK_OBJECT plane = {
#include "draw/debug_models/plane.inc"
};

static Sint16 sphere_polygons[341] = {
#include "draw/debug_models/sphere_polygons.inc"
};

static Sint32 sphere_vertices[807] = {
#include "draw/debug_models/sphere_vertices.inc"
};

static NJS_CNK_MODEL sphere_model = {
#include "draw/debug_models/sphere_model.inc"
};

NJS_CNK_OBJECT debug_sphere_object = {
#include "draw/debug_models/debug_sphere_object.inc"
};

static Sint16 ring_polygons[161] = {
#include "draw/debug_models/ring_polygons.inc"
};

static Sint32 ring_vertices[387] = {
#include "draw/debug_models/ring_vertices.inc"
};

static NJS_CNK_MODEL ring_model = {
#include "draw/debug_models/ring_model.inc"
};

static NJS_CNK_OBJECT ring = {
#include "draw/debug_models/ring.inc"
};

static Sint16 marker_polygons[65] = {
#include "draw/debug_models/marker_polygons.inc"
};

static Sint32 marker_vertices[153] = {
#include "draw/debug_models/marker_vertices.inc"
};

static NJS_CNK_MODEL marker_model = {
#include "draw/debug_models/marker_model.inc"
};

static NJS_CNK_OBJECT marker = {
#include "draw/debug_models/marker.inc"
};

NJS_CNK_OBJECT *debug_models[DEBUG_MODEL_COUNT] = {
#include "draw/debug_models/debug_models.inc"
};
