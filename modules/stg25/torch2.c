/* Death Chamber wall torch. */
#define TORCH2(name) chamber_torch2_##name
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "shared/common_object.h"
#include "stg25/chamber_color.h"
#include "stg25/death_chamber.h"
#include "stg25/flame_prop.h"
#include "stg25/torch2.h"

#define TORCH2_SHAPES 2

#include "stg25/torch2/text.inc"
#include "stg25/wall_torch_models.h"

static NJS_CNK_MODEL *chamber_torch2_models[CHAMBER_COLORS] = {
#include "stg25/torch2/chamber_torch2_models.inc"
};
static NJS_CNK_MODEL *chamber_torch2_flame_models[CHAMBER_COLORS] = {
#include "stg25/torch2/chamber_torch2_flame_models.inc"
};
NJS_TEXLIST *chamber_torch2_texlists[CHAMBER_COLORS + 1] = {
#include "stg25/torch2/chamber_torch2_texlists.inc"
};
static NJS_TEXLIST *chamber_torch2_flame_texlists[CHAMBER_COLORS] = {
#include "stg25/torch2/chamber_torch2_flame_texlists.inc"
};
static char *chamber_torch2_color_names[CHAMBER_COLORS] = {
#include "stg25/torch2/chamber_torch2_color_names.inc"
};
/* the flame's shape and the stand's, at a scale of 1 */
/* clang-format off */
static struct collision_shape chamber_torch2_shapes[TORCH2_SHAPES] = {
    {0, FLAME_FORM, FLAME_PUSH, FLAME_LEVELS, FLAME_SHAPE_FLAGS,
     {0.0F, 15.5F, 5.0F}, 3.0F, 5.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, BODY_PUSH, SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL),
     SHAPE_UNK_2000, {0.0F, 2.5F, 0.0F}, 10.0F, 2.5F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Sint32 chamber_torch2_volume = 20;
static Float chamber_torch2_sound_range = 100.0F;
static Float dat_8c5ce084 = 25.0F;
/* TRUE once one call of TORCH2(build) has compiled all three colors'
 * torches */
static Bool chamber_torch2_built = FALSE;
static struct model_build chamber_torch2_builds[CHAMBER_COLORS] = {
#include "stg25/torch2/chamber_torch2_builds.inc"
};

#include "stg25/wall_torch_impl.h" /* IWYU pragma: keep -- the code it compiles here */
