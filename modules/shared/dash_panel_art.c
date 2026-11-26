#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "shared/dash_panel.h"

#include "shared/dash_panel_art/text.inc"

/* the base's texture name, then the animated model's on a four-byte boundary */
const char dash_panel_texture_filenames[29] = TEXT_1 TEXT_2;

/* the texture lists, chunk models and UV scrolls that 54 images place, from
 * the disc. its D precedes dash_panel.c */
static NJS_TEXNAME panel_texture[1] = {
#include "shared/dash_panel_art/panel_texture.inc"
};
NJS_TEXLIST dash_panel_texlist = {
#include "shared/dash_panel_art/dash_panel_texlist.inc"
};
static Sint16 panel_polygons[333] = {
#include "shared/dash_panel_art/panel_polygons.inc"
};
static Sint32 panel_vertices[207] = {
#include "shared/dash_panel_art/panel_vertices.inc"
};
NJS_CNK_MODEL dash_panel_model = {
#include "shared/dash_panel_art/dash_panel_model.inc"
};
NJS_CNK_OBJECT dash_panel_object = {
#include "shared/dash_panel_art/dash_panel_object.inc"
};
static NJS_TEXNAME belt_texture[1] = {
#include "shared/dash_panel_art/belt_texture.inc"
};
NJS_TEXLIST dash_panel_anim_texlist = {
#include "shared/dash_panel_art/dash_panel_anim_texlist.inc"
};
static Sint16 belt_polygons[43] = {
#include "shared/dash_panel_art/belt_polygons.inc"
};
static Sint32 belt_vertices[63] = {
#include "shared/dash_panel_art/belt_vertices.inc"
};
NJS_CNK_MODEL dash_panel_anim_model = {
#include "shared/dash_panel_art/dash_panel_anim_model.inc"
};
NJS_CNK_OBJECT dash_panel_anim_object = {
#include "shared/dash_panel_art/dash_panel_anim_object.inc"
};
static Sint16 belt_uvs[24] = {
#include "shared/dash_panel_art/belt_uvs.inc"
};
struct texture_anim dash_panel_texture_anim = {
#include "shared/dash_panel_art/dash_panel_texture_anim.inc"
};
