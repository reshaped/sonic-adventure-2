#include <ninja.h>
#include <sg_xpt.h>

#include "stg27/red_lamps.h"

static NJS_TEXNAME textures[1] = {
#include "stg27/red_lamp_b_tex/textures.inc"
};
NJS_TEXLIST red_lamp_b_texlist = {
#include "stg27/red_lamp_b_tex/red_lamp_b_texlist.inc"
};

/* Lamp housing and its separately tinted lens. */

static Sint16 red_lamp_b_lens_polygons[91] = {
#include "stg27/red_lamp_b_tex/red_lamp_b_lens_polygons.inc"
};

static Sint32 red_lamp_b_lens_vertices[51] = {
#include "stg27/red_lamp_b_tex/red_lamp_b_lens_vertices.inc"
};

static NJS_CNK_MODEL red_lamp_b_lens_model = {
#include "stg27/red_lamp_b_tex/red_lamp_b_lens_model.inc"
};

static NJS_CNK_OBJECT red_lamp_b_lens_object = {
#include "stg27/red_lamp_b_tex/red_lamp_b_lens_object.inc"
};

static Sint16 red_lamp_b_polygons[103] = {
#include "stg27/red_lamp_b_tex/red_lamp_b_polygons.inc"
};

static Sint32 red_lamp_b_vertices[67] = {
#include "stg27/red_lamp_b_tex/red_lamp_b_vertices.inc"
};

static NJS_CNK_MODEL red_lamp_b_model = {
#include "stg27/red_lamp_b_tex/red_lamp_b_model.inc"
};

NJS_CNK_OBJECT red_lamp_b_object[1] = {
#include "stg27/red_lamp_b_tex/red_lamp_b_object.inc"
};
