/* Writable Ninja texture entries for efftex_common in archive order. */
#ifndef SHARED_EFFTEX_COMMON_H
#define SHARED_EFFTEX_COMMON_H

#include <ninja.h>

static NJS_TEXNAME efftex_common_texnames[121] = {
#include "shared/efftex_common/efftex_common_texnames.inc"
};
static NJS_TEXLIST efftex_common_texlist = {
#include "shared/efftex_common/efftex_common_texlist.inc"
};

#endif
