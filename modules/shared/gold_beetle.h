/* the gold beetle, E GOLD in a stage's object list. the beetle hides until a
 * player comes in range. it then appears, hovers a while and vanishes */
#ifndef SHARED_GOLD_BEETLE_H
#define SHARED_GOLD_BEETLE_H

struct task;

/* the object list's step for E GOLD. task needs a work and a placement, as
 * object/set.h's set manager gives them. the heap must have room for the
 * beetle's enemy_work and collision record, unchecked. it must also have room
 * for the task that keeps the beetle's textures loaded, where none does. a
 * later step returns at once where placement_try_leave returns TRUE.
 *
 * a player within scl.z + 60 of pos is in range. the beetle hovers
 * 10 * n + 1 steps, where n is the low byte of ang.x. the count stops while
 * game_state.time_stopped isn't 0. it bobs scl.y above and below where it was
 * set. the bob's angle starts at ang.x with its low byte cleared. each step of
 * the hover adds ang.z with its low byte cleared. a fraction in scl.x leaves
 * the beetle without a shadow.
 *
 * a hit during the hover defeats the beetle. the step after the defeat, or
 * after the beetle vanishes, passes task to placement_kill */
void gold_beetle_step(struct task *task);

#endif
