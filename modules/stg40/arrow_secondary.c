/* Secondary guidance arrow. Its texture list shares four names with the primary
 * arrow. */

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/arrow.h"
#include "shared/arrow_textures.h"

static NJS_TEXNAME textures[4] = {
#include "stg40/arrow_secondary/textures.inc"
};

NJS_TEXLIST arrow_2_texlist = {
#include "stg40/arrow_secondary/arrow_2_texlist.inc"
};

static Sint16 arrow_2_light_1_polygons[57] = {
#include "stg40/arrow_secondary/arrow_2_light_1_polygons.inc"
};

static Sint32 arrow_2_light_1_vertices[35] = {
#include "stg40/arrow_secondary/arrow_2_light_1_vertices.inc"
};

static NJS_CNK_MODEL arrow_2_light_1_mesh = {
#include "stg40/arrow_secondary/arrow_2_light_1_mesh.inc"
};

static NJS_CNK_OBJECT arrow_2_light_1 = {
#include "stg40/arrow_secondary/arrow_2_light_1.inc"
};

static Sint16 arrow_2_polygons[141] = {
#include "stg40/arrow_secondary/arrow_2_polygons.inc"
};

static Sint32 arrow_2_vertices[83] = {
#include "stg40/arrow_secondary/arrow_2_vertices.inc"
};

static NJS_CNK_MODEL arrow_2_mesh = {
#include "stg40/arrow_secondary/arrow_2_mesh.inc"
};

NJS_CNK_OBJECT arrow_2_object = {
#include "stg40/arrow_secondary/arrow_2_object.inc"
};
