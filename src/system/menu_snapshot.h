/* the menus' works, and their snapshot in the main executable, which lasts
 * while ADVERTISE.PRS isn't loaded */
#ifndef SYSTEM_MENU_SNAPSHOT_H
#define SYSTEM_MENU_SNAPSHOT_H
#include <ninja.h>
#include <sg_xpt.h>

#include "save/card.h"

/* the menus' themes. game_state.theme picks one, and must lie below
 * MENU_THEMES, unchecked. each theme gives the menus a color. the options
 * find MENU_DEFAULT_THEME without a theme file */
#define MENU_THEMES 13
#define MENU_DEFAULT_THEME 0

/* the menus' works. a work is the record a menu's step shares with the task
 * that draws the menu. the step says what shows, and the task says what
 * moved. each work opens with four words. the fourth, loaded, is 1 from the
 * work's load to its release.
 *
 * while a field named shown is 1, the task brings in what it names. while
 * it is 0, the task takes that out. a field named fading, moving or
 * swinging is 1 if the task's last step moved what it names. a work's own
 * such field is 1 if any of its parts' is */

/* the alpha title's work. the backdrop's level runs from 0, dark, to 1. each
 * step of the task moves it 1/30 toward a target. the target is 0 while shown
 * is 0. while shown is 1, the target is 1 if bright is 1 too, and dim picks it
 * otherwise */
struct alpha_title_work {
    Bool shown;
    Sint32 bright;
    Bool fading;
    Bool loaded;
    /* the prompt draws over the backdrop while it is 1 and the level isn't
     * 0. a step that moves the level clears it */
    Sint32 prompt;
    Sint32 dim; /* a level of 0.3 while it is 0, and of 0.1 while it is 1 */
};

struct share_data_clock {
    Sint32 hour;
    Sint32 minute;
    Sint32 second;
};

/* the file select reads the main files of slots 0 to FILE_SELECT_SLOTS - 1,
 * as save_set_slot names them */
#define FILE_SELECT_SLOTS 11

/* a file the file select listed: its slot, and its save_data's stage,
 * character, emblems and play time */
struct file_select_file {
    Sint8 stage;
    Sint8 character;
    Uint8 emblems;
    Uint8 slot;
    struct share_data_clock time;
};

/* the file select's work */
struct file_select_work {
    Bool open; /* while it is 0, the task takes every part out */
    Sint32 unk_04;
    Bool moving;
    Bool loaded;
    Bool drives_shown; /* the drives, which swing in */
    Bool drive_lit; /* while it is 1, the cursor's drive draws lit */
    Bool drives_moving;
    Sint32 unk_1c;
    /* what each drive holds, an enum file_select_device */
    Sint8 devices[CARD_DRIVES];
    Sint8 drive; /* the drive the cursor is on */
    Uint8 unk_29[3];
    /* the drive's free blocks and files, the new row and the message */
    Bool files_shown;
    /* 1 while the cursor is on the files. the unlit drives' rings dim while
     * it or dialog_shown is 1 */
    Bool choosing_file;
    /* 1 if the task's last step slid the files, or moved the rings' level */
    Bool files_moving;
    Sint32 unk_38;
    struct file_select_file files[FILE_SELECT_SLOTS];
    Sint32 file_count;
    /* the drive's free and total blocks, as buGetDiskInfo gives them */
    Uint16 free_blocks;
    Uint16 total_blocks;
    Sint8 file; /* the file the cursor is on, 0 for the first */
    /* 1 if free_blocks is SAVE_BLOCKS or more, the blocks a new file
     * takes */
    Sint8 room;
    /* 1 while the cursor is on the new row, past the files, which makes a
     * new file */
    Sint8 on_new;
    Uint8 unk_f7;
    Bool dialog_shown;
    Sint32 unk_fc;
    Sint32 unk_100;
    Sint32 unk_104;
    Sint8 dialog; /* the dialog that shows, an enum file_select_dialog */
    Sint8 choice; /* the dialog's choice the cursor is on, 0 for the first */
    Uint8 unk_10a[2];
    /* the message under the files, a number of message_tables, or
     * FILE_SELECT_NO_MESSAGE */
    Sint32 message;
};

