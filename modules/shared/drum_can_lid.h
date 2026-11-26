#ifndef SHARED_DRUM_CAN_LID_H
#define SHARED_DRUM_CAN_LID_H
/* All three drums use the same lid layout and carry their own stored mesh. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME drum_can_piece_0_texlist_names[2] = {
#include "shared/drum_can_lid/drum_can_piece_0_texlist_names.inc"
};

static NJS_TEXLIST drum_can_piece_0_texlist = {
#include "shared/drum_can_lid/drum_can_piece_0_texlist.inc"
};

static Sint16 drum_can_piece_0_polygons[97] = {
#include "shared/drum_can_lid/drum_can_piece_0_polygons.inc"
};

static Sint32 drum_can_piece_0_vertices[99] = {
#include "shared/drum_can_lid/drum_can_piece_0_vertices.inc"
};

static NJS_CNK_MODEL drum_can_piece_0_model = {
#include "shared/drum_can_lid/drum_can_piece_0_model.inc"
};

static NJS_CNK_OBJECT drum_can_piece_0_object = {
#include "shared/drum_can_lid/drum_can_piece_0_object.inc"
};

#endif
