#ifndef SHARED_CAR_MODELS_TAXI_FIXED_MODEL_H
#define SHARED_CAR_MODELS_TAXI_FIXED_MODEL_H
#include <ninja.h>
#include <sg_xpt.h>

/* One-piece taxi. */

static NJS_TEXNAME taxi_fixed_textures[8] = {
#include "shared/car_models/taxi_fixed_model/taxi_fixed_textures.inc"
};

static NJS_TEXLIST taxi_fixed_texlist = {
#include "shared/car_models/taxi_fixed_model/taxi_fixed_texlist.inc"
};

static Sint16 taxi_fixed_body_polygons[1017] = {
#include "shared/car_models/taxi_fixed_model/taxi_fixed_body_polygons.inc"
};

static Sint32 taxi_fixed_body_vertices[795] = {
#include "shared/car_models/taxi_fixed_model/taxi_fixed_body_vertices.inc"
};

static NJS_CNK_MODEL taxi_fixed_body_model = {
#include "shared/car_models/taxi_fixed_model/taxi_fixed_body_model.inc"
};

static NJS_CNK_OBJECT taxi_fixed_object = {
#include "shared/car_models/taxi_fixed_model/taxi_fixed_object.inc"
};

#endif