/* the main menu's groups of panels, and the most panels a group has */
#define MAIN_MENU_GROUPS 5
#define MAIN_MENU_GROUP_PANELS 8
/* a group of the main menu's panels, which swing on to the screen and off
 * it together */
struct main_menu_group {
    Bool shown;
    Sint32 lit; /* while it is 1, the cursor's panel draws its lit frame */
    Bool swinging;
    Sint32 unk_0c;
    Sint8 cursor; /* the panel the cursor is on, 0 for the group's first */
    /* by panel, 1 if the panel draws its refused face alone */
    Sint8 refused[MAIN_MENU_GROUP_PANELS];
    /* group 0's chosen panel, as its index plus 1, or 0 for none. while the
     * work is shown, that panel draws lit at full size, however far its
     * group has swung. it doesn't swing out with the group, but slides to
     * the middle of the screen's top */
    Sint8 chosen;
    Uint8 unk_1a[2];
};

/* the main menu's work */
struct main_menu_work {
    Bool shown; /* the panels draw only while it is 1 */
    Sint32 unk_04;
    Sint32 unk_08;
    Bool loaded;
    Sint8 language; /* the panels' faces: 0 for Japanese, 1 for English */
    Uint8 unk_11[3];
    /* @hack type: the groups as five members, where an array of
     * MAIN_MENU_GROUPS would do */
    struct main_menu_group group_0;
    struct main_menu_group group_1;
    struct main_menu_group group_2;
    struct main_menu_group group_3;
    struct main_menu_group group_4;
};
/* a part of a menu's panel. a work says what its parts' bright does, if
 * anything reads it. nothing reads unk_0c */
struct menu_part {
    Bool shown;
    Sint32 bright;
    Bool moving;
    Sint32 unk_0c;
};
/* the stage map's work. while shown is 0, the task takes every part out.
 * while it is 1, each step finds the lit cells again, as stage_map_cell_lit
 * reports them. the task clears grid.moving, unk_44.moving and
 * missions.moving each step, and nothing sets them. nothing sets
 * unk_44.shown */
struct stage_map_work {
    Bool shown;
    Sint32 unk_04;
    Bool moving;
    Bool loaded;
    /* backdrop.moving is 1 if the task's last step moved the backdrop's fade
     * or the grid's scroll */
    struct menu_part backdrop;
    Sint8 page; /* the page the grid scrolls to, below STAGE_MAP_PAGES */
    Uint8 unk_21[3];
    /* while grid.shown and grid.bright are 1, the cursor's cell pulses, and a
     * band over the grid shows a message */
    struct menu_part grid;
    /* the cursor's stage's name and its missions' ranks, or Chao World's
     * emblems. summary.moving is 1 if the task's last step moved the summary
     * and the missions. they move toward the corner of the screen away from
     * the cursor */
    struct menu_part summary;
    struct menu_part unk_44;
    /* the summary, with the chosen mission's records, or Chao World's
     * characters. while missions.shown and missions.bright are 1, a band over
     * the missions shows a message */
    struct menu_part missions;
    Sint8 mission_count; /* the count of choices the missions offer */
    /* the choice the cursor is on: a mission, or in Chao World a
     * character */
    Sint8 mission;
    Uint8 unk_66[2];
    Sint8 cursor; /* the cursor's cell, an index of stage_map_cells */
    Uint8 unk_69[3];
};
/* the story select's stories, STORY_HERO to STORY_LAST of synopsis.h, and
 * each story's count of chapters */
#define STORY_SELECT_STORIES 3
#define STORY_SELECT_HERO_CHAPTERS 15
#define STORY_SELECT_DARK_CHAPTERS 15
#define STORY_SELECT_LAST_CHAPTERS 4
/* a story's panel in the story select, with its list of chapters */
struct story_select_panel {
    Bool shown;
    Bool chosen; /* while it is 1, the panel moves to its chosen place */
    Bool fading;
    Sint32 unk_0c;
    /* the task clears list.shown while shown is 0. list.moving is 1 if the
     * task's last step moved the list's fade or its scroll */
    struct menu_part list;
    Sint8 chapter; /* the chapter the cursor is on, 0 for the first */
    /* while it is 1, the cursor's mark, the arrows and the help draw on the
     * list */
    Sint8 marked;
    Sint8 chapters;
    Sint8 labeled; /* while it is 1, a message draws under the panel */
};
/* the story select's work. while shown is 0, the task takes every part
 * out. a panel's move to its chosen place sets no flag */
