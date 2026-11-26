/* the mech bosses' gunfire: a flash at a gun's muzzle, and fire that follows
 * the muzzle. Big Foot, Hot Shot and Flying Dog fire them */
#ifndef SHARED_BOSS_GUNFIRE_H
#define SHARED_BOSS_GUNFIRE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;

/* make a muzzle flash at *pos as parent's child, and return its task. it
 * returns null for a null parent, or where task_create_child fails. the flash
 * turns by parent's ang.y at the call, and ang isn't read. it draws three
 * models in turn, and its fourth step kills it. it scales them by a random 1.7
 * up to 3.4 in x and z. it scales them in proportion by 1.05 up to 2.1 in y. it
 * holds still while game_state.time_stopped isn't 0 */
struct task *boss_gunfire_muzzle_create(struct task *parent, NJS_POINT3 *pos,
                                        struct angles *ang);
/* make a fire at *pos, a task named GroboGunFireExecute in level 3. it faces
 * dir's heading, and tilts by dir's pitch about its own z axis. each step puts
 * it at *pos plus *offset as it was at the call, so it follows pos. it takes
 * one of two models, and a random scale of 0.6 up to 1.2 along each axis. its
 * third step kills it. it holds still while game_state.time_stopped or
 * game_state.unk_09 isn't 0.
 *
 * where the heap can't give the task, nothing is made. pos must stay valid
 * while the task lives, and dir must not be the zero vector */
void boss_gunfire_create(NJS_POINT3 *pos, NJS_VECTOR *dir, NJS_VECTOR *offset);

#endif
