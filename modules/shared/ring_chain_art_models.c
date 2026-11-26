/* the chunk models that 54 images place, from the disc. its D lies between
 * ring_chain_art.c and ring.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/ring.h"

static Sint16 ring_chain_root_polygons[11] = {
#include "shared/ring_chain_art_models/ring_chain_root_polygons.inc"
};
static NJS_CNK_MODEL ring_chain_root_model = {
#include "shared/ring_chain_art_models/ring_chain_root_model.inc"
};
NJS_CNK_OBJECT ring_chain_root = {
#include "shared/ring_chain_art_models/ring_chain_root.inc"
};
static Sint16 ring_entry_template_polygons[79] = {
#include "shared/ring_chain_art_models/ring_entry_template_polygons.inc"
};
static Sint32 ring_entry_template_vertices[195] = {
#include "shared/ring_chain_art_models/ring_entry_template_vertices.inc"
};
static NJS_CNK_MODEL ring_entry_template_model = {
#include "shared/ring_chain_art_models/ring_entry_template_model.inc"
};
NJS_CNK_OBJECT ring_entry_template = {
#include "shared/ring_chain_art_models/ring_entry_template.inc"
};
