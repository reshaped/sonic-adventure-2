#define CORE_TAILS_LIGHTS
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_machine_lights.h"
#include "shared/core_lamp_art.h"
#include "stg36/route.h"
#include "stg36/entrance_lamps.h"

#include "shared/core_floor_glow_settings.inc"

Uint32 tails_pipe_tint = 0x60606060;

static NJS_CNK_OBJECT *strip_nodes[CORE_STRIPS][LAMPS_PER_STRIP] = {
#include "stg36/machine_lights/strip_nodes.inc"
};
NJS_CNK_OBJECT **core_strip_lamps[CORE_STRIPS] = {
#include "stg36/machine_lights/core_strip_lamps.inc"
};

static NJS_CNK_OBJECT *entrance_nodes[CORE_STRIPS][LAMPS_PER_STRIP] = {
#include "stg36/machine_lights/entrance_nodes.inc"
};
NJS_CNK_OBJECT **tails_entrance_lamps[CORE_STRIPS] = {
#include "stg36/machine_lights/tails_entrance_lamps.inc"
};

#include "shared/core_machine_lights.inc"
