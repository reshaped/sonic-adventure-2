#ifndef SHARED_MISSION_STREET_SMALL_SIGNBOARD_ART_H
#define SHARED_MISSION_STREET_SMALL_SIGNBOARD_ART_H
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME signboard_small_textures[8] = {
#include "shared/mission_street/small_signboard_art/signboard_small_textures.inc"
};

static NJS_TEXLIST signboard_small_texlist = {
#include "shared/mission_street/small_signboard_art/signboard_small_texlist.inc"
};

static Sint16 signboard_small_polygons[271] = {
#include "shared/mission_street/small_signboard_art/signboard_small_polygons.inc"
};

static Sint32 signboard_small_vertices[263] = {
#include "shared/mission_street/small_signboard_art/signboard_small_vertices.inc"
};

static NJS_CNK_MODEL signboard_small_model[1] = {
#include "shared/mission_street/small_signboard_art/signboard_small_model.inc"
};

static NJS_CNK_OBJECT signboard_small_object = {
#include "shared/mission_street/small_signboard_art/signboard_small_object.inc"
};

#endif
