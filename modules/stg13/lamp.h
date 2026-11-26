/* the lamp, LAMP in City Escape's object list: a lamp post that only draws.
 * nothing collides with it */
#ifndef STG13_LAMP_H
#define STG13_LAMP_H
#include <ninja.h>

struct task;

/* the lamp's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *lamp_texlists[];

/* the object list's step for LAMP. unless placement_try_leave(task) returns
 * TRUE, it gives task the lamp's own step, draw and destructor. the step only
 * makes the same test. task needs a work, whose pos places the lamp and whose
 * ang.y turns it */
void lamp_init(struct task *task);

#endif
