/* The three fire pots and their flames. */
#ifndef STG25_FIRE_POT_ART_H
#define STG25_FIRE_POT_ART_H

#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg25/prop_stage.h"

#include "stg25/chamber_color.h"

static NJS_TEXNAME fire_pot_blue_texture_names[2] = {
#include "stg25/fire_pot_art/fire_pot_blue_texture_names.inc"
};

static NJS_TEXLIST fire_pot_blue_texlist = {
#include "stg25/fire_pot_art/fire_pot_blue_texlist.inc"
};

static Sint16 fire_pot_blue_polygons[519] = {
#include "stg25/fire_pot_art/fire_pot_blue_polygons.inc"
};

static Sint32 fire_pot_blue_vertices[279] = {
#include "stg25/fire_pot_art/fire_pot_blue_vertices.inc"
};

static NJS_CNK_MODEL fire_pot_blue_model = {
#include "stg25/fire_pot_art/fire_pot_blue_model.inc"
};

static NJS_CNK_OBJECT fire_pot_blue_object = {
#include "stg25/fire_pot_art/fire_pot_blue_object.inc"
};

static NJS_TEXNAME fire_pot_green_texture_names[2] = {
#include "stg25/fire_pot_art/fire_pot_green_texture_names.inc"
};

static NJS_TEXLIST fire_pot_green_texlist = {
#include "stg25/fire_pot_art/fire_pot_green_texlist.inc"
};

static Sint16 fire_pot_green_polygons[519] = {
#include "stg25/fire_pot_art/fire_pot_green_polygons.inc"
};

static Sint32 fire_pot_green_vertices[279] = {
#include "stg25/fire_pot_art/fire_pot_green_vertices.inc"
};

static NJS_CNK_MODEL fire_pot_green_model = {
#include "stg25/fire_pot_art/fire_pot_green_model.inc"
};

static NJS_CNK_OBJECT fire_pot_green_object = {
#include "stg25/fire_pot_art/fire_pot_green_object.inc"
};

static NJS_TEXNAME fire_pot_red_texture_names[2] = {
#include "stg25/fire_pot_art/fire_pot_red_texture_names.inc"
};

static NJS_TEXLIST fire_pot_red_texlist = {
#include "stg25/fire_pot_art/fire_pot_red_texlist.inc"
};

static Sint16 fire_pot_red_polygons[519] = {
#include "stg25/fire_pot_art/fire_pot_red_polygons.inc"
};

static Sint32 fire_pot_red_vertices[279] = {
#include "stg25/fire_pot_art/fire_pot_red_vertices.inc"
};

static NJS_CNK_MODEL fire_pot_red_model = {
#include "stg25/fire_pot_art/fire_pot_red_model.inc"
};

static NJS_CNK_OBJECT fire_pot_red_object = {
#include "stg25/fire_pot_art/fire_pot_red_object.inc"
};

static NJS_TEXNAME fire_pot_blue_flame_texture_names[1] = {
#include "stg25/fire_pot_art/fire_pot_blue_flame_texture_names.inc"
};

static NJS_TEXLIST fire_pot_blue_flame_texlist = {
#include "stg25/fire_pot_art/fire_pot_blue_flame_texlist.inc"
};

static Sint16 fire_pot_blue_flame_polygons[25] = {
#include "stg25/fire_pot_art/fire_pot_blue_flame_polygons.inc"
};

static Sint32 fire_pot_blue_flame_vertices[19] = {
#include "stg25/fire_pot_art/fire_pot_blue_flame_vertices.inc"
};

static NJS_CNK_MODEL fire_pot_blue_flame_model = {
#include "stg25/fire_pot_art/fire_pot_blue_flame_model.inc"
};

static NJS_CNK_OBJECT fire_pot_blue_flame_object = {
#include "stg25/fire_pot_art/fire_pot_blue_flame_object.inc"
};

static NJS_TEXNAME fire_pot_green_flame_texture_names[1] = {
#include "stg25/fire_pot_art/fire_pot_green_flame_texture_names.inc"
};

static NJS_TEXLIST fire_pot_green_flame_texlist = {
#include "stg25/fire_pot_art/fire_pot_green_flame_texlist.inc"
};

static Sint16 fire_pot_green_flame_polygons[25] = {
#include "stg25/fire_pot_art/fire_pot_green_flame_polygons.inc"
};

static Sint32 fire_pot_green_flame_vertices[19] = {
#include "stg25/fire_pot_art/fire_pot_green_flame_vertices.inc"
};

static NJS_CNK_MODEL fire_pot_green_flame_model = {
#include "stg25/fire_pot_art/fire_pot_green_flame_model.inc"
};

static NJS_CNK_OBJECT fire_pot_green_flame_object = {
#include "stg25/fire_pot_art/fire_pot_green_flame_object.inc"
};

static NJS_TEXNAME fire_pot_red_flame_texture_names[1] = {
#include "stg25/fire_pot_art/fire_pot_red_flame_texture_names.inc"
};

static NJS_TEXLIST fire_pot_red_flame_texlist = {
#include "stg25/fire_pot_art/fire_pot_red_flame_texlist.inc"
};

static Sint16 fire_pot_red_flame_polygons[25] = {
#include "stg25/fire_pot_art/fire_pot_red_flame_polygons.inc"
};

static Sint32 fire_pot_red_flame_vertices[19] = {
#include "stg25/fire_pot_art/fire_pot_red_flame_vertices.inc"
};

static NJS_CNK_MODEL fire_pot_red_flame_model = {
#include "stg25/fire_pot_art/fire_pot_red_flame_model.inc"
};

static NJS_CNK_OBJECT fire_pot_red_flame_object = {
#include "stg25/fire_pot_art/fire_pot_red_flame_object.inc"
};

static NJS_CNK_MODEL *chamber_fire_pot_models[CHAMBER_COLORS] = {
#include "stg25/fire_pot_art/chamber_fire_pot_models.inc"
};

static NJS_CNK_MODEL *chamber_fire_pot_flame_models[CHAMBER_COLORS] = {
#include "stg25/fire_pot_art/chamber_fire_pot_flame_models.inc"
};

#endif
