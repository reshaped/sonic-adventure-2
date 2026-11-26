#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_shutter.h"
#include "shared/shutter_art.h"
#include "shared/shutter_textures/text.inc"

static NJS_TEXNAME textures[9] = {
#include "shared/shutter_textures/textures.inc"
};
NJS_TEXLIST core_shutter_texlist = {
#include "shared/shutter_textures/core_shutter_texlist.inc"
};

static Sint16 core_shutter_lamp_polygons[25] = {
#include "shared/shutter_textures/core_shutter_lamp_polygons.inc"
};

static Sint32 core_shutter_lamp_vertices[19] = {
#include "shared/shutter_textures/core_shutter_lamp_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_lamp_model = {
#include "shared/shutter_textures/core_shutter_lamp_model.inc"
};

NJS_CNK_OBJECT core_shutter_lamp[1] = {
#include "shared/shutter_textures/core_shutter_lamp.inc"
};

static Sint16 core_shutter_right_polygons[295] = {
#include "shared/shutter_textures/core_shutter_right_polygons.inc"
};

static Sint32 core_shutter_right_vertices[231] = {
#include "shared/shutter_textures/core_shutter_right_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_right_model = {
#include "shared/shutter_textures/core_shutter_right_model.inc"
};

NJS_CNK_OBJECT core_shutter_right[1] = {
#include "shared/shutter_textures/core_shutter_right.inc"
};

static Sint16 core_shutter_left_polygons[161] = {
#include "shared/shutter_textures/core_shutter_left_polygons.inc"
};

static Sint32 core_shutter_left_vertices[155] = {
#include "shared/shutter_textures/core_shutter_left_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_left_model = {
#include "shared/shutter_textures/core_shutter_left_model.inc"
};

NJS_CNK_OBJECT core_shutter_left[1] = {
#include "shared/shutter_textures/core_shutter_left.inc"
};

static Sint16 core_shutter_closed_lamp_polygons[15] = {
#include "shared/shutter_textures/core_shutter_closed_lamp_polygons.inc"
};

static Sint32 core_shutter_closed_lamp_vertices[19] = {
#include "shared/shutter_textures/core_shutter_closed_lamp_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_closed_lamp_model = {
#include "shared/shutter_textures/core_shutter_closed_lamp_model.inc"
};

NJS_CNK_OBJECT core_shutter_closed_lamp[1] = {
#include "shared/shutter_textures/core_shutter_closed_lamp.inc"
};

static Sint16 core_shutter_top_polygons[333] = {
#include "shared/shutter_textures/core_shutter_top_polygons.inc"
};

static Sint32 core_shutter_top_vertices[299] = {
#include "shared/shutter_textures/core_shutter_top_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_top_model = {
#include "shared/shutter_textures/core_shutter_top_model.inc"
};

NJS_CNK_OBJECT core_shutter_top[1] = {
#include "shared/shutter_textures/core_shutter_top.inc"
};

static Sint16 core_shutter_latch_polygons[237] = {
#include "shared/shutter_textures/core_shutter_latch_polygons.inc"
};

static Sint32 core_shutter_latch_vertices[131] = {
#include "shared/shutter_textures/core_shutter_latch_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_latch_model = {
#include "shared/shutter_textures/core_shutter_latch_model.inc"
};

NJS_CNK_OBJECT core_shutter_latch[1] = {
#include "shared/shutter_textures/core_shutter_latch.inc"
};

static Sint16 core_shutter_open_lamp_polygons[27] = {
#include "shared/shutter_textures/core_shutter_open_lamp_polygons.inc"
};

static Sint32 core_shutter_open_lamp_vertices[51] = {
#include "shared/shutter_textures/core_shutter_open_lamp_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_open_lamp_model = {
#include "shared/shutter_textures/core_shutter_open_lamp_model.inc"
};

NJS_CNK_OBJECT core_shutter_open_lamp[1] = {
#include "shared/shutter_textures/core_shutter_open_lamp.inc"
};

static NJS_CNK_OBJECT shutter_near_assembly = {
#include "shared/shutter_textures/shutter_near_assembly.inc"
};
