/* the door, CMDOOR in Aquatic Mine's object list, and the sign, SIGN */
#ifndef STG07_CMDOOR_H
#define STG07_CMDOOR_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the door and of the sign,
 * ended by null */
extern NJS_TEXLIST *cmdoor_texlists[];

/* the object list's step for CMDOOR. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the door's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. its one shape
 * is a box that pushes players, enemies and objects. each later step tests the
 * range as the first did. task needs a work, whose pos places the door and
 * whose ang.y turns it */
void cmdoor_init(struct task *task);
/* the object list's step for SIGN. it makes task a model object of the sign,
 * as model_object_init does under OBJECT_MAY_LEAVE, with no collision */
void mine_sign_init(struct task *task);

#endif
