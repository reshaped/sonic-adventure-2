#ifndef STG07_SCENERY_LAMP_H
#define STG07_SCENERY_LAMP_H

/* Lamp. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME mine_lamp_texlist_names[4] = {
#include "stg07/scenery/lamp/mine_lamp_texlist_names.inc"
};

static NJS_TEXLIST mine_lamp_texlist = {
#include "stg07/scenery/lamp/mine_lamp_texlist.inc"
};

static Sint16 mine_lamp_object_child_polygons[25] = {
#include "stg07/scenery/lamp/mine_lamp_object_child_polygons.inc"
};

static Sint32 mine_lamp_object_child_vertices[19] = {
#include "stg07/scenery/lamp/mine_lamp_object_child_vertices.inc"
};

static NJS_CNK_MODEL mine_lamp_object_child_model = {
#include "stg07/scenery/lamp/mine_lamp_object_child_model.inc"
};

static NJS_CNK_OBJECT mine_lamp_object_child = {
#include "stg07/scenery/lamp/mine_lamp_object_child.inc"
};

static Sint16 mine_lamp_polygons[529] = {
#include "stg07/scenery/lamp/mine_lamp_polygons.inc"
};

static Sint32 mine_lamp_vertices[299] = {
#include "stg07/scenery/lamp/mine_lamp_vertices.inc"
};

static NJS_CNK_MODEL mine_lamp_model = {
#include "stg07/scenery/lamp/mine_lamp_model.inc"
};

static NJS_CNK_OBJECT mine_lamp_object[1] = {
#include "stg07/scenery/lamp/mine_lamp_object.inc"
};

static NJS_TEXNAME mine_lamp_glow_texlist_names[16] = {
#include "stg07/scenery/lamp/mine_lamp_glow_texlist_names.inc"
};

static NJS_TEXLIST mine_lamp_glow_texlist = {
#include "stg07/scenery/lamp/mine_lamp_glow_texlist.inc"
};

#endif
