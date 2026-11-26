/* Egg Quarters coffin materials and meshes. */
#ifndef STG26_COFFIN_ART_H
#define STG26_COFFIN_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static const char coffin_material[16] = {
#include "stg26/coffin_art/coffin_material.inc"
};
static const char side_material[24] = {
#include "stg26/coffin_art/side_material.inc"
};
static const char top_material[20] = {
#include "stg26/coffin_art/top_material.inc"
};

#include "stg26/coffin_meshes/blue.h"
#include "stg26/coffin_meshes/green.h"
#include "stg26/coffin_meshes/red.h"
#include "stg26/coffin_meshes/blue_pieces.h"
#include "stg26/coffin_meshes/green_pieces.h"
#include "stg26/coffin_meshes/red_pieces.h"
#include "stg26/coffin_meshes/collision.h"

#endif
