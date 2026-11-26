#ifndef STG23_WALL_ART_H
#define STG23_WALL_ART_H

#include <ninja.h>

#include "shared/debris.h"
#include "stg23/wall_models.h"
#include "stg23/masonry_textures.h"

static NJS_CNK_MODEL *whole_models[LOOKS] = {
#include "stg23/wall_art/whole_models.inc"
};
static NJS_TEXLIST *whole_textures[LOOKS] = {
#include "stg23/wall_art/whole_textures.inc"
};
static NJS_CNK_MODEL *cracked_models[LOOKS] = {
#include "stg23/wall_art/cracked_models.inc"
};
static NJS_TEXLIST *cracked_textures[LOOKS] = {
#include "stg23/wall_art/cracked_textures.inc"
};
static NJS_CNK_MODEL *split_models[LOOKS] = {
#include "stg23/wall_art/split_models.inc"
};
static NJS_TEXLIST *split_textures[LOOKS] = {
#include "stg23/wall_art/split_textures.inc"
};
static NJS_CNK_MODEL *frame_models[LOOKS] = {
#include "stg23/wall_art/frame_models.inc"
};
static NJS_TEXLIST *frame_textures[LOOKS] = {
#include "stg23/wall_art/frame_textures.inc"
};
static const char *kind_names[LOOKS] = {
#include "stg23/wall_art/kind_names.inc"
};

static struct debris_part bright_fragments[9] = {
#include "stg23/wall_art/bright_fragments.inc"
};
static struct debris_part dark_fragments[9] = {
#include "stg23/wall_art/dark_fragments.inc"
};
static struct debris_part *fragments[LOOKS] = {
#include "stg23/wall_art/fragments.inc"
};

#endif
