#include <ninja.h>

#include "collision/collision.h"
#include "stg23/breakables.h"

#include "stg23/wall/text.inc"

#define LOOKS 2
#define HIT_DELAY 10
#define PROP_INIT base_wall_init
#define QUEUE_MODE wall_queue_mode

#include "stg23/wall_art.h" /* IWYU pragma: keep -- the data it defines here */

NJS_TEXLIST *base_wall_texlists[11] = {
#include "stg23/wall/base_wall_texlists.inc"
};
static struct collision_shape shape = {
#include "stg23/wall/shape.inc"
};

#include "stg23/breakable_impl.h" /* IWYU pragma: keep -- the code it compiles here */
