/* the emblem results' screen: a task that draws each page's lines with
 * their emblems. emblem_results_menu.h's menu drives it through
 * emblem_results_work */
#ifndef ADVERTISE_EMBLEM_RESULTS_H
#define ADVERTISE_EMBLEM_RESULTS_H
#include <sg_xpt.h>

#include "advertise/menus.h"

/* the screen's pages: one for each story's lines, and one for the Chao,
 * kart and boss lines. the four pages after them, up to
 * EMBLEM_RESULTS_PAGES, are the world ranking's, which nothing shows */
#define EMBLEM_RESULTS_HERO 0
#define EMBLEM_RESULTS_DARK 1
#define EMBLEM_RESULTS_LAST 2
#define EMBLEM_RESULTS_OTHERS 3
#define EMBLEM_RESULTS_PAGES 8

/* the work's message while it shows none */
#define EMBLEM_RESULTS_NO_MESSAGE (-1)

/* the screen's work, menu_works.work_19 */
extern struct emblem_results_work emblem_results_work;

/* create the task, emblemResultExec, and clear the whole work, its loaded
 * flag too. then reset the screen. the task's destructor releases the
 * textures */
void emblem_results_create(void);
/* clear the work but its loaded flag, and gather the pages' lines. every
 * panel's fade goes to 0, and the window and its cursor to the first line */
void emblem_results_reset(void);
/* load the textures, the PVM emblemResult, where the work says they aren't
 * loaded */
void emblem_results_load(void);
/* release the textures, where the work says they are loaded */
void emblem_results_release(void);
/* gather each page's lines from save_data. a line shows a box for each of
 * its emblems. a box holds its emblem once it is won.
 *
 * a stage's line shows once its first mission has a rank. its emblem for a
 * mission is won once the mission has a rank. STAGE_CART_1's lines read
 * their character's record of cart_stage. every other line of the emblem
 * results' pages shows.
 *
 * a locked line shows a locked name. the Chao line is locked until
 * SAVE_CHAO_ENTERED holds. the kart line is locked until either record of
 * cart_stage has a rank in its first mission. the boss line is locked until
 * the hero or the dark story's award is given */
void emblem_results_gather(void);

/* what the line the cursor is on asks the player to play */
enum emblem_results_choice {
    /* the current stage, its mission and player 0's character are set.
     * game_state.next_mode is GAME_MODE_STAGE, for one player */
    EMBLEM_RESULTS_PLAY_STAGE = 0,
    EMBLEM_RESULTS_PLAY_STORY = 1,
    EMBLEM_RESULTS_PLAY_CHAO = 2,
    EMBLEM_RESULTS_PLAY_KART = 3,
    EMBLEM_RESULTS_PLAY_BOSS = 4,
    EMBLEM_RESULTS_NOTHING = 5
};
/* find what the line the cursor is on asks the player to play.
 *
 * a stage's line plays the first of its missions without a rank, as its
 * character. the relay's line plays that mission from STAGE_RELAY_FIRST, as
 * the Tails mech. a character's top line plays the lowest mission short of
 * SAVE_RANK_TOP in the character's played stages. a stage is played once
 * its first mission has been started. the top line plays the first of them
 * short in that mission, as the character.
 *
 * a story's line asks for its story while its award or a later story's
 * isn't given. the Chao line asks once it is unlocked. the kart line asks
 * while the emblem of kart_records[0] or kart_records[1] isn't won. it
 * doesn't ask before either record of cart_stage is played. the boss line
 * asks while one of the SAVE_BOSS_AWARDS awards from SAVE_AWARD_FIRST_BOSS
 * isn't given. it doesn't ask while it is locked */
enum emblem_results_choice emblem_results_choose(void);
/* set the work's message to that of the line the cursor is on. a stage's
 * line takes its character's message, but for the relay's, which has its
 * own. any other line takes its kind's. a locked line takes
 * EMBLEM_RESULTS_NO_MESSAGE */
void emblem_results_pick_message(void);

#endif
