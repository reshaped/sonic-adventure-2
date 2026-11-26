/* the common charge, whose assets Sand Ocean's and Hidden Base's charges
 * read too */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "shared/dynamite/text.inc"

/* dynamite_assets.h declares the six assets other charges read. Sand Ocean's
 * charge reads all six, and Hidden Base's all but dynamite_glow_texlist. they
 * are static here, so each stage's symbols.txt gives their addresses */
#ifdef NONMATCHING
/* this side makes the assets external, as dynamite_assets.h declares them.
 * those charges take them by name */
#include "shared/dynamite_assets.h"
#define CHARGE_ASSET
#else
#define CHARGE_ASSET static
#endif

static NJS_TEXNAME dynamite_texnames[2] = {
#include "shared/dynamite/dynamite_texnames.inc"
};
CHARGE_ASSET
NJS_TEXLIST dynamite_texlist = {
#include "shared/dynamite/dynamite_texlist.inc"
};
#define DYNAMITE_POINTS 72
static Sint16 dynamite_polygons[335] = {
#include "shared/dynamite/dynamite_polygons.inc"
};
static Sint32 dynamite_vertices[435] = {
#include "shared/dynamite/dynamite_vertices.inc"
};
static NJS_CNK_MODEL dynamite_model = {
#include "shared/dynamite/dynamite_model.inc"
};
CHARGE_ASSET
NJS_CNK_OBJECT dynamite_object = {
#include "shared/dynamite/dynamite_object.inc"
};
/* the glow: a disc, red at its center and clear at its rim. no code draws it,
 * though the charge loads its texture list */
static NJS_TEXNAME dynamite_glow_texname[1] = {
#include "shared/dynamite/dynamite_glow_texname.inc"
};
CHARGE_ASSET
NJS_TEXLIST dynamite_glow_texlist = {
#include "shared/dynamite/dynamite_glow_texlist.inc"
};
#define GLOW_POINTS 17
static Sint16 dynamite_glow_polygons[141] = {
#include "shared/dynamite/dynamite_glow_polygons.inc"
};
static Sint32 dynamite_glow_vertices[71] = {
#include "shared/dynamite/dynamite_glow_vertices.inc"
};
static NJS_CNK_MODEL dynamite_glow_model = {
#include "shared/dynamite/dynamite_glow_model.inc"
};
static NJS_CNK_OBJECT dynamite_glow_object = {
#include "shared/dynamite/dynamite_glow_object.inc"
};
static NJS_TEXNAME dynamite_blink_texname[1] = {
#include "shared/dynamite/dynamite_blink_texname.inc"
};
CHARGE_ASSET
NJS_TEXLIST dynamite_blink_texlist = {
#include "shared/dynamite/dynamite_blink_texlist.inc"
};
#define BLINK_POINTS 60
static Sint16 dynamite_blink_polygons[263] = {
#include "shared/dynamite/dynamite_blink_polygons.inc"
};
static Sint32 dynamite_blink_vertices[243] = {
#include "shared/dynamite/dynamite_blink_vertices.inc"
};
CHARGE_ASSET
NJS_CNK_MODEL dynamite_blink_model = {
#include "shared/dynamite/dynamite_blink_model.inc"
};
static NJS_CNK_OBJECT dynamite_blink_object = {
#include "shared/dynamite/dynamite_blink_object.inc"
};
/* the glow's texture scroll. each frame moves the v of the glow's 8 strips by
 * 1, the low half of frames. dat_dynamite_0 is its uvs, with a row for each
 * strip's entry */
/* clang-format off */
static Sint16 dat_dynamite_0[98] = {
#include "shared/dynamite/dat_dynamite_0.inc"
};
static struct texture_anim dat_dynamite_1 = {
#include "shared/dynamite/dat_dynamite_1.inc"
};
/* clang-format on */
static NJS_TEXNAME dynamite_flare_texname[1] = {
#include "shared/dynamite/dynamite_flare_texname.inc"
};
CHARGE_ASSET
NJS_TEXLIST dynamite_flare_texlist = {
#include "shared/dynamite/dynamite_flare_texlist.inc"
};

#include "shared/dynamite_task.h" /* IWYU pragma: keep -- the code it compiles here */
