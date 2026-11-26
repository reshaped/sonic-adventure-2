/* TOGE's model, its spikes and base in one */

#include "stg31/prop_art.h"
#include <ninja.h>
#include <sg_xpt.h>

static Sint16 spikes_polygons[277] = {
#include "stg31/spike_art/spikes_polygons.inc"
};

static Sint32 spikes_vertices[235] = {
#include "stg31/spike_art/spikes_vertices.inc"
};

static NJS_CNK_MODEL spikes_model = {
#include "stg31/spike_art/spikes_model.inc"
};

NJS_CNK_OBJECT green_hill_spikes_object[1] = {
#include "stg31/spike_art/green_hill_spikes_object.inc"
};
