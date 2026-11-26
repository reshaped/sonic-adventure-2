/* the model that rides the Egg Golem's head, and a scroll of its UVs. no code
 * applies the scroll */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "shared/golem_material/names.h"
#include "boss_golem/attachment_art.h"

static NJS_TEXNAME head_model_textures[5] = {
#include "boss_golem/head_cap_art/head_model_textures.inc"
};

NJS_TEXLIST golem_head_model_texlist = {
#include "boss_golem/head_cap_art/golem_head_model_texlist.inc"
};

static Sint16 head_model_polygons[412] = {
#include "boss_golem/head_cap_art/head_model_polygons.inc"
};

static Sint32 head_model_vertices[297] = {
#include "boss_golem/head_cap_art/head_model_vertices.inc"
};

static NJS_CNK_MODEL head_model_model = {
#include "boss_golem/head_cap_art/head_model_model.inc"
};

NJS_CNK_OBJECT golem_head_model_object[1] = {
#include "boss_golem/head_cap_art/golem_head_model_object.inc"
};

/* Polygon-list index, vertex count and the strip's UV pairs. */
static Sint16 head_model_uvs[40] = {
#include "boss_golem/head_cap_art/head_model_uvs.inc"
};

/* SHC retains this unused scroll and its UV keys in the original member. */
static struct texture_anim head_cap_scroll = {
#include "boss_golem/head_cap_art/head_cap_scroll.inc"
};
