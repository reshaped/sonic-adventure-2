#ifndef STG09_FIXTURES_CAGE_DOOR_MATERIALS_H
#define STG09_FIXTURES_CAGE_DOOR_MATERIALS_H

#include <ninja.h>

static NJS_TEXNAME cage_door_names[6] = {
#include "stg09/fixtures/cage_door_materials/cage_door_names.inc"
};

static NJS_TEXLIST cage_door_texlist = {
#include "stg09/fixtures/cage_door_materials/cage_door_texlist.inc"
};

#endif
