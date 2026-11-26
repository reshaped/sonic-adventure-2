/* HAKO in Sand Ocean's object list: a stone box that an attack breaks */
#ifndef STG21_STONE_BOX_H
#define STG21_STONE_BOX_H
#include <ninja.h>

struct task;

/* the box's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *stone_box_texlists[];

/* the object list's step for HAKO. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the box's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. the heap must
 * have room for the whole box's record. without it, the box's step reads
 * through null.
 *
 * task needs a work, whose pos places the box and whose ang.y turns it. each
 * later step tests the range as the first did. an attack is a task whose
 * work's id is WORK_ID_ATTACK. where the latest touch the box's record kept
 * is by one, the box breaks into four pieces. it leaves its base, half its
 * height. the attacker's work takes WORK_HURT. the placement's state keeps
 * the break, so a box made again stands broken */
void stone_box_init(struct task *task);
extern NJS_CNK_MODEL box_base_model;
extern NJS_CNK_MODEL box_part_0_model;
extern NJS_CNK_MODEL box_part_1_model;
extern NJS_CNK_MODEL box_part_2_model;
extern NJS_CNK_MODEL box_part_3_model;
extern NJS_CNK_MODEL stone_box_model;

#endif
