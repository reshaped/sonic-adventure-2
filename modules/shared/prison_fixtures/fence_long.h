#ifndef SHARED_PRISON_FIXTURES_FENCE_LONG_H
#define SHARED_PRISON_FIXTURES_FENCE_LONG_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME fence_long_names[6] = {
#include "shared/prison_fixtures/fence_long/fence_long_names.inc"
};

static NJS_TEXLIST fence_long_texlist = {
#include "shared/prison_fixtures/fence_long/fence_long_texlist.inc"
};

static Sint16 fence_long_polygons[351] = {
#include "shared/prison_fixtures/fence_long/fence_long_polygons.inc"
};

static Sint32 fence_long_vertices[331] = {
#include "shared/prison_fixtures/fence_long/fence_long_vertices.inc"
};

static NJS_CNK_MODEL fence_long_model = {
#include "shared/prison_fixtures/fence_long/fence_long_model.inc"
};

static NJS_CNK_OBJECT fence_long_object = {
#include "shared/prison_fixtures/fence_long/fence_long_object.inc"
};

#endif
