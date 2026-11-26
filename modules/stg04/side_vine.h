/* White Jungle's side vine, YOKOTUTA in the stage's object list: a vine
 * reaching sideways from its work's pos. a player who catches its tip swings
 * round with it. the vine throws the player once the swing passes 100
 * degrees */
#ifndef STG04_SIDE_VINE_H
#define STG04_SIDE_VINE_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST side_vine_texlist;

/* the object list's step for YOKOTUTA. its first step gives task a record in
 * task->extra, from syCalloc, unchecked, and the destructor frees it. the
 * first step also gives task two children, each with a capsule shape. they
 * stand 13 either side of the tip's rest, along z as ang.y turns it. where
 * task->placement isn't null, the first step sets the placement's state to 1.
 * the vine then ends with task_kill once the state is 0.
 *
 * task needs a work. the vine reaches from pos. ang.y heads it, ang.z tilts
 * it, and ang.x turns the plane it swings in about x. scl.x stretches the
 * swing along z, so that the tip runs on an ellipse. |scl.z| drives the swing,
 * and with scl.y sets the throw's speed.
 *
 * a player whose first shape comes within 10 of the tip catches it. the catch
 * waits until the vine has moved 30 steps since it started or let a player
 * go. the player hangs upright, 9 below the tip. past 100 degrees the vine
 * throws its player by player_push, and rests. the throw goes from the tip's
 * rest toward the top and on.
 *
 * the vine lets its player go where the player has missed or the stage is
 * cleared. it then keeps turning at its speed. from its first let-go on, the
 * vine keeps the stretch it has at a swing of 100 degrees. it also wobbles up
 * to 3 degrees either way while it rests. the vine moves and draws only while
 * a player lies within 3000 of pos */
void side_vine_init(struct task *task);

#endif
