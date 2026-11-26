#ifndef SHARED_CAR_MODELS_FIXED_MODELS_H
#define SHARED_CAR_MODELS_FIXED_MODELS_H
#include <ninja.h>
#include <sg_xpt.h>

/* One-piece black car. */

static NJS_TEXNAME black_car_fixed_textures[8] = {
#include "shared/car_models/fixed_models/black_car_fixed_textures.inc"
};

static NJS_TEXLIST black_car_fixed_texlist = {
#include "shared/car_models/fixed_models/black_car_fixed_texlist.inc"
};

static Sint16 black_car_fixed_body_polygons[979] = {
#include "shared/car_models/fixed_models/black_car_fixed_body_polygons.inc"
};

static Sint32 black_car_fixed_body_vertices[731] = {
#include "shared/car_models/fixed_models/black_car_fixed_body_vertices.inc"
};

static NJS_CNK_MODEL black_car_fixed_body_model = {
#include "shared/car_models/fixed_models/black_car_fixed_body_model.inc"
};

static NJS_CNK_OBJECT black_car_fixed_object = {
#include "shared/car_models/fixed_models/black_car_fixed_object.inc"
};

/* One-piece blue car. */

static NJS_TEXNAME blue_car_fixed_textures[8] = {
#include "shared/car_models/fixed_models/blue_car_fixed_textures.inc"
};

static NJS_TEXLIST blue_car_fixed_texlist = {
#include "shared/car_models/fixed_models/blue_car_fixed_texlist.inc"
};

static Sint16 blue_car_fixed_body_polygons[1003] = {
#include "shared/car_models/fixed_models/blue_car_fixed_body_polygons.inc"
};

static Sint32 blue_car_fixed_body_vertices[859] = {
#include "shared/car_models/fixed_models/blue_car_fixed_body_vertices.inc"
};

static NJS_CNK_MODEL blue_car_fixed_body_model = {
#include "shared/car_models/fixed_models/blue_car_fixed_body_model.inc"
};

static NJS_CNK_OBJECT blue_car_fixed_object = {
#include "shared/car_models/fixed_models/blue_car_fixed_object.inc"
};

/* One-piece red car. */

static NJS_TEXNAME red_car_fixed_textures[9] = {
#include "shared/car_models/fixed_models/red_car_fixed_textures.inc"
};

static NJS_TEXLIST red_car_fixed_texlist = {
#include "shared/car_models/fixed_models/red_car_fixed_texlist.inc"
};

static Sint16 red_car_fixed_body_polygons[1001] = {
#include "shared/car_models/fixed_models/red_car_fixed_body_polygons.inc"
};

static Sint32 red_car_fixed_body_vertices[739] = {
#include "shared/car_models/fixed_models/red_car_fixed_body_vertices.inc"
};

static NJS_CNK_MODEL red_car_fixed_body_model = {
#include "shared/car_models/fixed_models/red_car_fixed_body_model.inc"
};

static NJS_CNK_OBJECT red_car_fixed_object = {
#include "shared/car_models/fixed_models/red_car_fixed_object.inc"
};

/* One-piece civic. */

static NJS_TEXNAME civic_fixed_textures[7] = {
#include "shared/car_models/fixed_models/civic_fixed_textures.inc"
};

static NJS_TEXLIST civic_fixed_texlist = {
#include "shared/car_models/fixed_models/civic_fixed_texlist.inc"
};

static Sint16 civic_fixed_body_polygons[909] = {
#include "shared/car_models/fixed_models/civic_fixed_body_polygons.inc"
};

static Sint32 civic_fixed_body_vertices[747] = {
#include "shared/car_models/fixed_models/civic_fixed_body_vertices.inc"
};

static NJS_CNK_MODEL civic_fixed_body_model = {
#include "shared/car_models/fixed_models/civic_fixed_body_model.inc"
};

static NJS_CNK_OBJECT civic_fixed_object = {
#include "shared/car_models/fixed_models/civic_fixed_object.inc"
};

#endif
