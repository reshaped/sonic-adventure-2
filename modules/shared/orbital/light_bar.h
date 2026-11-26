#ifndef SHARED_ORBITAL_LIGHT_BAR_H
#define SHARED_ORBITAL_LIGHT_BAR_H

#include <ninja.h>
#include <sg_xpt.h>

/* The bar housing and its seven individually tinted lights. */

static NJS_TEXNAME light_bar_textures[4] = {
#include "shared/orbital/light_bar/light_bar_textures.inc"
};

static NJS_TEXLIST light_bar_texlist = {
#include "shared/orbital/light_bar/light_bar_texlist.inc"
};

static Sint16 light_bar_lamp_7_polygons[35] = {
#include "shared/orbital/light_bar/light_bar_lamp_7_polygons.inc"
};

static Sint32 light_bar_lamp_7_vertices[31] = {
#include "shared/orbital/light_bar/light_bar_lamp_7_vertices.inc"
};

static NJS_CNK_MODEL light_bar_lamp_7_mesh = {
#include "shared/orbital/light_bar/light_bar_lamp_7_mesh.inc"
};

static NJS_CNK_OBJECT light_bar_lamp_7 = {
#include "shared/orbital/light_bar/light_bar_lamp_7.inc"
};

static Sint16 light_bar_lamp_6_polygons[31] = {
#include "shared/orbital/light_bar/light_bar_lamp_6_polygons.inc"
};

static Sint32 light_bar_lamp_6_vertices[27] = {
#include "shared/orbital/light_bar/light_bar_lamp_6_vertices.inc"
};

static NJS_CNK_MODEL light_bar_lamp_6_mesh = {
#include "shared/orbital/light_bar/light_bar_lamp_6_mesh.inc"
};

static NJS_CNK_OBJECT light_bar_lamp_6 = {
#include "shared/orbital/light_bar/light_bar_lamp_6.inc"
};

static Sint16 light_bar_lamp_5_polygons[31] = {
#include "shared/orbital/light_bar/light_bar_lamp_5_polygons.inc"
};

static Sint32 light_bar_lamp_5_vertices[27] = {
#include "shared/orbital/light_bar/light_bar_lamp_5_vertices.inc"
};

static NJS_CNK_MODEL light_bar_lamp_5_mesh = {
#include "shared/orbital/light_bar/light_bar_lamp_5_mesh.inc"
};

static NJS_CNK_OBJECT light_bar_lamp_5 = {
#include "shared/orbital/light_bar/light_bar_lamp_5.inc"
};

static Sint16 light_bar_lamp_4_polygons[31] = {
#include "shared/orbital/light_bar/light_bar_lamp_4_polygons.inc"
};

static Sint32 light_bar_lamp_4_vertices[27] = {
#include "shared/orbital/light_bar/light_bar_lamp_4_vertices.inc"
};

static NJS_CNK_MODEL light_bar_lamp_4_mesh = {
#include "shared/orbital/light_bar/light_bar_lamp_4_mesh.inc"
};

static NJS_CNK_OBJECT light_bar_lamp_4 = {
#include "shared/orbital/light_bar/light_bar_lamp_4.inc"
};

static Sint16 light_bar_lamp_3_polygons[31] = {
#include "shared/orbital/light_bar/light_bar_lamp_3_polygons.inc"
};

static Sint32 light_bar_lamp_3_vertices[27] = {
#include "shared/orbital/light_bar/light_bar_lamp_3_vertices.inc"
};

static NJS_CNK_MODEL light_bar_lamp_3_mesh = {
#include "shared/orbital/light_bar/light_bar_lamp_3_mesh.inc"
};

static NJS_CNK_OBJECT light_bar_lamp_3 = {
#include "shared/orbital/light_bar/light_bar_lamp_3.inc"
};

static Sint16 light_bar_lamp_2_polygons[31] = {
#include "shared/orbital/light_bar/light_bar_lamp_2_polygons.inc"
};

static Sint32 light_bar_lamp_2_vertices[27] = {
#include "shared/orbital/light_bar/light_bar_lamp_2_vertices.inc"
};

static NJS_CNK_MODEL light_bar_lamp_2_mesh = {
#include "shared/orbital/light_bar/light_bar_lamp_2_mesh.inc"
};

static NJS_CNK_OBJECT light_bar_lamp_2 = {
#include "shared/orbital/light_bar/light_bar_lamp_2.inc"
};

static Sint16 light_bar_lamp_1_polygons[41] = {
#include "shared/orbital/light_bar/light_bar_lamp_1_polygons.inc"
};

static Sint32 light_bar_lamp_1_vertices[31] = {
#include "shared/orbital/light_bar/light_bar_lamp_1_vertices.inc"
};

static NJS_CNK_MODEL light_bar_lamp_1_mesh = {
#include "shared/orbital/light_bar/light_bar_lamp_1_mesh.inc"
};

static NJS_CNK_OBJECT light_bar_lamp_1 = {
#include "shared/orbital/light_bar/light_bar_lamp_1.inc"
};

static Sint16 light_bar_polygons[749] = {
#include "shared/orbital/light_bar/light_bar_polygons.inc"
};

static Sint32 light_bar_vertices[559] = {
#include "shared/orbital/light_bar/light_bar_vertices.inc"
};

static NJS_CNK_MODEL light_bar_mesh = {
#include "shared/orbital/light_bar/light_bar_mesh.inc"
};

static NJS_CNK_OBJECT light_bar_object = {
#include "shared/orbital/light_bar/light_bar_object.inc"
};

#endif
