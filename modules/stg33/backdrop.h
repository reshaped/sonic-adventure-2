/* Texture archives and placed objects drawn by the arena backdrop. */
#ifndef STG33_BACKDROP_H
#define STG33_BACKDROP_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME background_pvm_textures[33] = {
#include "stg33/backdrop/background_pvm_textures.inc"
};

static NJS_TEXLIST background_pvm_texlist = {
#include "stg33/backdrop/background_pvm_texlist.inc"
};

static NJS_TEXNAME space_bg_2_textures[1] = {
#include "stg33/backdrop/space_bg_2_textures.inc"
};

static NJS_TEXLIST space_bg_2_texlist = {
#include "stg33/backdrop/space_bg_2_texlist.inc"
};

#include "shared/celestial/orbit_64.h"

static NJS_CNK_OBJECT space_bg_2_object = {
#include "stg33/backdrop/space_bg_2_object.inc"
};

static NJS_TEXNAME space_bg_textures[2] = {
#include "stg33/backdrop/space_bg_textures.inc"
};

static NJS_TEXLIST space_bg_texlist = {
#include "stg33/backdrop/space_bg_texlist.inc"
};

#include "shared/celestial/orbit_128.h"

static NJS_CNK_OBJECT space_bg_object = {
#include "stg33/backdrop/space_bg_object.inc"
};

static NJS_TEXNAME space_textures[17] = {
#include "stg33/backdrop/space_textures.inc"
};

static NJS_TEXLIST space_texlist = {
#include "stg33/backdrop/space_texlist.inc"
};

#include "shared/celestial/stars_herd.h"

static NJS_CNK_OBJECT space_object = {
#include "stg33/backdrop/space_object.inc"
};

static NJS_TEXNAME meteor_textures[1] = {
#include "stg33/backdrop/meteor_textures.inc"
};

static NJS_TEXLIST meteor_texlist = {
#include "stg33/backdrop/meteor_texlist.inc"
};

#include "shared/celestial/meteor.h"

static NJS_CNK_OBJECT meteor_object = {
#include "stg33/backdrop/meteor_object.inc"
};

static NJS_TEXNAME danger_textures[13] = {
#include "stg33/backdrop/danger_textures.inc"
};

static NJS_TEXLIST danger_texlist = {
#include "stg33/backdrop/danger_texlist.inc"
};

static Sint16 danger_polygons[23] = {
#include "stg33/backdrop/danger_polygons.inc"
};

static Sint32 danger_vertices[19] = {
#include "stg33/backdrop/danger_vertices.inc"
};

static NJS_CNK_MODEL danger_model = {
#include "stg33/backdrop/danger_model.inc"
};

static NJS_CNK_OBJECT danger_object = {
#include "stg33/backdrop/danger_object.inc"
};

#endif
