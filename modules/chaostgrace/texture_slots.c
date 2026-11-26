/* the race's texture lists, which the scene fills from their archives as a
 * course loads. all but the water's start with no names */
#include <ninja.h>

#include "chaostgrace/banner_draw.h"
#include "chaostgrace/props.h"
#include "chaostgrace/scene.h"

#include "chaostgrace/texture_slots/text.inc"

static NJS_TEXNAME hud_names[13] = {{NULL, 0, 0}};
NJS_TEXLIST race_hud_texlist = {hud_names,
                                sizeof(hud_names) / sizeof(hud_names[0])};

static NJS_TEXNAME fruit_names[23] = {{NULL, 0, 0}};
NJS_TEXLIST race_fruit_texlist = {fruit_names,
                                  sizeof(fruit_names) / sizeof(fruit_names[0])};

static NJS_TEXNAME neutral_names[16] = {{NULL, 0, 0}};
NJS_TEXLIST race_neutral_texlist = {
    neutral_names, sizeof(neutral_names) / sizeof(neutral_names[0])};

static NJS_TEXNAME hero_names[15] = {{NULL, 0, 0}};
NJS_TEXLIST race_hero_texlist = {hero_names,
                                 sizeof(hero_names) / sizeof(hero_names[0])};

static NJS_TEXNAME dark_names[12] = {{NULL, 0, 0}};
NJS_TEXLIST race_dark_texlist = {dark_names,
                                 sizeof(dark_names) / sizeof(dark_names[0])};

static NJS_TEXNAME water_names[23] = {
#include "chaostgrace/texture_slots/water_names.inc"
};
NJS_TEXLIST race_water_texlist = {water_names,
                                  sizeof(water_names) / sizeof(water_names[0])};
