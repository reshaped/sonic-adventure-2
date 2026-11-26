#ifndef SHARED_CAPSULE_BOMB_ART_UPPER_HALF_H
#define SHARED_CAPSULE_BOMB_ART_UPPER_HALF_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 capsule_bomb_upper_half_polygons[193] = {
#include "shared/capsule_bomb_art/upper_half/capsule_bomb_upper_half_polygons.inc"
};

static Sint32 capsule_bomb_upper_half_vertices[107] = {
#include "shared/capsule_bomb_art/upper_half/capsule_bomb_upper_half_vertices.inc"
};

static NJS_CNK_MODEL capsule_bomb_upper_half_model = {
#include "shared/capsule_bomb_art/upper_half/capsule_bomb_upper_half_model.inc"
};

#endif
