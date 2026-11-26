/* MUJINGUN, a cannon that turns toward player 0 while player 0 is in range. the
 * range is a circle on x and z whose radius is scl.z rounded. its center lies
 * 1000 times scl.z's fraction from pos, toward the placement's -x. in range,
 * the cannon fires with a chance of scl.x * 0.3 percent a step. its init holds
 * scl.x to 100. it holds fire while time stands still. its shots fly at
 * scl.y + 0.5 a step */
#ifndef STG36_CANNON_H
#define STG36_CANNON_H
#include <ninja.h>

#include "draw/texture_anim.h"

struct task;
extern struct texture_anim cannon_barrel_anim;
extern NJS_TEXLIST *core_cannon_texlists[];
void core_cannon_init(struct task *task);

#endif
