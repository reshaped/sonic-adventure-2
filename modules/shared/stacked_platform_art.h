#ifndef SHARED_STACKED_PLATFORM_ART_H
#define SHARED_STACKED_PLATFORM_ART_H

#include <ninja.h>

#include "shared/debris.h"

static NJS_CNK_MODEL *shells[2] = {
#include "shared/stacked_platform_art/shells.inc"
};
static NJS_CNK_MODEL *bases[2] = {
#include "shared/stacked_platform_art/bases.inc"
};
static NJS_TEXLIST *shell_texlists[2] = {
#include "shared/stacked_platform_art/shell_texlists.inc"
};
static NJS_TEXLIST *base_texlists[2] = {
#include "shared/stacked_platform_art/base_texlists.inc"
};
static const char *kind_names[2] = {
#include "shared/stacked_platform_art/kind_names.inc"
};
static struct debris_part fragments[7] = {
#include "shared/stacked_platform_art/fragments.inc"
};

#endif
