/* the messenger's texture names and list, models, tuning, trail and builds,
 * which shared/messenger.c reads */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/trail.h"
#include "effect/burst.h"
#include "shared/common_object.h"
#include "shared/messenger_art.h"
#include "shared/sparkle.h"
#include "shared/stage_texture_groups.h"

#include "shared/messenger_art/text.inc"

const struct messenger_texture_filenames messenger_texture_filenames = {
    TEXT_1,
    TEXT_2,
    TEXT_3,
};

static const char trail_filename[13] = {
#include "shared/messenger_art/trail_filename.inc"
};

/* the texture lists and chunk models that 54 images place, from the disc.
 * its D precedes this file's own D */
static NJS_TEXNAME body_textures[3] = {
#include "shared/messenger_art/body_textures.inc"
};
NJS_TEXLIST messenger_texlist = {
#include "shared/messenger_art/messenger_texlist.inc"
};
static Sint16 left_foot_polygons[189] = {
#include "shared/messenger_art/left_foot_polygons.inc"
};
static Sint32 left_foot_vertices[87] = {
#include "shared/messenger_art/left_foot_vertices.inc"
};
static NJS_CNK_MODEL left_foot_model = {
#include "shared/messenger_art/left_foot_model.inc"
};
NJS_CNK_OBJECT messenger_left_foot[1] = {
#include "shared/messenger_art/messenger_left_foot.inc"
};
static Sint16 left_arm_polygons[215] = {
#include "shared/messenger_art/left_arm_polygons.inc"
};
static Sint32 left_arm_vertices[99] = {
#include "shared/messenger_art/left_arm_vertices.inc"
};
static NJS_CNK_MODEL left_arm_model = {
#include "shared/messenger_art/left_arm_model.inc"
};
NJS_CNK_OBJECT messenger_left_arm[1] = {
#include "shared/messenger_art/messenger_left_arm.inc"
};
static Sint16 right_foot_polygons[189] = {
#include "shared/messenger_art/right_foot_polygons.inc"
};
static Sint32 right_foot_vertices[87] = {
#include "shared/messenger_art/right_foot_vertices.inc"
};
static NJS_CNK_MODEL right_foot_model = {
#include "shared/messenger_art/right_foot_model.inc"
};
NJS_CNK_OBJECT messenger_right_foot[1] = {
#include "shared/messenger_art/messenger_right_foot.inc"
};
static Sint16 right_arm_polygons[215] = {
#include "shared/messenger_art/right_arm_polygons.inc"
};
static Sint32 right_arm_vertices[99] = {
#include "shared/messenger_art/right_arm_vertices.inc"
};
static NJS_CNK_MODEL right_arm_model = {
#include "shared/messenger_art/right_arm_model.inc"
};
NJS_CNK_OBJECT messenger_right_arm[1] = {
#include "shared/messenger_art/messenger_right_arm.inc"
};
static Sint16 face_lights_polygons[45] = {
#include "shared/messenger_art/face_lights_polygons.inc"
};
static Sint32 face_lights_vertices[63] = {
#include "shared/messenger_art/face_lights_vertices.inc"
};
static NJS_CNK_MODEL face_lights_model = {
#include "shared/messenger_art/face_lights_model.inc"
};
NJS_CNK_OBJECT messenger_face_lights[1] = {
#include "shared/messenger_art/messenger_face_lights.inc"
};
static Sint16 propeller_polygons[99] = {
#include "shared/messenger_art/propeller_polygons.inc"
};
static Sint32 propeller_vertices[69] = {
#include "shared/messenger_art/propeller_vertices.inc"
};
static NJS_CNK_MODEL propeller_model = {
#include "shared/messenger_art/propeller_model.inc"
};
NJS_CNK_OBJECT messenger_propeller = {
#include "shared/messenger_art/messenger_propeller.inc"
};
static Sint16 head_polygons[555] = {
#include "shared/messenger_art/head_polygons.inc"
};
static Sint32 head_vertices[243] = {
#include "shared/messenger_art/head_vertices.inc"
};
static NJS_CNK_MODEL head_model = {
#include "shared/messenger_art/head_model.inc"
};
NJS_CNK_OBJECT messenger_head = {
#include "shared/messenger_art/messenger_head.inc"
};
static Sint16 winding_key_polygons[137] = {
#include "shared/messenger_art/winding_key_polygons.inc"
};
static Sint32 winding_key_vertices[105] = {
#include "shared/messenger_art/winding_key_vertices.inc"
};
static NJS_CNK_MODEL winding_key_model = {
#include "shared/messenger_art/winding_key_model.inc"
};
NJS_CNK_OBJECT messenger_winding_key[1] = {
#include "shared/messenger_art/messenger_winding_key.inc"
};
static Sint16 body_polygons[193] = {
#include "shared/messenger_art/body_polygons.inc"
};
static Sint32 body_vertices[135] = {
#include "shared/messenger_art/body_vertices.inc"
};
static NJS_CNK_MODEL body_model = {
#include "shared/messenger_art/body_model.inc"
};
NJS_CNK_OBJECT messenger_body[1] = {
#include "shared/messenger_art/messenger_body.inc"
};

static Float unused_rates[2] = {0.0003F, 0.0003F};
Float messenger_accel = 0.003F;
Float messenger_stall = 0.1F;
Float messenger_drag = 0.96F;
Sint32 messenger_twirl_rate = 0x1800;
Sint32 messenger_twirl_held_rate = 0x180;
Sint32 messenger_rotor_idle_rate = 0x111;
Sint32 messenger_rotor_rate = 0x280;
Float messenger_grow_rate = 0.1F;
Float messenger_shrink_rate = 0.05F;
Float messenger_swing_kick = 9.0F;
Float messenger_swing_damping = 0.985F;

static NJS_TEXNAME trail_textures[1] = {
#include "shared/messenger_art/trail_textures.inc"
};
static NJS_TEXLIST trail_texlist = {
#include "shared/messenger_art/trail_texlist.inc"
};

struct trail_style messenger_trail_style = {
#include "shared/messenger_art/messenger_trail_style.inc"
};

NJS_TEXLIST *messenger_texlists[5] = {
#include "shared/messenger_art/messenger_texlists.inc"
};

Sint32 messenger_built = FALSE;
NJS_DIRECT_HEAD *messenger_white_face_build = NULL;

struct model_build messenger_builds[9] = {
#include "shared/messenger_art/messenger_builds.inc"
};
