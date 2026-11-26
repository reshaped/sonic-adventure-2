#ifndef SHARED_PRISON_FIXTURES_LIFT_TEXTURES_H
#define SHARED_PRISON_FIXTURES_LIFT_TEXTURES_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME lift_names[7] = {
#include "shared/prison_fixtures/lift_textures/lift_names.inc"
};

static NJS_TEXLIST lift_texlist = {
#include "shared/prison_fixtures/lift_textures/lift_texlist.inc"
};

#endif
