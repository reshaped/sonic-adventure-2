/* the bar, BAR in Meteor Herd's object list: a solid object the players stand
 * on */
#ifndef STG32_BAR_H
#define STG32_BAR_H
#include <ninja.h>

struct task;
extern NJS_TEXLIST meteo_bar_texlist;
/* the bar's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *meteo_bar_textures[];

/* the object list's step for BAR. it makes task a solid object of
 * meteo_bar_collision, as solid_object_init does with OBJECT_AS_LAND and
 * OBJECT_MAY_LEAVE. task needs a work, whose pos places the bar and whose ang.y
 * turns it */
void meteo_bar_init(struct task *task);

#endif
