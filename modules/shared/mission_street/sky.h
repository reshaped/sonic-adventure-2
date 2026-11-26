/* The night sky drawn around each camera in both Mission Street routes. */
#ifndef SHARED_MISSION_STREET_SKY_H
#define SHARED_MISSION_STREET_SKY_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME street_sky_textures[3] = {
#include "shared/mission_street/sky/street_sky_textures.inc"
};

static NJS_TEXLIST street_sky_texlist = {
#include "shared/mission_street/sky/street_sky_texlist.inc"
};

static Sint16 street_sky_polygons[907] = {
#include "shared/mission_street/sky/street_sky_polygons.inc"
};

static Sint32 street_sky_vertices[459] = {
#include "shared/mission_street/sky/street_sky_vertices.inc"
};

static NJS_CNK_MODEL street_sky_mesh = {
#include "shared/mission_street/sky/street_sky_mesh.inc"
};

static NJS_CNK_OBJECT street_sky_object = {
#include "shared/mission_street/sky/street_sky_object.inc"
};

#endif
