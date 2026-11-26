/* Aquatic Mine drum, its lid and the side fragments scattered on impact. */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg07/drum_can.h"
#include "stg07/drum_can/text.inc"

/* the placement editor's labels for the can's heading and its id, which no
 * code reads. each size rounds its label up to whole words, so what follows
 * starts on a word boundary */
static const char drum_edit_yaw[12] = "<- ANGLE Y";
static const char drum_edit_id[8] = "<- ID";

static NJS_TEXNAME drum_can_texlist_names[4] = {
#include "stg07/drum_can/drum_can_texlist_names.inc"
};

static NJS_TEXLIST drum_can_texlist = {
#include "stg07/drum_can/drum_can_texlist.inc"
};

static Sint16 drum_can_polygons[351] = {
#include "stg07/drum_can/drum_can_polygons.inc"
};

static Sint32 drum_can_vertices[263] = {
#include "stg07/drum_can/drum_can_vertices.inc"
};

static NJS_CNK_MODEL drum_can_model = {
#include "stg07/drum_can/drum_can_model.inc"
};

static NJS_CNK_OBJECT drum_can_object = {
#include "stg07/drum_can/drum_can_object.inc"
};

#include "shared/drum_can_lid.h"

static NJS_TEXNAME drum_can_piece_1_texlist_names[1] = {
#include "stg07/drum_can/drum_can_piece_1_texlist_names.inc"
};

static NJS_TEXLIST drum_can_piece_1_texlist = {
#include "stg07/drum_can/drum_can_piece_1_texlist.inc"
};

static Sint16 drum_can_piece_1_polygons[57] = {
#include "stg07/drum_can/drum_can_piece_1_polygons.inc"
};

static Sint32 drum_can_piece_1_vertices[43] = {
#include "stg07/drum_can/drum_can_piece_1_vertices.inc"
};

static NJS_CNK_MODEL drum_can_piece_1_model = {
#include "stg07/drum_can/drum_can_piece_1_model.inc"
};

static NJS_CNK_OBJECT drum_can_piece_1_object = {
#include "stg07/drum_can/drum_can_piece_1_object.inc"
};

NJS_TEXLIST *mine_drum_can_texlists[4] = {
#include "stg07/drum_can/mine_drum_can_texlists.inc"
};

/* NOLINTBEGIN(readability-identifier-naming) */
#define drum_can_init mine_drum_can_init
/* NOLINTEND(readability-identifier-naming) */
#define DRUM_CAN_BREAK_SOUND(work) ((void)0)
#define DRUM_CAN_BREAK_SCORE() ((void)0)
#define DRUM_CAN_HIT(work)                                                     \
    score_add(BREAK_POINTS);                                                   \
    object_sound_play(BREAK_SOUND_KIND, &(work)->pos)

#include "shared/drum_can_impl.h" /* IWYU pragma: keep -- the code it compiles here */
