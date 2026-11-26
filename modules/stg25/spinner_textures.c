#include <ninja.h>
#include <sg_xpt.h>

#include "shared/spinner.h"
#include "stg25/spinner_textures/text.inc"

static NJS_TEXNAME textures[2] = {
#include "stg25/spinner_textures/textures.inc"
};

NJS_TEXLIST spinner_texlist = {
#include "stg25/spinner_textures/spinner_texlist.inc"
};

#include "shared/spinner_mesh.h"

static Sint32 spinner_object_child_vertices[515] = {
#include "stg25/spinner_textures/spinner_object_child_vertices.inc"
};

static NJS_CNK_MODEL spinner_object_child_model = {
#include "stg25/spinner_textures/spinner_object_child_model.inc"
};

static NJS_CNK_OBJECT spinner_object_child = {
#include "stg25/spinner_textures/spinner_object_child.inc"
};

NJS_CNK_OBJECT spinner_object = {
#include "stg25/spinner_textures/spinner_object.inc"
};

#include "shared/spinner_motion_keys.h"

NJS_MOTION spinner_motion[1] = {
#include "stg25/spinner_textures/spinner_motion.inc"
};
