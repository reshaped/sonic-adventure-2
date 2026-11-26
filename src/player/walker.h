/* the walkers, the mechs Eggman and Tails drive, and the lock-on of their
 * guns. each is a player whose task is named EggWalker */
#ifndef PLAYER_WALKER_H
#define PLAYER_WALKER_H

struct enemy_work;
struct player;
struct task_work;

/* the size in bytes of a mech's record, which its task's extra points to. its
 * own fields are struct player's by_character.mech */
#define MECH_RECORD_SIZE 0x400

/* a full gauge, and what UPGRADE_EWALKER_EXTRASHIELD adds to it */
#define WALKER_GAUGE_FULL 10.0F
#define WALKER_GAUGE_SHIELD 2.0F

/* the action a walker's list offers while it holds locks. a press takes it,
 * and starts nothing: the locks go once the action button is let go */
#define ACTION_RELEASE 66

/* create Eggman's walker, or Tails's, as the player in slot. the Chao walkers
 * stand in for them in a two-player game where game_state.alt_character[slot]
 * isn't 0. the dark one stands in for Eggman's, the other for Tails's.
 * otherwise a walker of a two-player game takes the look of the costume
 * game_state.costume[slot] names. its record's costume keeps that costume.
 * where task_create or syCalloc fails, no task is left. the loads go unchecked.
 *
 * the walker keeps the upgrades player_register gives it. in a two-player game
 * and in a demo, its booster is its only upgrade: UPGRADE_EWALKER_JETENGINE or
 * UPGRADE_TWALKER_BOOSTER. slot 1 of STAGE_VERSUS_MECH_2 takes
 * UPGRADE_EWALKER_EXTRASHIELD or UPGRADE_TWALKER_LAZERBALSTER besides. slot 1
 * of STAGE_VERSUS_MECH_1 takes none at all. walker_fill_gauge then fills the
 * gauge */
void walker_create_eggman(int slot);
void walker_create_tails(int slot);
/* fill the gauge of record, a mech's, to WALKER_GAUGE_FULL. a record of
 * PLAYER_EGGMAN_MECH with UPGRADE_EWALKER_EXTRASHIELD takes WALKER_GAUGE_SHIELD
 * more. slot 1 takes the shield's share on STAGE_VERSUS_MECH_2 whatever its
 * upgrades, and none on STAGE_VERSUS_MECH_1. on those two stages slot 1's
 * gauge fills whatever its character. elsewhere the gauge of a record of
 * another character stays as it is */
void walker_fill_gauge(struct player *record);

/* the lock-on's calls for its targets. a target's task's motion holds an
 * enemy_work, whose lock_task is the lock that holds it, or null. letting the
 * action button go releases a walker's locks. a release hands them to a score
 * task, which fires their shots */

/* empty enemy's lock_task. a lock there that still holds a target lets it go.
 * where the lock is released, and the target has a placement, placement_stop
 * stops the target first */
void lockon_release(struct enemy_work *enemy);
/* count a hit for a lock's blast on the enemy whose work is work. the call
 * does nothing where:
 * - enemy's lock_task isn't null;
 * - work has no collision record, or the record no latest touch;
 * - the record's own shape in that touch has SHAPE_UNK_800000;
 * - the touch's hit, the record touched, is null.
 * otherwise it starts the collision scans over, and takes the record's first
 * hit by an attack. where that attack is a task of WORK_ID_LOCKON_BLAST, the
 * task's lock counts the hit */
void lockon_count_blast(struct task_work *work, struct enemy_work *enemy);

#endif
