/* DOBJF, CONT and STAGE in Security Hall's object list */
#ifndef STG08_SOLID_PROPS_H
#define STG08_SOLID_PROPS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

extern NJS_TEXLIST *security_container_textures[];
extern NJS_TEXLIST *security_scenery_textures[];
extern NJS_TEXLIST *security_platform_textures[];

/* scl.x picks DOBJF's model, clamped to the models it has. the object shows
 * only on the camera's floor */
void security_scenery_init(struct task *task);
Uint8 *security_scenery_build(Uint8 *buffer);
/* CONT moves along its own x axis, and carries what rides on it */
void security_container_init(struct task *task);
extern NJS_TEXLIST security_container_texlist;
void security_platform_init(struct task *task);
extern NJS_TEXLIST security_platform_texlist;

#endif
