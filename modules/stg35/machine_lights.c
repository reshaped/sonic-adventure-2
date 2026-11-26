#include <ninja.h>

#include "shared/core_machine_lights.h"
#include "shared/core_lamp_art.h"
#include "stg35/machinery.h"

int eggman_hum_center = 10;
int eggman_hum_amplitude = 20;
int eggman_hum_period = 480;

#include "shared/core_floor_glow_settings.inc"

static NJS_CNK_OBJECT *strip_nodes[CORE_STRIPS][LAMPS_PER_STRIP] = {
#include "stg35/machine_lights/strip_nodes.inc"
};
NJS_CNK_OBJECT **core_strip_lamps[CORE_STRIPS] = {
#include "stg35/machine_lights/core_strip_lamps.inc"
};

#include "shared/core_machine_lights.inc"
