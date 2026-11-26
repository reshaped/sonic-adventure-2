#include <ninja.h>
#include <sg_xpt.h>

#include "stg26/propellers.h"
#include "stg26/propeller_captions.h" /* IWYU pragma: keep -- the data it defines here */

static const char hub_material[20] = {
#include "stg26/propeller2/hub_material.inc"
};
static const char blade_material[11] = {
#include "stg26/propeller2/blade_material.inc"
};

static NJS_TEXNAME red_textures[2] = {
#include "stg26/propeller2/red_textures.inc"
};

static NJS_TEXLIST red_texlist = {
#include "stg26/propeller2/red_texlist.inc"
};

static Sint16 red_polygons[397] = {
#include "stg26/propeller2/red_polygons.inc"
};

static Sint32 red_vertices[367] = {
#include "stg26/propeller2/red_vertices.inc"
};

static NJS_CNK_MODEL red_model = {
#include "stg26/propeller2/red_model.inc"
};

static NJS_CNK_OBJECT red_object = {
#include "stg26/propeller2/red_object.inc"
};

static NJS_TEXNAME green_textures[2] = {
#include "stg26/propeller2/green_textures.inc"
};

static NJS_TEXLIST green_texlist = {
#include "stg26/propeller2/green_texlist.inc"
};

static Sint16 green_polygons[397] = {
#include "stg26/propeller2/green_polygons.inc"
};

static Sint32 green_vertices[367] = {
#include "stg26/propeller2/green_vertices.inc"
};

static NJS_CNK_MODEL green_model = {
#include "stg26/propeller2/green_model.inc"
};

static NJS_CNK_OBJECT green_object = {
#include "stg26/propeller2/green_object.inc"
};

static NJS_TEXNAME blue_textures[2] = {
#include "stg26/propeller2/blue_textures.inc"
};

static NJS_TEXLIST blue_texlist = {
#include "stg26/propeller2/blue_texlist.inc"
};

static Sint16 blue_polygons[397] = {
#include "stg26/propeller2/blue_polygons.inc"
};

static Sint32 blue_vertices[367] = {
#include "stg26/propeller2/blue_vertices.inc"
};

static NJS_CNK_MODEL blue_model = {
#include "stg26/propeller2/blue_model.inc"
};

static NJS_CNK_OBJECT blue_object = {
#include "stg26/propeller2/blue_object.inc"
};

static NJS_CNK_MODEL *models[PROPELLER_COLORS] = {
#include "stg26/propeller2/models.inc"
};
NJS_TEXLIST *quarters_propeller2_texlists[PROPELLER_COLORS + 1] = {
#include "stg26/propeller2/quarters_propeller2_texlists.inc"
};

#define PROPELLER(name) quarters_propeller2_##name
#define SPHERE_PROPELLER

#include "stg26/propeller_impl.h" /* IWYU pragma: keep -- the code it compiles here */
