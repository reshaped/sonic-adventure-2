#ifndef STG09_FIXTURES_CAGE_GATE_H
#define STG09_FIXTURES_CAGE_GATE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cage_gate_names[6] = {
#include "stg09/fixtures/cage_gate/cage_gate_names.inc"
};

static NJS_TEXLIST cage_gate_texlist = {
#include "stg09/fixtures/cage_gate/cage_gate_texlist.inc"
};

static Sint16 cage_gate_panel_polygons[215] = {
#include "stg09/fixtures/cage_gate/cage_gate_panel_polygons.inc"
};

static Sint32 cage_gate_panel_vertices[231] = {
#include "stg09/fixtures/cage_gate/cage_gate_panel_vertices.inc"
};

static NJS_CNK_MODEL cage_gate_panel_model = {
#include "stg09/fixtures/cage_gate/cage_gate_panel_model.inc"
};

static NJS_CNK_OBJECT cage_gate_panel_object = {
#include "stg09/fixtures/cage_gate/cage_gate_panel_object.inc"
};

static Sint16 cage_gate_polygons[139] = {
#include "stg09/fixtures/cage_gate/cage_gate_polygons.inc"
};

static Sint32 cage_gate_vertices[99] = {
#include "stg09/fixtures/cage_gate/cage_gate_vertices.inc"
};

static NJS_CNK_MODEL cage_gate_model = {
#include "stg09/fixtures/cage_gate/cage_gate_model.inc"
};

static NJS_CNK_OBJECT cage_gate_object = {
#include "stg09/fixtures/cage_gate/cage_gate_object.inc"
};

#endif
