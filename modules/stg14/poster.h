/* the poster, POSTER in Radical Highway's object list: a flat textured quad */
#ifndef STG14_POSTER_H
#define STG14_POSTER_H
#include <ninja.h>

struct task;

/* the posters' texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *highway_poster_texlists[];

/* the object list's step for POSTER. unless placement_try_leave(task) returns
 * TRUE, it gives task the poster's own step, draw and destructor. it also
 * keeps the poster's texture in the work's submode.
 *
 * task needs a work. the poster is a quad of 2 * scl.x by 2 * scl.y about
 * pos. ang turns it about x, then y, then z. scl.z, modulo 2, picks its
 * texture */
void highway_poster_init(struct task *task);

#endif
