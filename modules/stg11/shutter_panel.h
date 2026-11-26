/* an Iron Gate shutter's number plate and the slide of its panels. the draw
 * sets the plate's texture slot to the shutter's number */
#ifndef STG11_SHUTTER_PANEL_H
#define STG11_SHUTTER_PANEL_H
#include "draw/chunk_polygon.h"
struct shutter_plate_polygon {
    struct {
        Uint16 head, words;
        struct part_color diffuse, ambient;
    } material1;
    struct {
        Uint16 head;
        Sint16 slot;
    } texture;
    struct part_strip_header strips1;
    Sint16 length1;
    struct part_uv strip1[4];
    Uint16 end;
};
#ifdef NONMATCHING
/* how far a panel has yet to slide onto the door, from PANEL_SLIDE_START down
 * to 0 */
#define PANEL_SLIDE(task) ((task)->work->scl.z)
#else
/* @hack type: the slide kept as a Float in force's pointer, where work's scl.z
 * would do */
/* @hack type: force read as a Float through its address, where (task)->force.f
 * would do */
#define PANEL_SLIDE(task) (*(Float *)&(task)->force)
#endif
#endif
