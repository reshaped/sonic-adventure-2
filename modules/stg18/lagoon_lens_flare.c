/* the lens flare of stg18/lagoon_lens_flare.h: shared/canyon_lens_flare.c
 * compiled again, with its own test for hiding. game_state.h declares
 * game_state_land_block_mask_0, which the test reads */
#include "stg18/lagoon_lens_flare.h"
#include "stg18/lagoon_util.h"
#include "system/game_state.h" /* IWYU pragma: keep -- names shared/canyon_lens_flare.c takes */

/* NOLINTBEGIN(readability-identifier-naming) */
#define canyon_lens_flare_init lagoon_lens_flare_init
/* NOLINTEND(readability-identifier-naming) */
#define HIDING_BLOCKS (SOUTH_EAST_BLOCKS | LOW_BLOCKS | HALL_BLOCKS)
/* @hack type: camera 0's land_block_mask read as game_state_land_block_mask_0,
 * where game_state.land_block_mask[0] would do */
#define CANYON_FLARE_HIDE                                                      \
    if (game_state_land_block_mask_0 & HIDING_BLOCKS) {                        \
        return;                                                                \
    }

#include "shared/canyon_lens_flare.c" /* NOLINT(bugprone-suspicious-include) */
