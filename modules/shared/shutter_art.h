/* the nodes of SHUTTER, core_shutter.c's door: its panels, latch, lamps and
 * far face. the door draws each node on its own */
#ifndef SHARED_SHUTTER_ART_H
#define SHARED_SHUTTER_ART_H

#include <ninja.h>

/* @hack type: each node declared as an array of one, where a single object
 * would do */
extern NJS_CNK_OBJECT core_shutter_closed_lamp[1];
extern NJS_CNK_OBJECT core_shutter_far_closed_lamp[1];
extern NJS_CNK_OBJECT core_shutter_far_face[1];
extern NJS_CNK_OBJECT core_shutter_far_lamp[1];
extern NJS_CNK_OBJECT core_shutter_lamp[1];
extern NJS_CNK_OBJECT core_shutter_latch[1];
extern NJS_CNK_OBJECT core_shutter_left[1];
extern NJS_CNK_OBJECT core_shutter_open_lamp[1];
extern NJS_CNK_OBJECT core_shutter_right[1];
extern NJS_CNK_OBJECT core_shutter_top[1];

#endif
