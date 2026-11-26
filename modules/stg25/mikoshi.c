/* the mikoshi, MIKOSHI in Death Chamber's object list: a portable shrine that
 * a Knuckles or a Rouge breaks */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "object/set.h"
#include "shared/debris.h"
#include "sound/sound.h"
#include "stg25/chamber_color.h"
#include "stg25/chamber_util.h"
#include "stg25/mikoshi.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"

/* a mikoshi's modes, in its work's mode, which index chamber_mikoshi_steps */
enum chamber_mikoshi_mode { MODE_WAIT, MODE_BROKEN };

/* the task's unk_3c holds the mode the mikoshi takes at its next step, or
 * NO_NEXT_MODE */
#define NEXT_MODE(task) ((task)->unk_3c.l)
#define NO_NEXT_MODE (-1)

/* the state a break leaves in the placement. a mikoshi whose placement holds
 * it starts broken */
#define BROKEN_STATE 1

#define MIKOSHI_SHAPES 1
#define PIECES 12

/* the break places the pieces' model BREAK_HEIGHT above the mikoshi's pos.
 * it throws each piece straight away from that point. the pieces vanish at the
 * highest floor at or below FLOOR_LOOK_HEIGHT above the point */
#define BREAK_HEIGHT 5.0F
#define FLOOR_LOOK_HEIGHT 1.0F
#define BREAK_SOUND 0x1011
#define BREAK_SOUND_PRIORITY 0

#include "stg25/mikoshi/text.inc"

static NJS_TEXNAME chamber_mikoshi_blue_texture_names[4] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_texture_names.inc"
};

static NJS_TEXLIST chamber_mikoshi_blue_texlist = {
#include "stg25/mikoshi/chamber_mikoshi_blue_texlist.inc"
};

static Sint16 chamber_mikoshi_blue_polygons[661] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_vertices[531] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_object.inc"
};

static NJS_TEXNAME chamber_mikoshi_green_texture_names[4] = {
#include "stg25/mikoshi/chamber_mikoshi_green_texture_names.inc"
};

static NJS_TEXLIST chamber_mikoshi_green_texlist = {
#include "stg25/mikoshi/chamber_mikoshi_green_texlist.inc"
};

static Sint16 chamber_mikoshi_green_polygons[611] = {
#include "stg25/mikoshi/chamber_mikoshi_green_polygons.inc"
};

static Sint32 chamber_mikoshi_green_vertices[535] = {
#include "stg25/mikoshi/chamber_mikoshi_green_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_object.inc"
};

static NJS_TEXNAME chamber_mikoshi_red_texture_names[4] = {
#include "stg25/mikoshi/chamber_mikoshi_red_texture_names.inc"
};

static NJS_TEXLIST chamber_mikoshi_red_texlist = {
#include "stg25/mikoshi/chamber_mikoshi_red_texlist.inc"
};

static Sint16 chamber_mikoshi_red_polygons[661] = {
#include "stg25/mikoshi/chamber_mikoshi_red_polygons.inc"
};

static Sint32 chamber_mikoshi_red_vertices[531] = {
#include "stg25/mikoshi/chamber_mikoshi_red_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_object.inc"
};

static NJS_TEXNAME chamber_mikoshi_blue_pieces_texture_names[4] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_pieces_texture_names.inc"
};

static NJS_TEXLIST chamber_mikoshi_blue_pieces_texlist = {
#include "stg25/mikoshi/chamber_mikoshi_blue_pieces_texlist.inc"
};

static Sint16 chamber_mikoshi_blue_piece_5_polygons[133] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_5_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_piece_5_vertices[99] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_5_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_piece_5_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_5_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_5_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_5_object.inc"
};

static Sint16 chamber_mikoshi_blue_piece_11_polygons[181] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_11_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_piece_11_vertices[155] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_11_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_piece_11_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_11_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_11_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_11_object.inc"
};

static Sint16 chamber_mikoshi_blue_piece_10_polygons[47] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_10_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_piece_10_vertices[47] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_10_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_piece_10_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_10_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_10_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_10_object.inc"
};

