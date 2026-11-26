/* STG26.PRS's chunk models, from the disc. its D lies between
 * guard_sensor_texture.c and guard_exhaust_tex.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg26/guard_art.h"

static Sint16 sensor_polygons[127] = {
#include "stg26/patrol_sensor/sensor_polygons.inc"
};
static Sint32 sensor_vertices[147] = {
#include "stg26/patrol_sensor/sensor_vertices.inc"
};
static NJS_CNK_MODEL sensor_model = {
#include "stg26/patrol_sensor/sensor_model.inc"
};
NJS_CNK_OBJECT guard_sensor_objects[1] = {
#include "stg26/patrol_sensor/guard_sensor_objects.inc"
};
static Sint16 shadow_polygons[365] = {
#include "stg26/patrol_sensor/shadow_polygons.inc"
};
static Sint32 shadow_vertices[189] = {
#include "stg26/patrol_sensor/shadow_vertices.inc"
};
static NJS_CNK_MODEL shadow_model = {
#include "stg26/patrol_sensor/shadow_model.inc"
};
NJS_CNK_OBJECT guard_shadow_objects[1] = {
#include "stg26/patrol_sensor/guard_shadow_objects.inc"
};
