/* the flute stand, KNUDAI in a stage's object list. the first play of the
 * flute at a stand sets the stand's bit of kept_object_bits. the objects of the
 * other knudai headers wait on that play */
#ifndef SHARED_KNUDAI_H
#define SHARED_KNUDAI_H
#include <ninja.h>

struct task;

/* stands take ids 0 to KNUDAI_IDS - 1. each knudai object reads its stand's id
 * from a field's bits in KNUDAI_ID_MASK, modulo KNUDAI_IDS */
#define KNUDAI_IDS 8
#define KNUDAI_ID_MASK 0xf

/* the object list's step for KNUDAI. task needs a work. its pos places the
 * stand, its ang.y turns it, and its ang.x holds the stand's id. the step does
 * nothing where placement_try_leave(task) returns TRUE. otherwise it takes a
 * record in task->motion, and gives task the stand's step, draw and destructor.
 * where the heap can't give the record, task keeps this step to try again.
 *
 * a player within 38 of pos, as player_near measures, is at the stand. while no
 * message window is open, the stand offers that player PLAYER_MODE_PLAY_FLUTE
 * as its action. once a player at the stand takes that mode, the stand sets bit
 * id of kept_object_bits for good. object/fall.h declares the word. from then
 * on the stand offers PLAYER_MODE_PLAY_FLUTE_AGAIN instead.
 *
 * while its bit is clear, the stand opens a message window for a player who
 * can't play. it opens one for a play again too. by a shipped bug, that window
 * waits on player 0's animation, whichever player is at the stand. while the
 * stand's window is open, the stand turns the player at it toward itself. a
 * stand destroyed while its window is open closes the window and stops every
 * voice */
void knudai_init(struct task *task);

/* an id's stand is the first stand of that id to start, until its destructor
 * runs. each call returns 0 for an id outside 0 to KNUDAI_IDS - 1.
 *
 * where bit id of kept_object_bits is set, knudai_get_pos sets *pos to the
 * origin and returns 1. otherwise it copies the pos of the id's stand's work to
 * *pos and returns 1. where the id has no stand, it returns 0 and leaves *pos
 * as it was.
 *
 * where bit id is set, knudai_played returns 1. otherwise it returns the
 * submode of the id's stand's work, which the stand never sets. it returns 0
 * where the id has no stand */
int knudai_get_pos(int id, NJS_POINT3 *pos);
int knudai_played(int id);

/* the stand's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *knudai_texlists[];

#endif
