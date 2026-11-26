/* Treasure chest red. */
#ifndef SHARED_TREASURE_MESHES_CHEST_RED_H
#define SHARED_TREASURE_MESHES_CHEST_RED_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME red_textures[1] = {
#include "shared/treasure_meshes/chest_red/red_textures.inc"
};

static NJS_TEXLIST red_texlist = {
#include "shared/treasure_meshes/chest_red/red_texlist.inc"
};

static Sint16 red_polygons[257] = {
#include "shared/treasure_meshes/chest_red/red_polygons.inc"
};

static Sint32 red_vertices[195] = {
#include "shared/treasure_meshes/chest_red/red_vertices.inc"
};

static NJS_CNK_MODEL red_model = {
#include "shared/treasure_meshes/chest_red/red_model.inc"
};

static NJS_CNK_OBJECT red_object = {
#include "shared/treasure_meshes/chest_red/red_object.inc"
};

#endif
