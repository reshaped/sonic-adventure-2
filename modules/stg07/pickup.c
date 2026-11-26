/* Aquatic Mine's carryable pickup, PIC in its object list. */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg07/pickup.h"
#include "stg07/pickup/text.inc"

/* the placement editor's labels for ang.y and for the low byte of ang.z,
 * which no code reads */
static const char pumpkin_pickup_edit_angle_y[12] = "<- ANGLE Y";
static const char pumpkin_pickup_edit_minimal_ap_lev[28] =
    "<- MINIMAL AP(  ) LEV( )";

static NJS_TEXNAME pickup_texlist_names[2] = {
#include "stg07/pickup/pickup_texlist_names.inc"
};

static NJS_TEXLIST pumpkin_pickup_texlist = {
#include "stg07/pickup/pumpkin_pickup_texlist.inc"
};

static Sint16 pickup_polygons[379] = {
#include "stg07/pickup/pickup_polygons.inc"
};

static Sint32 pickup_vertices[227] = {
#include "stg07/pickup/pickup_vertices.inc"
};

static NJS_CNK_MODEL pumpkin_pickup_model = {
#include "stg07/pickup/pumpkin_pickup_model.inc"
};

static NJS_CNK_OBJECT pickup_object = {
#include "stg07/pickup/pickup_object.inc"
};

NJS_TEXLIST *pickup_texlists[2] = {
#include "stg07/pickup/pickup_texlists.inc"
};

/* NOLINTBEGIN(readability-identifier-naming) */
#define pumpkin_pickup_init pickup_init
/* NOLINTEND(readability-identifier-naming) */
#define PUMPKIN_PICKUP_REST(task) ((void)0)

#include "shared/pumpkin_pickup_task.h" /* IWYU pragma: keep -- the code it compiles here */
