/* the chaos enemies, which a stage's object list names "E CHAOS" and
 * "E PATHCHAOS" */
#ifndef SHARED_ENEMY_CHAOS_H
#define SHARED_ENEMY_CHAOS_H

struct task;

/* the kinds of E CHAOS */
enum chaos_kind {
    /* wanders near home. it reaches its two arms toward a player, and fires
     * a laser from each */
    CHAOS_NORMAL,
    /* spins in place, and fires as it turns, pitched toward the player */
    CHAOS_ROUND,
    /* CHAOS_ROUND drawn upside down, which faces and spins the other way */
    CHAOS_ROUND_REVERSE,
    CHAOS_ROUND_PARALLEL, /* CHAOS_ROUND, firing level */
    CHAOS_ROUND_REVERSE_PARALLEL, /* CHAOS_ROUND_REVERSE, firing level */
    /* floats and bobs, and reaches as CHAOS_NORMAL does. it throws back a
     * player that touches it */
    CHAOS_FLOAT,
    /* floats until a player comes near. it then sheds pieces, which crawl
     * after the nearest player and burst */
    CHAOS_SHEDDING,
    CHAOS_NORMAL_REVERSE, /* CHAOS_NORMAL drawn upside down */
    /* CHAOS_NORMAL, which shuts itself in a shell while no player is near */
    CHAOS_GUARD,
    CHAOS_SHEDDING_RED, /* CHAOS_SHEDDING, whose pieces flash red */
    CHAOS_KINDS
};

/* the object list's step for "E CHAOS". its first call sets the chaos up,
 * and each later call runs it. task needs a work and a placement, as
 * object/set.h's set manager gives them. the work holds the chaos's entry in
 * the set file:
 *
 * - scl.x: the kind, rounded. one that names no kind gives CHAOS_NORMAL;
 * - scl.y: the lasers' speed. a shedding kind floats scl.y, rounded, above
 *   home, and sheds 100 times its fraction in pieces;
 * - scl.z: the wake range less 60. a round or shedding kind takes its whole
 *   part for that. its fraction sets the point such a kind measures from, as
 *   below;
 * - ang.x: in its low byte, signed, the lasers' length. a round kind spins
 *   by its high byte, in 256ths of a turn a step. CHAOS_FLOAT's bob takes
 *   the same byte as its phase's high byte;
 * - ang.y: the heading, with the group in its low byte, as
 *   enemy_work_alloc_grouped of enemy/enemy.h takes it;
 * - ang.z: the steps between reaches. a round kind's are the steps from a
 *   shot to the start of its next charge.
 *
 * a shedding kind's pieces stay in a box across x and z. the kind reads the
 * box from ang.x and ang.z. twice the high byte of ang.x, signed, is the
 * box's center less pos in x. twice its low byte is the box's half width in
 * x. ang.z gives z the same way.
 *
 * a player wakes the chaos by coming within the wake range of home across x
 * and z. a round or shedding kind measures from a point ahead of home
 * instead. the point lies 1000 times scl.z's fraction away, taken without its
 * sign. a round reverse kind's point lies behind home. a shedding kind stands
 * h above home. it also wakes only while some player lies within 2h of it in
 * y.
 *
 * one hit beats a chaos, which throws scraps and scores. it bursts and leaves
 * a chaos drive, as enemy/enemy.h's ENEMY_BURST and ENEMY_LEAVES_CHAOS_DRIVE
 * say. where a magic hand took the chaos, the hit also makes it a ball. while a
 * walker's lock holds CHAOS_GUARD, a hit on its shell beats it only where
 * collision_hurt allows. the next call kills task by placement_kill. once a
 * shedding kind has shed, its placement's state is 1. a chaos made again
 * from that placement starts spent, with nothing to shed.
 *
 * each call after the setup returns at once where placement_try_leave
 * returns TRUE. a call without room in the heap for the chaos's state sets
 * nothing up. the next call tries again. the heap must have room for the
 * chaos's enemy_work, its textures' holder and its pieces' collision
 * records, unchecked. without room for the chaos's own collision record,
 * later calls read through null. a first call without room for
 * piece_a_object's builds writes through null. the work of the slot
 * player_nearest gives must exist, unchecked */
void chaos_step(struct task *task);

/* the object list's step for "E PATHCHAOS". its first call sets the chaos
 * up, and each later call runs it. task needs a work and a placement, as
 * object/set.h's set manager gives them. the work holds the chaos's entry in
 * the set file:
 *
 * - pos: the path's origin;
 * - scl: the wake point less pos. each part's fraction, taken without its
 *   sign, also gives a value of its own. the wake range is 10 plus 400 times
 *   scl.x's fraction. the chaos starts from point n of the path, n 100 times
 *   scl.y's fraction. its speed along the path is 10 times scl.z's fraction,
 *   in distance a step;
 * - ang.x: the lasers' speed in bits 0 to 3. bits 4 to 11 hold the
 *   PATH_CHAOS_ flags. bits 12 to 15 are the index of the path, one of the
 *   stage's 12. an index of 12 or more gives path 0;
 * - ang.y: the path's turn about y, with only its high byte kept;
 * - ang.z: in its high byte, the point of the path where the chaos starts
 *   firing. in its low byte, the point where it stops.
 *
 * a point past the path's count stands for the path's start. the chaos
 * waits, undrawn and touching nothing. player 0 or 1 wakes it by coming
 * within the wake range of the wake point. the chaos then flies along the
 * path, and bobs as it goes. on every path but path 4 it leans as the path
 * does. at the path's last segment it flies on in a straight line. under
 * PATH_CHAOS_STOP it hovers there instead, facing the nearest player.
 *
 * the chaos fires two lasers at the nearest player, 40 steps apart. it
 * fires while it hovers. elsewhere it fires while its distance along the
 * path lies between its two firing points. flying on, it keeps the distance
 * at which it left the path.
 *
 * one hit beats the chaos, which throws scraps and scores. it bursts and
 * leaves a chaos drive, as ENEMY_BURST and ENEMY_LEAVES_CHAOS_DRIVE say. where
 * a magic hand took the chaos, the hit also makes it a ball. the next call
 * kills task by placement_kill. while the chaos waits, flies on or hovers, each
 * call returns at once where placement_try_leave returns TRUE. the heap must
 * have room for the chaos's enemy_work, route and collision record, and the
 * textures' holder, unchecked. the work of the slot player_nearest gives must
 * exist, unchecked */
void path_chaos_step(struct task *task);

/* the PATH_CHAOS_ flags, in an E PATHCHAOS's work->tick */
#define PATH_CHAOS_STOP 0x1 /* hover at the path's last segment, not fly on */
#define PATH_CHAOS_SHADOW 0x2 /* draw a shadow */
#define PATH_CHAOS_MIRROR 0x10 /* fly the path with its x negated */

#endif
