/* the emi, an enemy that a stage's object list names "E EMI". most kinds
 * walk about their home, and charge at a player who comes near home */
#ifndef SHARED_ENEMY_EMI_H
#define SHARED_ENEMY_EMI_H

struct task;

/* the kinds, named for the titles the image gives them, as
 * "< ENEMY EMI NORMAL >". EMI_NORMAL, EMI_SPIKED and EMI_ARMORED walk */
enum emi_kind {
    EMI_NORMAL,
    EMI_SPIKED, /* EMI_NORMAL, with a top that attacks at level 2 */
    EMI_DOOL, /* EMI_NORMAL, standing where it was set */
    EMI_CANNON, /* stands where it was set, and throws bombs */
    EMI_ARMORED, /* guards at level 2, and throws back a player */
    EMI_KINDS
};

/* the object list's step for "E EMI". its first call sets the emi up, and
 * each later call runs it. task needs a work and a placement, as
 * object/set.h's set manager gives them. the work holds the emi's entry in
 * the set file:
 *
 * - scl.x: the kind, by its whole part. one that names no kind gives
 *   EMI_NORMAL;
 * - scl.y: a cannon's interval, in steps;
 * - scl.z: the range, less 80;
 * - ang.x: in its low byte, signed, the bomb_kind of a cannon's bombs, as
 *   shared/enemy_bomb.h names them. one outside 0 to BOMB_KINDS - 1 gives 0;
 * - ang.y: the heading. every kind but EMI_ARMORED holds its group in the
 *   low byte, as enemy_work_alloc_grouped of enemy/enemy.h takes it.
 *
 * the setup moves pos.y down to the highest floor at or below pos, where
 * there is one. it then clears ang.x and ang.z.
 *
 * a walking kind walks along its heading. it turns toward home once it lies
 * farther than the range from home, in all three axes. it turns toward a
 * point at random near home once it meets an object. within the range of
 * home, it spots the nearest player where two tests hold. that player must
 * lie within the range of the emi in y. player 0 or 1 must lie within the
 * range of home, across x and z. the emi then aims for more than 100 steps,
 * turning toward the nearest player. it then charges toward a point at or
 * near the player it spotted. a charge ends after more than 180 steps, or
 * once player 0 lies behind the emi. the emi then brakes, and turns toward a
 * point near home.
 *
 * a dool and a cannon never move. a cannon turns toward the nearest player.
 * it starts a shot once more than the interval has passed since the setup or
 * its last bomb. each shot throws a bomb toward the nearest player, as
 * enemy_bomb_create's BOMB_THROWN starts it.
 *
 * one hit beats an emi, and the next call breaks it into pieces and kills
 * task by placement_kill. a beaten emi bursts and leaves a chaos drive, as
 * enemy/enemy.h's ENEMY_BURST and ENEMY_LEAVES_CHAOS_DRIVE say. where a magic
 * hand took the emi, as ENEMY_IN_MAGIC_HAND says, the hit also makes it a ball.
 *
 * a touch that doesn't beat an EMI_ARMORED can knock it back. Sonic and
 * Shadow knock it back in the modes from PLAYER_MODE_SONIC_COMBO_1 to
 * PLAYER_MODE_SONIC_COMBO_RUNNING_3.
 * Knuckles knocks it back in the modes from PLAYER_MODE_HUNTER_ATTACK_1 to
 * PLAYER_MODE_HUNTER_RUN_ATTACK_3. any other touch by player 0 or 1 off the
 * ground throws that player back.
 *
 * a player within the placement's leave range of home keeps the emi in
 * range, however far it strays. otherwise each call after the setup returns
 * at once where placement_try_leave returns TRUE. the heap must have room for
 * the emi's enemy_work, collision record and texture tasks, unchecked. the work
 * of the slot player_nearest gives must exist, unchecked. a charge reads player
 * 0's work, which must exist too, unchecked.
 *
 * while game_state.time_stopped is set, an emi doesn't move, and its action
 * doesn't play. a cannon still turns toward the nearest player, and still
 * counts the steps since its last bomb. a cannon stopped as its shot throws a
 * bomb then throws one each time more than the interval passes */
void emi_step(struct task *task);

#endif
