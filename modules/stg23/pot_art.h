#ifndef STG23_POT_ART_H
#define STG23_POT_ART_H

#include "shared/debris.h"
#include "shared/ocean_pot.h"
#include "stg23/pot_models.h"
#include "stg23/pot_textures.h"

static NJS_CNK_MODEL *pot_models[2] = {
#include "stg23/pot_art/pot_models.inc"
};
static NJS_TEXLIST *pot_textures[3] = {
#include "stg23/pot_art/pot_textures.inc"
};
static const char *kind_names[] = {"BRIGHT", "DARK  "};

/* clang-format off */
static struct debris_part bright_fragments[OCEAN_POT_PIECES] = {
    {&bright_fragments_texlist, &bright_pot_fragment_0, {-1.5F, 12.0F, 1.5F}, {0, 0, 0}},
    {&bright_fragments_texlist, &bright_pot_fragment_1, {2.25F, 6.0F, 4.5F}, {0, 0, 0}},
    {&bright_fragments_texlist, &bright_pot_fragment_2, {2.25F, 2.0F, 2.25F}, {0, 0, 0}},
    {&bright_fragments_texlist, &bright_pot_fragment_3, {1.5F, 6.0F, -2.25F}, {0, 0, 0}},
    {&bright_fragments_texlist, &bright_pot_fragment_4, {-2.25F, 6.0F, -2.25F}, {0, 0, 0}},
};
static struct debris_part dark_fragments[OCEAN_POT_PIECES] = {
    {&dark_fragments_texlist, &dark_pot_fragment_0, {-1.5F, 12.0F, 1.5F}, {0, 0, 0}},
    {&dark_fragments_texlist, &dark_pot_fragment_1, {2.25F, 6.0F, 4.5F}, {0, 0, 0}},
    {&dark_fragments_texlist, &dark_pot_fragment_2, {2.25F, 2.0F, 2.25F}, {0, 0, 0}},
    {&dark_fragments_texlist, &dark_pot_fragment_3, {1.5F, 6.0F, -2.25F}, {0, 0, 0}},
    {&dark_fragments_texlist, &dark_pot_fragment_4, {-2.25F, 6.0F, -2.25F}, {0, 0, 0}},
};
/* clang-format on */
NJS_TEXLIST *ocean_pot_texlists[] = {
    &bright_pot_texlist,
    &dark_pot_texlist,
    &bright_fragments_texlist,
    &dark_fragments_texlist,
    NULL,
};

#endif
