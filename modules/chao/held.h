/* a garden Chao the player holds, throws or strikes */
#ifndef CHAO_HELD_H
#define CHAO_HELD_H
struct task;
/* while the Chao's WORK_HELD_BY_PLAYER is set, start a held behavior unless
 * one runs, and show the Chao's name. returns TRUE where it starts one */
int chao_try_player_hold(struct task *chao);
/* a thrown Chao's behavior. the throw costs the Chao affection and hurts its
 * feelings. the Chao drops its hat against a wall. where it lands, it lies
 * dazed for 180 to 300 steps */
int chao_thrown_behavior(struct task *chao);
/* clear WORK_HURT where it is set, unless the Chao is struck already,
 * evolving or dying. a player that then touches the Chao's body sphere knocks
 * it away and drops its hat. returns TRUE where a player struck it */
int chao_try_knockback(struct task *chao);
/* the behavior of a Chao flying at its mover's spd, as one from a breaking egg
 * does. where it lands, it lies dazed for 180 to 300 steps */
int chao_knockback_behavior(struct task *chao);
/* a jack-in-the-box springs a field as it opens. where the Chao touches it,
 * the Chao falls over crying for 500 steps. returns TRUE where it starts to,
 * and FALSE while it lies crying already */
int chao_try_cry(struct task *chao);
#endif
