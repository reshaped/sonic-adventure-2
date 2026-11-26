/* the options. their panel is menu_works.work_08, options_draw.h's
 * options_work */
#include <ninja.h>
#include <sg_bup.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>

#include "advertise/file_select.h"
#include "advertise/menu_textures.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/new_menu_screen.h"
#include "advertise/options.h"
#include "system/port.h"
#include "advertise/options/text.inc"
#include "advertise/options_draw.h"
#include "advertise/sound_test.h"
#include "advertise/title_screen.h"
#include "advertise/tv_setting.h"
#include "save/card.h"
#include "save/save.h"
#include "shared/card_warning.h"
#include "shared/message_sprite.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "sound/voice_set.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/text.h"
#include "vmu/vibration.h"

struct options_work options_work;

/* the options' state */
/* TRUE once a list or the theme wheel has had a choice */
static Bool options_changed;
static struct task *options_save_warning; /* the save's warning */
/* what a cancel puts back */
static Sint32 options_kept_setting;
static Sint8 options_kept_theme;
static struct task *options_card_warning; /* the file item's warning */
/* save_check's last result while the save's warning shows */
static Sint32 options_save_check;
#define THEME_READ_SIZE 0x400
/* the save data of each theme file read */
static char options_theme_file[THEME_READ_SIZE];

/* voice_set_play's sets */
#define OPEN_VOICES 9
#define MUSIC_VOICES 10
#define THEMES_VOICES 22
#define TV_SETTING_VOICES 23
/* vibration_start's buzz as a choice leaves vibration on */
#define VIBRATION_POWER 7
#define VIBRATION_FREQ 59
#define VIBRATION_TIME 1
/* message_sprite_draw's messages: the help for each row a list's cursor
 * rests on, and the theme wheel's help */
#define STEREO_HELP 29
#define MONO_HELP 30
#define VIBRATION_ON_HELP 31
#define VIBRATION_OFF_HELP 32
#define ENGLISH_VOICE_HELP 33
#define JAPANESE_VOICE_HELP 34
#define ENGLISH_TEXT_HELP 35
#define JAPANESE_TEXT_HELP 36
#define GERMAN_TEXT_HELP 37
#define FRENCH_TEXT_HELP 38
#define SPANISH_TEXT_HELP 39
#define THEME_HELP 43
/* a card may hold the theme files SONIC2___T01 to SONIC2___T12. a file's
 * save data opens with its key's digits, which lie KEY_STRIDE apart in the
 * key. KEY_LENGTH is the key's size, its NUL included */
#define THEME_FILES 12
#define KEY_LENGTH 16
#define KEY_STRIDE 2
#define NAME_SIZE 32

/* menu_control.step. each wait below lasts until the panel stops moving.
 * each _OPEN step waits, then lights its list or the theme wheel. it also
 * keeps the setting a cancel there puts back */
enum options_step {
    STEP_OPEN = 0, /* show the panel */
    STEP_SHOW_ITEMS = 1, /* wait, then bring the item wheel in */
    STEP_LIGHT_ITEMS = 2, /* wait, then light the item wheel and its help */
    STEP_ITEMS = 3, /* move along the item wheel, and choose an item */
    STEP_SOUND_OPEN = 4,
    STEP_SOUND = 5,
    STEP_VIBRATION_OPEN = 6,
    STEP_VIBRATION = 7,
    /* STEP_TEXT_OPEN keeps game_state.voice_language, which either language
     * list's cancel puts back. STEP_VOICE_OPEN keeps nothing */
    STEP_TEXT_OPEN = 8,
    STEP_TEXT = 9,
    STEP_VOICE_OPEN = 10,
    STEP_VOICE = 11,
    STEP_THEMES_OPEN = 12,
    STEP_THEMES = 13,
    STEP_MUSIC = 14, /* wait, then start the sound test */
    STEP_LEAVE = 18, /* wait, then go back to the new menu */
    /* wait, then write the save once a drive's device changes and
     * save_check then passes. leave unwritten once the warning ends */
    STEP_SAVE_WARNING = 19,
    /* show the file item's warning until it ends. a memory card put in fades
     * it out */
    STEP_CARD_WARNING = 20
};

static void options_find_themes(void);

static NJS_POINT3 help_pos = {
#include "advertise/options/help_pos.inc"
};
/* by theme file: its key. there is no file 0 */
KEPT static char *theme_keys[THEME_FILES + 1] = {
#include "advertise/options/theme_keys.inc"
};
/* by theme file: the theme it holds */
static Sint32 file_themes[THEME_FILES + 1] = {
    0, 10, 11, 5, 8, 3, 7, 1, 6, 9, 2, THEME_COSTUME_H, THEME_COSTUME_X,
};

