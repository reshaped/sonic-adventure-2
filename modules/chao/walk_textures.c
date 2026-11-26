/* the walk menu's two texture lists, one for Japanese and one for the other
 * languages. both start with the same three sheets. each then has six of
 * its language's own. they are the title's two parts, the take-out
 * heading's two, and the confirm and cancel labels */
#include <ninja.h>

#include "chao/walk_art.h"

static const char sheet_1[16] = {
#include "chao/walk_textures/sheet_1.inc"
};
static const char sheet_2[16] = {
#include "chao/walk_textures/sheet_2.inc"
};
static const char sheet_3[16] = {
#include "chao/walk_textures/sheet_3.inc"
};
static const char jp_title_1[16] = {
#include "chao/walk_textures/jp_title_1.inc"
};
static const char jp_title_2[16] = {
#include "chao/walk_textures/jp_title_2.inc"
};
static const char jp_take_out_1[16] = {
#include "chao/walk_textures/jp_take_out_1.inc"
};
static const char jp_take_out_2[16] = {
#include "chao/walk_textures/jp_take_out_2.inc"
};
static const char jp_confirm[12] = {
#include "chao/walk_textures/jp_confirm.inc"
};
static const char jp_cancel[12] = {
#include "chao/walk_textures/jp_cancel.inc"
};
static const char intl_title_1[16] = {
#include "chao/walk_textures/intl_title_1.inc"
};
static const char intl_title_2[16] = {
#include "chao/walk_textures/intl_title_2.inc"
};
static const char intl_take_out_1[16] = {
#include "chao/walk_textures/intl_take_out_1.inc"
};
static const char intl_take_out_2[16] = {
#include "chao/walk_textures/intl_take_out_2.inc"
};
static const char intl_confirm[12] = {
#include "chao/walk_textures/intl_confirm.inc"
};
static const char intl_cancel[12] = {
#include "chao/walk_textures/intl_cancel.inc"
};

static NJS_TEXNAME names_jp[WALK_TEXTURES] = {
#include "chao/walk_textures/names_jp.inc"
};
NJS_TEXLIST walk_textures_jp = {names_jp, WALK_TEXTURES};

static NJS_TEXNAME names_intl[WALK_TEXTURES] = {
#include "chao/walk_textures/names_intl.inc"
};
NJS_TEXLIST walk_textures_intl = {names_intl, WALK_TEXTURES};