struct story_select_work {
    Bool shown;
    /* while it is 1, a frame draws around the panel of the story the cursor
     * is on */
    Bool framed;
    Bool moving;
    Bool loaded;
    struct menu_part backdrop;
    Sint8 story; /* the story the cursor is on, a STORY_ value */
    Uint8 unk_21[3];
    struct story_select_panel panels[STORY_SELECT_STORIES];
    /* the emblem results' menu sets it as it starts the story select. while
     * it is 1, Cancel goes back to that menu */
    Sint8 from_emblems;
    Uint8 unk_91[3];
};
/* a team's look in the versus select */
enum versus_select_look {
    VERSUS_SELECT_LOOK_NORMAL,
    VERSUS_SELECT_LOOK_ALT, /* the character's other look */
    VERSUS_SELECT_LOOK_COSTUME /* COSTUME_PSO of player/player.h */
};
/* the bits of looks_open that open a look. the costume opens once
 * game_flags holds FLAG_RANKED of the character's group. the other look
 * opens once save_data.awards holds the character's top award. neither
 * opens while game_state.theme is THEME_COSTUME_H or THEME_COSTUME_X */
#define VERSUS_SELECT_LOOK_OPEN_COSTUME 0x1
#define VERSUS_SELECT_LOOK_OPEN_ALT 0x2
/* the versus select's games. the first VERSUS_SELECT_PAIRS are pairs of
 * characters. the random game and the game that opens the list of stages
 * follow them. VERSUS_SELECT_LAST_STAGE is the list's last stage, which the
 * cursor stops a stage short of */
#define VERSUS_SELECT_PAIRS 3
#define VERSUS_SELECT_RANDOM_GAME 3
#define VERSUS_SELECT_STAGES_GAME 4
#define VERSUS_SELECT_GAMES 5
#define VERSUS_SELECT_STAGES 10
#define VERSUS_SELECT_LAST_STAGE 9
/* the versus select's work. a team is 0 for the hero characters, each pair's
 * first, and 1 for the dark. while shown is 0, the task takes every part out */
struct versus_select_work {
    Bool shown;
    Sint32 unk_04;
    Bool moving;
    Bool loaded;
    /* 1 unless game_state.text_language is TEXT_JAPANESE. while it is 1,
     * the list of stages draws its names in English */
    Sint8 english_names;
    Sint8 looks[2]; /* each team's look, an enum versus_select_look */
    /* by team, the VERSUS_SELECT_LOOK_OPEN_ bits of its character on the
     * cursor's row */
    Sint8 looks_open[2];
    Uint8 unk_15[3];
    /* backdrop.moving is 1 if the task's last step moved the backdrop's
     * fade, or the titles' fade or slide */
    struct menu_part backdrop;
    /* the titles. the task clears title.moving each step: the titles' fade
     * and slide set backdrop.moving instead */
    struct menu_part title;
    /* 1 while player 0 takes the dark team, and player 1 the hero. the
     * hero's title stands on the right while it is 1 */
    Sint8 swapped;
    /* the two side cells, under the titles' left and right places, which
     * show without a fade */
    Sint8 sides_shown;
    Sint8 sides_help_shown;
    Uint8 unk_3b;
    struct menu_part list; /* the list of games */
    /* the game the cursor is on, from 0 to VERSUS_SELECT_STAGES_GAME */
    Sint8 game;
    /* by pair game, each team's character, a PLAYER_ value of
     * player/player.h */
    Sint8 pairs[VERSUS_SELECT_PAIRS][2];
    Sint8 games_help_shown;
    /* by player, game_state.match_wins, which the task copies each step */
    Sint8 wins[2];
    Uint8 unk_56[2];
    struct menu_part stages; /* the list of stages, which slides in */
    Sint8 stage; /* the stage the cursor is on, 0 for the first */
    /* 1 if save_data held every emblem as the versus select started. the
     * last stage draws open while it is 1 */
    Sint8 last_stage_open;
    /* by stage, each team's character, as pairs holds them */
    Sint8 stage_pairs[VERSUS_SELECT_STAGES][2];
    Sint8 stages_help_shown;
    Uint8 unk_7f;
};
/* the most buttons a new menu group has room for */
#define NEW_MENU_SLOTS 8
/* a group of the new menu's buttons, which swing on to the screen and off
 * it together */
