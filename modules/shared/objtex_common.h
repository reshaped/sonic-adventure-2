/* Writable Ninja texture entries for objtex_common in archive order. */
#ifndef SHARED_OBJTEX_COMMON_H
#define SHARED_OBJTEX_COMMON_H

#include <ninja.h>

static NJS_TEXNAME objtex_common_texnames[87] = {
#include "shared/objtex_common/objtex_common_texnames.inc"
};
static NJS_TEXLIST objtex_common_texlist = {
#include "shared/objtex_common/objtex_common_texlist.inc"
};

#endif
