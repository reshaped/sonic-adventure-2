/* the searchlight, SEARCHLIGHT in Prison Lane's object list: a lamp whose head
 * tilts up and down */
#ifndef STG09_SEARCHLIGHT_H
#define STG09_SEARCHLIGHT_H
#include <ninja.h>

struct task;

/* the searchlight's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *searchlight_texlists[];

/* the object list's step for SEARCHLIGHT. it does nothing where task leaves at
 * its placement's range. otherwise it gives task the searchlight's step, draw
 * and destructor.
 *
 * task needs a work, whose pos places the searchlight and whose ang.y turns
 * it. each later step tests the range as the first did. it then adds ang.x to
 * the work's timer, an angle. the head tilts about its x by 22.5 degrees times
 * one more than the timer's sine. so it swings between 0 and 45 degrees */
void searchlight_init(struct task *task);

#endif
