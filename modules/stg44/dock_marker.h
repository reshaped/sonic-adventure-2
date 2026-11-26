/* the dock marker, DMK in Mad Space's object list: a solid marker whose lamp
 * blinks */
#ifndef STG44_DOCK_MARKER_H
#define STG44_DOCK_MARKER_H

#include <ninja.h>

struct task;

/* the object list's step for DMK. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the marker's step, draw and
 * destructor. it puts a copy of mad_space_marker_collision into the world as
 * WORLD_SOLID. the work's pos and three angles place the copy. where the heap
 * can't give the copy, it tries again at task's next step.
 *
 * task needs a work. scl.x picks one of four models: its whole part, taken
 * unsigned, modulo 4. all four collide alike. the work keeps WORLD_ADDED while
 * a player lies within 22 of pos */
void mad_space_marker_init(struct task *task);
/* the markers' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *mad_space_marker_textures[];

#endif
