#include <ninja.h>
#include <sg_xpt.h>
#include "collision/collision.h"

#include "shared/fluorescent_lamp/text.inc"

#ifdef NONMATCHING
/* this side keeps the set editor's captions as arrays of their own. GCC drops
 * the literals fluorescent_lamp_step names */
__attribute__((used)) static const char fluorescent_lamp_editor_angle_x[] =
    FLUORESCENT_LAMP_EDITOR_ANGLE_X;
__attribute__((used)) static const char fluorescent_lamp_editor_angle_y[] =
    FLUORESCENT_LAMP_EDITOR_ANGLE_Y;
__attribute__((used)) static const char fluorescent_lamp_editor_angle_z[] =
    FLUORESCENT_LAMP_EDITOR_ANGLE_Z;
#endif

#include "shared/prison_machinery/fluorescent_lamp_task.h"

#include "shared/prison_machinery/fluorescent_lamp_art.h"

NJS_TEXLIST *fluorescent_lamp_texlists[2] = {
#include "shared/fluorescent_lamp/fluorescent_lamp_texlists.inc"
};

static Float lit_least = 1.0F;

static Float lit_range = 100.0F;

static Float dark_least = 1.0F;

static Float dark_range = 2.0F;

/* clang-format off */
static struct collision_shape
    fluorescent_lamp_shapes[FLUORESCENT_LAMP_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     0, {0.0F, 0.0F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
