/* The lower sky follows the camera. The upper layer rotates around it. */
#ifndef STG04_BACKGROUND_H
#define STG04_BACKGROUND_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 background_polygons[521] = {
#include "stg04/background/background_polygons.inc"
};

static Sint32 background_vertices[267] = {
#include "stg04/background/background_vertices.inc"
};

static NJS_CNK_MODEL background_model = {
#include "stg04/background/background_model.inc"
};

static NJS_CNK_OBJECT background_object = {
#include "stg04/background/background_object.inc"
};

static NJS_TEXNAME background_textures[5] = {
#include "stg04/background/background_textures.inc"
};

static NJS_TEXLIST background_texlist = {
#include "stg04/background/background_texlist.inc"
};

static Sint16 turning_sky_polygons[461] = {
#include "stg04/background/turning_sky_polygons.inc"
};

static Sint32 turning_sky_vertices[235] = {
#include "stg04/background/turning_sky_vertices.inc"
};

static NJS_CNK_MODEL turning_sky_model = {
#include "stg04/background/turning_sky_model.inc"
};

static NJS_CNK_OBJECT turning_sky_object = {
#include "stg04/background/turning_sky_object.inc"
};

#endif