void options_start(void)
{
    menu_textures_select(MENU_TEXTURES_OPTIONS);
    menu_control.step = STEP_OPEN;
    music_change(OPTIONS_MUSIC);
    menu_works.work_08->unk_10 = game_state.text_language > TEXT_JAPANESE;
    menu_works.work_08->text_row = game_state.text_language > TEXT_JAPANESE;
    options_changed = FALSE;
    options_find_themes();
}

/* hide the panel. nothing calls options_hide, and no header declares it. it
 * is external only because SHC leaves out a static function that nothing
 * calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void options_hide(void);
/* NOLINTEND(misc-use-internal-linkage) */

void options_hide(void)
{
    menu_works.work_08->shown = FALSE;
    menu_works.work_08->unk_04 = FALSE;
}

void options_continue(int which)
{
    switch (which) {
    case OPTIONS_CONTINUE_ITEMS:
        menu_control.step = STEP_SHOW_ITEMS;
        menu_works.work_08->items.shown = TRUE;
        break;
    }
}

Bool options_step(void)
{
    Sint8 setting;

    menu_works.work_08->vibration.bright = file_select_vibration_present();
    switch (menu_control.step) {
    case STEP_OPEN:
        title_screen_hide();
        menu_works.work_08->shown = TRUE;
        menu_works.work_08->backdrop.shown = TRUE;
        menu_works.work_08->backdrop.bright = TRUE;
        menu_control.step = STEP_SHOW_ITEMS;
        voice_set_play(OPEN_VOICES);
        break;
    case STEP_SHOW_ITEMS:
        if (menu_works.work_08->moving) {
            break;
        }
        menu_works.work_08->items.shown = TRUE;
        menu_works.work_08->backdrop.bright = TRUE;
        menu_control.step = STEP_LIGHT_ITEMS;
        break;
    case STEP_LIGHT_ITEMS:
        if (!menu_works.work_08->moving) {
            menu_works.work_08->items.bright = TRUE;
            menu_works.work_08->help_shown = TRUE;
            menu_control.step = STEP_ITEMS;
        }
        break;
    case STEP_ITEMS:
        /* @hack spelling: an empty branch for a moving panel, where a test
         * of !moving alone would do */
        if (menu_works.work_08->moving) {
        } else {
            if (pad_press_repeat[0] & PDD_DGT_KR) {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                menu_works.work_08->item++;
                if (menu_works.work_08->tv_setting_shown) {
                    if (menu_works.work_08->item >= OPTIONS_ITEMS) {
                        menu_works.work_08->item = 0;
                    }
                } else {
                    if (menu_works.work_08->item >= OPTIONS_ITEM_TV_SETTING) {
                        menu_works.work_08->item = 0;
                    }
                }
            } else if (pad_press_repeat[0] & PDD_DGT_KL) {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                menu_works.work_08->item--;
                if (menu_works.work_08->item < 0) {
                    if (menu_works.work_08->tv_setting_shown) {
                        menu_works.work_08->item = OPTIONS_ITEM_TV_SETTING;
                    } else {
                        menu_works.work_08->item = OPTIONS_ITEM_THEME;
                    }
                }
                /* @hack type: the pad's masks cast to int in each cancel
                 * test of options_step, and in the choose tests of STEP_TEXT
                 * and STEP_THEMES, where their own Uint32 would do */
            } else if ((int)pad_press_repeat[0] & (int)pad_cancel_buttons) {
                menu_works.work_08->shown = FALSE;
                menu_works.work_08->unk_04 = FALSE;
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
                if (options_changed) {
                    /* @hack spelling: the assignment inside the test, where a
                     * statement of its own would do */
                    if ((game_state.card_warning = save_check(FALSE)) !=
                        SAVE_OK) {
                        game_state.save_drive = save_place.drive;
                        options_save_warning = save_warning_create();
                        menu_control.step = STEP_SAVE_WARNING;
                        menu_control.fade_held = TRUE;
                    } else {
                        save_write();
                        menu_control.step = STEP_LEAVE;
                    }
                    syCfgInit(DEMO_AREA);
                    if (game_state.mono == 0) {
                        syCfgSetSoundMode(SYD_CFG_STEREO);
                    } else {
                        syCfgSetSoundMode(SYD_CFG_MONO);
                    }
                } else {
                    menu_control.step = STEP_LEAVE;
                }
            } else if (pad_press_repeat[0] & pad_choose_buttons) {
                switch (menu_works.work_08->item) {
                case OPTIONS_ITEM_MUSIC:
                    menu_works.work_08->shown = FALSE;
                    menu_works.work_08->unk_04 = FALSE;
                    menu_control.step = STEP_MUSIC;
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    voice_set_play(MUSIC_VOICES);
                    break;
                case OPTIONS_ITEM_FILE:
                    if (file_select_card_present()) {
                        menu_works.work_08->items.shown = FALSE;
                        menu_works.work_08->backdrop.bright = FALSE;
                        sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                        file_select_return_mode = MENU_MODE_OPTIONS;
                        file_select_start();
                        menu_control.mode = MENU_MODE_FILE_SELECT;
                    } else {
                        options_card_warning = save_warning_create();
                        game_state.card_warning = SAVE_NO_ROOM;
                        sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                        menu_control.step = STEP_CARD_WARNING;
                    }
                    break;
                case OPTIONS_ITEM_VIBRATION:
                    if (file_select_vibration_present()) {
                        menu_control.step = STEP_VIBRATION_OPEN;
                        menu_works.work_08->items.bright = FALSE;
                        menu_works.work_08->vibration.shown = TRUE;
                        menu_works.work_08->backdrop.bright = FALSE;
                        sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    } else {
                        sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                    }
                    break;
                case OPTIONS_ITEM_SOUND:
                    menu_control.step = STEP_SOUND_OPEN;
                    menu_works.work_08->items.bright = FALSE;
                    menu_works.work_08->sound.shown = TRUE;
                    menu_works.work_08->backdrop.bright = FALSE;
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    break;
                case OPTIONS_ITEM_LANGUAGE:
                    menu_works.work_08->text_row = game_state.text_language;
                    menu_control.step = STEP_TEXT_OPEN;
                    menu_works.work_08->items.bright = FALSE;
                    menu_works.work_08->text.shown = TRUE;
                    menu_works.work_08->backdrop.bright = FALSE;
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    break;
                /* @bug logic: a choice before any turn sets the theme at the
                 * wheel's row, which may not be the one in use (the call at
                 * 0x8c50e218). the wheel opens at the row it was last left
                 * on, 0 after options_draw_create, where game_state.theme's
                 * row is meant. the language item sets its list's row from
                 * game_state first (the store at 0x8c50db78) */
                case OPTIONS_ITEM_THEME:
                    menu_control.step = STEP_THEMES_OPEN;
                    menu_works.work_08->items.bright = FALSE;
                    menu_works.work_08->items.shown = FALSE;
                    menu_works.work_08->themes.shown = TRUE;
                    menu_works.work_08->backdrop.bright = TRUE;
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    voice_set_play(THEMES_VOICES);
                    break;
                case OPTIONS_ITEM_TV_SETTING:
                    menu_works.work_08->shown = FALSE;
                    menu_works.work_08->unk_04 = FALSE;
                    tv_setting_start();
                    menu_control.mode = MENU_MODE_TV_SETTING;
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    voice_set_play(TV_SETTING_VOICES);
                    break;
                }
            }
        }
        if (game_state.cable == SYE_CBL_PAL) {
            menu_works.work_08->tv_setting_shown = TRUE;
        }
        break;
    case STEP_SOUND_OPEN:
        /* @hack spelling: an empty branch for a moving panel, where a test
         * of !moving alone would do */
        if (menu_works.work_08->moving) {
        } else {
            menu_works.work_08->sound.bright = TRUE;
            menu_control.step = STEP_SOUND;
            options_kept_setting = game_state.mono;
        }
        break;
    case STEP_SOUND:
        if (game_state.mono == FALSE) {
            message_sprite_draw(STEREO_HELP, &help_pos, 1.0F);
        } else {
            message_sprite_draw(MONO_HELP, &help_pos, 1.0F);
        }
        if ((int)pad_press_repeat[0] & (int)pad_cancel_buttons) {
            menu_works.work_08->sound.bright = FALSE;
            menu_works.work_08->sound.shown = FALSE;
            menu_control.step = STEP_SHOW_ITEMS;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            game_state.mono = options_kept_setting;
        } else if (pad_press_repeat[0] & pad_choose_buttons) {
            menu_works.work_08->sound.bright = FALSE;
            menu_works.work_08->sound.shown = FALSE;
            menu_control.step = STEP_SHOW_ITEMS;
            sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
            options_changed = TRUE;
            sound_apply_mono();
        } else if (pad_press_repeat[0] & PDD_DGT_KD) {
            if (game_state.mono == FALSE) {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                game_state.mono = TRUE;
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KU) {
            if (game_state.mono == TRUE) {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                game_state.mono = FALSE;
            }
        }
        break;
    case STEP_VIBRATION_OPEN:
        if (menu_works.work_08->moving) {
            break;
        }
        menu_works.work_08->vibration.bright = TRUE;
        menu_control.step = STEP_VIBRATION;
        options_kept_setting = game_state.vibration;
        break;
    case STEP_VIBRATION:
        if (game_state.vibration) {
            message_sprite_draw(VIBRATION_ON_HELP, &help_pos, 1.0F);
        } else {
            message_sprite_draw(VIBRATION_OFF_HELP, &help_pos, 1.0F);
        }
        if ((int)pad_press_repeat[0] & (int)pad_cancel_buttons) {
            menu_works.work_08->vibration.bright = FALSE;
            menu_works.work_08->vibration.shown = FALSE;
            menu_control.step = STEP_SHOW_ITEMS;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            game_state.vibration = options_kept_setting;
        } else if (pad_press_repeat[0] & pad_choose_buttons) {
            menu_works.work_08->vibration.bright = FALSE;
            menu_works.work_08->vibration.shown = FALSE;
            menu_control.step = STEP_SHOW_ITEMS;
            sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
            options_changed = TRUE;
            if (game_state.vibration) {
                vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ,
                                VIBRATION_TIME);
            }
        } else {
            /* @hack spelling: the setting read into a local before the pad's
             * tests, where game_state's own field would do */
            setting = game_state.vibration;
            if (pad_press_repeat[0] & PDD_DGT_KU) {
                if (setting == FALSE) {
                    game_state.vibration = TRUE;
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (pad_press_repeat[0] & PDD_DGT_KD) {
                if (setting != FALSE) {
                    game_state.vibration = FALSE;
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            }
        }
        break;
    case STEP_TEXT_OPEN:
        if (menu_works.work_08->moving) {
            break;
        }
        menu_works.work_08->text.bright = TRUE;
        menu_control.step = STEP_TEXT;
        options_kept_setting = game_state.voice_language;
        break;
    case STEP_TEXT:
        switch (menu_works.work_08->text_row) {
        case TEXT_JAPANESE:
            message_sprite_draw(JAPANESE_TEXT_HELP, &help_pos, 1.0F);
            break;
        case TEXT_ENGLISH:
            message_sprite_draw(ENGLISH_TEXT_HELP, &help_pos, 1.0F);
            break;
        case TEXT_FRENCH:
            message_sprite_draw(FRENCH_TEXT_HELP, &help_pos, 1.0F);
            break;
        case TEXT_SPANISH:
            message_sprite_draw(SPANISH_TEXT_HELP, &help_pos, 1.0F);
            break;
        case TEXT_GERMAN:
            message_sprite_draw(GERMAN_TEXT_HELP, &help_pos, 1.0F);
            break;
        }
        if ((int)pad_press_repeat[0] & (int)pad_cancel_buttons) {
            menu_works.work_08->text.bright = FALSE;
            menu_works.work_08->text.shown = FALSE;
            menu_control.step = STEP_SHOW_ITEMS;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            game_state.voice_language = options_kept_setting;
        } else if ((int)pad_press_repeat[0] & (int)pad_choose_buttons) {
            game_state.text_language = menu_works.work_08->text_row;
            sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
            music_stop();
            sound_task_kill();
            sound_task_create(NULL);
            music_play_name(OPTIONS_MUSIC);
            options_changed = TRUE;
            menu_works.work_08->text.bright = FALSE;
            menu_works.work_08->text.shown = FALSE;
            menu_works.work_08->voice.bright = TRUE;
            menu_works.work_08->voice.shown = TRUE;
            menu_control.step = STEP_VOICE_OPEN;
            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
        } else if (pad_press_repeat[0] & PDD_DGT_KU) {
            if (menu_works.work_08->text_row) {
                menu_works.work_08->text_row--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KD) {
            if (menu_works.work_08->text_row < TEXT_LANGUAGES - 1) {
                menu_works.work_08->text_row++;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
            /* @hack spelling: the pad tested for left and for right apart,
             * where one test of both would do */
        } else if ((pad_press_repeat[0] & PDD_DGT_KL) |
                   (pad_press_repeat[0] & PDD_DGT_KR)) {
            menu_works.work_08->text.bright = FALSE;
            menu_works.work_08->text.shown = FALSE;
            menu_works.work_08->voice.bright = TRUE;
            menu_works.work_08->voice.shown = TRUE;
            menu_control.step = STEP_VOICE_OPEN;
            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
        }
        break;
    case STEP_VOICE_OPEN:
        if (menu_works.work_08->moving) {
            break;
        }
        menu_works.work_08->voice.bright = TRUE;
        menu_control.step = STEP_VOICE;
        break;
    case STEP_VOICE:
        if (game_state.voice_language) {
            message_sprite_draw(ENGLISH_VOICE_HELP, &help_pos, 1.0F);
        } else {
            message_sprite_draw(JAPANESE_VOICE_HELP, &help_pos, 1.0F);
        }
        if ((int)pad_press_repeat[0] & (int)pad_cancel_buttons) {
            menu_works.work_08->voice.bright = FALSE;
            menu_works.work_08->voice.shown = FALSE;
            menu_control.step = STEP_SHOW_ITEMS;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            game_state.voice_language = options_kept_setting;
        } else if (pad_press_repeat[0] & pad_choose_buttons) {
            game_state.text_language = menu_works.work_08->text_row;
            menu_works.work_08->unk_10 = menu_works.work_08->text_row;
            menu_works.work_08->voice.bright = FALSE;
            menu_works.work_08->voice.shown = FALSE;
            menu_control.step = STEP_SHOW_ITEMS;
            sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
            music_stop();
            sound_task_kill();
            sound_task_create(NULL);
            music_play_name(OPTIONS_MUSIC);
            options_changed = TRUE;
        } else {
            /* @hack spelling: the setting read into a local before the pad's
             * tests, where game_state's own field would do */
            setting = game_state.voice_language;
            if (pad_press_repeat[0] & PDD_DGT_KU) {
                if (setting != VOICE_JAPANESE) {
                    game_state.voice_language = VOICE_JAPANESE;
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (pad_press_repeat[0] & PDD_DGT_KD) {
                if (setting == VOICE_JAPANESE) {
                    game_state.voice_language = VOICE_ENGLISH;
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
                /* @hack spelling: the pad tested for left and for right
                 * apart, where one test of both would do */
            } else if ((pad_press_repeat[0] & PDD_DGT_KL) |
                       (pad_press_repeat[0] & PDD_DGT_KR)) {
                menu_works.work_08->text.bright = TRUE;
                menu_works.work_08->text.shown = TRUE;
                menu_works.work_08->voice.bright = FALSE;
                menu_works.work_08->voice.shown = FALSE;
                menu_control.step = STEP_TEXT_OPEN;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        }
        break;
    case STEP_THEMES_OPEN:
        if (menu_works.work_08->moving) {
            break;
        }
        menu_works.work_08->themes.bright = TRUE;
        menu_control.step = STEP_THEMES;
        options_kept_theme = game_state.theme;
        break;
    case STEP_THEMES:
        message_sprite_draw(THEME_HELP, &help_pos, 1.0F);
        if (menu_works.work_08->moving) {
            break;
        }
        if ((int)pad_press_repeat[0] & (int)pad_cancel_buttons) {
            menu_works.work_08->themes.bright = FALSE;
            menu_works.work_08->themes.shown = FALSE;
            menu_control.step = STEP_SHOW_ITEMS;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            game_state.theme = options_kept_theme;
        } else if ((int)pad_press_repeat[0] & (int)pad_choose_buttons) {
            menu_works.work_08->themes.bright = FALSE;
            menu_works.work_08->themes.shown = FALSE;
            menu_control.step = STEP_SHOW_ITEMS;
            game_state.theme = options_row_theme();
            sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
            options_changed = TRUE;
        } else if (pad_press_repeat[0] & PDD_DGT_KL) {
            if (menu_works.work_08->themes_found > 1) {
                menu_works.work_08->theme_row--;
                if (menu_works.work_08->theme_row < 0) {
                    menu_works.work_08->theme_row =
                        menu_works.work_08->themes_found - 1;
                }
                game_state.theme = options_row_theme();
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KR) {
            if (menu_works.work_08->themes_found > 1) {
                menu_works.work_08->theme_row++;
                if (menu_works.work_08->theme_row >
                    menu_works.work_08->themes_found - 1) {
                    menu_works.work_08->theme_row = 0;
                }
                game_state.theme = options_row_theme();
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        }
        break;
    case STEP_LEAVE:
        if (menu_works.work_08->moving) {
            break;
        }
        new_menu_screen_start();
        new_menu_screen_reopen(NEW_MENU_REOPEN_TOP);
        menu_control.mode = MENU_MODE_NEW_MENU;
        menu_control.fade_held = FALSE;
        break;
    case STEP_MUSIC:
        if (menu_works.work_08->moving) {
            break;
        }
        menu_sound_test_start();
        menu_control.mode = MENU_MODE_SOUND_TEST;
        break;
    case STEP_SAVE_WARNING:
        if (menu_works.work_08->moving) {
            break;
        }
        if (save_check_sockets() &&
            (options_save_check = save_check(FALSE)) == SAVE_OK) {
            save_write();
            menu_control.step = STEP_LEAVE;
            game_state.card_warning = options_save_check;
        } else if (options_save_warning->step == task_destroy) {
            menu_control.step = STEP_LEAVE;
            save_set_drive(SAVE_DRIVE_REFUSED);
        } else {
            game_state.card_warning = options_save_check;
        }
        break;
    case STEP_CARD_WARNING:
        if (options_card_warning->step == task_destroy) {
            menu_control.step = STEP_ITEMS;
        } else if (file_select_card_present()) {
            game_state.card_warning = SAVE_OK;
        }
        break;
    }
    return FALSE;
}

static void options_find_themes(void)
{
    BUS_DISKINFO info;
    char name[NAME_SIZE];
    Bool refused;
    Bool match;
    Sint32 file;
    Sint32 drive;
    Sint32 i;
    Sint32 j;

    refused = FALSE;
    menu_works.work_08->theme_found[MENU_DEFAULT_THEME] = TRUE;
    for (i = MENU_DEFAULT_THEME + 1; i < MENU_THEMES; i++) {
        menu_works.work_08->theme_found[i] = FALSE;
    }
    /* @hack type: the save's bits read as a Sint8, where their own Uint8
     * would do */
    if ((Sint8)save_data.status & SAVE_THEMES_REFUSED) {
        return;
    }
    for (file = 1; file < THEME_FILES + 1; file++) {
#ifdef NONMATCHING
        /* the disc-free build has no key to compare a file with */
        if (theme_keys[file] == NULL) {
            continue;
        }
#endif
        for (drive = 0; drive < CARD_DRIVES; drive++) {
            if (buGetDiskInfo(drive, &info) != 0) {
                continue;
            }
            sprintf(name, "SONIC2___T%02d", file);
            if (card_load_file(drive, name, options_theme_file,
                               THEME_READ_SIZE) == 0) {
                match = TRUE;
                while (buStat(drive) != 0) {
                }
                /* @hack spelling: j beside i, where i / KEY_STRIDE would do */
                j = 0;
#ifdef NONMATCHING
                /* this side repairs the bug tagged below. it skips a file that
                 * read fewer bytes than the key, which the loop compares */
                if (card_load_result(drive) < KEY_LENGTH / KEY_STRIDE) {
                    continue;
                }
#endif
                /* @bug uninitialized: a file whose read fails is judged by
                 * the bytes an earlier read, or the bss, left in
                 * options_theme_file. a file with its key can then be
                 * refused, and SAVE_THEMES_REFUSED set in the save, which is
                 * written. nothing tests the read's result (the call at
                 * 0x8c50e4aa, the wait at 0x8c50e4b6, the first compare at
                 * 0x8c50e4c8) */
                for (i = 0; i < KEY_LENGTH; i += KEY_STRIDE) {
                    if (theme_keys[file][i] != options_theme_file[j]) {
                        match = FALSE;
                    }
                    j++;
                }
                menu_works.work_08->theme_found[file_themes[file]] = match;
                if (!match) {
                    refused = TRUE;
                }
                break;
            }
            while (buStat(drive) != 0) {
            }
        }
    }
    if (refused) {
        save_data.status |= SAVE_THEMES_REFUSED;
        save_write();
    }
}

Sint8 options_row_theme(void)
{
    Sint32 found = 0;
    /* @hack type: the counter as a Sint8, where an Int would do */
    Sint8 i;

    for (i = 0; i < MENU_THEMES; i++) {
        if (menu_works.work_08->theme_found[i]) {
            if (found == menu_works.work_08->theme_row) {
                return i;
            }
            found++;
        }
    }
    return MENU_DEFAULT_THEME;
}
