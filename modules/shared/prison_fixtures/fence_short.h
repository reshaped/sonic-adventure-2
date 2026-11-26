#ifndef SHARED_PRISON_FIXTURES_FENCE_SHORT_H
#define SHARED_PRISON_FIXTURES_FENCE_SHORT_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME fence_short_names[4] = {
#include "shared/prison_fixtures/fence_short/fence_short_names.inc"
};

static NJS_TEXLIST fence_short_texlist = {
#include "shared/prison_fixtures/fence_short/fence_short_texlist.inc"
};

static Sint16 fence_short_polygons[427] = {
#include "shared/prison_fixtures/fence_short/fence_short_polygons.inc"
};

static Sint32 fence_short_vertices[431] = {
#include "shared/prison_fixtures/fence_short/fence_short_vertices.inc"
};

static NJS_CNK_MODEL fence_short_model = {
#include "shared/prison_fixtures/fence_short/fence_short_model.inc"
};

static NJS_CNK_OBJECT fence_short_object = {
#include "shared/prison_fixtures/fence_short/fence_short_object.inc"
};

#endif
