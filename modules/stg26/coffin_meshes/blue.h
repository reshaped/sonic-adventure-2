/* Egg Quarters coffin blue. */
#ifndef STG26_COFFIN_MESHES_BLUE_H
#define STG26_COFFIN_MESHES_BLUE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME blue_textures[3] = {
#include "stg26/coffin_meshes/blue/blue_textures.inc"
};

static NJS_TEXLIST blue_texlist = {
#include "stg26/coffin_meshes/blue/blue_texlist.inc"
};

static Sint16 blue_polygons[431] = {
#include "stg26/coffin_meshes/blue/blue_polygons.inc"
};

static Sint32 blue_vertices[451] = {
#include "stg26/coffin_meshes/blue/blue_vertices.inc"
};

static NJS_CNK_MODEL blue_model = {
#include "stg26/coffin_meshes/blue/blue_model.inc"
};

static NJS_CNK_OBJECT blue_object = {
#include "stg26/coffin_meshes/blue/blue_object.inc"
};

#endif
