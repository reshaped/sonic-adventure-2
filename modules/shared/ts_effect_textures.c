/* The time switch's two rotating arcs share one 240-frame motion. */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk_polygon.h"
#include "shared/time_switch.h"
#include "shared/time_switch_art.h"

#include "shared/ts_effect_textures/text.inc"

struct arc_material {
    Uint16 head, words;
    struct part_color diffuse, ambient;
};

struct back_arc_polygons {
    struct arc_material material1;
    struct part_texture texture1;
    struct part_strip_header strips1;
    Sint16 length1;
    struct part_uv strip1[4];
    Sint16 length2;
    struct part_uv strip2[4];
    Sint16 length3;
    struct part_uv strip3[4];
    Sint16 length4;
    struct part_uv strip4[4];
    Sint16 length5;
    struct part_uv strip5[4];
    Sint16 length6;
    struct part_uv strip6[4];
    Sint16 length7;
    struct part_uv strip7[4];
    Sint16 length8;
    struct part_uv strip8[4];
    Sint16 length9;
    struct part_uv strip9[4];
    Sint16 length10;
    struct part_uv strip10[4];
    Sint16 length11;
    struct part_uv strip11[4];
    Sint16 length12;
    struct part_uv strip12[4];
    Uint16 strip_pad1;
    struct arc_material material2;
    struct part_strip_header strips2;
    Sint16 length13;
    Uint16 strip13[6];
    Sint16 length14;
    Uint16 strip14[7];
    Sint16 length15;
    Uint16 strip15[6];
    Sint16 length16;
    Uint16 strip16[4];
    Sint16 length17;
    Uint16 strip17[4];
    Sint16 length18;
    Uint16 strip18[16];
    Sint16 length19;
    Uint16 strip19[7];
    Uint16 end;
};

struct front_arc_polygons {
    struct arc_material material1;
    struct part_strip_header strips1;
    Sint16 length1;
    Uint16 strip1[6];
    Sint16 length2;
    Uint16 strip2[7];
    Sint16 length3;
    Uint16 strip3[4];
    Sint16 length4;
    Uint16 strip4[4];
    Sint16 length5;
    Uint16 strip5[16];
    Sint16 length6;
    Uint16 strip6[7];
    Sint16 length7;
    Uint16 strip7[6];
    struct arc_material material2;
    struct part_texture texture1;
    struct part_strip_header strips2;
    Sint16 length8;
    struct part_uv strip8[4];
    Sint16 length9;
    struct part_uv strip9[4];
    Sint16 length10;
    struct part_uv strip10[4];
    Sint16 length11;
    struct part_uv strip11[4];
    Sint16 length12;
    struct part_uv strip12[4];
    Sint16 length13;
    struct part_uv strip13[4];
    Sint16 length14;
    struct part_uv strip14[4];
    Sint16 length15;
    struct part_uv strip15[4];
    Sint16 length16;
    struct part_uv strip16[4];
    Sint16 length17;
    struct part_uv strip17[4];
    Sint16 length18;
    struct part_uv strip18[4];
    Sint16 length19;
    struct part_uv strip19[4];
    Uint16 strip_pad2;
    Uint16 end;
};

static NJS_TEXNAME textures[1] = {
#include "shared/ts_effect_textures/textures.inc"
};

NJS_TEXLIST time_switch_effect_texlist = {
#include "shared/ts_effect_textures/time_switch_effect_texlist.inc"
};

static struct back_arc_polygons back_arc_polygons = {
#include "shared/ts_effect_textures/back_arc_polygons.inc"
};

static Sint32 back_arc_vertices[179] = {
#include "shared/ts_effect_textures/back_arc_vertices.inc"
};

static NJS_CNK_MODEL back_arc_model = {
#include "shared/ts_effect_textures/back_arc_model.inc"
};

static NJS_CNK_OBJECT back_arc = {
#include "shared/ts_effect_textures/back_arc.inc"
};

static struct front_arc_polygons front_arc_polygons = {
#include "shared/ts_effect_textures/front_arc_polygons.inc"
};

static Sint32 front_arc_vertices[179] = {
#include "shared/ts_effect_textures/front_arc_vertices.inc"
};

static NJS_CNK_MODEL front_arc_model = {
#include "shared/ts_effect_textures/front_arc_model.inc"
};

static NJS_CNK_OBJECT front_arc = {
#include "shared/ts_effect_textures/front_arc.inc"
};

NJS_CNK_OBJECT time_switch_effect = {
#include "shared/ts_effect_textures/time_switch_effect.inc"
};

static NJS_MKEY_A front_arc_rotation[240] = {
#include "shared/ts_effect_textures/front_arc_rotation.inc"
};

static NJS_MKEY_A back_arc_rotation[240] = {
#include "shared/ts_effect_textures/back_arc_rotation.inc"
};

static NJS_MDATA3 arc_channels[3] = {
#include "shared/ts_effect_textures/arc_channels.inc"
};

NJS_MOTION time_switch_motion[1] = {
#include "shared/ts_effect_textures/time_switch_motion.inc"
};
