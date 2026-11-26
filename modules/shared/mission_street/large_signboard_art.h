#ifndef SHARED_MISSION_STREET_LARGE_SIGNBOARD_ART_H
#define SHARED_MISSION_STREET_LARGE_SIGNBOARD_ART_H
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME signboard_large_textures[6] = {
#include "shared/mission_street/large_signboard_art/signboard_large_textures.inc"
};

static NJS_TEXLIST signboard_large_texlist = {
#include "shared/mission_street/large_signboard_art/signboard_large_texlist.inc"
};

static NJS_TEXNAME signboard_multiplayer_textures[6] = {
#include "shared/mission_street/large_signboard_art/signboard_multiplayer_textures.inc"
};

static NJS_TEXLIST signboard_multiplayer_texlist = {
#include "shared/mission_street/large_signboard_art/signboard_multiplayer_texlist.inc"
};

static Sint16 signboard_large_polygons[423] = {
#include "shared/mission_street/large_signboard_art/signboard_large_polygons.inc"
};

static Sint32 signboard_large_vertices[435] = {
#include "shared/mission_street/large_signboard_art/signboard_large_vertices.inc"
};

static NJS_CNK_MODEL signboard_large_model = {
#include "shared/mission_street/large_signboard_art/signboard_large_model.inc"
};

static NJS_CNK_OBJECT signboard_large_object = {
#include "shared/mission_street/large_signboard_art/signboard_large_object.inc"
};

#endif
