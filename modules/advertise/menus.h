/* the menus: the title screen and every menu after it, each a mode of
 * menu_control */
#ifndef ADVERTISE_MENUS_H
#define ADVERTISE_MENUS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_util.h"
#include "system/menu_snapshot.h" /* IWYU pragma: export */

struct save_time;

/* the menu that runs, as menu_control.mode holds it. menus_step runs none
 * under MENU_MODE_UNK_0E or MENU_MODE_UNK_11, which no source sets */
enum menu_mode {
    MENU_MODE_TITLE = 0, /* title_screen.h's */
    MENU_MODE_NEW_MENU = 1, /* new_menu_screen.h's */
    MENU_MODE_STORY_SELECT = 2,
    MENU_MODE_STAGE_MAP = 3, /* stage_map_menu.h's */
    MENU_MODE_VERSUS_SELECT = 4,
    MENU_MODE_SYNOPSIS = 5, /* synopsis_screen.h's */
    MENU_MODE_OPTIONS = 6,
    MENU_MODE_TV_SETTING = 7,
    MENU_MODE_SOUND_TEST = 8,
    MENU_MODE_FILE_SELECT = 9,
    MENU_MODE_KART_RACE = 10,
    MENU_MODE_DOWNLOAD = 11,
    MENU_MODE_BOSS_ATTACK = 12,
    MENU_MODE_TUTORIAL = 13,
    MENU_MODE_UNK_0E = 14,
    /* world_ranking_menu.h's, whose step does nothing */
    MENU_MODE_WORLD_RANKING = 15,
    MENU_MODE_EMBLEM_RESULTS = 16, /* emblem_results_menu.h's */
    MENU_MODE_UNK_11 = 17
};

/* sounds of list 0, the main executable's, which sound_load_menus puts in
 * place 0. the cursor's and a choice's are sound.h's paused ids */
#define MENU_SOUND_START 0x8008 /* Start pressed at a title */
#define MENU_SOUND_BACK 0x8009
#define MENU_SOUND_REFUSED 0x800a /* a row that can't be chosen */
/* sounds of the program's own list, which sound_load_menus puts in place 5 */
#define MENU_SOUND_NONE_FOUND 5 /* a search that found no download file */
#define MENU_SOUND_RACE 6 /* a kart race chosen */

/* the flag that has menus_init put back the save_data save_keep kept. the
 * title screen sets it as it starts a demo */
#define MENU_FLAG_SAVE_KEPT 35

/* the titles' prompt shows while bit MENU_PROMPT_SHIFT of game_state.steps
 * is set: 64 steps shown, then 64 hidden */
#define MENU_PROMPT_SHIFT 6
/* the steps without Start after which a title plays a demo */
#define MENU_IDLE_STEPS 1200
/* the demos each title plays in turn, the next at game_state.next_demo */
#define MENU_DEMOS 6
/* a demo's stage that plays SEQUENCE_TAIKEN_EVENT instead */
#define MENU_NO_STAGE (-1)
struct menu_demo {
    Sint32 stage; /* a stage, or MENU_NO_STAGE */
    Sint32 character; /* player 0's */
};

/* the running menu and its step, which menus_init clears */
struct menu_control {
    enum menu_mode mode;
    Sint32 step; /* the running menu's own, from 0, which its start sets */
    Sint32 timer; /* the title screen's count of steps without Start */
    Sint32 fade_held; /* menus_step steps the fade in only while it is 0 */
};
extern struct menu_control menu_control;

/* while it is TRUE, the menus end with OVERLAY_BACK, for the browser, in
 * place of OVERLAY_DONE. the new menu's Internet button sets it */
extern Bool menus_leave_game;

/* Work 0 belongs to the alpha title and work 12 to file select.
 * menu_textures loads and releases each work's textures by this index. */
struct menu_works {
    struct alpha_title_work *alpha_title;
    struct main_menu_work *work_01;
    struct stage_map_work *work_02;
    struct story_select_work *work_03;
    struct versus_select_work *work_04;
    struct new_menu_work *work_05;
    struct synopsis_work *work_06;
    struct screen_fade_work *work_07;
    struct options_work *work_08;
    struct tv_setting_work *work_09;
    struct menu_sound_test_work *work_10;
    struct share_data_work *work_11;
    struct file_select_work *work_12;
    struct kart_race_work *work_13;
    struct download_panel_work *work_14;
    struct boss_attack_work *work_15;
    struct tutorial_work *work_16;
    struct screen_effect_work *work_17;
    struct world_ranking_work *work_18;
    struct emblem_results_work *work_19;
};
extern struct menu_works menu_works;
/* The versus menus use an array spelling for this same table in SHC. */
#ifdef NONMATCHING
/* the table by its one name: this side's objects don't define menu_works_array,
 * which only symbols.txt places, at menu_works */
#define menu_works_array (&menu_works)
#else
extern struct menu_works menu_works_array[];
#endif

/* the record of one of the program's two halves, the fake title or the
 * menus. works points at the half's works. nothing reads either field */
struct menu_set {
    Sint32 unk_00[4];
    const void *works;
};
extern struct menu_set menus_set; /* the menus' */

/* start the menus: create their tasks, clear menu_control, fade the music
 * in and start the menu menus_resume picks. first, where MENU_FLAG_SAVE_KEPT
 * is set, it puts back the save_data save_keep kept, and clears the flag */
void menus_init(void);
/* the program's step while the menus run. it steps the menu
 * menu_control.mode names, runs every task a frame, and counts
 * game_state.steps. each step sets game_state.idle_dim to 0, and turns
 * Ninja's fog off. it returns OVERLAY_RUNNING until the menus end, and then
 * OVERLAY_DONE, or OVERLAY_BACK while menus_leave_game is TRUE.
 *
 * the menus end once the menu's step returns nonzero. where player 0's
 * lives are then below LIVES_START, lives_reset runs. the reset buttons end
 * the menus too: they set game_state.next_mode to GAME_MODE_TO_MENU, and
 * clear menu_resume_target and game_state.skip_menus. on the title screen, the
 * reset buttons leave the game for the console's system instead */
int menus_step(void);
/* run the reset of each work whose header declares one */
void menus_reset(void);

#endif
