/* The first walker duel uses the carrier background and deck lights. */
#include "draw/light.h"

#include "shared/carrier_background_names.h" /* IWYU pragma: keep -- the data it defines here */

#include "shared/carrier_background_groups.h" /* IWYU pragma: keep -- the data it defines here */
#include "shared/carrier_water.h" /* IWYU pragma: keep -- the data it defines here */

static struct light deck_lights[2] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};

#define CARRIER_DUEL

#include "shared/carrier_stage_task.h" /* IWYU pragma: keep -- the code it compiles here */
