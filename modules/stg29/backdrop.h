/* Texture archives and placed objects drawn by the arena backdrop. */
#ifndef STG29_BACKDROP_H
#define STG29_BACKDROP_H

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk.h"
#include "draw/chunk_polygon.h"

static NJS_TEXNAME backdrop_pvm_textures[4] = {
#include "stg29/backdrop/backdrop_pvm_textures.inc"
};

static NJS_TEXLIST backdrop_pvm_texlist = {
#include "stg29/backdrop/backdrop_pvm_texlist.inc"
};

static NJS_TEXNAME space_inner_textures[1] = {
#include "stg29/backdrop/space_inner_textures.inc"
};

static NJS_TEXLIST space_inner_texlist = {
#include "stg29/backdrop/space_inner_texlist.inc"
};

#include "shared/celestial/orbit_64.h"

static NJS_CNK_OBJECT space_inner_object = {
#include "stg29/backdrop/space_inner_object.inc"
};

static NJS_TEXNAME space_outer_textures[2] = {
#include "stg29/backdrop/space_outer_textures.inc"
};

static NJS_TEXLIST space_outer_texlist = {
#include "stg29/backdrop/space_outer_texlist.inc"
};

#include "shared/celestial/orbit_128.h"

static NJS_CNK_OBJECT space_outer_object = {
#include "stg29/backdrop/space_outer_object.inc"
};

static NJS_TEXNAME earth_textures[2] = {
#include "stg29/backdrop/earth_textures.inc"
};

static NJS_TEXLIST earth_texlist = {
#include "stg29/backdrop/earth_texlist.inc"
};

#include "shared/celestial/earth.h"

static NJS_CNK_OBJECT earth_object = {
#include "stg29/backdrop/earth_object.inc"
};

#endif
