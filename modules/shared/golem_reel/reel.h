#ifndef SHARED_GOLEM_REEL_REEL_H
#define SHARED_GOLEM_REEL_REEL_H

#include <ninja.h>

static NJS_TEXNAME reel_names[6] = {
#include "shared/golem_reel/reel/reel_names.inc"
};

static NJS_TEXLIST reel_texlist = {
#include "shared/golem_reel/reel/reel_texlist.inc"
};

#endif
