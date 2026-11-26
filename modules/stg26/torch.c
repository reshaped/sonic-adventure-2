/* Egg Quarters standing torch. */
#define EGG_QUARTERS
#define TORCH(name) quarters_torch_##name
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "shared/common_object.h"
#include "stg25/chamber_color.h"
#include "stg25/flame_prop.h"
#include "stg26/flame_props.h"
#include "system/angle.h"

#define TORCH_SHAPES 3

#include "stg26/torch/text.inc"
#include "stg25/standing_torch_models.h"

static NJS_TEXNAME round_shadow_small_texture_names[1] = {
#include "stg26/torch/round_shadow_small_texture_names.inc"
};

NJS_TEXLIST quarters_round_shadow_small_texlist = {
#include "stg26/torch/quarters_round_shadow_small_texlist.inc"
};

static Sint16 round_shadow_small_polygons[25] = {
#include "stg26/torch/round_shadow_small_polygons.inc"
};

static Sint32 round_shadow_small_vertices[19] = {
#include "stg26/torch/round_shadow_small_vertices.inc"
};

NJS_CNK_MODEL quarters_round_shadow_small_model = {
#include "stg26/torch/quarters_round_shadow_small_model.inc"
};

static NJS_CNK_OBJECT round_shadow_small_object = {
#include "stg26/torch/round_shadow_small_object.inc"
};
static NJS_CNK_MODEL *chamber_torch_models[CHAMBER_COLORS] = {
#include "stg26/torch/chamber_torch_models.inc"
};
static NJS_CNK_MODEL *chamber_torch_flame_models[CHAMBER_COLORS] = {
#include "stg26/torch/chamber_torch_flame_models.inc"
};
NJS_TEXLIST *quarters_torch_texlists[CHAMBER_COLORS + 1] = {
#include "stg26/torch/quarters_torch_texlists.inc"
};
static NJS_TEXLIST *chamber_torch_flame_texlists[CHAMBER_COLORS] = {
#include "stg26/torch/chamber_torch_flame_texlists.inc"
};
static char *chamber_torch_color_names[CHAMBER_COLORS] = {
#include "stg26/torch/chamber_torch_color_names.inc"
};
/* the flame's shape, the stick's, and a box at the stick's top */
/* clang-format off */
static struct collision_shape chamber_torch_shapes[TORCH_SHAPES] = {
    {0, FLAME_FORM, FLAME_PUSH, FLAME_LEVELS, FLAME_SHAPE_FLAGS,
     {0.0F, 72.5F, 0.0F}, 6.0F, 6.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, BODY_PUSH, SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000,
     {0.0F, 27.5F, 0.0F}, 2.0F, 27.5F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, BODY_PUSH, SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000,
     {0.0F, 57.5F, 0.0F}, 7.071F, 2.5F, 7.071F, 0, {0, EIGHTH_TURN, 0}},
};
/* clang-format on */
static Sint32 chamber_torch_volume = 20;
static Float chamber_torch_sound_range = 100.0F;
/* TRUE once one call of TORCH(build) has compiled all three colors'
 * torches */
static Bool chamber_torch_built = FALSE;
static struct model_build chamber_torch_builds[CHAMBER_COLORS] = {
#include "stg26/torch/chamber_torch_builds.inc"
};

#include "stg25/standing_torch_impl.h" /* IWYU pragma: keep -- the code it compiles here */
