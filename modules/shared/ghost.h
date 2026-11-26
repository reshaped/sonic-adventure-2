/* the ghost, an enemy that a stage's object list names "E GHOST". most of its
 * kinds scare a player or chase one */
#ifndef SHARED_GHOST_H
#define SHARED_GHOST_H

struct task;

/* the kinds, named for their titles in the image, such as
 * "< ENEMY GHOST GO AROUND >" */
enum ghost_kind {
    GHOST_GO_AROUND, /* circles home, and scares a player it finds */
    GHOST_HIDE, /* waits unseen at home, and scares a player it finds */
    GHOST_RUSH, /* wanders, and chases a player it finds */
    /* waits at home, chases a player it finds, and holds one it touches */
    GHOST_CAPTURE,
    GHOST_BIG, /* wanders, and shrinks at its first two hits */
    GHOST_FLOAT, /* turns toward a player it finds, and never moves */
    GHOST_MAKIKO, /* waits at home, and scares a player once it is hit */
    GHOST_KINDS
};

/* the object list's step for "E GHOST". its first call sets the ghost up,
 * and each later call runs it. a later call for a task without a parent does
 * nothing more where placement_try_leave returns TRUE. task needs a work, as
 * object/set.h's set manager gives it. the work holds the ghost's entry in
 * the set file:
 *
 * - pos: home;
 * - scl.x: the kind, in its whole part. one that names no kind gives
 *   GHOST_GO_AROUND;
 * - scl.y: the sight. a ghost finds a player within scl.y + 50 of it. a
 *   GHOST_RUSH, a GHOST_CAPTURE, a GHOST_BIG and a GHOST_FLOAT measure across
 *   the ground;
 * - scl.z: the range, less 100. a GHOST_GO_AROUND circles home at that
 *   radius, which must not be 0, unchecked. a wandering GHOST_RUSH or
 *   GHOST_BIG turns back toward home while it lies beyond the range;
 * - ang.y: the heading. a GHOST_RUSH's or a GHOST_BIG's holds its group in
 *   the low byte, as enemy_work_alloc_grouped of enemy/enemy.h takes it.
 *
 * where the heap can't give the record of the ghost's parts, the call sets
 * nothing up. the next call tries again. the heap must also have room,
 * unchecked, for the ghost's enemy_work and its jiggles. a GHOST_RUSH, a
 * GHOST_CAPTURE and a GHOST_BIG need room for a collision record too. each of
 * a GHOST_BIG's two shrinks needs room for three more jiggles. a ghost set up
 * while no task keeps the ghosts' textures loaded needs room for that task.
 *
 * a scare sends the nearest player PLAYER_REQUEST_SCARE where the player
 * stands on the ground or a shape. the player's mode must also lie below
 * PLAYER_MODE_UNK_3B. a GHOST_CAPTURE's touch sends the player
 * PLAYER_REQUEST_CAPTURE. the ghost then holds the player while the player's
 * work stays in PLAYER_MODE_CAUGHT. once a GHOST_CAPTURE has let go or ended a
 * chase, it catches no one. a stage must place a GHOST_CAPTURE where no
 * PLAYER_EGGMAN, PLAYER_SUPER_SONIC or PLAYER_SUPER_SHADOW can touch it,
 * unchecked. a GHOST_CAPTURE that holds one takes an undefined pos.
 *
 * a hit beats every kind but a GHOST_HIDE, which has no collision record,
 * and a GHOST_BIG. a GHOST_BIG shrinks at its first hit and at a hit at least
 * 60 steps later. a hit at least 60 steps after that beats it. a beaten
 * GHOST_RUSH scores 100, or 200 where player_touch_attacks returns TRUE. a
 * beaten GHOST_BIG scores 300, or 600. the other kinds score nothing.
 *
 * a GHOST_RUSH that gives up a chase fades out and goes home, where it fades
 * back in. one that finds a player before it has faded in chases at the fade
 * it has reached, with its shape off. the chase neither fades it in nor turns
 * its shape on, so until it gives up again it can't be hit or touch a player.
 *
 * a beaten ghost ends for good, and so does a GHOST_HIDE that has scared. a
 * GHOST_MAKIKO that has scared ends, but its placement stays active. the set
 * manager makes it again once a player is in range. a GHOST_GO_AROUND that
 * has scared waits unseen until it leaves range.
 *
 * while game_state.time_stopped isn't 0, a ghost doesn't wander, chase,
 * circle or coast. its animation holds too. it still finds and scares
 * players, and a GHOST_CAPTURE still moves with the player it holds. a
 * scare's fade holds, but every other fade goes on, and hits still land */
void ghost_step(struct task *task);

#endif
