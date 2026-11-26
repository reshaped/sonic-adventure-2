#include <ninja.h>
#include <sg_xpt.h>

#include "shared/colony_shutter_textures.h"
#include "shared/core_shutter.h"
#include "shared/shutter_art.h"

/* the names that the shutter's texture lists point at. each size rounds its
 * name up to whole words, so what follows starts on a word boundary */
static const char miu128_cg004[16] = {
#include "shared/shutter_panel_tex/miu128_cg004.inc"
};
static const char miu128_cg008[16] = {
#include "shared/shutter_panel_tex/miu128_cg008.inc"
};
static const char miu128_cg016[16] = {
#include "shared/shutter_panel_tex/miu128_cg016.inc"
};
static const char miu128_cg022[16] = {
#include "shared/shutter_panel_tex/miu128_cg022.inc"
};
const char shutter_miu256_cg004[16] = "miu256_cg004";
const char shutter_miu256_cg005[16] = "miu256_cg005";
const char shutter_miu256_cg006[16] = "miu256_cg006";
const char shutter_miu32_cg001[12] = "miu32_cg001";
const char shutter_miu64_cg009[12] = "miu64_cg009";
const char shutter_miu64_cg019[12] = "miu64_cg019";
static const char miu64_cg021[12] = {
#include "shared/shutter_panel_tex/miu64_cg021.inc"
};

static NJS_TEXNAME textures[11] = {
#include "shared/shutter_panel_tex/textures.inc"
};

NJS_TEXLIST core_shutter_texlist = {
#include "shared/shutter_panel_tex/core_shutter_texlist.inc"
};

static Sint16 core_shutter_lamp_polygons[25] = {
#include "shared/shutter_panel_tex/core_shutter_lamp_polygons.inc"
};

static Sint32 core_shutter_lamp_vertices[19] = {
#include "shared/shutter_panel_tex/core_shutter_lamp_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_lamp_model = {
#include "shared/shutter_panel_tex/core_shutter_lamp_model.inc"
};

NJS_CNK_OBJECT core_shutter_lamp[1] = {
#include "shared/shutter_panel_tex/core_shutter_lamp.inc"
};

static Sint16 core_shutter_right_polygons[295] = {
#include "shared/shutter_panel_tex/core_shutter_right_polygons.inc"
};

static Sint32 core_shutter_right_vertices[263] = {
#include "shared/shutter_panel_tex/core_shutter_right_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_right_model = {
#include "shared/shutter_panel_tex/core_shutter_right_model.inc"
};

NJS_CNK_OBJECT core_shutter_right[1] = {
#include "shared/shutter_panel_tex/core_shutter_right.inc"
};

static Sint16 core_shutter_left_polygons[161] = {
#include "shared/shutter_panel_tex/core_shutter_left_polygons.inc"
};

static Sint32 core_shutter_left_vertices[155] = {
#include "shared/shutter_panel_tex/core_shutter_left_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_left_model = {
#include "shared/shutter_panel_tex/core_shutter_left_model.inc"
};

NJS_CNK_OBJECT core_shutter_left[1] = {
#include "shared/shutter_panel_tex/core_shutter_left.inc"
};

static Sint16 core_shutter_closed_lamp_polygons[15] = {
#include "shared/shutter_panel_tex/core_shutter_closed_lamp_polygons.inc"
};

static Sint32 core_shutter_closed_lamp_vertices[19] = {
#include "shared/shutter_panel_tex/core_shutter_closed_lamp_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_closed_lamp_model = {
#include "shared/shutter_panel_tex/core_shutter_closed_lamp_model.inc"
};

NJS_CNK_OBJECT core_shutter_closed_lamp[1] = {
#include "shared/shutter_panel_tex/core_shutter_closed_lamp.inc"
};

static Sint16 core_shutter_top_polygons[359] = {
#include "shared/shutter_panel_tex/core_shutter_top_polygons.inc"
};

static Sint32 core_shutter_top_vertices[319] = {
#include "shared/shutter_panel_tex/core_shutter_top_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_top_model = {
#include "shared/shutter_panel_tex/core_shutter_top_model.inc"
};

NJS_CNK_OBJECT core_shutter_top[1] = {
#include "shared/shutter_panel_tex/core_shutter_top.inc"
};

static Sint16 core_shutter_latch_polygons[269] = {
#include "shared/shutter_panel_tex/core_shutter_latch_polygons.inc"
};

static Sint32 core_shutter_latch_vertices[155] = {
#include "shared/shutter_panel_tex/core_shutter_latch_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_latch_model = {
#include "shared/shutter_panel_tex/core_shutter_latch_model.inc"
};

NJS_CNK_OBJECT core_shutter_latch[1] = {
#include "shared/shutter_panel_tex/core_shutter_latch.inc"
};

static Sint16 core_shutter_open_lamp_polygons[27] = {
#include "shared/shutter_panel_tex/core_shutter_open_lamp_polygons.inc"
};

static Sint32 core_shutter_open_lamp_vertices[51] = {
#include "shared/shutter_panel_tex/core_shutter_open_lamp_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_open_lamp_model = {
#include "shared/shutter_panel_tex/core_shutter_open_lamp_model.inc"
};

NJS_CNK_OBJECT core_shutter_open_lamp[1] = {
#include "shared/shutter_panel_tex/core_shutter_open_lamp.inc"
};

static NJS_CNK_OBJECT shutter_near_assembly = {
#include "shared/shutter_panel_tex/shutter_near_assembly.inc"
};
