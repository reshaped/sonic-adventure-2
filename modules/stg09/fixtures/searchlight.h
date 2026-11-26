/* Prison Lane's searchlight base, its tilting head and the beam. */
#ifndef STG09_FIXTURES_SEARCHLIGHT_H
#define STG09_FIXTURES_SEARCHLIGHT_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME searchlight_names[5] = {
#include "stg09/fixtures/searchlight/searchlight_names.inc"
};

static NJS_TEXLIST searchlight_texlist = {
#include "stg09/fixtures/searchlight/searchlight_texlist.inc"
};

static Sint16 searchlight_beam_polygons[289] = {
#include "stg09/fixtures/searchlight/searchlight_beam_polygons.inc"
};

static Sint32 searchlight_beam_vertices[203] = {
#include "stg09/fixtures/searchlight/searchlight_beam_vertices.inc"
};

static NJS_CNK_MODEL searchlight_beam_model = {
#include "stg09/fixtures/searchlight/searchlight_beam_model.inc"
};

static NJS_CNK_OBJECT searchlight_beam_object = {
#include "stg09/fixtures/searchlight/searchlight_beam_object.inc"
};

static Sint16 searchlight_head_polygons[67] = {
#include "stg09/fixtures/searchlight/searchlight_head_polygons.inc"
};

static Sint32 searchlight_head_vertices[67] = {
#include "stg09/fixtures/searchlight/searchlight_head_vertices.inc"
};

static NJS_CNK_MODEL searchlight_head_model = {
#include "stg09/fixtures/searchlight/searchlight_head_model.inc"
};

static NJS_CNK_OBJECT searchlight_head_object = {
#include "stg09/fixtures/searchlight/searchlight_head_object.inc"
};

static Sint16 searchlight_polygons[273] = {
#include "stg09/fixtures/searchlight/searchlight_polygons.inc"
};

static Sint32 searchlight_vertices[235] = {
#include "stg09/fixtures/searchlight/searchlight_vertices.inc"
};

static NJS_CNK_MODEL searchlight_model = {
#include "stg09/fixtures/searchlight/searchlight_model.inc"
};

static NJS_CNK_OBJECT searchlight_object = {
#include "stg09/fixtures/searchlight/searchlight_object.inc"
};

#endif
