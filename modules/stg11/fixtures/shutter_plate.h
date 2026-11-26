#ifndef STG11_FIXTURES_SHUTTER_PLATE_H
#define STG11_FIXTURES_SHUTTER_PLATE_H

static struct shutter_plate_polygon shutter_plate_polygons = {
#include "stg11/fixtures/shutter_plate/shutter_plate_polygons.inc"
};

static Sint32 shutter_plate_vertices[19] = {
#include "stg11/fixtures/shutter_plate/shutter_plate_vertices.inc"
};

static NJS_CNK_MODEL shutter_plate_model = {
#include "stg11/fixtures/shutter_plate/shutter_plate_model.inc"
};

static NJS_CNK_OBJECT shutter_plate_object = {
#include "stg11/fixtures/shutter_plate/shutter_plate_object.inc"
};

#endif
