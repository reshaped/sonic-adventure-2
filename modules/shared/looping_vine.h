/* the looping vine, ROLTUTAT in a stage's object list: a vine that hangs from
 * a tree. a player who catches its tip swings with it to the trunk. the vine
 * then loops round the trunk and throws its player */
#ifndef SHARED_LOOPING_VINE_H
#define SHARED_LOOPING_VINE_H

#include <ninja.h>

struct task;

/* the one texture, tutates, of vine_preview_object. stg19/object_archives.c
 * loads the list. the debug preview draws that model with vine_texlist */
extern NJS_TEXLIST vine_preview_texlist;

/* the object list's step for ROLTUTAT. it gives task a record in
 * task->extra and a copy of vine_object's chain, both from syCalloc,
 * unchecked. the destructor frees both.
 *
 * task needs a work. the vine's top lies (scl.y + 1) * 285 + 210 above its
 * pos, and ang.y turns it. its swing starts at minus the low 16 bits of
 * ang.x. a player who catches the tip swings the vine on by scl.x degrees, to
 * the trunk. the vine then curls round the trunk. it throws its player once
 * it has curled a turn and a half less ang.z. the throw's speed is the vine's
 * plus scl.z.
 *
 * the vine moves and draws only while a player lies within 4000 of its pos.
 * a player whose first shape comes within 10 of the tip catches it. the vine's
 * speed then becomes the player's times 2.5, held to 3 to 30. it lets its
 * player go at the throw, at the player's miss and at the stage's clear. a
 * catch then waits for 30 steps on which time moves. a player near the tip
 * starts the wait over. the task ends as the wait after a throw ends. it also
 * ends at a step that finds its placement's state 0. each step sets that state
 * to 1 */
void looping_vine_init(struct task *task);

#endif
