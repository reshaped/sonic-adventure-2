/* Death Chamber's fan, FAN in the stage's object list: a model that spins
 * about its z axis */
#ifndef STG25_FAN_H
#define STG25_FAN_H
#include <ninja.h>

struct task;

/* the fan's texture lists for texture_load_shared: one list, ended by null */
extern NJS_TEXLIST *chamber_fan_texlists[];

/* the object list's step for FAN. task needs a work. its pos places the fan,
 * and its ang.y turns it. scl.x plus 1 scales it along each axis. the fan
 * starts at a random angle about its own z axis. each step turns it by ang.z
 * about that axis */
void chamber_fan_init(struct task *task);

#endif
