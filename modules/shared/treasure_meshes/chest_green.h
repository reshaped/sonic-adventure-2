/* Treasure chest green. */
#ifndef SHARED_TREASURE_MESHES_CHEST_GREEN_H
#define SHARED_TREASURE_MESHES_CHEST_GREEN_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME green_textures[1] = {
#include "shared/treasure_meshes/chest_green/green_textures.inc"
};

static NJS_TEXLIST green_texlist = {
#include "shared/treasure_meshes/chest_green/green_texlist.inc"
};

static Sint16 green_polygons[257] = {
#include "shared/treasure_meshes/chest_green/green_polygons.inc"
};

static Sint32 green_vertices[195] = {
#include "shared/treasure_meshes/chest_green/green_vertices.inc"
};

static NJS_CNK_MODEL green_model = {
#include "shared/treasure_meshes/chest_green/green_model.inc"
};

static NJS_CNK_OBJECT green_object = {
#include "shared/treasure_meshes/chest_green/green_object.inc"
};

#endif
