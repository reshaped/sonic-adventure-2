/* The plane of water that follows the jungle camera. */
#ifndef SHARED_JUNGLE_WATER_SHAPE_H
#define SHARED_JUNGLE_WATER_SHAPE_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 jungle_background_water_polygons[23] = {
#include "shared/jungle_water_shape/jungle_background_water_polygons.inc"
};

static Sint32 jungle_background_water_vertices[19] = {
#include "shared/jungle_water_shape/jungle_background_water_vertices.inc"
};

/* @hack type: the water animator addresses this model through an array. */
static NJS_CNK_MODEL jungle_background_water_model[1] = {
#include "shared/jungle_water_shape/jungle_background_water_model.inc"
};

static NJS_CNK_OBJECT background_water_object = {
#include "shared/jungle_water_shape/background_water_object.inc"
};

#endif
