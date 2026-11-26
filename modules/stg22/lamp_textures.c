#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/fixture_models.h"
#include "stg22/prop_textures.h"

static const char miu64_cg022[12] = {
#include "stg22/lamp_textures/miu64_cg022.inc"
};
static const char miu64_cg023[12] = {
#include "stg22/lamp_textures/miu64_cg023.inc"
};
static const char miu64_cg024[12] = {
#include "stg22/lamp_textures/miu64_cg024.inc"
};

struct gadget_lamp_frame gadget_lamp_frames[GADGET_LIGHT_FRAMES] = {
#include "stg22/lamp_textures/gadget_lamp_frames.inc"
};

/* The animated warning lamp. */

static Sint16 gadget_lamp_polygons[51] = {
#include "stg22/lamp_textures/gadget_lamp_polygons.inc"
};

static Sint32 gadget_lamp_vertices[51] = {
#include "stg22/lamp_textures/gadget_lamp_vertices.inc"
};

static NJS_CNK_MODEL gadget_lamp_mesh = {
#include "stg22/lamp_textures/gadget_lamp_mesh.inc"
};

NJS_CNK_OBJECT gadget_lamp_object = {
#include "stg22/lamp_textures/gadget_lamp_object.inc"
};
