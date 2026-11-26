#include <ninja.h>
#include <sg_xpt.h>

#include "shared/fire_dish.h"

static const char material_sara1[12] = {
#include "shared/ocean_fire_dish_art/material_sara1.inc"
};

static NJS_TEXNAME fire_dish_textures[1] = {
#include "shared/ocean_fire_dish_art/fire_dish_textures.inc"
};

NJS_TEXLIST fire_dish_texlist = {
#include "shared/ocean_fire_dish_art/fire_dish_texlist.inc"
};

static Sint16 fire_dish_polygons[121] = {
#include "shared/ocean_fire_dish_art/fire_dish_polygons.inc"
};

static Sint32 fire_dish_vertices[87] = {
#include "shared/ocean_fire_dish_art/fire_dish_vertices.inc"
};

NJS_CNK_MODEL fire_dish_model = {
#include "shared/ocean_fire_dish_art/fire_dish_model.inc"
};

static NJS_CNK_OBJECT fire_dish_object = {
#include "shared/ocean_fire_dish_art/fire_dish_object.inc"
};
