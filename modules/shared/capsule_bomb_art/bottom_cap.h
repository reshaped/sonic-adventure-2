#ifndef SHARED_CAPSULE_BOMB_ART_BOTTOM_CAP_H
#define SHARED_CAPSULE_BOMB_ART_BOTTOM_CAP_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 capsule_bomb_bottom_cap_poison_polygons[31] = {
#include "shared/capsule_bomb_art/bottom_cap/capsule_bomb_bottom_cap_poison_polygons.inc"
};

static Sint32 capsule_bomb_bottom_cap_poison_vertices[27] = {
#include "shared/capsule_bomb_art/bottom_cap/capsule_bomb_bottom_cap_poison_vertices.inc"
};

static NJS_CNK_MODEL capsule_bomb_bottom_cap_poison_model = {
#include "shared/capsule_bomb_art/bottom_cap/capsule_bomb_bottom_cap_poison_model.inc"
};

static NJS_CNK_OBJECT capsule_bomb_bottom_cap_poison = {
#include "shared/capsule_bomb_art/bottom_cap/capsule_bomb_bottom_cap_poison.inc"
};

static Sint16 capsule_bomb_bottom_cap_polygons[221] = {
#include "shared/capsule_bomb_art/bottom_cap/capsule_bomb_bottom_cap_polygons.inc"
};

static Sint32 capsule_bomb_bottom_cap_vertices[127] = {
#include "shared/capsule_bomb_art/bottom_cap/capsule_bomb_bottom_cap_vertices.inc"
};

static NJS_CNK_MODEL capsule_bomb_bottom_cap_model = {
#include "shared/capsule_bomb_art/bottom_cap/capsule_bomb_bottom_cap_model.inc"
};

#endif
