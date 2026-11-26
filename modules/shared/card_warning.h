/* the memory card warnings that MCWARN.PRS and ADVERTISE.PRS show at the
 * screen's center. their texts come from the mcwarn file */
#ifndef SHARED_CARD_WARNING_H
#define SHARED_CARD_WARNING_H

struct task;

/* each call returns a new warning's task in level 0, or null where
 * task_create fails. the task ends once its text has faded out. where
 * text_sprite_create fails, the task reads through null.
 *
 * the texts come from ManTxt, a task in level 0 that holds the mcwarn file
 * of game_state.text_language, mcwarn_<letter>.prs. ManTxt reads the file
 * again whenever the language changes. a call that finds no ManTxt creates one
 * after the warning's task. the heap must have room for it, unchecked. where
 * ManTxt's first read fails, ManTxt ends on its next step, and the next call
 * creates another.
 *
 * a warning that reads its text while ManTxt holds no file shows TEXT LOAD
 * ERROR */

/* the warning that no card is in. its text fades in over 60 steps, holds for
 * 1200 and then fades out */
struct task *no_card_warning_create(void);
/* the warning that no card has room for a save of SAVE_BLOCKS blocks. its
 * text shows whole from the start, and fades out when
 * no_card_warning_create's would */
struct task *no_room_warning_create(void);
/* the warning for game_state.card_warning, which the task reads on its first
 * step. it has texts for SAVE_NOT_READY, SAVE_NO_ROOM and SAVE_NOT_OURS. under
 * any other value, the task ends after that step. the text for
 * SAVE_NOT_READY names the port and the socket of game_state.save_drive.
 *
 * the text fades in over 60 steps and holds. it fades out once card_warning
 * is 0. it also fades out at a press of player 0's choose or cancel buttons.
 * a change of card_warning replaces it with the new value's text, which fades
 * in again. neither of the last two acts while player 0 has no pad */
struct task *save_warning_create(void);

#endif
