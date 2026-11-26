/* the models of ROCK and KAGE, the shadow. both draw with
 * green_hill_rock_texlist's one texture */
#include "stg31/prop_art.h"
#include <ninja.h>
#include <sg_xpt.h>

static Sint16 rock_polygons[367] = {
#include "stg31/rock_art/rock_polygons.inc"
};

static Sint32 rock_vertices[247] = {
#include "stg31/rock_art/rock_vertices.inc"
};

static NJS_CNK_MODEL rock_model = {
#include "stg31/rock_art/rock_model.inc"
};

NJS_CNK_OBJECT green_hill_rock_object[1] = {
#include "stg31/rock_art/green_hill_rock_object.inc"
};
static Sint16 shadow_polygons[85] = {
#include "stg31/rock_art/shadow_polygons.inc"
};

static Sint32 shadow_vertices[167] = {
#include "stg31/rock_art/shadow_vertices.inc"
};

static NJS_CNK_MODEL shadow_model = {
#include "stg31/rock_art/shadow_model.inc"
};

NJS_CNK_OBJECT green_hill_shadow_object[1] = {
#include "stg31/rock_art/green_hill_shadow_object.inc"
};