struct new_menu_group {
    Bool shown;
    Sint32 lit; /* while it is 1, the cursor's button draws lit */
    Bool swinging;
    Sint32 unk_0c;
    Sint8 cursor; /* the button the cursor is on, 0 for the first */
    /* by button, 1 if the button can't be chosen */
    Sint8 refused[NEW_MENU_SLOTS];
    Uint8 unk_19[3];
};
/* the new menu's work. while shown is 0, the task clears every group's shown */
struct new_menu_work {
    Bool shown;
    Sint32 unk_04;
    Bool swinging;
    Bool loaded;
    /* the buttons' cells: 0 for Japanese, 1 for any other language */
    Sint8 language;
    /* the groups NEW_MENU_TOP to NEW_MENU_EXTRAS, then one with no buttons */
    struct new_menu_group top;
    struct new_menu_group single;
    struct new_menu_group versus;
    struct new_menu_group extras;
    struct new_menu_group unused;
};
/* the story synopsis's work */
struct synopsis_work {
    Bool shown; /* the backdrop */
    Sint32 scrolling; /* while it is 1, the set's lines scroll up */
    Bool fading;
    Bool loaded;
    /* the backdrop's character, a PLAYER_ value or PLAYER_KIND_AMY */
    Sint32 character;
    Sint32 set; /* the set of lines synopsis_select picked */
    Sint32 done; /* 1 once the set's last line has scrolled away */
};
/* the screen fade's work */
struct screen_fade_work {
    /* the cover over the whole screen. menus_resume clears it */
    Bool shown;
    Sint32 unk_04;
    Bool fading;
    Bool loaded;
    /* the cover's color. the fade's level takes the place of its alpha */
    NJS_COLOR color;
};
/* the options' work. while shown is 0, the task takes every part out. nothing
 * reads unk_04 or unk_10 */
struct options_work {
    Bool shown;
    Sint32 unk_04;
    Bool moving;
    Bool loaded;
    Sint8 unk_10;
    /* the backdrop, with the title and the theme's picture. it fades to half
     * while bright is 0 */
    struct menu_part backdrop;
    /* the settings' lists. the step sets a list's bright to 1 while the
     * list takes the pad. nothing reads it but the vibration list's. each
     * step first sets that one to whether a drive holds a vibration unit.
     * the vibration item draws at half while that bright is 0 */
    struct menu_part sound;
    struct menu_part vibration;
    struct menu_part text;
    /* the text list's row, an enum text_language. options_start sets it to
     * 1 for any language but TEXT_JAPANESE. choosing the language item sets
     * it to game_state.text_language */
    Sint32 text_row;
    struct menu_part voice;
    struct menu_part themes;
    /* by theme, 1 if options_start found the theme. MENU_DEFAULT_THEME is
     * always found, and any other by its file on a memory card */
    Sint32 theme_found[MENU_THEMES];
    Sint32 theme_row; /* the theme wheel's row, counting the themes found */
    /* the count of themes found, as the theme wheel last counted them */
    Sint32 themes_found;
    Sint32 unk_b4;
    /* the item wheel, which fades to half while bright is 0 */
    struct menu_part items;
    Sint8 item; /* the item the cursor is on, an enum options_item */
    /* 1 once a step on the item wheel finds game_state.cable SYE_CBL_PAL.
     * the wheel holds OPTIONS_ITEM_TV_SETTING only while it is 1 */
    Sint8 tv_setting_shown;
    Sint8 help_shown; /* the item's help */
};
/* the TV setting's work. while shown is 0, the task takes every part out.
 * nothing reads unk_04 */
struct tv_setting_work {
    Bool shown;
    Sint32 unk_04;
    Bool moving;
    Bool loaded;
    struct menu_part backdrop;
    /* the choices, which swing into place. the cursor's draws lit while
     * bright is 1 */
    struct menu_part choices;
    /* the choice the cursor is on, an enum tv_setting_choice */
    Sint8 cursor;
    Sint8 help_shown; /* the choice's help */
    struct menu_part test; /* the test picture */
    Sint32 test_steps; /* the steps the test has left */
};
/* the sound test's lists. list 0's row n opens list n + 1, a list of
 * songs */
