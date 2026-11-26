/* Egg Quarters breakable chest. */
#define EGG_QUARTERS
#define TREASURE(name) quarters_treasure_##name

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "stg25/chamber_color.h"
#include "stg26/flame_props.h"

struct task;
typedef void (*treasure_step)(struct task *task);

#define PIECES 8
#define TREASURE_SHAPES 1
#define BOX_HALF_X 10.8F
#define BOX_HALF_Y 12.5F
#define BOX_HALF_Z 10.8F

static void chamber_treasure_wait(struct task *task);
static void chamber_treasure_broken(struct task *task);

#include "stg26/treasure/text.inc"
#include "shared/treasure_art.h" /* IWYU pragma: keep -- the data it defines here */

static NJS_CNK_MODEL *chamber_treasure_models[CHAMBER_COLORS] = {
#include "stg26/treasure/chamber_treasure_models.inc"
};

NJS_TEXLIST *quarters_treasure_texlists[CHAMBER_COLORS + 1] = {
#include "stg26/treasure/quarters_treasure_texlists.inc"
};

NJS_TEXLIST *quarters_treasure_pieces_texlists[CHAMBER_COLORS + 1] = {
#include "stg26/treasure/quarters_treasure_pieces_texlists.inc"
};

static char *chamber_treasure_color_names[CHAMBER_COLORS] = {
#include "stg26/treasure/chamber_treasure_color_names.inc"
};

static struct debris_part chamber_treasure_blue_parts[PIECES] = {
#include "stg26/treasure/chamber_treasure_blue_parts.inc"
};

static struct debris_part chamber_treasure_green_parts[PIECES] = {
#include "stg26/treasure/chamber_treasure_green_parts.inc"
};

static struct debris_part chamber_treasure_red_parts[PIECES] = {
#include "stg26/treasure/chamber_treasure_red_parts.inc"
};

static struct debris_part *chamber_treasure_parts[CHAMBER_COLORS] = {
#include "stg26/treasure/chamber_treasure_parts.inc"
};

/* clang-format off */
static struct collision_shape chamber_treasure_shapes[TREASURE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, BOX_HALF_Y, 0.0F}, BOX_HALF_X, BOX_HALF_Y,
     BOX_HALF_Z, 0, {0, 0, 0}},
};
/* clang-format on */

/* TRUE once one call of TREASURE(build) has compiled all three colors'
 * chests */
static Bool chamber_treasure_built = FALSE;

static struct model_build chamber_treasure_builds[CHAMBER_COLORS] = {
#include "stg26/treasure/chamber_treasure_builds.inc"
};

static treasure_step chamber_treasure_steps[2] = {
#include "stg26/treasure/chamber_treasure_steps.inc"
};

#define TREASURE_WAIT chamber_treasure_wait
#define TREASURE_BROKEN chamber_treasure_broken
#include "stg25/treasure_impl.h"
