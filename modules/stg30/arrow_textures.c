/* Primary guidance arrow and its two lamps. */

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/arrow.h"
#include "shared/arrow_textures.h"

const char arrow_texture_0[12] = {
#include "stg30/arrow_textures/arrow_texture_0.inc"
};

const char arrow_texture_1[12] = {
#include "stg30/arrow_textures/arrow_texture_1.inc"
};

const char arrow_texture_2[12] = {
#include "stg30/arrow_textures/arrow_texture_2.inc"
};

static const char first_only_texture[12] = {
#include "stg30/arrow_textures/first_only_texture.inc"
};

const char arrow_last_texture[12] = {
#include "stg30/arrow_textures/arrow_last_texture.inc"
};

static NJS_TEXNAME textures[5] = {
#include "stg30/arrow_textures/textures.inc"
};

NJS_TEXLIST arrow_texlist = {
#include "stg30/arrow_textures/arrow_texlist.inc"
};

static Sint16 arrow_light_2_polygons[57] = {
#include "stg30/arrow_textures/arrow_light_2_polygons.inc"
};

static Sint32 arrow_light_2_vertices[35] = {
#include "stg30/arrow_textures/arrow_light_2_vertices.inc"
};

static NJS_CNK_MODEL arrow_light_2_mesh = {
#include "stg30/arrow_textures/arrow_light_2_mesh.inc"
};

static NJS_CNK_OBJECT arrow_light_2 = {
#include "stg30/arrow_textures/arrow_light_2.inc"
};

static Sint16 arrow_light_1_polygons[57] = {
#include "stg30/arrow_textures/arrow_light_1_polygons.inc"
};

static Sint32 arrow_light_1_vertices[35] = {
#include "stg30/arrow_textures/arrow_light_1_vertices.inc"
};

static NJS_CNK_MODEL arrow_light_1_mesh = {
#include "stg30/arrow_textures/arrow_light_1_mesh.inc"
};

static NJS_CNK_OBJECT arrow_light_1 = {
#include "stg30/arrow_textures/arrow_light_1.inc"
};

static Sint16 arrow_polygons[397] = {
#include "stg30/arrow_textures/arrow_polygons.inc"
};

static Sint32 arrow_vertices[315] = {
#include "stg30/arrow_textures/arrow_vertices.inc"
};

static NJS_CNK_MODEL arrow_mesh = {
#include "stg30/arrow_textures/arrow_mesh.inc"
};

NJS_CNK_OBJECT arrow_object = {
#include "stg30/arrow_textures/arrow_object.inc"
};