#define MENU_SOUND_TEST_LISTS 7
struct menu_sound_test_list {
    Bool shown;
    Sint32 unk_04;
    Bool fading;
    Sint32 unk_0c;
    Sint8 row; /* the row the cursor is on */
    Sint8 rows;
};
/* the sound test's work */
struct menu_sound_test_work {
    /* while it is 0, the task takes every part out, and clears prompt */
    Bool active;
    Sint32 unk_04;
    Bool moving;
    Bool loaded;
    /* the backdrop, which fades to full while bright is 1 too, and to half
     * otherwise */
    Bool shown;
    Sint32 bright;
    /* 1 if the task's last step moved the backdrop's fade */
    Bool fading;
    Sint32 unk_1c;
    Bool disc_shown; /* the disc, which slides in */
    /* 1 while a song plays. the disc then tilts and spins faster, and the
     * prompt shows the song's message */
    Bool playing;
    Bool disc_moving;
    Sint32 unk_2c;
    struct menu_sound_test_list lists[MENU_SOUND_TEST_LISTS];
    Sint8 prompt; /* while it is 1, the panel draws the shown list's message */
};
/* the share data's work */
struct share_data_work {
    Sint32 unk_00;
    Sint32 unk_04;
    Sint32 unk_08;
    Bool loaded;
};
/* a kart race player's driver and its four stats. while the drivers show,
 * each step of the panel's task copies the driver's stats in */
struct kart_race_pick {
    /* an index of kart_race_panel.c's previews, or -1 for none */
    Sint8 driver;
    /* @hack type: four stats as fields of their own, where an array of four
     * would do */
    Uint8 stat_0;
    Uint8 stat_1;
    Uint8 stat_2;
    Uint8 stat_3;
};
/* the kart race panel's work, which both the kart race menu and the download
 * menu show. while shown is 0, the task takes every part out */
struct kart_race_work {
    Bool shown;
    Sint32 unk_04;
    Bool fading;
    Bool loaded;
    struct menu_part backdrop;
    /* the backdrop is the download menu's while it is 1, and the kart
     * race's while it is 0 */
    Sint32 download_backdrop;
    /* the drivers' icons and stats. drivers.moving is 1 if the task's last
     * step moved the drivers' fade or a stat's bar */
    struct menu_part drivers;
    struct kart_race_pick picks[2]; /* each player's pick */
    Uint8 unk_3e[2];
    /* courses.moving is 1 if the task's last step moved the courses' fade or
     * the list's */
    struct menu_part courses;
    Sint32 course; /* the course row the cursor is on */
    /* the list of download files. the task clears list.moving each step: the
     * list's fade sets courses.moving instead */
    struct menu_part list;
    Sint32 entry; /* the list's entry the cursor is on */
    Sint32 entry_count;
    /* the question over the entry's text, which shows only while none_found
     * is 0. while question.shown is 0, the task clears none_found. while the
     * question doesn't show, the task sets choice to KART_RACE_YES */
    struct menu_part question;
    /* the question's answer the cursor is on, KART_RACE_YES or
     * KART_RACE_NO */
    Sint8 choice;
    /* while it is 1, the panel says no download file was found */
    Sint8 none_found;
    Uint8 unk_7e[2];
    /* the picked drivers' karts. their fade sets fading */
    Sint8 preview_shown;
    /* while it is 1, the two players' drivers show side by side */
    Sint8 split;
    Sint8 hint_shown; /* the line of help under the panel, which doesn't fade */
    /* while it is 1, the hint says the memory cards are being read */
    Sint8 reading;
    /* 1 for a kart race of two players. the new menu's versus group sets it
     * to 1 as it starts the kart race. its single-player group and the
     * emblem results' menu set it to 0 */
    Sint8 two_players;
    /* the emblem results' menu sets it as it starts the kart race. while it
     * is 1, the kart race goes back to that menu as it leaves */
    Sint8 from_emblems;
    Uint8 unk_86[2];
};
/* the download panel's work. its load leaves loaded 0 */
struct download_panel_work {
    Sint32 unk_00;
    Sint32 unk_04;
    Sint32 unk_08;
    Bool loaded;
};
/* the boss attack panel's work */
struct boss_attack_work {
    Bool shown;
    Sint32 unk_04;
    Bool fading;
    Bool loaded;
    Sint32 course; /* the course the cursor is on, 0 for the first */
    /* the emblem results' menu sets it as it starts the boss attack. while
     * it is 1, the boss attack goes back to that menu as it leaves, and
     * clears it */
    Sint8 from_emblems;
};
/* the tutorial's lessons, each a row of its list and a picture list of its
 * own */
