#include "shared/deck_shutter/text.inc"
#include <ninja.h>
#include <sg_xpt.h>
#include "collision/collision.h"
#include "shared/debris.h"
#include "shared/deck_shutter.h"

/* the set editor's labels for ang.x's low byte, the shutter's group of
 * charges, and for ang.y. no code reads them. each size rounds its label up to
 * whole words, so what follows starts on a word boundary */
static const char editor_id[8] = {
#include "shared/deck_shutter/editor_id.inc"
};
static const char editor_angle_y[12] = {
#include "shared/deck_shutter/editor_angle_y.inc"
};

static NJS_TEXNAME deck_shutter_textures[2] = {
#include "shared/deck_shutter/deck_shutter_textures.inc"
};

static NJS_TEXLIST deck_shutter_texlist = {
#include "shared/deck_shutter/deck_shutter_texlist.inc"
};

static Sint16 deck_shutter_polygons[69] = {
#include "shared/deck_shutter/deck_shutter_polygons.inc"
};

static Sint32 deck_shutter_vertices[51] = {
#include "shared/deck_shutter/deck_shutter_vertices.inc"
};

static NJS_CNK_MODEL deck_shutter_model = {
#include "shared/deck_shutter/deck_shutter_model.inc"
};

static NJS_CNK_OBJECT deck_shutter_object = {
#include "shared/deck_shutter/deck_shutter_object.inc"
};

static NJS_TEXNAME shutter_piece_textures[1] = {
#include "shared/deck_shutter/shutter_piece_textures.inc"
};

static NJS_TEXLIST shutter_piece_texlist = {
#include "shared/deck_shutter/shutter_piece_texlist.inc"
};

static Sint16 shutter_piece_polygons[89] = {
#include "shared/deck_shutter/shutter_piece_polygons.inc"
};

static Sint32 shutter_piece_vertices[83] = {
#include "shared/deck_shutter/shutter_piece_vertices.inc"
};

static NJS_CNK_MODEL shutter_piece_model = {
#include "shared/deck_shutter/shutter_piece_model.inc"
};

static NJS_CNK_OBJECT shutter_piece_piece_object = {
#include "shared/deck_shutter/shutter_piece_piece_object.inc"
};
/* the half width of the box that lockon_box_create gives the shutter,
 * which stops a mech's lock-on */
#define BOX_HALF_WIDTH 110.0F
NJS_TEXLIST *deck_shutter_texlists[3] = {
#include "shared/deck_shutter/deck_shutter_texlists.inc"
};
static struct debris_part parts[5] = {
#include "shared/deck_shutter/parts.inc"
};
static struct collision_shape shutter_box[1] = {
#include "shared/deck_shutter/shutter_box.inc"
};

#include "shared/deck_breakables/shutter_impl.h" /* IWYU pragma: keep -- the code it compiles here */
