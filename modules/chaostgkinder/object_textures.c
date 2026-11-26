#include <ninja.h>

#include "chaostgkinder/objects.h"

#include "chaostgkinder/object_textures/text.inc"

#define TEXTURE(name) {name, 0, 0}

static NJS_TEXNAME textures[42] = {
#include "chaostgkinder/object_textures/textures.inc"
};
NJS_TEXLIST kinder_object_texlist = {textures,
                                     sizeof(textures) / sizeof(textures[0])};
