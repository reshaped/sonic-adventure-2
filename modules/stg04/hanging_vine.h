/* White Jungle's hanging vine, TATETUTA in the stage's object list: a vine
 * that hangs from its work's pos. a player who catches its tip swings out with
 * it. the vine throws the player once the swing passes 100 degrees */
#ifndef STG04_HANGING_VINE_H
#define STG04_HANGING_VINE_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST hanging_vine_texlist;

/* the object list's step for TATETUTA. its first step gives task a record in
 * task->extra, from syCalloc, unchecked, and the destructor frees it. the
 * first step also gives task two children, each with a capsule shape. they
 * stand 13 either side of the tip's rest, along z as ang.y turns it. where
 * task->placement isn't null, the first step sets the placement's state to 1.
 * the vine then ends with task_kill once the state is 0.
 *
 * task needs a work. the vine hangs from pos. ang.y heads it, ang.z tilts it,
 * and ang.x turns the plane it swings in about y. scl.x stretches the swing
 * along z, so that the tip runs on an ellipse. |scl.z| drives the swing, and
 * with scl.y sets the throw's speed.
 *
 * a player whose first shape comes within 10 of the tip catches it. the catch
 * waits until the vine has moved 30 steps since it started or let a player
 * go. past 100 degrees the vine throws its player by player_push, and rests.
 * the throw goes from the tip's rest toward the top and on. player_face_along
 * turns the player along it. neither ang.y nor ang.z may be a multiple of a
 * half turn. the throw's direction then has a z of 0, which player_face_along
 * refuses.
 *
 * the vine lets its player go where the player has missed or the stage is
 * cleared. it then keeps turning at its speed. the vine moves and draws only
 * while a player lies within 3000 of pos */
void hanging_vine_init(struct task *task);

#endif
