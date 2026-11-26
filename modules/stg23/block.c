#include <ninja.h>

#include "collision/collision.h"
#include "stg23/breakables.h"

#include "stg23/block/text.inc"

#define BASE_BLOCK
#define LOOKS 1
#define HIT_DELAY 30
#define PROP_INIT base_block_init
#define QUEUE_MODE block_queue_mode

#include "stg23/block_art.h" /* IWYU pragma: keep -- the data it defines here */

NJS_TEXLIST *base_block_texlists[6] = {
#include "stg23/block/base_block_texlists.inc"
};
/* clang-format off */
static struct collision_shape shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(1),
    SHAPE_HURT_NO_PUSH | SHAPE_HURTABLE, {0.0F, 25.0F, 0.0F}, 25.0F, 25.0F,
    25.0F, 0, {0, 0, 0},
};
/* clang-format on */

#include "stg23/breakable_impl.h" /* IWYU pragma: keep -- the code it compiles here */
