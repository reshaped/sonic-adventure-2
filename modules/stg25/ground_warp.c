/* Death Chamber's ground warp. The wall warp shares its task callbacks. */
#include "stg25/ground_warp.h"

/* NOLINTBEGIN(readability-identifier-naming) */
#define chamber_wall_warp_init chamber_ground_warp_init
/* NOLINTEND(readability-identifier-naming) */
#define DIGGING(mode)                                                          \
    ((mode) == PLAYER_MODE_HUNTER_DIG ||                                       \
     (mode) == PLAYER_MODE_HUNTER_DIG_OUT || (mode) == PLAYER_MODE_UNK_3F ||   \
     (mode) == PLAYER_MODE_HUNTER_DIVE)
#define DIG_FRAME 0.5F
#define MOVE_LIGHTS (void)0
#define DRAW_TURN njRotateX(NULL, -QUARTER_TURN)
#define DEAD_CALL njCos

#include "stg25/warp_task.h"

NJS_TEXLIST *chamber_ground_warp_texlists[2] = {
#include "stg25/ground_warp/chamber_ground_warp_texlists.inc"
};
/* the box at a size of SIZE_UNIT */
/* clang-format off */
static struct collision_shape chamber_warp_shapes[WARP_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000, {0.0F, 0.0F, 0.0F},
     10.0F, 10.0F, 10.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Float chamber_warp_sizes[WARP_SIZES] = {60.0F, 50.0F, 40.0F};

#include "stg25/warp_task_impl.h" /* IWYU pragma: keep -- the code it compiles here */
