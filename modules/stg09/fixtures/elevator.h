#ifndef STG09_FIXTURES_ELEVATOR_H
#define STG09_FIXTURES_ELEVATOR_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME elevator_names[16] = {
#include "stg09/fixtures/elevator/elevator_names.inc"
};

static NJS_TEXLIST elevator_texlist = {
#include "stg09/fixtures/elevator/elevator_texlist.inc"
};

static Sint16 elevator_gate_polygons[359] = {
#include "stg09/fixtures/elevator/elevator_gate_polygons.inc"
};

static Sint32 elevator_gate_vertices[275] = {
#include "stg09/fixtures/elevator/elevator_gate_vertices.inc"
};

static NJS_CNK_MODEL elevator_gate_model = {
#include "stg09/fixtures/elevator/elevator_gate_model.inc"
};

static NJS_CNK_OBJECT elevator_gate_object = {
#include "stg09/fixtures/elevator/elevator_gate_object.inc"
};

static Sint16 elevator_rails_polygons[307] = {
#include "stg09/fixtures/elevator/elevator_rails_polygons.inc"
};

static Sint32 elevator_rails_vertices[227] = {
#include "stg09/fixtures/elevator/elevator_rails_vertices.inc"
};

static NJS_CNK_MODEL elevator_rails_model = {
#include "stg09/fixtures/elevator/elevator_rails_model.inc"
};

static NJS_CNK_OBJECT elevator_rails_object = {
#include "stg09/fixtures/elevator/elevator_rails_object.inc"
};

static Sint16 elevator_polygons[1003] = {
#include "stg09/fixtures/elevator/elevator_polygons.inc"
};

static Sint32 elevator_vertices[875] = {
#include "stg09/fixtures/elevator/elevator_vertices.inc"
};

static NJS_CNK_MODEL elevator_model = {
#include "stg09/fixtures/elevator/elevator_model.inc"
};

static NJS_CNK_OBJECT elevator_object = {
#include "stg09/fixtures/elevator/elevator_object.inc"
};

#endif
