/* the new menu's buttons, in groups that swing on to the screen and off it
 * on circles. new_menu_screen.h's screen runs them through new_menu_work */
#ifndef ADVERTISE_NEW_MENU_H
#define ADVERTISE_NEW_MENU_H

#include "advertise/menus.h"

/* the groups, in the work's order. the work holds one more group after
 * them, unused, with no buttons */
#define NEW_MENU_TOP 0
#define NEW_MENU_SINGLE 1
#define NEW_MENU_VERSUS 2
#define NEW_MENU_EXTRAS 3

/* each group's buttons, in their order, with each one's help line as
 * message_tables gives it in English. a top button that opens a group names
 * it */
enum new_menu_top_button {
    NEW_MENU_TOP_SINGLE, /* "Single player mode": NEW_MENU_SINGLE */
    NEW_MENU_TOP_VERSUS, /* "2P versus mode": NEW_MENU_VERSUS */
    NEW_MENU_TOP_EXTRAS, /* "Extra menus": NEW_MENU_EXTRAS */
    NEW_MENU_TOP_OPTIONS, /* "Change options" */
    NEW_MENU_TOP_INTERNET, /* "Connect to Internet" */
    NEW_MENU_TOP_BUTTONS
};
enum new_menu_single_button {
    NEW_MENU_SINGLE_STORY, /* "Play along with the story." */
    NEW_MENU_SINGLE_STAGES, /* "Play the stages you cleared." */
    NEW_MENU_SINGLE_KART, /* "This is the Kart racing game." */
    NEW_MENU_SINGLE_BOSSES, /* "Play boss stages in each Story." */
    NEW_MENU_SINGLE_BUTTONS
};
enum new_menu_versus_button {
    NEW_MENU_VERSUS_BATTLE, /* "There are 3 types of games." */
    NEW_MENU_VERSUS_KART, /* "This is the Kart racing game." */
    NEW_MENU_VERSUS_CHAO_RACE, /* "Chao racing allows up to (8) Chao." */
    NEW_MENU_VERSUS_BUTTONS
};
enum new_menu_extras_button {
    NEW_MENU_EXTRAS_RANKING, /* "Display or update world ranking data." */
    NEW_MENU_EXTRAS_TUTORIAL, /* "Omochao will explain the game." */
    NEW_MENU_EXTRAS_EMBLEMS, /* "Display the list of emblems collected." */
    NEW_MENU_EXTRAS_DOWNLOADS, /* "Play downloaded events." */
    NEW_MENU_EXTRAS_BUTTONS
};

/* the buttons' work, menu_works.work_05 */
extern struct new_menu_work new_menu_work;

/* create the task, newMenuExec, with the whole work cleared and every group
 * swung out. the task's destructor releases the textures */
void new_menu_create(void);
/* clear the work but loaded, and swing every group out at once. the extras'
 * cursor starts on NEW_MENU_EXTRAS_TUTORIAL */
void new_menu_reset(void);
/* load the textures, or release them, and set loaded to match. neither
 * does anything where loaded matches already */
void new_menu_load(void);
void new_menu_release(void);

#endif
