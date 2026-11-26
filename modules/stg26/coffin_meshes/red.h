/* Egg Quarters coffin red. */
#ifndef STG26_COFFIN_MESHES_RED_H
#define STG26_COFFIN_MESHES_RED_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME red_textures[3] = {
#include "stg26/coffin_meshes/red/red_textures.inc"
};

static NJS_TEXLIST red_texlist = {
#include "stg26/coffin_meshes/red/red_texlist.inc"
};

static Sint16 red_polygons[431] = {
#include "stg26/coffin_meshes/red/red_polygons.inc"
};

static Sint32 red_vertices[435] = {
#include "stg26/coffin_meshes/red/red_vertices.inc"
};

static NJS_CNK_MODEL red_model = {
#include "stg26/coffin_meshes/red/red_model.inc"
};

static NJS_CNK_OBJECT red_object = {
#include "stg26/coffin_meshes/red/red_object.inc"
};

#endif
