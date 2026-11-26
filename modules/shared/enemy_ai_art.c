#include <ninja.h>

#include "shared/enemy_ai_art.h"

static const char ai_texture_filenames[50] = {
#include "shared/enemy_ai_art/ai_texture_filenames.inc"
};

static NJS_TEXNAME hunter_textures[5] = {
#include "shared/enemy_ai_art/hunter_textures.inc"
};

NJS_TEXLIST ai_texlist = {
#include "shared/enemy_ai_art/ai_texlist.inc"
};
