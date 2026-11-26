/* the title card a stage opens with: its name, and six overlays that move
 * and fade about it. the name's task holds the card up until play starts */
#ifndef HUD_TITLE_CARD_H
#define HUD_TITLE_CARD_H
#include <ninja.h>
#include <sg_xpt.h>

/* the textures of a character's MISSIONTEX_<code> PVM, by their places in
 * missiontex_texlist. the mission message draws its window and its title
 * from the first four. the card loads the list as it starts, and releases
 * it as it ends */
#define MISSIONTEX_BG 0
#define MISSIONTEX_W0 1
#define MISSIONTEX_W1 2
#define MISSIONTEX_NUMALL 3
#define MISSIONTEX_KAZARI0 4
#define MISSIONTEX_TEXTURES 5
extern NJS_TEXLIST missiontex_texlist;

/* start the current stage's title card, and return TRUE. for a stage
 * without a title, it returns FALSE and starts nothing. it does the same
 * where the heap can't give the title's texture list or the card's task. the
 * stages with a title are those numbered 3 to 14, 16 to 18, 21 to 28, 30 to
 * 32, 40, 43 and 44, with STAGE_RELAY_FIRST, STAGE_CART_1 and STAGE_CHAO.
 *
 * the card loads its title from the PVM stg_title<nn>, nn being the stage
 * in two digits, into a list of its own. STAGE_CART_1 takes stg_title70, or
 * stg_title71 for Rouge. the card loads the mission textures of player 0's
 * character, plays its sound and calls mission_message_load. nothing checks
 * that its six overlays start.
 *
 * the card's task, which shows the stage's name, keeps Start from opening
 * the pause menu. while the name holds, the task starts the mission
 * message. it then records STAGE_END_CARD_SHOWN, which starts play. its end
 * releases the card's textures and lets Start open the pause menu again.
 * nothing of the card draws while game_state.unk_0b is 0 */
Bool title_card_start(void);

#endif
