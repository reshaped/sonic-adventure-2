/* Ring decoration and its scrolling texture strips. */

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "shared/ringkaz.h"

#include "stg40/ringkaz_textures/text.inc"

static NJS_TEXNAME ringkaz_textures[6] = {
#include "stg40/ringkaz_textures/ringkaz_textures.inc"
};

NJS_TEXLIST ringkaz_texlist = {
#include "stg40/ringkaz_textures/ringkaz_texlist.inc"
};

static Sint16 ringkaz_scroll_polygons[91] = {
#include "stg40/ringkaz_textures/ringkaz_scroll_polygons.inc"
};

static Sint32 ringkaz_scroll_vertices[75] = {
#include "stg40/ringkaz_textures/ringkaz_scroll_vertices.inc"
};

static NJS_CNK_MODEL ringkaz_scroll_mesh = {
#include "stg40/ringkaz_textures/ringkaz_scroll_mesh.inc"
};

static NJS_CNK_OBJECT ringkaz_scroll = {
#include "stg40/ringkaz_textures/ringkaz_scroll.inc"
};

static Sint16 ringkaz_polygons[745] = {
#include "stg40/ringkaz_textures/ringkaz_polygons.inc"
};

static Sint32 ringkaz_vertices[711] = {
#include "stg40/ringkaz_textures/ringkaz_vertices.inc"
};

static NJS_CNK_MODEL ringkaz_mesh = {
#include "stg40/ringkaz_textures/ringkaz_mesh.inc"
};

NJS_CNK_OBJECT ringkaz_object = {
#include "stg40/ringkaz_textures/ringkaz_object.inc"
};

/* Polygon strip offsets, vertex counts and base texture coordinates. */
static Sint16 ringkaz_uvs[62] = {
#include "stg40/ringkaz_textures/ringkaz_uvs.inc"
};

struct texture_anim ringkaz_anim = {
#include "stg40/ringkaz_textures/ringkaz_anim.inc"
};
