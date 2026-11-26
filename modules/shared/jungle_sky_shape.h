/* The jungle sky dome and its two texture strips. */
#ifndef SHARED_JUNGLE_SKY_SHAPE_H
#define SHARED_JUNGLE_SKY_SHAPE_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 jungle_background_polygons[429] = {
#include "shared/jungle_sky_shape/jungle_background_polygons.inc"
};

static Sint32 jungle_background_vertices[231] = {
#include "shared/jungle_sky_shape/jungle_background_vertices.inc"
};

static NJS_CNK_MODEL jungle_background_model = {
#include "shared/jungle_sky_shape/jungle_background_model.inc"
};

static NJS_CNK_OBJECT jungle_background_object = {
#include "shared/jungle_sky_shape/jungle_background_object.inc"
};

#endif