static Sint16 chamber_mikoshi_blue_piece_9_polygons[39] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_9_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_piece_9_vertices[23] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_9_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_piece_9_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_9_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_9_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_9_object.inc"
};

static Sint16 chamber_mikoshi_blue_piece_8_polygons[39] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_8_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_piece_8_vertices[23] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_8_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_piece_8_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_8_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_8_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_8_object.inc"
};

static Sint16 chamber_mikoshi_blue_piece_4_polygons[107] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_4_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_piece_4_vertices[95] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_4_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_piece_4_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_4_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_4_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_4_object.inc"
};

static Sint16 chamber_mikoshi_blue_piece_6_polygons[55] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_6_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_piece_6_vertices[47] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_6_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_piece_6_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_6_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_6_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_6_object.inc"
};

static Sint16 chamber_mikoshi_blue_piece_1_polygons[25] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_1_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_piece_1_vertices[19] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_1_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_piece_1_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_1_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_1_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_1_object.inc"
};

static Sint16 chamber_mikoshi_blue_piece_0_polygons[83] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_0_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_piece_0_vertices[67] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_0_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_piece_0_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_0_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_0_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_0_object.inc"
};

static Sint16 chamber_mikoshi_blue_piece_2_polygons[25] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_2_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_piece_2_vertices[19] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_2_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_piece_2_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_2_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_2_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_2_object.inc"
};

static Sint16 chamber_mikoshi_blue_piece_3_polygons[167] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_3_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_piece_3_vertices[131] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_3_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_piece_3_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_3_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_3_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_3_object.inc"
};

static Sint16 chamber_mikoshi_blue_piece_7_polygons[95] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_7_polygons.inc"
};

static Sint32 chamber_mikoshi_blue_piece_7_vertices[99] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_7_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_blue_piece_7_model = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_7_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_7_object = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_7_object.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_blue_piece_7_assembly = {
#include "stg25/mikoshi/chamber_mikoshi_blue_piece_7_assembly.inc"
};

static NJS_TEXNAME chamber_mikoshi_green_pieces_texture_names[4] = {
#include "stg25/mikoshi/chamber_mikoshi_green_pieces_texture_names.inc"
};

static NJS_TEXLIST chamber_mikoshi_green_pieces_texlist = {
#include "stg25/mikoshi/chamber_mikoshi_green_pieces_texlist.inc"
};

static Sint16 chamber_mikoshi_green_piece_5_polygons[133] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_5_polygons.inc"
};

static Sint32 chamber_mikoshi_green_piece_5_vertices[99] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_5_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_piece_5_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_5_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_5_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_5_object.inc"
};

static Sint16 chamber_mikoshi_green_piece_11_polygons[181] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_11_polygons.inc"
};

static Sint32 chamber_mikoshi_green_piece_11_vertices[151] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_11_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_piece_11_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_11_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_11_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_11_object.inc"
};

static Sint16 chamber_mikoshi_green_piece_10_polygons[47] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_10_polygons.inc"
};

static Sint32 chamber_mikoshi_green_piece_10_vertices[47] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_10_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_piece_10_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_10_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_10_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_10_object.inc"
};

static Sint16 chamber_mikoshi_green_piece_9_polygons[39] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_9_polygons.inc"
};

static Sint32 chamber_mikoshi_green_piece_9_vertices[23] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_9_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_piece_9_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_9_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_9_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_9_object.inc"
};

static Sint16 chamber_mikoshi_green_piece_8_polygons[39] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_8_polygons.inc"
};

static Sint32 chamber_mikoshi_green_piece_8_vertices[23] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_8_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_piece_8_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_8_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_8_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_8_object.inc"
};

static Sint16 chamber_mikoshi_green_piece_4_polygons[107] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_4_polygons.inc"
};

static Sint32 chamber_mikoshi_green_piece_4_vertices[91] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_4_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_piece_4_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_4_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_4_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_4_object.inc"
};

static Sint16 chamber_mikoshi_green_piece_6_polygons[55] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_6_polygons.inc"
};

