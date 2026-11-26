#ifndef STG09_FIXTURES_ROLLER_H
#define STG09_FIXTURES_ROLLER_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME roller_names[6] = {
#include "stg09/fixtures/roller/roller_names.inc"
};

static NJS_TEXLIST roller_texlist = {
#include "stg09/fixtures/roller/roller_texlist.inc"
};

static Sint16 roller_drum_polygons[901] = {
#include "stg09/fixtures/roller/roller_drum_polygons.inc"
};

static Sint32 roller_drum_vertices[899] = {
#include "stg09/fixtures/roller/roller_drum_vertices.inc"
};

static NJS_CNK_MODEL roller_drum_model = {
#include "stg09/fixtures/roller/roller_drum_model.inc"
};

static NJS_CNK_OBJECT roller_drum_object = {
#include "stg09/fixtures/roller/roller_drum_object.inc"
};

static Sint16 roller_polygons[121] = {
#include "stg09/fixtures/roller/roller_polygons.inc"
};

static Sint32 roller_vertices[91] = {
#include "stg09/fixtures/roller/roller_vertices.inc"
};

static NJS_CNK_MODEL roller_model = {
#include "stg09/fixtures/roller/roller_model.inc"
};

static NJS_CNK_OBJECT roller_object = {
#include "stg09/fixtures/roller/roller_object.inc"
};

#endif
