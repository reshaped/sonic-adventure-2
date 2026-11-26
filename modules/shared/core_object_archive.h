#ifndef SHARED_CORE_OBJECT_ARCHIVE_H
#define SHARED_CORE_OBJECT_ARCHIVE_H

#include <ninja.h>

static NJS_TEXNAME core_object_textures_names[54] = {
#include "shared/core_object_archive/core_object_textures_names.inc"
};
static NJS_TEXLIST core_object_textures = {
#include "shared/core_object_archive/core_object_textures.inc"
};

#endif
