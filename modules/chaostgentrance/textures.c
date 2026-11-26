#include <ninja.h>

#include "chaostgentrance/entrance.h"
#include "chaostgentrance/fixture_textures.h"
#include "system/util.h"

#include "chaostgentrance/textures/text.inc"

#define TEXLIST(array) {array, ARRAY_COUNT(array)}

static NJS_TEXNAME object_textures[ENTRANCE_FIXTURE_TEXTURE_COUNT] = {
#include "chaostgentrance/textures/object_textures.inc"
};
NJS_TEXLIST entrance_object_texlist = TEXLIST(object_textures);

static NJS_TEXNAME common_textures[6] = {{NULL, 0, 0}};
NJS_TEXLIST entrance_common_texlist = TEXLIST(common_textures);

static NJS_TEXNAME japanese_chars[36];
static NJS_TEXNAME japanese_titles[17];
static NJS_TEXNAME english_chars[40];
static NJS_TEXNAME english_titles[15];
static NJS_TEXNAME rival_textures[20];
NJS_TEXLIST entrance_japanese_chars = TEXLIST(japanese_chars);
NJS_TEXLIST entrance_japanese_titles = TEXLIST(japanese_titles);
NJS_TEXLIST entrance_english_chars = TEXLIST(english_chars);
NJS_TEXLIST entrance_english_titles = TEXLIST(english_titles);
NJS_TEXLIST entrance_rival_texlist = TEXLIST(rival_textures);

static NJS_TEXNAME vms_textures[6] = {{NULL, 0, 0}};
NJS_TEXLIST entrance_vms_texlist = TEXLIST(vms_textures);
static NJS_TEXNAME map_textures[11];
NJS_TEXLIST entrance_map_texlist = TEXLIST(map_textures);
