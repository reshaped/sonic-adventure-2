#ifndef STG09_FIXTURES_WALL_GATE_H
#define STG09_FIXTURES_WALL_GATE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME wall_gate_names[5] = {
#include "stg09/fixtures/wall_gate/wall_gate_names.inc"
};

static NJS_TEXLIST wall_gate_texlist = {
#include "stg09/fixtures/wall_gate/wall_gate_texlist.inc"
};

static Sint16 wall_gate_panel_polygons[133] = {
#include "stg09/fixtures/wall_gate/wall_gate_panel_polygons.inc"
};

static Sint32 wall_gate_panel_vertices[95] = {
#include "stg09/fixtures/wall_gate/wall_gate_panel_vertices.inc"
};

static NJS_CNK_MODEL wall_gate_panel_model = {
#include "stg09/fixtures/wall_gate/wall_gate_panel_model.inc"
};

static NJS_CNK_OBJECT wall_gate_panel_object = {
#include "stg09/fixtures/wall_gate/wall_gate_panel_object.inc"
};

static Sint16 wall_gate_polygons[139] = {
#include "stg09/fixtures/wall_gate/wall_gate_polygons.inc"
};

static Sint32 wall_gate_vertices[99] = {
#include "stg09/fixtures/wall_gate/wall_gate_vertices.inc"
};

static NJS_CNK_MODEL wall_gate_model = {
#include "stg09/fixtures/wall_gate/wall_gate_model.inc"
};

static NJS_CNK_OBJECT wall_gate_object = {
#include "stg09/fixtures/wall_gate/wall_gate_object.inc"
};

#endif