#define TUTORIAL_LESSONS 5
/* the tutorial panel's work. while shown is 0, the task takes every part out */
struct tutorial_work {
    Bool shown;
    Sint32 unk_04;
    Bool fading;
    Bool loaded;
    struct menu_part model; /* the model, its shadow, the frame and backdrop */
    Sint32 model_mode; /* how the model moves, an enum tutorial_model_mode */
    struct menu_part list;
    /* the lesson the list's cursor is on, 0 for the first */
    Sint32 lesson;
    struct menu_part board;
    Sint32 page; /* the lesson whose page shows */
    /* the board's picture, an index of the page's lesson's pictures, or
     * below 0 for none */
    Sint32 picture;
    /* while it is 1, the camera moves to its motion's last frame. while it
     * is 0, it moves back to the first */
    Sint32 camera_moved;
    /* 1 while each lesson's pictures are loaded */
    Sint8 pictures_loaded[TUTORIAL_LESSONS];
};
/* the screen effect's work */
struct screen_effect_work {
    Bool shown;
    Sint32 unk_04;
    Bool fading;
    Bool loaded;
};
/* the world ranking's work */
struct world_ranking_work {
    Sint32 unk_00;
    Sint32 unk_04;
    Sint32 unk_08;
    Bool loaded;
};
/* the emblem results' work. nothing sets ranking, crossed or time to anything
 * but 0, nor dialog.shown, ranks.shown or unk_d4.shown */
struct emblem_results_work {
    /* while it is 0, the task takes every panel out, and sets message to
     * EMBLEM_RESULTS_NO_MESSAGE */
    Sint32 open;
    Sint32 unk_04;
    Bool fading;
    Bool loaded;
    struct menu_part title; /* the backdrop and the title */
    /* while it is 1, the title and the totals are the world ranking's */
    Sint8 ranking;
    Sint8 crossed; /* while it is 1, a cross draws with the title */
    Uint8 unk_22[2];
    struct menu_part page_name;
    Sint8 page; /* the page that shows, an EMBLEM_RESULTS_ page */
    Uint8 unk_35[3];
    struct menu_part time_window;
    Sint32 time; /* the time the time window shows, in 60ths of a second */
    struct menu_part totals;
    struct menu_part lines;
    Sint8 line; /* the page's line the cursor is on, 0 for the first */
    /* the page's count of lines, which the task copies each step while
     * lines.shown is 1 */
    Sint8 line_count;
    Uint8 unk_6e[2];
    struct menu_part boxes; /* each line's mission boxes */
    /* the ranks on the world ranking's pages */
    struct menu_part ranks;
    /* the dialog. dialog_box picks its message box, and dialog_column the
     * choice its arrow stands under. each ++ in the box's text takes the
     * next string of dialog_args in its place */
    struct menu_part dialog;
    Sint32 dialog_box;
    Sint32 dialog_column;
    char *dialog_args[11];
    struct menu_part unk_d4;
    Uint8 unk_e4[0x14];
    /* the message drawn at the screen's foot, or EMBLEM_RESULTS_NO_MESSAGE */
    Sint32 message;
};

/* a copy of every menu's work, as menu_snapshot_save takes it */
struct menu_snapshot {
    struct alpha_title_work alpha_title;
    struct main_menu_work work_01;
    struct stage_map_work work_02;
    struct story_select_work work_03;
    struct versus_select_work work_04;
    struct new_menu_work work_05;
    struct synopsis_work work_06;
    struct screen_fade_work work_07;
    struct options_work work_08;
    struct tv_setting_work work_09;
    struct menu_sound_test_work work_10;
    struct share_data_work work_11;
    struct file_select_work work_12;
    struct kart_race_work work_13;
    struct download_panel_work work_14;
    struct boss_attack_work work_15;
    struct tutorial_work work_16;
    struct screen_effect_work work_17;
    struct world_ranking_work work_18;
    struct emblem_results_work work_19;
};
extern struct menu_snapshot menu_snapshot;

#endif
