/* the stage rocket, STROCKET in Final Rush's object list: rocket.h's launcher,
 * quicker to rise, aim and let go */
#ifndef STG30_STAGE_ROCKET_H
#define STG30_STAGE_ROCKET_H
#include <ninja.h>

struct task;

/* the object list's step for STROCKET, whose work is as rocket_init's. once
 * its rocket has risen, a player within 100 of pos takes hold of it unasked */
void stage_rocket_init(struct task *task);
extern NJS_TEXLIST *stage_rocket_texlists[];

#endif
