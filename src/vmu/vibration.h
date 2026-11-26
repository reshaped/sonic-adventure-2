/* vibration: shaking a pad through the pack in one of its sockets */
#ifndef VMU_VIBRATION_H
#define VMU_VIBRATION_H

/* vibrate the first ready pack in the sockets of player's pad, which
 * player_pad_ports gives. power is clamped to -7..7, and then 1 and -1
 * become 2 and -2. freq is clamped to 15..59, and time to 0..255. a time
 * above 0 becomes the pack's stop time, and makes the vibration
 * PDD_VIB_FLAG_CONTINUOUS. player must be below PAD_PORTS, unchecked.
 *
 * a task starts the vibration on its second step. nothing starts where
 * game_state.demo isn't 0, game_state.vibration is 0, or player's pad has no
 * ready pack. nor does it start for a player other than 0 in a one-player game
 * outside STAGE_CHAO. it doesn't start where the task or its parameters can't
 * be allocated either. game_state.card_state at CARD_SAVING keeps it from
 * starting too, at the call or at either of the task's steps */
void vibration_start(int player, int power, int freq, int time);
/* the time that gives vibration_start's vibration no stop time */
#define VIBRATION_UNTIMED 0
/* vibrate as vibration_start does, without a stop time, and with the flag
 * PDD_VIB_FLAG_CONVERGENCE. the vibration's inc is 255 / span, rounded down.
 * span is (freq * length) / (power * 4), from the clamped power and freq. a
 * span below 1 is 1. power must not be 0, which span divides by */
void vibration_start_fading(int player, int power, int freq, int length);
/* stop the first ready pack in the sockets of player 0's pad, and in those
 * of player 1's. a vibration whose task has yet to take its second step
 * starts all the same */
void vibration_stop(void);

#endif
