/* the texture lists of walker_title_letters.h's two titles, which the rival
 * walker fills from TITLETEX_TAILS and TITLETEX_EGGMAN */
#ifndef SHARED_WALKER_TITLE_TEXTURES_H
#define SHARED_WALKER_TITLE_TEXTURES_H
#include <ninja.h>

static NJS_TEXNAME tails_title_texnames[5] = {
#include "shared/walker_title_textures/tails_title_texnames.inc"
};
static NJS_TEXLIST tails_title_texlist = {
#include "shared/walker_title_textures/tails_title_texlist.inc"
};
static NJS_TEXNAME eggman_title_texnames[8] = {
#include "shared/walker_title_textures/eggman_title_texnames.inc"
};
static NJS_TEXLIST eggman_title_texlist = {
#include "shared/walker_title_textures/eggman_title_texlist.inc"
};

#endif
