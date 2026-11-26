#ifndef STG11_FIXTURES_SGOAL_H
#define STG11_FIXTURES_SGOAL_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME sgoal_names[7] = {
#include "stg11/fixtures/sgoal/sgoal_names.inc"
};

static NJS_TEXLIST sgoal_texlist = {
#include "stg11/fixtures/sgoal/sgoal_texlist.inc"
};

static Sint16 sgoal_polygons[379] = {
#include "stg11/fixtures/sgoal/sgoal_polygons.inc"
};

static Sint32 sgoal_vertices[251] = {
#include "stg11/fixtures/sgoal/sgoal_vertices.inc"
};

static NJS_CNK_MODEL sgoal_model = {
#include "stg11/fixtures/sgoal/sgoal_model.inc"
};

static NJS_CNK_OBJECT sgoal_object = {
#include "stg11/fixtures/sgoal/sgoal_object.inc"
};

#endif
