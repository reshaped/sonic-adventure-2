#include <ninja.h>
#include <sg_xpt.h>

#include "shared/wall_fire.h"

static Sint16 wall_fire_flame_polygons[25] = {
#include "shared/ocean_wall_fire_flame_art/wall_fire_flame_polygons.inc"
};

static Sint32 wall_fire_flame_vertices[19] = {
#include "shared/ocean_wall_fire_flame_art/wall_fire_flame_vertices.inc"
};

NJS_CNK_MODEL wall_fire_flame_model[1] = {
#include "shared/ocean_wall_fire_flame_art/wall_fire_flame_model.inc"
};

static NJS_CNK_OBJECT wall_fire_flame_object = {
#include "shared/ocean_wall_fire_flame_art/wall_fire_flame_object.inc"
};
