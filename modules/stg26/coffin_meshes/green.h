/* Egg Quarters coffin green. */
#ifndef STG26_COFFIN_MESHES_GREEN_H
#define STG26_COFFIN_MESHES_GREEN_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME green_textures[3] = {
#include "stg26/coffin_meshes/green/green_textures.inc"
};

static NJS_TEXLIST green_texlist = {
#include "stg26/coffin_meshes/green/green_texlist.inc"
};

static Sint16 green_polygons[431] = {
#include "stg26/coffin_meshes/green/green_polygons.inc"
};

static Sint32 green_vertices[451] = {
#include "stg26/coffin_meshes/green/green_vertices.inc"
};

static NJS_CNK_MODEL green_model = {
#include "stg26/coffin_meshes/green/green_model.inc"
};

static NJS_CNK_OBJECT green_object = {
#include "stg26/coffin_meshes/green/green_object.inc"
};

#endif
