/* The flash, its falling drops and the glow on the head share this model. */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_golem/attachment_art.h"
#include "draw/texture_anim.h"
#include "shared/golem_material/names.h"

const char golem_flash_texture[12] = {
#include "boss_golem/flash_art/golem_flash_texture.inc"
};

static NJS_TEXNAME flash_textures[1] = {
#include "boss_golem/flash_art/flash_textures.inc"
};

NJS_TEXLIST golem_flash_texlist = {
#include "boss_golem/flash_art/golem_flash_texlist.inc"
};

static Sint16 flash_polygons[213] = {
#include "boss_golem/flash_art/flash_polygons.inc"
};

static Sint32 flash_vertices[195] = {
#include "boss_golem/flash_art/flash_vertices.inc"
};

static NJS_CNK_MODEL flash_model = {
#include "boss_golem/flash_art/flash_model.inc"
};

NJS_CNK_OBJECT golem_flash_object[1] = {
#include "boss_golem/flash_art/golem_flash_object.inc"
};

/* Each of the eight strips has one scroll key with eight UV pairs. */
static Sint16 flash_uvs[146] = {
#include "boss_golem/flash_art/flash_uvs.inc"
};

struct texture_anim golem_flash_anim = {
#include "boss_golem/flash_art/golem_flash_anim.inc"
};
