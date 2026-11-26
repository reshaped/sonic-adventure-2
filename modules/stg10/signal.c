/* Metal Harbor uses the machine callbacks with its own signal model and
 * collision shape. */
#include <ninja.h>
#include <sg_xpt.h>
#include "stg10/signal/text.inc"
#include "collision/collision.h"

#include "stg10/signal.h"

/* NOLINTBEGIN(readability-identifier-naming) */
#define machine_init signal_init
/* NOLINTEND(readability-identifier-naming) */

#define MACHINE_EDITOR_ROT_Y "<- ROT Y"

#include "shared/prison_machinery/machine_task.h"

static NJS_TEXNAME machine_names[6] = {
#include "stg10/signal/machine_names.inc"
};

static NJS_TEXLIST machine_texlist = {
#include "stg10/signal/machine_texlist.inc"
};

static Sint16 machine_lamp_polygons[25] = {
#include "stg10/signal/machine_lamp_polygons.inc"
};

static Sint32 machine_lamp_vertices[19] = {
#include "stg10/signal/machine_lamp_vertices.inc"
};

static NJS_CNK_MODEL machine_lamp_model = {
#include "stg10/signal/machine_lamp_model.inc"
};

static NJS_CNK_OBJECT machine_lamp = {
#include "stg10/signal/machine_lamp.inc"
};

static Sint16 machine_polygons[321] = {
#include "stg10/signal/machine_polygons.inc"
};

static Sint32 machine_vertices[347] = {
#include "stg10/signal/machine_vertices.inc"
};

static NJS_CNK_MODEL machine_model = {
#include "stg10/signal/machine_model.inc"
};

static NJS_CNK_OBJECT machine_object = {
#include "stg10/signal/machine_object.inc"
};

NJS_TEXLIST *signal_texlists[2] = {
#include "stg10/signal/signal_texlists.inc"
};
/* the machine's one shape. collision_init gives each machine's record its
 * own copy */
static struct collision_shape machine_shapes[MACHINE_SHAPES] = {
#include "stg10/signal/machine_shapes.inc"
};
