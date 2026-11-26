/* MACHINE's model: its belt, and the panel it draws twice. the belt's
 * scroll moves u */
#include <ninja.h>
#include <sg_xpt.h>
#include "stg24/panel_art.h"
#include "draw/texture_anim.h"

static Sint16 engine_machine_object_child_sibling_polygons[25] = {
#include "stg24/engine_machine_art/engine_machine_object_child_sibling_polygons.inc"
};

static Sint32 engine_machine_object_child_sibling_vertices[19] = {
#include "stg24/engine_machine_art/engine_machine_object_child_sibling_vertices.inc"
};

static NJS_CNK_MODEL engine_machine_object_child_sibling_model = {
#include "stg24/engine_machine_art/engine_machine_object_child_sibling_model.inc"
};

static NJS_CNK_OBJECT engine_machine_object_child_sibling = {
#include "stg24/engine_machine_art/engine_machine_object_child_sibling.inc"
};

static Sint16 engine_machine_object_child_polygons[101] = {
#include "stg24/engine_machine_art/engine_machine_object_child_polygons.inc"
};

static Sint32 engine_machine_object_child_vertices[99] = {
#include "stg24/engine_machine_art/engine_machine_object_child_vertices.inc"
};

static NJS_CNK_MODEL engine_machine_object_child_model = {
#include "stg24/engine_machine_art/engine_machine_object_child_model.inc"
};

static NJS_CNK_OBJECT engine_machine_object_child = {
#include "stg24/engine_machine_art/engine_machine_object_child.inc"
};

NJS_CNK_OBJECT engine_machine_object = {
#include "stg24/engine_machine_art/engine_machine_object.inc"
};

static Sint16 engine_machine_scroll_uv_keys[66] = {
#include "stg24/engine_machine_art/engine_machine_scroll_uv_keys.inc"
};

struct texture_anim engine_machine_scroll = {
#include "stg24/engine_machine_art/engine_machine_scroll.inc"
};
