/* the two skies' models and the background's texture lists. bgtex24 is the
 * PVM that texture_load_shared loads for the background. each list takes its
 * textures from it by name */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg24/scenery.h"
#include "system/texture.h"

#include "stg24/sky_tex/text.inc"

static NJS_TEXNAME engine_background_archive_names[20] = {
#include "stg24/sky_tex/engine_background_archive_names.inc"
};

NJS_TEXLIST engine_background_archive = {
#include "stg24/sky_tex/engine_background_archive.inc"
};

static NJS_TEXNAME engine_sky_texlist_names[2] = {
#include "stg24/sky_tex/engine_sky_texlist_names.inc"
};

NJS_TEXLIST engine_sky_texlist = {
#include "stg24/sky_tex/engine_sky_texlist.inc"
};

static Sint16 engine_outer_sky_polygons[925] = {
#include "stg24/sky_tex/engine_outer_sky_polygons.inc"
};

static Sint32 engine_outer_sky_vertices[463] = {
#include "stg24/sky_tex/engine_outer_sky_vertices.inc"
};

static NJS_CNK_MODEL engine_outer_sky_model = {
#include "stg24/sky_tex/engine_outer_sky_model.inc"
};

NJS_CNK_OBJECT engine_outer_sky = {
#include "stg24/sky_tex/engine_outer_sky.inc"
};

static NJS_TEXNAME inner_sky_textures_names[2] = {
#include "stg24/sky_tex/inner_sky_textures_names.inc"
};

static NJS_TEXLIST inner_sky_textures = {
#include "stg24/sky_tex/inner_sky_textures.inc"
};

static Sint16 engine_inner_sky_polygons[925] = {
#include "stg24/sky_tex/engine_inner_sky_polygons.inc"
};

static Sint32 engine_inner_sky_vertices[463] = {
#include "stg24/sky_tex/engine_inner_sky_vertices.inc"
};

static NJS_CNK_MODEL engine_inner_sky_model = {
#include "stg24/sky_tex/engine_inner_sky_model.inc"
};

NJS_CNK_OBJECT engine_inner_sky = {
#include "stg24/sky_tex/engine_inner_sky.inc"
};

static NJS_TEXLIST *sky_textures[5] = {
#include "stg24/sky_tex/sky_textures.inc"
};

NJS_TEXLIST **engine_bg_groups[2] = {
#include "stg24/sky_tex/engine_bg_groups.inc"
};

struct texture_pvm engine_bg_pvms[2] = {
#include "stg24/sky_tex/engine_bg_pvms.inc"
};
