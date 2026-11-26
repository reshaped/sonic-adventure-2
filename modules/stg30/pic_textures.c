/* Orbital display, its pulsing and scrolling surfaces, and the strip UVs used
 * by the scroll. */

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "shared/pic.h"

#include "stg30/pic_textures/text.inc"

static NJS_TEXNAME pic_textures[8] = {
#include "stg30/pic_textures/pic_textures.inc"
};

NJS_TEXLIST pic_texlist = {
#include "stg30/pic_textures/pic_texlist.inc"
};

static Sint16 pic_scroll_polygons[37] = {
#include "stg30/pic_textures/pic_scroll_polygons.inc"
};

static Sint32 pic_scroll_vertices[35] = {
#include "stg30/pic_textures/pic_scroll_vertices.inc"
};

static NJS_CNK_MODEL pic_scroll_mesh = {
#include "stg30/pic_textures/pic_scroll_mesh.inc"
};

NJS_CNK_OBJECT pic_scroll_object[1] = {
#include "stg30/pic_textures/pic_scroll_object.inc"
};

static Sint16 pic_pulse_polygons[121] = {
#include "stg30/pic_textures/pic_pulse_polygons.inc"
};

static Sint32 pic_pulse_vertices[67] = {
#include "stg30/pic_textures/pic_pulse_vertices.inc"
};

static NJS_CNK_MODEL pic_pulse_mesh = {
#include "stg30/pic_textures/pic_pulse_mesh.inc"
};

NJS_CNK_OBJECT pic_pulse_object[1] = {
#include "stg30/pic_textures/pic_pulse_object.inc"
};

static Sint16 pic_polygons[397] = {
#include "stg30/pic_textures/pic_polygons.inc"
};

static Sint32 pic_vertices[327] = {
#include "stg30/pic_textures/pic_vertices.inc"
};

static NJS_CNK_MODEL pic_mesh = {
#include "stg30/pic_textures/pic_mesh.inc"
};

NJS_CNK_OBJECT pic_object[1] = {
#include "stg30/pic_textures/pic_object.inc"
};

/* Polygon strip offsets, vertex counts and base texture coordinates. */
static Sint16 pic_uvs[20] = {
#include "stg30/pic_textures/pic_uvs.inc"
};

struct texture_anim pic_anim = {
#include "stg30/pic_textures/pic_anim.inc"
};
