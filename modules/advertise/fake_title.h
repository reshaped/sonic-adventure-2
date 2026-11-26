/* the fake title: a title and mode select in the menus' place. it runs
 * under the values of game_state.edition that game_state.h names for it. a
 * choice starts a sequence or a two-player game. MENU_IDLE_STEPS without
 * Start play a demo */
#ifndef ADVERTISE_FAKE_TITLE_H
#define ADVERTISE_FAKE_TITLE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menus.h"

/* the mode select's pages: the modes and the three lists they open. the
 * last page takes the modes' place under EDITION_TENTO. each page has
 * FAKE_TITLE_ROWS rows */
#define FAKE_TITLE_PAGES 5
#define FAKE_TITLE_MODES 0
#define FAKE_TITLE_LIST_0 1
#define FAKE_TITLE_LIST_1 2
#define FAKE_TITLE_LIST_2 3
#define FAKE_TITLE_TENTO 4
#define FAKE_TITLE_ROWS 3

/* the title's backdrop. its level runs from 0, dark, to 1. each step of
 * fake_title_draw.h's task moves it 1/30 toward a target. the target is
 * 0 while shown is 0. while shown is 1, it is 1 if bright is 1 too, and the
 * level dim picks otherwise */
struct fake_title_backdrop {
    Sint32 shown;
    Sint32 bright;
    Sint32 fading; /* 1 if the task's last step moved the fade */
    Sint32 unk_0c;
    /* the prompt draws over the backdrop while it is 1 and the backdrop
     * isn't dark. a step that moves the fade clears it */
    Sint32 prompt;
    Sint32 dim; /* a level of 0.5 while it is 0, and of 0.3 while it is 1 */
};
/* a page of the mode select. the draw's task fades it in while shown is 1,
 * and out while it is 0 */
struct fake_title_page {
    Sint32 shown;
    /* the cursor draws at row while it is 1. the task clears it while the
     * page fades or is hidden */
    Sint32 cursor;
    Sint32 fading; /* 1 if the task's last step moved the fade */
    Sint32 unk_0c;
    Sint32 row; /* the row with the cursor, 0 to FAKE_TITLE_ROWS - 1 */
    /* by row, on FAKE_TITLE_TENTO's page: 1 if the row can't be chosen */
    Sint32 refused[FAKE_TITLE_ROWS];
};
/* what the draw's task fades */
struct fake_title_works {
    struct fake_title_backdrop *backdrop;
    struct fake_title_page *pages; /* FAKE_TITLE_PAGES of them */
};
extern struct fake_title_works fake_title_works;

/* the fake title's menu_set, and a cover's fade, which the draw's task
 * moves toward shown. nothing sets shown, and the task draws nothing for
 * the cover */
struct fake_title_set {
    struct menu_set set;
    Sint32 shown;
    Sint32 unk_18;
    Sint32 fading; /* 1 if the task's last step moved the cover's fade */
    Sint32 unk_20;
};
extern struct fake_title_set fake_title_set;

/* the backdrop's and the pages' textures, which fake_title_draw_create
 * loads */
extern NJS_TEXLIST fake_title_texlist;

/* start the title and its draw's task, and reset the lives. it clears
 * every game flag and game bit, and fades the music in to full volume */
void fake_title_init(void);
/* the program's step while the title runs. it runs every task a frame, and
 * counts game_state.steps. each step sets the fade's color and
 * game_state.idle_dim to 0, and turns Ninja's fog off. it returns
 * OVERLAY_DONE once the title has readied a game or a demo, and
 * OVERLAY_RUNNING otherwise. the reset buttons leave the game for the
 * console's system */
int fake_title_step(void);

#endif
