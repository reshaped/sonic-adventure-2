/* the Chao pipe, CHAOPIPE in a stage's object list. a near player can take
 * the pipe's action. the pipe then wobbles and lets out a small animal */
#ifndef SHARED_CHAO_PIPE_H
#define SHARED_CHAO_PIPE_H

struct task;

/* the object list's step for CHAOPIPE. task needs a work, whose pos places
 * the pipe and whose ang turns it. task's motion and force must hold no
 * record, since the pipe keeps a Float in each. where the heap can't give the
 * collision record, the pipe has no shape.
 *
 * the pipe offers its action, PLAYER_MODE_UNK_2A, to a player within 13 of its
 * pos. the range is 25 while player 0 is a mech. a player who takes it starts
 * the pipe's wobble, which lets out a small animal. the animal's kind for
 * animal_release of object/animal.h is the work's scl.x, as a Uint32, modulo 3.
 * the pipe then sets its placement's state to 1, and PLACEMENT_UNK_20 of
 * object/set.h in the placement's flags. a pipe without a placement, or whose
 * placement has PLACEMENT_UNK_20, offers nothing */
void chao_pipe_init(struct task *task);

#endif
