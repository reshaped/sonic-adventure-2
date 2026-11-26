#ifndef STG23_BLOCK_ART_H
#define STG23_BLOCK_ART_H

#include <ninja.h>

#include "shared/debris.h"
#include "stg23/block_models.h"
#include "stg23/masonry_textures.h"

static NJS_CNK_MODEL *whole_models[LOOKS] = {
#include "stg23/block_art/whole_models.inc"
};
static NJS_TEXLIST *whole_textures[LOOKS] = {
#include "stg23/block_art/whole_textures.inc"
};
static NJS_CNK_MODEL *cracked_models[LOOKS] = {
#include "stg23/block_art/cracked_models.inc"
};
static NJS_TEXLIST *cracked_textures[LOOKS] = {
#include "stg23/block_art/cracked_textures.inc"
};
static NJS_CNK_MODEL *split_models[LOOKS] = {
#include "stg23/block_art/split_models.inc"
};
static NJS_TEXLIST *split_textures[LOOKS] = {
#include "stg23/block_art/split_textures.inc"
};
static const char *kind_names[LOOKS] = {
#include "stg23/block_art/kind_names.inc"
};

static struct debris_part cracked_fragments[8] = {
#include "stg23/block_art/cracked_fragments.inc"
};
static struct debris_part split_fragments[11] = {
#include "stg23/block_art/split_fragments.inc"
};

#endif
