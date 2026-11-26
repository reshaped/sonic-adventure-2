#ifndef STG43_FOREGROUND_INDICATOR_H
#define STG43_FOREGROUND_INDICATOR_H

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"

static NJS_TEXNAME indicator_textures[3] = {
#include "stg43/foreground/indicator/indicator_textures.inc"
};

static NJS_TEXLIST indicator_texlist = {
#include "stg43/foreground/indicator/indicator_texlist.inc"
};

static Sint16 indicator_panel_polygons[39] = {
#include "stg43/foreground/indicator/indicator_panel_polygons.inc"
};

static Sint32 indicator_panel_vertices[35] = {
#include "stg43/foreground/indicator/indicator_panel_vertices.inc"
};

static NJS_CNK_MODEL indicator_panel_model = {
#include "stg43/foreground/indicator/indicator_panel_model.inc"
};

static NJS_CNK_OBJECT indicator_panel[1] = {
#include "stg43/foreground/indicator/indicator_panel.inc"
};

static Sint16 indicator_polygons[501] = {
#include "stg43/foreground/indicator/indicator_polygons.inc"
};

static Sint32 indicator_vertices[455] = {
#include "stg43/foreground/indicator/indicator_vertices.inc"
};

static NJS_CNK_MODEL indicator_model = {
#include "stg43/foreground/indicator/indicator_model.inc"
};

static NJS_CNK_OBJECT indicator_object = {
#include "stg43/foreground/indicator/indicator_object.inc"
};

static NJS_TEXNAME indicator_2_textures[3] = {
#include "stg43/foreground/indicator/indicator_2_textures.inc"
};

static NJS_TEXLIST indicator_2_texlist = {
#include "stg43/foreground/indicator/indicator_2_texlist.inc"
};

static Sint16 indicator_2_panel_polygons[39] = {
#include "stg43/foreground/indicator/indicator_2_panel_polygons.inc"
};

static Sint32 indicator_2_panel_vertices[35] = {
#include "stg43/foreground/indicator/indicator_2_panel_vertices.inc"
};

static NJS_CNK_MODEL indicator_2_panel_model = {
#include "stg43/foreground/indicator/indicator_2_panel_model.inc"
};

static NJS_CNK_OBJECT indicator_2_panel[1] = {
#include "stg43/foreground/indicator/indicator_2_panel.inc"
};

static Sint16 indicator_2_polygons[501] = {
#include "stg43/foreground/indicator/indicator_2_polygons.inc"
};

static Sint32 indicator_2_vertices[455] = {
#include "stg43/foreground/indicator/indicator_2_vertices.inc"
};

static NJS_CNK_MODEL indicator_2_model = {
#include "stg43/foreground/indicator/indicator_2_model.inc"
};

static NJS_CNK_OBJECT indicator_2_object = {
#include "stg43/foreground/indicator/indicator_2_object.inc"
};

static Sint16 indicator_scroll_uvs[22] = {
#include "stg43/foreground/indicator/indicator_scroll_uvs.inc"
};

static struct texture_anim indicator_scroll = {
#include "stg43/foreground/indicator/indicator_scroll.inc"
};

static Sint16 indicator_2_scroll_uvs[22] = {
#include "stg43/foreground/indicator/indicator_2_scroll_uvs.inc"
};

static struct texture_anim indicator_2_scroll = {
#include "stg43/foreground/indicator/indicator_2_scroll.inc"
};

#endif
