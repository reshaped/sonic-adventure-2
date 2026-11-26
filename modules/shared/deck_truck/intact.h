#ifndef SHARED_DECK_TRUCK_INTACT_H
#define SHARED_DECK_TRUCK_INTACT_H

#include <ninja.h>

static NJS_TEXNAME deck_truck_textures[11] = {
#include "shared/deck_truck/intact/deck_truck_textures.inc"
};

static NJS_TEXLIST deck_truck_texlist = {
#include "shared/deck_truck/intact/deck_truck_texlist.inc"
};

static Sint16 deck_truck_lamp_polygons[57] = {
#include "shared/deck_truck/intact/deck_truck_lamp_polygons.inc"
};

static Sint32 deck_truck_lamp_vertices[51] = {
#include "shared/deck_truck/intact/deck_truck_lamp_vertices.inc"
};

static NJS_CNK_MODEL deck_truck_lamp_model = {
#include "shared/deck_truck/intact/deck_truck_lamp_model.inc"
};

static NJS_CNK_OBJECT deck_truck_lamp[1] = {
#include "shared/deck_truck/intact/deck_truck_lamp.inc"
};

static Sint16 deck_truck_body_polygons[677] = {
#include "shared/deck_truck/intact/deck_truck_body_polygons.inc"
};

static Sint32 deck_truck_body_vertices[595] = {
#include "shared/deck_truck/intact/deck_truck_body_vertices.inc"
};

static NJS_CNK_MODEL deck_truck_body_model = {
#include "shared/deck_truck/intact/deck_truck_body_model.inc"
};

static NJS_CNK_OBJECT deck_truck_body[1] = {
#include "shared/deck_truck/intact/deck_truck_body.inc"
};

static Sint16 deck_truck_front_wheel_polygons[39] = {
#include "shared/deck_truck/intact/deck_truck_front_wheel_polygons.inc"
};

static Sint32 deck_truck_front_wheel_vertices[35] = {
#include "shared/deck_truck/intact/deck_truck_front_wheel_vertices.inc"
};

static NJS_CNK_MODEL deck_truck_front_wheel_model = {
#include "shared/deck_truck/intact/deck_truck_front_wheel_model.inc"
};

static NJS_CNK_OBJECT deck_truck_front_wheel[1] = {
#include "shared/deck_truck/intact/deck_truck_front_wheel.inc"
};

static Sint16 deck_truck_back_wheel_polygons[39] = {
#include "shared/deck_truck/intact/deck_truck_back_wheel_polygons.inc"
};

static Sint32 deck_truck_back_wheel_vertices[35] = {
#include "shared/deck_truck/intact/deck_truck_back_wheel_vertices.inc"
};

static NJS_CNK_MODEL deck_truck_back_wheel_model = {
#include "shared/deck_truck/intact/deck_truck_back_wheel_model.inc"
};

static NJS_CNK_OBJECT deck_truck_back_wheel[1] = {
#include "shared/deck_truck/intact/deck_truck_back_wheel.inc"
};

static Sint16 deck_truck_base_polygons[25] = {
#include "shared/deck_truck/intact/deck_truck_base_polygons.inc"
};

static Sint32 deck_truck_base_vertices[19] = {
#include "shared/deck_truck/intact/deck_truck_base_vertices.inc"
};

static NJS_CNK_MODEL deck_truck_base_model = {
#include "shared/deck_truck/intact/deck_truck_base_model.inc"
};

static NJS_CNK_OBJECT deck_truck_base[1] = {
#include "shared/deck_truck/intact/deck_truck_base.inc"
};

#endif
