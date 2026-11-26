/* the posters, POSTER, POSTERM and POSTER3 in City Escape's object list: flat
 * textured quads */
#ifndef STG13_POSTER_H
#define STG13_POSTER_H
#include <ninja.h>

struct task;

/* the posters' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *poster_texlists[];

/* the object list's steps for POSTER, POSTERM and POSTER3. unless
 * placement_try_leave(task) returns TRUE, each gives task its kind's own step,
 * draw and destructor. each step makes the same test.
 *
 * task needs a work. the poster is a quad 2 * scl.x wide and 2 * scl.y high,
 * about pos. it draws under DRAW_CONTROL_SHADOW of system/util.h. a POSTER and
 * a POSTERM turn by ang.y. scl.z, modulo 10, picks the texture of each, and
 * texture 8 alone draws translucent. a POSTERM tints its quad gray. the
 * gray's red, green and blue are each the low byte of ang.z. a POSTER3 turns by
 * ang about z, then x, then y, and draws translucent */
void poster_init(struct task *task);
void poster_m_init(struct task *task);
void poster_3_init(struct task *task);

#endif
