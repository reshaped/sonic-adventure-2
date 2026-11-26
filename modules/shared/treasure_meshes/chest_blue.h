/* Treasure chest blue. */
#ifndef SHARED_TREASURE_MESHES_CHEST_BLUE_H
#define SHARED_TREASURE_MESHES_CHEST_BLUE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME blue_textures[1] = {
#include "shared/treasure_meshes/chest_blue/blue_textures.inc"
};

static NJS_TEXLIST blue_texlist = {
#include "shared/treasure_meshes/chest_blue/blue_texlist.inc"
};

static Sint16 blue_polygons[257] = {
#include "shared/treasure_meshes/chest_blue/blue_polygons.inc"
};

static Sint32 blue_vertices[195] = {
#include "shared/treasure_meshes/chest_blue/blue_vertices.inc"
};

static NJS_CNK_MODEL blue_model = {
#include "shared/treasure_meshes/chest_blue/blue_model.inc"
};

static NJS_CNK_OBJECT blue_object = {
#include "shared/treasure_meshes/chest_blue/blue_object.inc"
};

#endif
