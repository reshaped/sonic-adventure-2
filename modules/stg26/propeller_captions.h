#ifndef STG26_PROPELLER_CAPTIONS_H
#define STG26_PROPELLER_CAPTIONS_H

/* the set editor's labels for the propeller's placement. no code reads them.
 * each size rounds its label up to whole words */
static const char editor_angle_x[20] = {
#include "stg26/propeller_captions/editor_angle_x.inc"
};
static const char editor_angle_y[20] = {
#include "stg26/propeller_captions/editor_angle_y.inc"
};
static const char editor_angle_z[20] = {
#include "stg26/propeller_captions/editor_angle_z.inc"
};
static const char editor_color[20] = {
#include "stg26/propeller_captions/editor_color.inc"
};
static const char editor_collision_on[16] = {
#include "stg26/propeller_captions/editor_collision_on.inc"
};
static const char editor_collision_off[12] = {
#include "stg26/propeller_captions/editor_collision_off.inc"
};
static const char editor_scale[20] = {
#include "stg26/propeller_captions/editor_scale.inc"
};

#endif
