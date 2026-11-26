/* the clouds the missile rises through: puffs and sheets of cloud in a band
 * about camera 0's height */
#ifndef STG10_CLOUDS_H
#define STG10_CLOUDS_H
#include <ninja.h>

struct task;

/* make the clouds' task, CloudExec, for player, at pos's x and z. returns the
 * task, or null where player's slot is empty. it returns null too where the
 * heap can't give the task and its records. where owner isn't null, *owner
 * takes the task. the task's destructor nulls *owner while it still holds the
 * task. the task destroys itself once player's slot is empty */
struct task *clouds_create(int player, NJS_POINT3 *pos, struct task **owner);

/* the clouds' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *clouds_texlists[];

#endif
