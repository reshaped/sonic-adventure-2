/* the mission message: the mission's title over its text from the MISSION
 * file. it shows in a framed window low on the screen, and the title card
 * starts it */
#ifndef HUD_MISSION_MESSAGE_H
#define HUD_MISSION_MESSAGE_H

/* The matching build reads the stages' mission texts into STAGE_SLOT, from
 * MISSION<letter>.PRS, the letter being game_state.text_language's. where no
 * earlier call's sprite remains, draw the current mission's text into a new
 * sprite. a sprite that remains keeps its own text. then measure the current
 * text into the message's size. where there is no sprite, the size is 16 by 16.
 *
 * STAGE_CART_1 takes the texts of stage 19 for Tails or his mech, and of stage
 * 20 for Rouge. a MISSION_TIMED text is a format, which takes the stage's time
 * limit as its minutes and its seconds. the limit comes from hud/hud.h's
 * time_limits, or from cart_time_limits on STAGE_CART_1. a stage time_limits
 * lacks takes 1 and 0. the formatted text takes a block of the Shinobi heap,
 * unchecked, which the call frees.
 *
 * it reads nothing above STAGE_STG_END but STAGE_CART_1, nor in a two-player
 * game. it stops where the file can't be read or the mission has no text. on
 * STAGE_CART_1 it stops for a driver but Tails, his mech and Rouge.
 *
 * The port validates the selected caption in the disc scratch area and keeps
 * only its rasterized sprite and dimensions. A missing caption omits the
 * message from this title card. */
void mission_message_load(void);
/* start the message's task, MissionMessageDisplayerExecutor, and its window's,
 * MissionWindow. the window opens from its middle. the call loads the text
 * again in the matching build to size the window. The port uses the size
 * measured when the sprite was prepared. It starts nothing
 * where no sprite holds the message or task_create fails. it starts no window
 * where the window's task_create or syCalloc fails.
 *
 * the message shows from the step its timer passes 20 until it passes 135. it
 * then kills its window and itself, and its end destroys the sprite. the
 * message draws nothing while game_state.unk_0b is 0, though its window still
 * does */
void mission_message_start(void);

#endif
