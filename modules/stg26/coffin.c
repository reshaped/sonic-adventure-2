/* Egg Quarters' breakable coffin, built from stg25/treasure_impl.h */
#define EGG_QUARTERS
#define QUARTERS_COFFIN
#define TREASURE(name) quarters_coffin_##name

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "stg25/chamber_color.h"
#include "stg26/coffin.h"

struct task;
typedef void (*treasure_step)(struct task *task);

#define PIECES 9
#define TREASURE_SHAPES 1
#define BOX_HALF_X 15.0F
#define BOX_HALF_Y 16.0F
#define BOX_HALF_Z 17.5F

static void coffin_wait(struct task *task);
static void coffin_broken(struct task *task);

#include "stg26/coffin/text.inc"
#include "stg26/coffin_art.h" /* IWYU pragma: keep -- the data it defines here */

static NJS_CNK_MODEL *chamber_treasure_models[CHAMBER_COLORS] = {
#include "stg26/coffin/chamber_treasure_models.inc"
};

NJS_TEXLIST *quarters_coffin_texlists[CHAMBER_COLORS + 1] = {
#include "stg26/coffin/quarters_coffin_texlists.inc"
};

NJS_TEXLIST *quarters_coffin_pieces_texlists[CHAMBER_COLORS + 1] = {
#include "stg26/coffin/quarters_coffin_pieces_texlists.inc"
};

static char *chamber_treasure_color_names[CHAMBER_COLORS] = {
#include "stg26/coffin/chamber_treasure_color_names.inc"
};

static struct debris_part chamber_treasure_blue_parts[PIECES] = {
#include "stg26/coffin/chamber_treasure_blue_parts.inc"
};

static struct debris_part chamber_treasure_green_parts[PIECES] = {
#include "stg26/coffin/chamber_treasure_green_parts.inc"
};

static struct debris_part chamber_treasure_red_parts[PIECES] = {
#include "stg26/coffin/chamber_treasure_red_parts.inc"
};

static struct debris_part *chamber_treasure_parts[CHAMBER_COLORS] = {
#include "stg26/coffin/chamber_treasure_parts.inc"
};

/* clang-format off */
static struct collision_shape chamber_treasure_shapes[TREASURE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, BOX_HALF_Y, 0.0F}, BOX_HALF_X, BOX_HALF_Y,
     BOX_HALF_Z, 0, {0, 0, 0}},
};
/* clang-format on */

static Sint32 coffin_break_volume = 125;

/* TRUE once one call of TREASURE(build) has compiled all three colors'
 * coffins */
static Bool chamber_treasure_built = FALSE;

static struct model_build chamber_treasure_builds[CHAMBER_COLORS] = {
#include "stg26/coffin/chamber_treasure_builds.inc"
};

static treasure_step chamber_treasure_steps[2] = {
#include "stg26/coffin/chamber_treasure_steps.inc"
};

static Float coffin_break_speed = 0.7F;
static Float coffin_break_spread = 0.35F;
static Float coffin_break_rise = 0.25F;

#define TREASURE_WAIT coffin_wait
#define TREASURE_BROKEN coffin_broken
#include "stg25/treasure_impl.h"
