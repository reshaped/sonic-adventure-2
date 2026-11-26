#ifndef STG11_FIXTURES_ELEVATOR_H
#define STG11_FIXTURES_ELEVATOR_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME elevator_names[13] = {
#include "stg11/fixtures/elevator/elevator_names.inc"
};

static NJS_TEXLIST elevator_texlist = {
#include "stg11/fixtures/elevator/elevator_texlist.inc"
};

static Sint16 elevator_rails_polygons[279] = {
#include "stg11/fixtures/elevator/elevator_rails_polygons.inc"
};

static Sint32 elevator_rails_vertices[207] = {
#include "stg11/fixtures/elevator/elevator_rails_vertices.inc"
};

static NJS_CNK_MODEL elevator_rails_model = {
#include "stg11/fixtures/elevator/elevator_rails_model.inc"
};

static NJS_CNK_OBJECT elevator_rails_object = {
#include "stg11/fixtures/elevator/elevator_rails_object.inc"
};

static Sint16 elevator_gate_polygons[99] = {
#include "stg11/fixtures/elevator/elevator_gate_polygons.inc"
};

static Sint32 elevator_gate_vertices[75] = {
#include "stg11/fixtures/elevator/elevator_gate_vertices.inc"
};

static NJS_CNK_MODEL elevator_gate_model = {
#include "stg11/fixtures/elevator/elevator_gate_model.inc"
};

static NJS_CNK_OBJECT elevator_gate_object = {
#include "stg11/fixtures/elevator/elevator_gate_object.inc"
};

static Sint16 elevator_polygons[1099] = {
#include "stg11/fixtures/elevator/elevator_polygons.inc"
};

static Sint32 elevator_vertices[967] = {
#include "stg11/fixtures/elevator/elevator_vertices.inc"
};

static NJS_CNK_MODEL elevator_model = {
#include "stg11/fixtures/elevator/elevator_model.inc"
};

static NJS_CNK_OBJECT elevator_object = {
#include "stg11/fixtures/elevator/elevator_object.inc"
};

#endif
