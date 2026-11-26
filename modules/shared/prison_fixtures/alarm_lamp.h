#ifndef SHARED_PRISON_FIXTURES_ALARM_LAMP_H
#define SHARED_PRISON_FIXTURES_ALARM_LAMP_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME alarm_lamp_names[6] = {
#include "shared/prison_fixtures/alarm_lamp/alarm_lamp_names.inc"
};

static NJS_TEXLIST alarm_lamp_texlist = {
#include "shared/prison_fixtures/alarm_lamp/alarm_lamp_texlist.inc"
};

static Sint16 alarm_lamp_head_polygons[111] = {
#include "shared/prison_fixtures/alarm_lamp/alarm_lamp_head_polygons.inc"
};

static Sint32 alarm_lamp_head_vertices[99] = {
#include "shared/prison_fixtures/alarm_lamp/alarm_lamp_head_vertices.inc"
};

static NJS_CNK_MODEL alarm_lamp_head_model = {
#include "shared/prison_fixtures/alarm_lamp/alarm_lamp_head_model.inc"
};

static NJS_CNK_OBJECT alarm_lamp_head_object = {
#include "shared/prison_fixtures/alarm_lamp/alarm_lamp_head_object.inc"
};

static Sint16 alarm_lamp_polygons[343] = {
#include "shared/prison_fixtures/alarm_lamp/alarm_lamp_polygons.inc"
};

static Sint32 alarm_lamp_vertices[231] = {
#include "shared/prison_fixtures/alarm_lamp/alarm_lamp_vertices.inc"
};

static NJS_CNK_MODEL alarm_lamp_model = {
#include "shared/prison_fixtures/alarm_lamp/alarm_lamp_model.inc"
};

static NJS_CNK_OBJECT alarm_lamp_object = {
#include "shared/prison_fixtures/alarm_lamp/alarm_lamp_object.inc"
};

#endif
