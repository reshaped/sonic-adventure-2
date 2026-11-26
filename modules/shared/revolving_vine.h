/* the revolving vine, ROLTUTA in Green Forest's object list: a vine whose top
 * runs round a circle. a player who catches its tip is carried round as the
 * vine swings out and back. the vine then throws it */
#ifndef SHARED_REVOLVING_VINE_H
#define SHARED_REVOLVING_VINE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the vine's texture list, of one texture. the stage's own table of texture
 * lists holds it */
extern NJS_TEXLIST revolving_vine_texlist;

/* what the bungee vine's fall speed gains on each step a player hangs from
 * it. the revolving vine defines it, and only the bungee vine reads it */
extern Float vine_gravity;
/* a vine's parts after the first lag behind its motion. vine_lag_y scales a
 * step's motion into their turn about y. vine_lag_z scales it into their turn
 * about z. the twining and looping vines read both */
extern Float vine_lag_y;
extern Float vine_lag_z;

/* the object list's step for ROLTUTA. its first step gives task the vine's
 * draw and destructor. it gives task a record in task->extra too, with a copy
 * of the vine's chain. both come from syCalloc, unchecked, and the destructor
 * frees both. where task->placement isn't null, the first step sets the
 * placement's state to 1. the vine then ends with task_kill once the state is
 * 0.
 *
 * task needs a work, whose set file entry gives the vine its settings.
 * - scl.x: the radius of the circle the vine's top runs round. it must not be
 *   0: the vine's turn divides by it.
 * - scl.y: the vine's length is its model's times (scl.y + 1).
 * - scl.z: the turns the vine carries its player before it throws it. the
 *   vine stops at the next whole turn after them.
 * - ang.x: its low half, as an Angle, is the most the vine swings out.
 * - ang.y: turns the circle's start about y.
 * - ang.z: its low half, times 0.1, adds to the throw's speed.
 * the circle's center lies (scl.y + 1) * 100 * cos(ang.x) above pos. it lies
 * scl.x + (scl.y + 1) * 100 * sin(ang.x) from pos along -x, whatever ang.y.
 *
 * a player whose first shape comes within 10 of the tip catches it. the catch
 * waits until the vine has moved 30 steps since its last carry ended. a
 * player near the tip before then starts the count over. the first catch
 * needn't wait. the vine's speed is then the player's, held to 3 to 10. the
 * player hangs at (-2, -7, 0) from the tip, in the tip's axes. once the vine
 * has run scl.z turns, it throws its player by player_fling. the throw goes
 * along the top's way round, and up. its speed is the vine's, plus ang.z's
 * share. a player with PLAYER_MISSED set, or a cleared stage, ends the carry
 * without a throw. the count then starts over.
 * after a throw, the task ends with task_kill on a step on which time moves.
 *
 * a vine whose scl.z is at least 0 but under 1 / 36000 throws no player. once
 * it slows to a stop, it stops placing its player, but sends it no
 * PLAYER_REQUEST_RELEASE.
 *
 * the vine moves and draws only while a player lies within 4000 of pos. it
 * moves only on steps on which time moves. a catch during a time stop moves
 * it through that stop */
void revolving_vine_init(struct task *task);

#endif
