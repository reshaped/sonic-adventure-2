#include <ninja.h>
#include <sg_xpt.h>

#include "shared/wall_fire.h"

static Sint16 wall_fire_polygons[121] = {
#include "shared/ocean_wall_fire_art/wall_fire_polygons.inc"
};

static Sint32 wall_fire_vertices[95] = {
#include "shared/ocean_wall_fire_art/wall_fire_vertices.inc"
};

NJS_CNK_MODEL wall_fire_model = {
#include "shared/ocean_wall_fire_art/wall_fire_model.inc"
};

static NJS_CNK_OBJECT wall_fire_object = {
#include "shared/ocean_wall_fire_art/wall_fire_object.inc"
};
