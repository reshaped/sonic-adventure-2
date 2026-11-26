/* the pumpkin, PICPUM1 in Pumpkin Hill's object list: a pumpkin a player
 * picks up and carries. stg07/pickup.h's pickup is this source compiled a
 * second time */
#ifndef SHARED_PUMPKIN_PICKUP_H
#define SHARED_PUMPKIN_PICKUP_H

#include <ninja.h>

struct task;

/* the pumpkin's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *pumpkin_pickup_texlists[];

/* the object list's step for PICPUM1. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it sets task up as the
 * pumpkin, a body at task->force that rests while it is still. task needs a
 * work and a motion record. the heap must have room for the body: without
 * it, each step reads and writes through null.
 *
 * the work's pos places the pumpkin, and its ang.y turns it until a player
 * carries it. the low byte of ang.z holds a chance and an animal's kind. the
 * pumpkin's first carry releases a small animal of that kind by that chance.
 * the chance is 0.05 plus 0.1 times the byte's high four bits. high bits of 0
 * release none. the low four bits, modulo 3, pick the kind. a carry sets the
 * state of the pumpkin's placement to 1. it releases an animal only where the
 * state was 0 */
void pumpkin_pickup_init(struct task *task);

#endif
