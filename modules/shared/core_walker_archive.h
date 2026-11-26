#ifndef SHARED_CORE_WALKER_ARCHIVE_H
#define SHARED_CORE_WALKER_ARCHIVE_H

#include <ninja.h>

static NJS_TEXNAME walker_object_textures_names[11] = {
#include "shared/core_walker_archive/walker_object_textures_names.inc"
};
static NJS_TEXLIST walker_object_textures = {
#include "shared/core_walker_archive/walker_object_textures.inc"
};

#endif
