#include <ninja.h>

#include "shared/staff_art.h"
#include "shared/staff_roll.h"

/* a photograph's texture name, padded with spaces to 28 characters as the
 * logos' names are */
#include "shared/staff_art/text.inc"

static NJS_TEXNAME roll_textures[8] = {
#include "shared/staff_art/roll_textures.inc"
};

NJS_TEXLIST staff_roll_texlist = {
#include "shared/staff_art/staff_roll_texlist.inc"
};

static NJS_TEXNAME hero_textures[38] = {
#include "shared/staff_art/hero_textures.inc"
};

NJS_TEXLIST staff_hero_texlist = {
#include "shared/staff_art/staff_hero_texlist.inc"
};

static NJS_TEXNAME dark_textures[47] = {
#include "shared/staff_art/dark_textures.inc"
};

NJS_TEXLIST staff_dark_texlist = {
#include "shared/staff_art/staff_dark_texlist.inc"
};

NJS_TEXANIM staff_roll_cells[STAFF_ROLL_CELLS] = {
#include "shared/staff_art/staff_roll_cells.inc"
};

/* each story's cues, in order of frame. the last, at frame 216000, outlasts
 * any roll */
static struct staff_cue hero_cues[41] = {
#include "shared/staff_art/hero_cues.inc"
};

static struct staff_cue dark_cues[50] = {
#include "shared/staff_art/dark_cues.inc"
};

const struct staff_cue *staff_photo_cues[2] = {
#include "shared/staff_art/staff_photo_cues.inc"
};
