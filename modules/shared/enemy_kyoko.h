/* the kyoko, an enemy that a stage's object list names "E KYOKO". it walks or
 * stands by its kind, and most kinds fire at a player near their home */
#ifndef SHARED_ENEMY_KYOKO_H
#define SHARED_ENEMY_KYOKO_H

struct task;

/* the kinds. a gunner is a kind that fires bullets, and a laser kyoko one
 * that fires lasers. a catcher is one that fires capturing bullets. an entering
 * kind acts as the kind its comment names. it first waits at home until a
 * player comes in range, and then climbs by scl.y */
enum kyoko_kind {
    KYOKO_WALKER, /* walks at a player in range, sending out jets */
    KYOKO_GUNNER, /* stands, and fires bursts of bullets */
    KYOKO_WALKING_GUNNER, /* walks at a player in range, firing bursts */
    KYOKO_LASER, /* stands, and fires lasers */
    /* stands, and fires capturing bullets. it walks as KYOKO_WALKER does
     * while the nearest player is in PLAYER_MODE_CAUGHT */
    KYOKO_CATCHER,
    KYOKO_ENTERING_WALKER, /* KYOKO_WALKER */
    KYOKO_ENTERING_GUNNER, /* KYOKO_GUNNER */
    KYOKO_ENTERING_WALKING_GUNNER, /* KYOKO_WALKING_GUNNER */
    KYOKO_ENTERING_LASER, /* KYOKO_LASER */
    KYOKO_ENTERING_CATCHER, /* KYOKO_CATCHER, but it walks without jets */
    KYOKO_KINDS
};

/* the object list's step for "E KYOKO". its first call sets the kyoko up,
 * and each later call runs it. task needs a work and a placement, as
 * object/set.h's set manager gives them. the work holds the kyoko's entry in
 * the set file:
 *
 * - scl.x: the shots' speed. a kind that fires must not have 0, unchecked:
 *   each shot divides by its speed;
 * - scl.y: pos.y stays between home.y and home.y + scl.y. an entering kind
 *   climbs to home.y + scl.y. it must not have 0, unchecked: its climb then
 *   never ends;
 * - scl.z: the range, less 80. a kyoko that walks must have one above -80,
 *   unchecked: its pos can otherwise become NaNs;
 * - ang.x: the kind, in its low 4 bits. 10 to 15 give KYOKO_WALKER. bit 0x10
 *   raises dust, and bit 0x20 gives a gunner bullets that pass through the
 *   stage. a laser kyoko's high byte is its laser's length. the byte must
 *   lie below 0x80, unchecked: the kyoko reads it signed, and
 *   enemy_laser_fire takes no negative length;
 * - ang.y: the heading, with the group in its low byte, as
 *   enemy_work_alloc_grouped of enemy/enemy.h takes it;
 * - ang.z: for a gunner, the shots in a burst, by its low byte, signed. a
 *   burst has one shot at least. for a laser kyoko, the interval in steps, by
 *   its low half, signed.
 *
 * the setup clears ang.x and ang.z once it has read them. home is where the
 * kyoko was set. a player in range is player 0 or 1 within the range of
 * home, across x and z.
 *
 * while a player is in range, a kyoko turns toward the nearest player. it
 * also climbs toward that player's height, but not while it enters. a
 * walking kind walks along its heading within the range, and turns back
 * toward home at its edge. a player's touch knocks it back.
 *
 * a kyoko that fires does so once a player has stayed in range longer than
 * its wait. a gunner's first wait is 60 steps, and a catcher's 180. after
 * each burst, and for a catcher after each catch by its bullet, the wait is
 * 120. a laser kyoko's wait is its interval, and starts over as each charge
 * ends. an entering kind's first wait counts from its setup, whether or not
 * a player is in range.
 *
 * one hit beats a kyoko, which breaks into pieces and scores. it bursts and
 * leaves a chaos drive, as enemy/enemy.h's ENEMY_BURST and
 * ENEMY_LEAVES_CHAOS_DRIVE say. where a magic hand took the kyoko, as
 * ENEMY_IN_MAGIC_HAND says, the hit also makes it a ball. the next call kills
 * task by placement_kill. each call after the setup returns at once where
 * placement_try_leave returns TRUE.
 *
 * the heap must have room for the kyoko's enemy_work, collision record and
 * texture tasks, unchecked. the work of the slot player_nearest gives must
 * exist, unchecked. a kind that fires also needs that work's collision
 * record, unchecked.
 *
 * while game_state.time_stopped is set, a kyoko neither turns, moves nor
 * climbs, and its gun and its action hold. its wait doesn't count. a
 * standing or walking kyoko's wait still starts over while no player is in
 * range. a hit still beats it. a kyoko sending out jets goes on sending
 * them. a gunner stopped as a shot leaves fires a shot each step until its
 * burst ends */
void kyoko_step(struct task *task);

#endif
