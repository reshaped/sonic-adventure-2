/* LIGHT in Security Hall's object list: a lamp whose lens glows and fades */
#ifndef STG08_LAMP_H
#define STG08_LAMP_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST *security_lamp_textures[];

/* the object list's init for LIGHT. the placement's ang.y turns the lamp.
 * its lens fades from clear to opaque white and back every 64 steps. a task
 * named ManDraw draws each lamp not wholly behind the camera, up to 256 a
 * frame */
void security_lamp_init(struct task *task);
extern NJS_TEXLIST security_lamp_texlist;

#endif
