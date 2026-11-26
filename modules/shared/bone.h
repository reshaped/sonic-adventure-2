/* the bone, BONE in Wild Canyon's object list: a cow's bone that a player
 * picks up and carries. the first pickup may release an animal */
#ifndef SHARED_BONE_H
#define SHARED_BONE_H
#include <ninja.h>

struct task;

/* the bone's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *bone_texlists[];

/* the object list's step for BONE. task needs a work and a motion record. the
 * work's pos places the bone, and its ang.y turns it. the bone takes a
 * collision record, and a body of shared/common_object.h. the heap must have
 * room for the body. where it has none for the record, the bone goes on
 * without one. the bone ends where half_try_leave or placement_try_leave
 * returns TRUE.
 *
 * while a player holds the bone, the bone turns with the player. the low byte
 * of ang.z picks an animal, which the bone's first pickup may release. its low
 * 4 bits, modulo 3, give the animal's kind. its high 4 bits, as a count n, make
 * a release's chance 0.05 + n / 10. an n of 0 releases none. a pickup can
 * release only where the bone's placement has a state of 0. it then sets that
 * state to 1 */
void bone_init(struct task *task);

#endif
