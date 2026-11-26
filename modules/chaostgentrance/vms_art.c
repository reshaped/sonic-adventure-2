/* the models of a memory card and of a card of more than one bank. the
 * picker draws one in each card's slot, by the kind of card its drive holds.
 * both draw with AL_ENT_VMS_TEX's textures */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgentrance/fixture_art.h"

static Sint16 vms_polygons[267] = {
#include "chaostgentrance/vms_art/vms_polygons.inc"
};

static Sint32 vms_vertices[315] = {
#include "chaostgentrance/vms_art/vms_vertices.inc"
};

static NJS_CNK_MODEL vms_geometry = {
#include "chaostgentrance/vms_art/vms_geometry.inc"
};

NJS_CNK_OBJECT entrance_vms_model = {
#include "chaostgentrance/vms_art/entrance_vms_model.inc"
};

static Sint16 multibank_polygons[207] = {
#include "chaostgentrance/vms_art/multibank_polygons.inc"
};

static Sint32 multibank_vertices[171] = {
#include "chaostgentrance/vms_art/multibank_vertices.inc"
};

static NJS_CNK_MODEL multibank_geometry = {
#include "chaostgentrance/vms_art/multibank_geometry.inc"
};

NJS_CNK_OBJECT entrance_multibank_model = {
#include "chaostgentrance/vms_art/entrance_multibank_model.inc"
};
