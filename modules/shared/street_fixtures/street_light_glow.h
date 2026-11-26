/* Street light glow in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_STREET_LIGHT_GLOW_H
#define SHARED_STREET_FIXTURES_STREET_LIGHT_GLOW_H

#include <ninja.h>

#include "draw/chunk.h"
#include "draw/chunk_polygon.h"

static NJS_TEXNAME street_light_glow_textures[1] = {
#include "shared/street_fixtures/street_light_glow/street_light_glow_textures.inc"
};

static NJS_TEXLIST street_light_glow_texlist = {
#include "shared/street_fixtures/street_light_glow/street_light_glow_texlist.inc"
};

#endif
