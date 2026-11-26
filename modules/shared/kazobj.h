/* KAZOBJ, the scenery Eternal Engine's and Lost Colony's set files place */
#ifndef SHARED_KAZOBJ_H
#define SHARED_KAZOBJ_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* Eternal Engine defines lists 0 to 3 and 5 to 7, and Lost Colony 0 to 5 */
extern NJS_TEXLIST kazobj_texlist_0;
extern NJS_TEXLIST kazobj_texlist_1;
extern NJS_TEXLIST kazobj_texlist_2;
extern NJS_TEXLIST kazobj_texlist_3;
extern NJS_TEXLIST kazobj_texlist_4;
extern NJS_TEXLIST kazobj_texlist_5;
extern NJS_TEXLIST kazobj_texlist_6;
extern NJS_TEXLIST kazobj_texlist_7;

/* compile the model of each KIND_BUILD kind at buffer for njDirectDrawObject,
 * and return buffer past them. a kind built already is passed over. a model
 * whose build wouldn't end below DEMO_AREA, of system/areas.h, isn't built,
 * and draws by njCnkEasyDrawModel */
Uint8 *kazobj_build(Uint8 *buffer);

/* the object list's step for KAZOBJ. it does nothing where
 * placement_try_leave(task) returns TRUE. task needs a work. scl.x, held to the
 * stage's kinds, picks the kind. pos places its model, and ang.y turns it. a
 * kind with shapes registers them in a COLLISION_OBJECT record each step */
void kazobj_init(struct task *task);

#endif
