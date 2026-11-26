/* the Chao world's texture lists, which its garden areas share. each takes
 * its textures from its PVM by position, through texture_load_pvm */
#include <ninja.h>

#include "chao/egg.h"
#include "chao/garden.h"
#include "chao/garden_scene.h"
#include "chao/icon.h"
#include "chao/model.h"
#include "chao/toy_art.h"
#include "chao/texture_slots.h"

#include "chao/garden_textures/text.inc"

#define TEXLIST(slots) {(slots), sizeof(slots) / sizeof((slots)[0])}

static NJS_TEXNAME body_textures[92] = {{0}};
NJS_TEXLIST chao_body_texlist = TEXLIST(body_textures);

static NJS_TEXNAME common_textures[9] = {
#include "chao/garden_textures/common_textures.inc"
};
NJS_TEXLIST chao_common_texlist = TEXLIST(common_textures);

NJS_TEXLIST chao_eye_texlist = TEXLIST(chao_eye_textures);
NJS_TEXLIST chao_env_texlist = TEXLIST(chao_environment_textures);
NJS_TEXLIST chao_icon_texlist = TEXLIST(chao_icon_textures);
static NJS_TEXNAME jewel_textures[29] = {{0}};
NJS_TEXLIST chao_jewel_texlist = TEXLIST(jewel_textures);
NJS_TEXLIST chao_mouth_texlist = TEXLIST(chao_mouth_textures);

static NJS_TEXNAME minimal_parts_textures[145] = {{0}};
NJS_TEXLIST chao_mini_parts_texlist = TEXLIST(minimal_parts_textures);
static NJS_TEXNAME animal_textures[53] = {{0}};
NJS_TEXLIST garden_animal_texlist = TEXLIST(animal_textures);
static NJS_TEXNAME egg_textures[88] = {{0}};
NJS_TEXLIST egg_texlist = TEXLIST(egg_textures);
static NJS_TEXNAME toy_textures[113] = {{0}};
NJS_TEXLIST garden_toy_texlist = TEXLIST(toy_textures);

static NJS_TEXNAME neutral_textures[13] = {{0}};
NJS_TEXLIST neutral_garden_texlist = TEXLIST(neutral_textures);
static NJS_TEXNAME hero_textures[28] = {{0}};
NJS_TEXLIST hero_garden_texlist = TEXLIST(hero_textures);
static NJS_TEXNAME dark_textures[32] = {{0}};
NJS_TEXLIST dark_garden_texlist = TEXLIST(dark_textures);
