/* OIL in Hidden Base's object list: a pumping oil derrick */
#ifndef STG23_OIL_DERRICK_H
#define STG23_OIL_DERRICK_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST oil_derrick_texlist;
extern NJS_TEXLIST *oil_derrick_texlists[];

/* the object list's step for OIL. task needs a work. the derrick turns by ang
 * through njRotateZ, njRotateX and njRotateY, in that order. it scales by
 * scl.x + 1. its motion starts at a random frame. each step adds the integer
 * part of scl.y + 1 to the frame. a frame at or past oil_derrick_shape's count
 * takes the count off. a frame still past it goes to 0 */
void oil_derrick_init(struct task *task);
extern NJS_CNK_OBJECT oil_derrick_object;
extern NJS_MOTION oil_derrick_motion;
extern NJS_MOTION oil_derrick_shape;

#endif
