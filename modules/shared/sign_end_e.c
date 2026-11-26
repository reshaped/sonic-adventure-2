/* SIGNENDE in the object lists of stg30 and Final Chase: a model whose
 * texture scrolls */
#include <ninja.h>
#include <sg_xpt.h>
#include "shared/sign_end_e.h"
#include "draw/texture_anim.h"
#include "shared/sign_end_e/text.inc"

static NJS_TEXNAME sign_end_e_textures[1] = {
#include "shared/sign_end_e/sign_end_e_textures.inc"
};

static NJS_TEXLIST sign_end_e_texlist = {
#include "shared/sign_end_e/sign_end_e_texlist.inc"
};

static NJS_TEXNAME sign_end_e_2_textures[1] = {
#include "shared/sign_end_e/sign_end_e_2_textures.inc"
};

static NJS_TEXLIST sign_end_e_2_texlist = {
#include "shared/sign_end_e/sign_end_e_2_texlist.inc"
};

static Sint16 sign_end_e_polygons[77] = {
#include "shared/sign_end_e/sign_end_e_polygons.inc"
};

static Sint32 sign_end_e_vertices[83] = {
#include "shared/sign_end_e/sign_end_e_vertices.inc"
};

static NJS_CNK_MODEL sign_end_e_mesh = {
#include "shared/sign_end_e/sign_end_e_mesh.inc"
};

static NJS_CNK_OBJECT sign_end_e_object = {
#include "shared/sign_end_e/sign_end_e_object.inc"
};

static Sint16 sign_end_e_uvs[50] = {
#include "shared/sign_end_e/sign_end_e_uvs.inc"
};

static struct texture_anim sign_end_e_anim = {
#include "shared/sign_end_e/sign_end_e_anim.inc"
};

NJS_TEXLIST *sign_end_e_texlists[3] = {
#include "shared/sign_end_e/sign_end_e_texlists.inc"
};

/* the scroll's speeds along u and v, in texture coordinates a step */
static Sint32 scroll_u_speed = 8;
static Sint32 scroll_v_speed = 0;

#define SIGN_TEXLIST sign_end_e_texlist
#define SIGN_OBJECT sign_end_e_object
#define SIGN_ANIM sign_end_e_anim
#define SIGN_INIT sign_end_e_init
#include "shared/orbital/sign_scroll_task.h" /* IWYU pragma: keep -- the code it compiles here */
