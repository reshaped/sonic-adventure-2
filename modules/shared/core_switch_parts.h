/* the parts of the clear switch, CLEAR SW, by kind, of which there is one. its
 * plate and base draw with core_switch_texlist. each switch copies
 * core_switch_collision into a node of the world's pool, placed and turned as
 * the switch stands */
#ifndef SHARED_CORE_SWITCH_PARTS_H
#define SHARED_CORE_SWITCH_PARTS_H
#include <ninja.h>

extern NJS_OBJECT core_switch_collision;
extern NJS_CNK_MODEL *core_switch_plates[], *core_switch_bases[];
extern NJS_TEXLIST core_switch_texlist;
extern NJS_TEXLIST *core_switch_texlists[];

#endif
