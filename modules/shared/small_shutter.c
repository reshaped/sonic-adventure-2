#include "shared/small_shutter/text.inc"
#include <ninja.h>
#include <sg_xpt.h>
#include "collision/collision.h"
#include "shared/debris.h"
#include "shared/deck_shutter.h"

/* the set editor's labels for ang.x's low byte, the shutter's group of
 * charges, and for ang.y. no code reads them. each size rounds its label up to
 * whole words, so what follows starts on a word boundary */
static const char editor_id[8] = "<- ID";
static const char editor_angle_y[12] = "<- ANGLE Y";

static NJS_TEXNAME deck_shutter_small_textures[2] = {
#include "shared/small_shutter/deck_shutter_small_textures.inc"
};

static NJS_TEXLIST deck_shutter_small_texlist = {
#include "shared/small_shutter/deck_shutter_small_texlist.inc"
};

static Sint16 deck_shutter_small_polygons[43] = {
#include "shared/small_shutter/deck_shutter_small_polygons.inc"
};

static Sint32 deck_shutter_small_vertices[35] = {
#include "shared/small_shutter/deck_shutter_small_vertices.inc"
};

static NJS_CNK_MODEL deck_shutter_small_model = {
#include "shared/small_shutter/deck_shutter_small_model.inc"
};

static NJS_CNK_OBJECT deck_shutter_small_object = {
#include "shared/small_shutter/deck_shutter_small_object.inc"
};

static NJS_TEXNAME shutter_end_textures[1] = {
#include "shared/small_shutter/shutter_end_textures.inc"
};

static NJS_TEXLIST shutter_end_texlist = {
#include "shared/small_shutter/shutter_end_texlist.inc"
};

static Sint16 shutter_end_polygons[67] = {
#include "shared/small_shutter/shutter_end_polygons.inc"
};

static Sint32 shutter_end_vertices[43] = {
#include "shared/small_shutter/shutter_end_vertices.inc"
};

static NJS_CNK_MODEL shutter_end_model = {
#include "shared/small_shutter/shutter_end_model.inc"
};

static NJS_CNK_OBJECT shutter_end_piece_object = {
#include "shared/small_shutter/shutter_end_piece_object.inc"
};

static NJS_TEXNAME shutter_small_piece_textures[1] = {
#include "shared/small_shutter/shutter_small_piece_textures.inc"
};

static NJS_TEXLIST shutter_small_piece_texlist = {
#include "shared/small_shutter/shutter_small_piece_texlist.inc"
};

static Sint16 shutter_small_piece_polygons[55] = {
#include "shared/small_shutter/shutter_small_piece_polygons.inc"
};

static Sint32 shutter_small_piece_vertices[39] = {
#include "shared/small_shutter/shutter_small_piece_vertices.inc"
};

static NJS_CNK_MODEL shutter_small_piece_model = {
#include "shared/small_shutter/shutter_small_piece_model.inc"
};

static NJS_CNK_OBJECT shutter_small_piece_piece_object = {
#include "shared/small_shutter/shutter_small_piece_piece_object.inc"
};
#define BOX_HALF_WIDTH 70.0F
NJS_TEXLIST *deck_shutter_small_texlists[4] = {
#include "shared/small_shutter/deck_shutter_small_texlists.inc"
};
static struct debris_part parts[5] = {
#include "shared/small_shutter/parts.inc"
};
/* clang-format off */
static struct collision_shape shutter_box[1] = {
    {0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 30.0F, 0.0F}, 30.0F, 30.0F, 1.0F, 0, {0, 0, 0}},
};
/* clang-format on */

#define SMALL_SHUTTER
/* NOLINTBEGIN(readability-identifier-naming) */
#define deck_shutter_init deck_shutter_small_init
#define deck_shutter_step deck_shutter_small_step
#define deck_shutter_draw deck_shutter_small_draw
#define deck_shutter_destroy deck_shutter_small_destroy
#define deck_shutter_texlist deck_shutter_small_texlist
#define deck_shutter_object deck_shutter_small_object
/* NOLINTEND(readability-identifier-naming) */
#include "shared/deck_breakables/shutter_impl.h" /* IWYU pragma: keep -- the code it compiles here */
