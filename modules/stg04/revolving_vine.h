/* White Jungle's revolving vine, ROLTUTA in the stage's object list: a vine
 * whose top runs round a circle. a player who catches its tip is carried round
 * as the vine swings out and back. the vine then throws it */
#ifndef STG04_REVOLVING_VINE_H
#define STG04_REVOLVING_VINE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the vine's texture list, of one texture. the stage's own table of texture
 * lists holds it */
extern NJS_TEXLIST white_jungle_revolving_vine_texlist;

/* what the bungee vine's fall speed gains on each step a player hangs from
 * it. the revolving vine defines it, and only the bungee vine reads it */
extern Float white_jungle_vine_gravity;

/* the object list's step for ROLTUTA. its first step gives task a record in
 * task->extra, with a copy of the vine's chain. both come from syCalloc,
 * unchecked, and the destructor frees both. where task->placement isn't null,
 * the first step sets the placement's state to 1. the vine then ends with
 * task_kill once the state is 0.
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
 * waits until the vine has moved 30 steps since it started or its last carry
 * ended. the vine's speed is then the player's, held to 3 to 10. the player
 * hangs at the tip itself. once the vine has run scl.z turns, it throws its
 * player by player_fling. the throw goes along the top's way round, and up.
 * its speed is the vine's, plus ang.z's share. the vine lets its player go only
 * by a throw. it never ends after one, and a player can catch it again.
 *
 * a vine whose scl.z is at least 0 but under 1 / 36000 throws no player. once
 * it slows to a stop, it stops placing its player, but sends it no
 * PLAYER_REQUEST_RELEASE.
 *
 * the vine moves and draws only while a player lies within 4000 of pos */
void white_jungle_revolving_vine_init(struct task *task);

#endif
