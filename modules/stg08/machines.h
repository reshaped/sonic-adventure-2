/* LAZER, TANK and BOMBE in Security Hall's object list */
#ifndef STG08_MACHINES_H
#define STG08_MACHINES_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

extern NJS_TEXLIST *security_tank_textures[];
extern NJS_TEXLIST *security_cylinder_textures[];

Uint8 *security_laser_build(Uint8 *buffer);
Uint8 *security_cylinder_build(Uint8 *buffer);
void security_laser_init(struct task *task);
void security_tank_init(struct task *task);
void security_cylinder_init(struct task *task);

#endif
