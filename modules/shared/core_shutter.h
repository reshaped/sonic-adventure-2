/* the shutter, SHUTTER in a stage's object list: a door whose panels slide
 * apart once its trigger holds. Cannon's Core, Lost Colony and Eternal Engine
 * place it. colony_shutter.c compiles core_shutter.c again for Lost Colony's
 * and Eternal Engine's, which have a fifth part, the far face */
#ifndef SHARED_CORE_SHUTTER_H
#define SHARED_CORE_SHUTTER_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

extern NJS_TEXLIST core_shutter_texlist;
/* the shutter's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *core_shutter_texlists[];

/* build at buffer the models of the shutter's rigid parts. it returns buffer
 * past the builds. a shutter set up after a call that built them all draws from
 * the builds */
Uint8 *core_shutter_build(Uint8 *buffer);
/* the object list's step for SHUTTER. it does nothing where
 * placement_try_leave(task) returns TRUE. task needs a work, whose pos places
 * the shutter and whose ang.y turns it. the shutter keeps a record of its own
 * in task->motion.
 *
 * ang.z's low two bits choose the trigger, by shared/shutter_motion.h's enum
 * shutter_trigger. ang.x's low byte names BY_SWITCH's switch, BY_ENEMIES's
 * enemy group, or BY_DYNAMITE's charges by their id. BY_DYNAMITE holds once no
 * charge of that id is left. BY_PROXIMITY holds while player 0 or 1 is near,
 * and its shutter closes again once neither is. a shutter of another trigger
 * shuts at once where its trigger stops holding */
void core_shutter_init(struct task *task);

#endif
