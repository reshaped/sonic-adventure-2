/* the radar, RADER in Metal Harbor's object list: a dish that turns on its
 * base */
#ifndef STG10_RADAR_H
#define STG10_RADAR_H
#include <ninja.h>

struct task;

/* the radar's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *radar_texlists[];

/* the object list's step for RADER. unless placement_try_leave(task) returns
 * TRUE, it gives task the radar's own step, draw and destructor. task needs a
 * work, whose pos places the radar. ang.y is the dish's turn a step, and the
 * base never turns */
void radar_init(struct task *task);

#endif
