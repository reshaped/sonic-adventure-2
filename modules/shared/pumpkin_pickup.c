/* the pumpkin, PICPUM1 in Pumpkin Hill's object list: a pumpkin a player
 * picks up and carries. its first carry may release a small animal */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/pumpkin_pickup.h"
#include "shared/pumpkin_pickup/text.inc"

/* the placement editor's labels for ang.y and for the low byte of ang.z,
 * which no code reads */
static const char pumpkin_pickup_edit_angle_y[12] = "<- ANGLE Y";
static const char pumpkin_pickup_edit_minimal_ap_lev[28] =
    "<- MINIMAL AP(  ) LEV( )";

static NJS_TEXNAME pumpkin_pickup_texlist_names[3] = {
#include "shared/pumpkin_pickup/pumpkin_pickup_texlist_names.inc"
};

static NJS_TEXLIST pumpkin_pickup_texlist = {
#include "shared/pumpkin_pickup/pumpkin_pickup_texlist.inc"
};

static Sint16 pumpkin_pickup_polygons[475] = {
#include "shared/pumpkin_pickup/pumpkin_pickup_polygons.inc"
};

static Sint32 pumpkin_pickup_vertices[199] = {
#include "shared/pumpkin_pickup/pumpkin_pickup_vertices.inc"
};

static NJS_CNK_MODEL pumpkin_pickup_model = {
#include "shared/pumpkin_pickup/pumpkin_pickup_model.inc"
};

static NJS_CNK_OBJECT pumpkin_pickup_object = {
#include "shared/pumpkin_pickup/pumpkin_pickup_object.inc"
};

NJS_TEXLIST *pumpkin_pickup_texlists[2] = {
#include "shared/pumpkin_pickup/pumpkin_pickup_texlists.inc"
};

#define PUMPKIN_PICKUP_REST(task) (BODY(task)->flags |= BODY_RESTS)

#include "shared/pumpkin_pickup_task.h" /* IWYU pragma: keep -- the code it compiles here */
