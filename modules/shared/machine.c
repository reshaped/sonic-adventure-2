#include <ninja.h>
#include "collision/collision.h"

#include "shared/machine/text.inc"

/* SHC retains the SET editor captions in its constant pool. */
#ifdef NONMATCHING
/* this side marks the caption used, so that GCC keeps it as SHC does. GCC drops
 * a static datum that nothing reads */
__attribute__((used)) static const char machine_editor_rot_y[] =
    MACHINE_EDITOR_ROT_Y;
#endif

#include "shared/prison_machinery/machine_task.h"

#include "stg09/fixtures/machine.h"

NJS_TEXLIST *machine_texlists[2] = {
#include "shared/machine/machine_texlists.inc"
};

/* clang-format off */
static struct collision_shape machine_shapes[MACHINE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 0.0F, 2.0F}, 10.0F, 10.0F, 5.0F, 0, {0, 0, 0}},
};
/* clang-format on */
