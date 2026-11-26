/* the title textures. the creator loads one of the lists, by player 1's
 * kind, and its destructor releases it */
#ifndef SHARED_RIVAL_TITLE_TEXTURES_H
#define SHARED_RIVAL_TITLE_TEXTURES_H

#include <ninja.h>

static NJS_TEXNAME title_sonic_texnames[5] = {
#include "shared/rival_title_textures/title_sonic_texnames.inc"
};

static NJS_TEXLIST title_sonic_texlist = {
#include "shared/rival_title_textures/title_sonic_texlist.inc"
};

static NJS_TEXNAME title_shadow_texnames[6] = {
#include "shared/rival_title_textures/title_shadow_texnames.inc"
};

static NJS_TEXLIST title_shadow_texlist = {
#include "shared/rival_title_textures/title_shadow_texlist.inc"
};

#endif
