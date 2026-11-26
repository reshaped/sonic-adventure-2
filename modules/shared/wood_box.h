/* the wooden box, CONTWOOD in a stage's object list. a player breaks it for
 * points. a broken box can release one of the stage's small animals. a box
 * falls when a box under it breaks */
#ifndef SHARED_WOOD_BOX_H
#define SHARED_WOOD_BOX_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* build at buf the box's model and its three pieces', each not built yet. a
 * model waits until its texture list is loaded. return buf past what it built.
 * once a single call has built all four, every box draws and throws the
 * builds. a box that draws see-through, near the camera in a boss stage, draws
 * its model instead */
Uint8 *wood_box_build(Uint8 *buf);
/* the object list's step for CONTWOOD. unless placement_try_leave(task) returns
 * TRUE, it gives task the box's step, draw and destructor, and a collision
 * record. the heap must have room for the record, unchecked. task needs a work,
 * and no record in extra, where the box keeps its floor's height. the box
 * enters stack_tasks by stack_add of shared/common_object.h, and falls when a
 * box under it breaks. a box stack_add can't take never falls. player 0's
 * record must exist, unchecked: the steps of a whole or a broken box read its
 * upgrades.
 *
 * a hit breaks the box where player_attacks_task of player/player.h returns
 * TRUE for the hitting task. where that task is a player, the player stops
 * dead. Sonic, Knuckles and Rouge don't stop, nor does a player in
 * PLAYER_MODE_SONIC_BOUNCE or PLAYER_MODE_HUNTER_DIVE.
 *
 * bit 0 of ang.x lets the box's pieces fall through their floor. the low byte
 * of ang.z sets the box's small animal. its high 4 bits are the chance of one,
 * in percent. a chance of 10 or more is certain. its low 4 bits, modulo
 * ANIMAL_RELEASE_KINDS, are the animal's kind, as animal_release in
 * object/animal.h takes it.
 *
 * the box rolls for the animal once its pieces have flown. it rolls only with a
 * chance above 0 and a placement without PLACEMENT_UNK_20. the roll sets that
 * flag, whether the animal comes or not */
void wood_box_init(struct task *task);

/* the box's and its pieces' texture lists, for texture_load_shared, ended by
 * null */
extern NJS_TEXLIST *wood_box_texlists[];

extern NJS_TEXLIST wood_box_texlist;
extern NJS_CNK_MODEL wood_box_model;
extern NJS_CNK_OBJECT wood_box_object;

/* the pieces a box breaks into. Crazy Gadget's floating wooden box throws them
 * too */
extern NJS_TEXLIST wood_box_piece1_texlist;
extern NJS_CNK_MODEL wood_box_piece1_model;
extern NJS_CNK_OBJECT wood_box_piece1_object;

extern NJS_TEXLIST wood_box_piece2_texlist;
extern NJS_CNK_MODEL wood_box_piece2_model;
extern NJS_CNK_OBJECT wood_box_piece2_object;

extern NJS_TEXLIST wood_box_piece3_texlist;
extern NJS_CNK_MODEL wood_box_piece3_model;
extern NJS_CNK_OBJECT wood_box_piece3_object;

#endif
