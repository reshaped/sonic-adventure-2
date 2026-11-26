/* Iron Gate's version of the blinking machine. */
#include <ninja.h>
#include "collision/collision.h"

#include "stg11/machine.h"

/* NOLINTBEGIN(readability-identifier-naming) */
#define machine_init iron_gate_machine_init
/* NOLINTEND(readability-identifier-naming) */

#include "stg11/machine/text.inc"

#define MACHINE_EDITOR_ROT_Y "<- ROT Y"

#include "shared/prison_machinery/machine_task.h"

#include "stg11/fixtures/machine.h"

NJS_TEXLIST *iron_gate_machine_texlists[2] = {
#include "stg11/machine/iron_gate_machine_texlists.inc"
};

/* clang-format off */
static struct collision_shape machine_shapes[MACHINE_SHAPES] = {
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 20.0F, 2.0F}, 10.0F, 20.0F, 5.0F, 0, {0, 0, 0}},
};
/* clang-format on */
