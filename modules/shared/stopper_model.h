/* the stopper's texture list, model and solid, which stopper.c draws. Metal
 * Harbor defines its own in stg10/slider_art.c, and the other stages take
 * stopper_art.c's */
#ifndef SHARED_STOPPER_MODEL_H
#define SHARED_STOPPER_MODEL_H
#include <ninja.h>

extern NJS_TEXLIST stopper_texlist;
extern NJS_CNK_OBJECT stopper_object[1];
extern NJS_OBJECT stopper_collision;

#endif