static Sint32 chamber_mikoshi_green_piece_6_vertices[47] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_6_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_piece_6_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_6_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_6_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_6_object.inc"
};

static Sint16 chamber_mikoshi_green_piece_1_polygons[25] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_1_polygons.inc"
};

static Sint32 chamber_mikoshi_green_piece_1_vertices[19] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_1_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_piece_1_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_1_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_1_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_1_object.inc"
};

static Sint16 chamber_mikoshi_green_piece_0_polygons[83] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_0_polygons.inc"
};

static Sint32 chamber_mikoshi_green_piece_0_vertices[67] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_0_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_piece_0_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_0_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_0_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_0_object.inc"
};

static Sint16 chamber_mikoshi_green_piece_2_polygons[25] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_2_polygons.inc"
};

static Sint32 chamber_mikoshi_green_piece_2_vertices[19] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_2_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_piece_2_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_2_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_2_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_2_object.inc"
};

static Sint16 chamber_mikoshi_green_piece_3_polygons[161] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_3_polygons.inc"
};

static Sint32 chamber_mikoshi_green_piece_3_vertices[135] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_3_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_piece_3_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_3_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_3_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_3_object.inc"
};

static Sint16 chamber_mikoshi_green_piece_7_polygons[95] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_7_polygons.inc"
};

static Sint32 chamber_mikoshi_green_piece_7_vertices[99] = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_7_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_green_piece_7_model = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_7_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_7_object = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_7_object.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_green_piece_7_assembly = {
#include "stg25/mikoshi/chamber_mikoshi_green_piece_7_assembly.inc"
};

static NJS_TEXNAME chamber_mikoshi_red_pieces_texture_names[4] = {
#include "stg25/mikoshi/chamber_mikoshi_red_pieces_texture_names.inc"
};

static NJS_TEXLIST chamber_mikoshi_red_pieces_texlist = {
#include "stg25/mikoshi/chamber_mikoshi_red_pieces_texlist.inc"
};

static Sint16 chamber_mikoshi_red_piece_5_polygons[133] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_5_polygons.inc"
};

static Sint32 chamber_mikoshi_red_piece_5_vertices[103] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_5_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_piece_5_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_5_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_5_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_5_object.inc"
};

static Sint16 chamber_mikoshi_red_piece_11_polygons[181] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_11_polygons.inc"
};

static Sint32 chamber_mikoshi_red_piece_11_vertices[155] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_11_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_piece_11_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_11_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_11_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_11_object.inc"
};

static Sint16 chamber_mikoshi_red_piece_10_polygons[47] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_10_polygons.inc"
};

static Sint32 chamber_mikoshi_red_piece_10_vertices[47] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_10_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_piece_10_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_10_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_10_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_10_object.inc"
};

static Sint16 chamber_mikoshi_red_piece_9_polygons[39] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_9_polygons.inc"
};

static Sint32 chamber_mikoshi_red_piece_9_vertices[23] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_9_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_piece_9_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_9_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_9_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_9_object.inc"
};

static Sint16 chamber_mikoshi_red_piece_8_polygons[39] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_8_polygons.inc"
};

static Sint32 chamber_mikoshi_red_piece_8_vertices[23] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_8_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_piece_8_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_8_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_8_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_8_object.inc"
};

static Sint16 chamber_mikoshi_red_piece_4_polygons[107] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_4_polygons.inc"
};

static Sint32 chamber_mikoshi_red_piece_4_vertices[95] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_4_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_piece_4_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_4_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_4_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_4_object.inc"
};

static Sint16 chamber_mikoshi_red_piece_6_polygons[55] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_6_polygons.inc"
};

static Sint32 chamber_mikoshi_red_piece_6_vertices[47] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_6_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_piece_6_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_6_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_6_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_6_object.inc"
};

static Sint16 chamber_mikoshi_red_piece_1_polygons[25] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_1_polygons.inc"
};

static Sint32 chamber_mikoshi_red_piece_1_vertices[19] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_1_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_piece_1_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_1_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_1_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_1_object.inc"
};

