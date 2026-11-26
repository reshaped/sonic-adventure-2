#ifndef SHARED_CAPSULE_BOMB_ART_BODY_H
#define SHARED_CAPSULE_BOMB_ART_BODY_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 capsule_bomb_polygons[505] = {
#include "shared/capsule_bomb_art/body/capsule_bomb_polygons.inc"
};

static Sint32 capsule_bomb_vertices[335] = {
#include "shared/capsule_bomb_art/body/capsule_bomb_vertices.inc"
};

static NJS_CNK_MODEL capsule_bomb_model = {
#include "shared/capsule_bomb_art/body/capsule_bomb_model.inc"
};

#endif
