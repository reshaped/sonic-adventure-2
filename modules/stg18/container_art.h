#ifndef STG18_CONTAINER_ART_H
#define STG18_CONTAINER_ART_H
#include <ninja.h>
#include <sg_xpt.h>
#include "collision/collision.h"
#include "shared/common_object.h"

#define CONTAINER_PIECES 24
#define CONTAINER_BUILDS (1 + CONTAINER_PIECES)
/* a box's variant, an index of container_models, container_variants and
 * container_texlists */
enum container_variant { CONTAINER_BRIGHT, CONTAINER_DARK, CONTAINER_VARIANTS };
/* how many shapes container_shape holds */
#define CONTAINER_SHAPES 1

struct container_piece {
    NJS_TEXLIST *textures;
    NJS_CNK_MODEL *model;
    Sint32 count;
};

struct container_texture_filenames {
    char blueline[24];
    char deck[20];
    char triangle[20];
};

struct container_variant_labels {
    char bright[8];
    char dark[7];
};

extern const struct container_texture_filenames lagoon_container_texture_names;
extern const struct container_variant_labels lagoon_container_variant_names;
extern NJS_CNK_MODEL *container_models[CONTAINER_VARIANTS];
extern NJS_TEXLIST *container_texlists[5];
extern const char *container_variants[CONTAINER_VARIANTS];
extern struct container_piece bright_container_pieces[CONTAINER_PIECES];
extern struct container_piece dark_container_pieces[CONTAINER_PIECES];
extern Bool container_built;
extern struct model_build bright_container_builds[CONTAINER_BUILDS];
extern struct model_build dark_container_builds[CONTAINER_BUILDS];
extern struct collision_shape container_shape[CONTAINER_SHAPES];
extern Float container_piece_spread;
extern Float container_piece_speed;

#endif
