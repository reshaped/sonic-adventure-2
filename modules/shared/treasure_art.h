/* Treasure chest materials and meshes. */
#ifndef SHARED_TREASURE_ART_H
#define SHARED_TREASURE_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static const char chest_material[20] = {
#include "shared/treasure_art/chest_material.inc"
};

#include "shared/treasure_meshes/chest_blue.h"
#include "shared/treasure_meshes/chest_green.h"
#include "shared/treasure_meshes/chest_red.h"
#include "shared/treasure_meshes/chest_blue_pieces.h"
#include "shared/treasure_meshes/chest_green_pieces.h"
#include "shared/treasure_meshes/chest_red_pieces.h"
#include "shared/treasure_meshes/chest_collision.h"

#endif
