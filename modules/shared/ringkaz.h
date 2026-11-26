/* RINGKAZ in a stage's object list: a ring that turns about its z axis. a
 * texture scrolls on its child. it plays one sound as a player comes near,
 * and another while a player is in range */
#ifndef SHARED_RINGKAZ_H
#define SHARED_RINGKAZ_H
#include <ninja.h>

#include "draw/texture_anim.h"

struct task;

/* ringkaz_anim scrolls the ring's child model */
extern NJS_CNK_OBJECT ringkaz_object;
extern struct texture_anim ringkaz_anim;

extern NJS_TEXLIST ringkaz_texlist;

/* the ring's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *ringkaz_texlists[];

/* the object list's step for RINGKAZ. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the ring's
 * own step, draw and destructor. task needs a work, whose pos places the ring
 * and whose ang turns it. the turn is about x, then y, then z. sound 8 of the
 * stage's sound list plays as a player comes within 15 of pos. sound 7 plays at
 * pos while a player is within 300 */
void ringkaz_init(struct task *task);

#endif