static Sint16 chamber_mikoshi_red_piece_0_polygons[89] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_0_polygons.inc"
};

static Sint32 chamber_mikoshi_red_piece_0_vertices[67] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_0_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_piece_0_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_0_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_0_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_0_object.inc"
};

static Sint16 chamber_mikoshi_red_piece_2_polygons[25] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_2_polygons.inc"
};

static Sint32 chamber_mikoshi_red_piece_2_vertices[19] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_2_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_piece_2_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_2_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_2_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_2_object.inc"
};

static Sint16 chamber_mikoshi_red_piece_3_polygons[139] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_3_polygons.inc"
};

static Sint32 chamber_mikoshi_red_piece_3_vertices[123] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_3_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_piece_3_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_3_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_3_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_3_object.inc"
};

static Sint16 chamber_mikoshi_red_piece_7_polygons[95] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_7_polygons.inc"
};

static Sint32 chamber_mikoshi_red_piece_7_vertices[99] = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_7_vertices.inc"
};

static NJS_CNK_MODEL chamber_mikoshi_red_piece_7_model = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_7_model.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_7_object = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_7_object.inc"
};

static NJS_CNK_OBJECT chamber_mikoshi_red_piece_7_assembly = {
#include "stg25/mikoshi/chamber_mikoshi_red_piece_7_assembly.inc"
};

/* SHC expands each call of chamber_mikoshi_set_next_mode under #pragma inline.
 * the image also holds a copy of the function, which SHC writes only for an
 * external one. no header declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void chamber_mikoshi_set_next_mode(struct task *task, int mode);
/* NOLINTEND(misc-use-internal-linkage) */

static void chamber_mikoshi_destroy(struct task *task);
static void chamber_mikoshi_step(struct task *task);
static void chamber_mikoshi_draw(struct task *task);
static void chamber_mikoshi_take_mode(struct task *task);
static void chamber_mikoshi_wait(struct task *task);
static void chamber_mikoshi_break(struct task *task);
static void chamber_mikoshi_broken(struct task *task);

KEPT static NJS_CNK_MODEL *chamber_mikoshi_models[CHAMBER_COLORS] = {
#include "stg25/mikoshi/chamber_mikoshi_models.inc"
};
NJS_TEXLIST *chamber_mikoshi_texlists[CHAMBER_COLORS + 1] = {
#include "stg25/mikoshi/chamber_mikoshi_texlists.inc"
};
NJS_TEXLIST *chamber_mikoshi_pieces_texlists[CHAMBER_COLORS + 1] = {
#include "stg25/mikoshi/chamber_mikoshi_pieces_texlists.inc"
};
static char *chamber_mikoshi_color_names[CHAMBER_COLORS] = {
#include "stg25/mikoshi/chamber_mikoshi_color_names.inc"
};
static struct debris_part chamber_mikoshi_blue_parts[PIECES] = {
#include "stg25/mikoshi/chamber_mikoshi_blue_parts.inc"
};
static struct debris_part chamber_mikoshi_green_parts[PIECES] = {
#include "stg25/mikoshi/chamber_mikoshi_green_parts.inc"
};
static struct debris_part chamber_mikoshi_red_parts[PIECES] = {
#include "stg25/mikoshi/chamber_mikoshi_red_parts.inc"
};
KEPT static struct debris_part *chamber_mikoshi_parts[CHAMBER_COLORS] = {
#include "stg25/mikoshi/chamber_mikoshi_parts.inc"
};
/* clang-format off */
static struct collision_shape chamber_mikoshi_shapes[MIKOSHI_SHAPES] = {
    {0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000, {0.0F, 12.5F, 0.0F},
     10.8F, 12.5F, 10.8F, 0, {0, 0, 0}},
};
/* clang-format on */
static Sint32 chamber_mikoshi_break_volume = 125;
typedef void (*chamber_mikoshi_action)(struct task *task);
KEPT static chamber_mikoshi_action chamber_mikoshi_steps[2] = {
#include "stg25/mikoshi/chamber_mikoshi_steps.inc"
};
/* a break throws each piece at chamber_mikoshi_speed, plus up to
 * chamber_mikoshi_spread more at random. it adds chamber_mikoshi_rise to each
 * piece's upward speed */
