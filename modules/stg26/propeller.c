/* PROPELLER in Egg Quarters' object list: its red, green and blue models and
 * their texture lists. propeller_impl.h holds its code, which propeller2.c
 * builds again as PROPELLER2 */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg26/propellers.h"
#include "stg26/propeller_captions.h" /* IWYU pragma: keep -- the data it defines here */

static const char hub_material[20] = {
#include "stg26/propeller/hub_material.inc"
};
static const char blade_material[11] = {
#include "stg26/propeller/blade_material.inc"
};

static NJS_TEXNAME red_textures[2] = {
#include "stg26/propeller/red_textures.inc"
};

static NJS_TEXLIST red_texlist = {
#include "stg26/propeller/red_texlist.inc"
};

static Sint16 red_polygons[473] = {
#include "stg26/propeller/red_polygons.inc"
};

static Sint32 red_vertices[415] = {
#include "stg26/propeller/red_vertices.inc"
};

static NJS_CNK_MODEL red_model = {
#include "stg26/propeller/red_model.inc"
};

static NJS_CNK_OBJECT red_object = {
#include "stg26/propeller/red_object.inc"
};

static NJS_TEXNAME green_textures[2] = {
#include "stg26/propeller/green_textures.inc"
};

static NJS_TEXLIST green_texlist = {
#include "stg26/propeller/green_texlist.inc"
};

static Sint16 green_polygons[473] = {
#include "stg26/propeller/green_polygons.inc"
};

static Sint32 green_vertices[415] = {
#include "stg26/propeller/green_vertices.inc"
};

static NJS_CNK_MODEL green_model = {
#include "stg26/propeller/green_model.inc"
};

static NJS_CNK_OBJECT green_object = {
#include "stg26/propeller/green_object.inc"
};

static NJS_TEXNAME blue_textures[2] = {
#include "stg26/propeller/blue_textures.inc"
};

static NJS_TEXLIST blue_texlist = {
#include "stg26/propeller/blue_texlist.inc"
};

static Sint16 blue_polygons[473] = {
#include "stg26/propeller/blue_polygons.inc"
};

static Sint32 blue_vertices[415] = {
#include "stg26/propeller/blue_vertices.inc"
};

static NJS_CNK_MODEL blue_model = {
#include "stg26/propeller/blue_model.inc"
};

static NJS_CNK_OBJECT blue_object = {
#include "stg26/propeller/blue_object.inc"
};

static NJS_CNK_MODEL *models[PROPELLER_COLORS] = {
#include "stg26/propeller/models.inc"
};
NJS_TEXLIST *quarters_propeller_texlists[PROPELLER_COLORS + 1] = {
#include "stg26/propeller/quarters_propeller_texlists.inc"
};

#define PROPELLER(name) quarters_propeller_##name

#include "stg26/propeller_impl.h" /* IWYU pragma: keep -- the code it compiles here */
