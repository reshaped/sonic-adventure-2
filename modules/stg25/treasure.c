/* Death Chamber breakable chest. */
#define TREASURE(name) chamber_treasure_##name

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "stg25/chamber_color.h"
#include "stg25/treasure.h"

struct task;
typedef void (*treasure_step)(struct task *task);

#define PIECES 8
#define TREASURE_SHAPES 1
#define BOX_HALF_HEIGHT 12.5F

static void chamber_treasure_wait(struct task *task);
static void chamber_treasure_broken(struct task *task);

#include "stg25/treasure/text.inc"
#include "shared/treasure_art.h" /* IWYU pragma: keep -- the data it defines here */

static NJS_CNK_MODEL *chamber_treasure_models[CHAMBER_COLORS] = {
#include "stg25/treasure/chamber_treasure_models.inc"
};

NJS_TEXLIST *chamber_treasure_texlists[CHAMBER_COLORS + 1] = {
#include "stg25/treasure/chamber_treasure_texlists.inc"
};

NJS_TEXLIST *chamber_treasure_pieces_texlists[CHAMBER_COLORS + 1] = {
#include "stg25/treasure/chamber_treasure_pieces_texlists.inc"
};

static char *chamber_treasure_color_names[CHAMBER_COLORS] = {
#include "stg25/treasure/chamber_treasure_color_names.inc"
};

static struct debris_part chamber_treasure_blue_parts[PIECES] = {
#include "stg25/treasure/chamber_treasure_blue_parts.inc"
};

static struct debris_part chamber_treasure_green_parts[PIECES] = {
#include "stg25/treasure/chamber_treasure_green_parts.inc"
};

static struct debris_part chamber_treasure_red_parts[PIECES] = {
#include "stg25/treasure/chamber_treasure_red_parts.inc"
};

static struct debris_part *chamber_treasure_parts[CHAMBER_COLORS] = {
#include "stg25/treasure/chamber_treasure_parts.inc"
};

/* the chest's box, which sits on its pos */
/* clang-format off */
static struct collision_shape chamber_treasure_shapes[TREASURE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000,
     {0.0F, BOX_HALF_HEIGHT, 0.0F}, 10.8F, BOX_HALF_HEIGHT, 10.8F, 0,
     {0, 0, 0}},
};
/* clang-format on */

/* TRUE once one call of chamber_treasure_build has compiled all three colors'
 * chests */
static Bool chamber_treasure_built = FALSE;

static struct model_build chamber_treasure_builds[CHAMBER_COLORS] = {
#include "stg25/treasure/chamber_treasure_builds.inc"
};

static treasure_step chamber_treasure_steps[2] = {
#include "stg25/treasure/chamber_treasure_steps.inc"
};

#define TREASURE_WAIT chamber_treasure_wait
#define TREASURE_BROKEN chamber_treasure_broken
#include "stg25/treasure_impl.h"