static Float chamber_mikoshi_speed = 0.7F;
static Float chamber_mikoshi_spread = 0.35F;
static Float chamber_mikoshi_rise = 0.25F;

void chamber_mikoshi_init(struct task *task)
{
    /* @hack spelling: task->work read again for mode, where work would do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    if (!placement_try_leave(task)) {
        task->draw = chamber_mikoshi_draw;
        task->step = chamber_mikoshi_step;
        task->destructor = chamber_mikoshi_destroy;
        if (placement_get_state(task)) {
            task->work->mode = MODE_BROKEN;
        } else {
            /* @hack redundant: pos, a point above the mikoshi that nothing
             * reads */
            pos = work->pos;
            pos.y += 15.0F;
            collision_init(task, chamber_mikoshi_shapes, MIKOSHI_SHAPES,
                           COLLISION_OBJECT);
            task->work->mode = MODE_WAIT;
        }
        NEXT_MODE(task) = NO_NEXT_MODE;
    }
}

static void chamber_mikoshi_destroy(struct task *task)
{
}

static void chamber_mikoshi_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        chamber_mikoshi_take_mode(task);
        chamber_mikoshi_steps[task->work->mode](task);
    }
}

static void chamber_mikoshi_draw(struct task *task)
{
    /* @hack spelling: task->work read again for mode and the color, where
     * work would do */
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: a switch of one case, where an if would do */
    switch (task->work->mode) {
    case MODE_WAIT:
        njSetTexture(chamber_mikoshi_texlists[CHAMBER_COLOR(task->work)]);
        njCnkEasyDrawModel(chamber_mikoshi_models[CHAMBER_COLOR(task->work)]);
        break;
    }
    njPopMatrixEx();
}

static void chamber_mikoshi_take_mode(struct task *task)
{
    if (NEXT_MODE(task) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task);
        NEXT_MODE(task) = NO_NEXT_MODE;
    }
}

#pragma inline(chamber_mikoshi_set_next_mode)
void chamber_mikoshi_set_next_mode(struct task *task, int mode)
{
    NEXT_MODE(task) = mode;
}

static void chamber_mikoshi_wait(struct task *task)
{
    struct task_work *work = task->work;
    struct task *player;

    /* @hack spelling: the hunter's test in an if of its own, where a third
     * operand of the && would do */
    if ((work->collision->flags & COLLISION_TOUCHED) &&
        (player = collision_next_player(task))) {
        if (chamber_hunter_attacking(player)) {
            chamber_mikoshi_break(task);
            chamber_mikoshi_set_next_mode(task, MODE_BROKEN);
            return;
        }
    }
    collision_register(task);
}

static void chamber_mikoshi_break(struct task *task)
{
    /* @hack spelling: task->work read again for the color, where work would
     * do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR spd;
    Float floor;

    pos = work->pos;
    /* @hack spelling: the height taken off as its negative, where
     * += BREAK_HEIGHT would do */
    pos.y -= -BREAK_HEIGHT;
    spd.x = 0.0F;
    spd.y = chamber_mikoshi_rise;
    spd.z = 0.0F;
    floor = surface_probe_floor(pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z, &ang);
    debris_break_flags(&pos, work->ang.y,
                       chamber_mikoshi_parts[CHAMBER_COLOR(task->work)], PIECES,
                       chamber_mikoshi_speed, chamber_mikoshi_spread, floor,
                       &spd, DEBRIS_TRAIL | DEBRIS_VANISH);
    placement_set_state(task, BROKEN_STATE);
    sound_play(BREAK_SOUND, NULL, BREAK_SOUND_PRIORITY,
               chamber_mikoshi_break_volume);
    placement_kill(task);
}

static void chamber_mikoshi_broken(struct task *task)
{
}
