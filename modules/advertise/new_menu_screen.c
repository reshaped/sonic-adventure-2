/* the new menu's screen, MENU_MODE_NEW_MENU: the groups of buttons after the
 * title screen. the other menus start from them */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "advertise/alpha_title.h"
#include "advertise/boss_attack.h"
#include "advertise/download.h"
#include "advertise/emblem_results_menu.h"
#include "advertise/kart_race.h"
#include "advertise/menu_snapshot.h"
#include "advertise/menu_textures.h"
#include "advertise/menus.h"
#include "advertise/new_menu.h"
#include "advertise/new_menu_screen.h"
#include "advertise/options.h"
#include "advertise/stage_map_menu.h"
#include "advertise/story_select.h"
#include "advertise/title_screen.h"
#include "advertise/tutorial.h"
#include "advertise/versus_select.h"
#include "advertise/world_ranking_menu.h"
#include "main.h"
#include "player/player.h"
#include "save/save.h"
#include "shared/message_sprite.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/sequence.h"
#include "system/text.h"

struct new_menu_work new_menu_work;

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* hide the buttons */
void new_menu_screen_hide(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* the player whose pad the versus group needs */
#define SECOND_PLAYER 1

/* the buttons' help lines, as numbers of message_tables */
enum help_message {
    HELP_SINGLE = 0,
    HELP_VERSUS = 1,
    HELP_EXTRAS = 2,
    HELP_OPTIONS = 3,
    HELP_INTERNET = 4,
    HELP_STORY = 5,
    HELP_STAGES = 7,
    HELP_BATTLE = 8,
    HELP_KART = 9,
    HELP_CHAO_RACE = 10,
    HELP_RANKING = 11,
    HELP_TUTORIAL = 12,
    HELP_EMBLEMS = 13,
    HELP_DOWNLOADS = 14,
    HELP_BOSSES = 70
};

/* menu_control.step, while the screen runs. a _SWING step lights its group
 * once the group has swung in. a _CHOSEN step starts the chosen button's
 * menu once the buttons have swung out. the story select starts at once */
enum {
    STEP_OPEN, /* swing the top group in */
    STEP_TOP_SWING, /* waits for every group, not the top's alone */
    STEP_TOP,
    STEP_SINGLE_SWING,
    STEP_SINGLE,
    STEP_SINGLE_CHOSEN,
    STEP_VERSUS_SWING,
    STEP_VERSUS,
    STEP_VERSUS_CHOSEN,
    STEP_EXTRAS_SWING,
    STEP_EXTRAS,
    STEP_EXTRAS_CHOSEN,
    STEP_OPTIONS, /* start the options once the buttons have swung out */
    STEP_INTERNET /* leave the game once the buttons have swung out */
};

static NJS_POINT3 help_pos = {0.0F, 400.0F, -12.0F};

static Bool new_menu_screen_has_choice(struct new_menu_group *group,
                                       int buttons);
static void new_menu_screen_next(struct new_menu_group *group, int buttons);
static void new_menu_screen_previous(struct new_menu_group *group);

void new_menu_screen_start(void)
{
    /* @hack spelling: each flag picked by an if, where its negation stored
     * would do */
    if (game_state.network_info_found) {
        menu_works.work_05->top.refused[NEW_MENU_TOP_INTERNET] = FALSE;
    } else {
        menu_works.work_05->top.refused[NEW_MENU_TOP_INTERNET] = TRUE;
    }
    menu_works.alpha_title->shown = TRUE;
    menu_works.alpha_title->bright = FALSE;
    menu_textures_select(MENU_TEXTURES_NEW_MENU);
    menu_control.step = STEP_OPEN;
    music_play_name("advsng_1.adx");
    menu_works.work_05->language = game_state.text_language > TEXT_JAPANESE;
    if (save_data.stages[STAGE_CITY_ESCAPE].ranks[MISSION_FIRST] ||
        save_data.stages[STAGE_IRON_GATE].ranks[MISSION_FIRST]) {
        menu_works.work_05->single.refused[NEW_MENU_SINGLE_STAGES] = FALSE;
    } else {
        menu_works.work_05->single.refused[NEW_MENU_SINGLE_STAGES] = TRUE;
    }
    if (save_data.cart_stage[SAVE_CART_OTHERS].ranks[MISSION_FIRST] ||
        save_data.cart_stage[SAVE_CART_ROUGE].ranks[MISSION_FIRST]) {
        menu_works.work_05->single.refused[NEW_MENU_SINGLE_KART] = FALSE;
    } else {
        menu_works.work_05->single.refused[NEW_MENU_SINGLE_KART] = TRUE;
    }
    if (save_data.awards[SAVE_AWARD_HERO_STORY] ||
        save_data.awards[SAVE_AWARD_DARK_STORY]) {
        menu_works.work_05->single.refused[NEW_MENU_SINGLE_BOSSES] = FALSE;
    } else {
        menu_works.work_05->single.refused[NEW_MENU_SINGLE_BOSSES] = TRUE;
    }
    if (save_data.cart_stage[SAVE_CART_OTHERS].ranks[MISSION_FIRST] &&
        save_data.cart_stage[SAVE_CART_ROUGE].ranks[MISSION_FIRST]) {
        menu_works.work_05->versus.refused[NEW_MENU_VERSUS_KART] = FALSE;
    } else {
        menu_works.work_05->versus.refused[NEW_MENU_VERSUS_KART] = TRUE;
    }
    if (SAVE_CHAO_ENTERED) {
        menu_works.work_05->versus.refused[NEW_MENU_VERSUS_CHAO_RACE] = FALSE;
    } else {
        menu_works.work_05->versus.refused[NEW_MENU_VERSUS_CHAO_RACE] = TRUE;
    }
    menu_works.work_05->extras.refused[NEW_MENU_EXTRAS_RANKING] = TRUE;
}

void new_menu_screen_hide(void)
{
    menu_works.work_05->shown = FALSE;
    menu_works.work_05->unk_04 = FALSE;
}

void new_menu_screen_reopen(int group)
{
    switch (group) {
    case NEW_MENU_REOPEN_SINGLE:
        if (new_menu_screen_has_choice(&menu_works.work_05->single,
                                       NEW_MENU_SINGLE_BUTTONS)) {
            menu_works.work_05->shown = TRUE;
            menu_works.work_05->single.shown = TRUE;
            menu_control.step = STEP_SINGLE_SWING;
        } else {
            menu_works.work_05->shown = TRUE;
            menu_works.work_05->top.shown = TRUE;
            menu_control.step = STEP_TOP_SWING;
        }
        break;
    case NEW_MENU_REOPEN_VERSUS:
        if (new_menu_screen_has_choice(&menu_works.work_05->versus,
                                       NEW_MENU_VERSUS_BUTTONS)) {
            menu_works.work_05->shown = TRUE;
            menu_works.work_05->versus.shown = TRUE;
            menu_control.step = STEP_VERSUS_SWING;
        } else {
            menu_works.work_05->shown = TRUE;
            menu_works.work_05->top.shown = TRUE;
            menu_control.step = STEP_TOP_SWING;
        }
        break;
    case NEW_MENU_REOPEN_EXTRAS:
        menu_works.work_05->shown = TRUE;
        menu_works.work_05->extras.shown = TRUE;
        menu_control.step = STEP_EXTRAS_SWING;
        break;
    case NEW_MENU_REOPEN_TOP:
        menu_works.work_05->shown = TRUE;
        menu_works.work_05->top.shown = TRUE;
        menu_control.step = STEP_TOP_SWING;
        break;
    }
}

Bool new_menu_screen_step(void)
{
    /* @hack spelling: the flag picked by an if, where its negation stored
     * would do */
    if (pad_player_is_connected(SECOND_PLAYER)) {
        menu_works.work_05->top.refused[NEW_MENU_TOP_VERSUS] = FALSE;
    } else {
        menu_works.work_05->top.refused[NEW_MENU_TOP_VERSUS] = TRUE;
    }
    switch (menu_control.step) {
    case STEP_OPEN:
        menu_works.work_05->shown = TRUE;
        menu_works.work_05->unk_04 = TRUE;
        menu_works.work_05->top.shown = TRUE;
        menu_control.step = STEP_TOP_SWING;
        break;
    case STEP_TOP_SWING:
        if (!menu_works.work_05->swinging) {
            menu_control.step = STEP_TOP;
            menu_works.work_05->top.lit = TRUE;
            menu_works.work_17->shown = TRUE;
        }
        break;
    case STEP_TOP:
        switch (menu_works.work_05->top.cursor) {
        case NEW_MENU_TOP_SINGLE:
            message_sprite_draw(HELP_SINGLE, &help_pos, 1.0F);
            break;
        case NEW_MENU_TOP_VERSUS:
            message_sprite_draw(HELP_VERSUS, &help_pos, 1.0F);
            break;
        case NEW_MENU_TOP_EXTRAS:
            message_sprite_draw(HELP_EXTRAS, &help_pos, 1.0F);
            break;
        case NEW_MENU_TOP_OPTIONS:
            message_sprite_draw(HELP_OPTIONS, &help_pos, 1.0F);
            break;
        case NEW_MENU_TOP_INTERNET:
            message_sprite_draw(HELP_INTERNET, &help_pos, 1.0F);
            break;
        }
        if (pad_press_repeat[0] & PDD_DGT_KU) {
            menu_works.work_05->top.cursor--;
            if (menu_works.work_05->top.cursor < 0) {
                menu_works.work_05->top.cursor = 0;
            } else {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KD) {
            menu_works.work_05->top.cursor++;
            if (menu_works.work_05->top.cursor > NEW_MENU_TOP_BUTTONS - 1) {
                menu_works.work_05->top.cursor = NEW_MENU_TOP_BUTTONS - 1;
            } else {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
            /* @hack type: the pad's masks cast to int, where their own
             * Uint32 would do */
        } else if ((int)pad_press_repeat[0] & (int)pad_choose_buttons) {
            switch (menu_works.work_05->top.cursor) {
            case NEW_MENU_TOP_SINGLE:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_works.work_05->top.shown = FALSE;
                menu_works.work_05->top.lit = FALSE;
                if (new_menu_screen_has_choice(&menu_works.work_05->single,
                                               NEW_MENU_SINGLE_BUTTONS)) {
                    menu_control.step = STEP_SINGLE_SWING;
                    menu_works.work_05->single.shown = TRUE;
                } else {
                    menu_control.step = STEP_SINGLE_CHOSEN;
                    menu_works.work_17->shown = FALSE;
                    menu_works.work_05->single.cursor = NEW_MENU_SINGLE_STORY;
                }
                break;
            case NEW_MENU_TOP_VERSUS:
                if (pad_player_is_connected(SECOND_PLAYER)) {
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    if (new_menu_screen_has_choice(&menu_works.work_05->versus,
                                                   NEW_MENU_VERSUS_BUTTONS)) {
                        menu_works.work_05->top.shown = FALSE;
                        menu_works.work_05->top.lit = FALSE;
                        menu_control.step = STEP_VERSUS_SWING;
                        menu_works.work_05->versus.shown = TRUE;
                    } else {
                        menu_works.work_05->shown = FALSE;
                        menu_works.work_05->unk_04 = FALSE;
                        menu_control.step = STEP_VERSUS_CHOSEN;
                        menu_works.work_05->versus.cursor =
                            NEW_MENU_VERSUS_BATTLE;
                    }
                } else {
                    sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                }
                break;
            case NEW_MENU_TOP_EXTRAS:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_works.work_05->top.shown = FALSE;
                menu_works.work_05->top.lit = FALSE;
                menu_control.step = STEP_EXTRAS_SWING;
                menu_works.work_05->extras.shown = TRUE;
                break;
            case NEW_MENU_TOP_OPTIONS:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_works.work_05->top.shown = FALSE;
                menu_works.work_05->top.lit = FALSE;
                menu_works.work_05->shown = FALSE;
                menu_works.work_05->unk_04 = FALSE;
                menu_works.work_17->shown = FALSE;
                menu_control.step = STEP_OPTIONS;
                break;
            case NEW_MENU_TOP_INTERNET:
                if (menu_works.work_05->top.refused[NEW_MENU_TOP_INTERNET] ==
                    0) {
                    menu_works.work_05->shown = FALSE;
                    menu_works.work_05->unk_04 = FALSE;
                    title_screen_hide();
                    menu_control.step = STEP_INTERNET;
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                } else {
                    sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                }
                break;
            }
        } else if (pad_press_repeat[0] & pad_cancel_buttons) {
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            menu_works.work_05->shown = FALSE;
            menu_works.work_05->unk_04 = FALSE;
            title_screen_start();
            menu_control.mode = MENU_MODE_TITLE;
            menu_works.work_17->shown = FALSE;
        }
        break;
    case STEP_SINGLE_SWING:
        if (menu_works.work_05->single.swinging) {
            break;
        }
        menu_works.work_05->single.lit = TRUE;
        menu_control.step = STEP_SINGLE;
        menu_works.work_17->shown = TRUE;
        break;
    case STEP_SINGLE:
        switch (menu_works.work_05->single.cursor) {
        case NEW_MENU_SINGLE_STORY:
            message_sprite_draw(HELP_STORY, &help_pos, 1.0F);
            break;
        case NEW_MENU_SINGLE_STAGES:
            message_sprite_draw(HELP_STAGES, &help_pos, 1.0F);
            break;
        case NEW_MENU_SINGLE_KART:
            message_sprite_draw(HELP_KART, &help_pos, 1.0F);
            break;
        case NEW_MENU_SINGLE_BOSSES:
            message_sprite_draw(HELP_BOSSES, &help_pos, 1.0F);
            break;
        }
        if (pad_press_repeat[0] & PDD_DGT_KR) {
            new_menu_screen_next(&menu_works.work_05->single,
                                 NEW_MENU_SINGLE_BUTTONS);
        } else if (pad_press_repeat[0] & PDD_DGT_KL) {
            new_menu_screen_previous(&menu_works.work_05->single);
            /* @hack type: the pad's masks cast to int, where their own
             * Uint32 would do */
        } else if ((int)pad_press_repeat[0] & (int)pad_cancel_buttons) {
            menu_control.step = STEP_TOP_SWING;
            menu_works.work_05->single.shown = FALSE;
            menu_works.work_05->single.lit = FALSE;
            menu_works.work_05->top.shown = TRUE;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
        } else if (pad_press_repeat[0] & pad_choose_buttons) {
            switch (menu_works.work_05->single.cursor) {
            case NEW_MENU_SINGLE_STORY:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_works.work_05->shown = FALSE;
                menu_works.work_05->unk_04 = FALSE;
                menu_control.step = STEP_SINGLE_CHOSEN;
                menu_works.work_17->shown = FALSE;
                break;
            case NEW_MENU_SINGLE_STAGES:
                if (save_data.stages[STAGE_CITY_ESCAPE].ranks[MISSION_FIRST] ||
                    save_data.stages[STAGE_IRON_GATE].ranks[MISSION_FIRST]) {
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    menu_works.work_05->shown = FALSE;
                    menu_works.work_05->unk_04 = FALSE;
                    title_screen_hide();
                    menu_control.step = STEP_SINGLE_CHOSEN;
                } else {
                    sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                }
                break;
            case NEW_MENU_SINGLE_KART:
                if (save_data.cart_stage[SAVE_CART_OTHERS]
                        .ranks[MISSION_FIRST] ||
                    save_data.cart_stage[SAVE_CART_ROUGE]
                        .ranks[MISSION_FIRST]) {
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    menu_works.work_05->shown = FALSE;
                    menu_works.work_05->unk_04 = FALSE;
                    title_screen_hide();
                    menu_works.work_13->two_players = FALSE;
                    menu_control.step = STEP_SINGLE_CHOSEN;
                } else {
                    sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                }
                break;
            case NEW_MENU_SINGLE_BOSSES:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_works.work_05->shown = FALSE;
                menu_works.work_05->unk_04 = FALSE;
                title_screen_hide();
                menu_control.step = STEP_SINGLE_CHOSEN;
                break;
            default:
                sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                break;
            }
        }
        break;
    case STEP_SINGLE_CHOSEN:
        switch (menu_works.work_05->single.cursor) {
        case NEW_MENU_SINGLE_STORY:
            menu_works.work_05->shown = FALSE;
            menu_works.work_05->unk_04 = FALSE;
            title_screen_hide();
            story_select_start();
            menu_control.mode = MENU_MODE_STORY_SELECT;
            break;
        case NEW_MENU_SINGLE_STAGES:
            if (!menu_works.work_05->swinging &&
                !menu_works.alpha_title->fading) {
                stage_map_menu_start();
                menu_control.mode = MENU_MODE_STAGE_MAP;
            }
            break;
        case NEW_MENU_SINGLE_KART:
            if (!menu_works.work_05->swinging &&
                !menu_works.alpha_title->fading) {
                kart_race_start();
                menu_control.mode = MENU_MODE_KART_RACE;
            }
            break;
        case NEW_MENU_SINGLE_BOSSES:
            if (!menu_works.work_05->swinging &&
                !menu_works.alpha_title->fading) {
                boss_attack_start();
                menu_control.mode = MENU_MODE_BOSS_ATTACK;
            }
        }
        break;
    case STEP_VERSUS_SWING:
        if (menu_works.work_05->versus.swinging) {
            break;
        }
        menu_control.step = STEP_VERSUS;
        menu_works.work_05->versus.lit = TRUE;
        break;
    case STEP_VERSUS:
        switch (menu_works.work_05->versus.cursor) {
        case NEW_MENU_VERSUS_BATTLE:
            message_sprite_draw(HELP_BATTLE, &help_pos, 1.0F);
            break;
        case NEW_MENU_VERSUS_KART:
            message_sprite_draw(HELP_KART, &help_pos, 1.0F);
            break;
        case NEW_MENU_VERSUS_CHAO_RACE:
            message_sprite_draw(HELP_CHAO_RACE, &help_pos, 1.0F);
            break;
        }
        if (pad_press_repeat[0] & PDD_DGT_KR) {
            new_menu_screen_next(&menu_works.work_05->versus,
                                 NEW_MENU_VERSUS_BUTTONS);
        } else if (pad_press_repeat[0] & PDD_DGT_KL) {
            new_menu_screen_previous(&menu_works.work_05->versus);
        } else if ((pad_press_repeat[0] & pad_cancel_buttons) ||
                   !pad_player_is_connected(SECOND_PLAYER)) {
            menu_control.step = STEP_TOP_SWING;
            menu_works.work_05->versus.shown = FALSE;
            menu_works.work_05->versus.lit = FALSE;
            menu_works.work_05->top.shown = TRUE;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            /* @hack type: the pad's masks cast to int, where their own
             * Uint32 would do */
        } else if ((int)pad_press_repeat[0] & (int)pad_choose_buttons) {
            switch (menu_works.work_05->versus.cursor) {
            case NEW_MENU_VERSUS_BATTLE:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_works.work_05->shown = FALSE;
                menu_works.work_05->unk_04 = FALSE;
                menu_control.step = STEP_VERSUS_CHOSEN;
                break;
            case NEW_MENU_VERSUS_KART:
                if (save_data.cart_stage[SAVE_CART_OTHERS]
                        .ranks[MISSION_FIRST] ||
                    save_data.cart_stage[SAVE_CART_ROUGE]
                        .ranks[MISSION_FIRST]) {
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    menu_works.work_05->shown = FALSE;
                    menu_works.work_05->unk_04 = FALSE;
                    title_screen_hide();
                    menu_works.work_13->two_players = TRUE;
                    menu_control.step = STEP_VERSUS_CHOSEN;
                } else {
                    sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                }
                break;
            case NEW_MENU_VERSUS_CHAO_RACE:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_snapshot_save();
                menu_resume_target = MENU_RESUME_VERSUS;
                menu_works.work_05->shown = FALSE;
                menu_works.work_05->unk_04 = FALSE;
                title_screen_hide();
                menu_control.step = STEP_VERSUS_CHOSEN;
                break;
            }
        }
        break;
    case STEP_VERSUS_CHOSEN:
        if (menu_works.work_05->swinging) {
            break;
        }
        switch (menu_works.work_05->versus.cursor) {
        case NEW_MENU_VERSUS_BATTLE:
            menu_works.work_05->shown = FALSE;
            menu_works.work_05->unk_04 = FALSE;
            title_screen_hide();
            versus_select_start();
            menu_control.mode = MENU_MODE_VERSUS_SELECT;
            break;
        case NEW_MENU_VERSUS_KART:
            /* @hack redundant: the swing tested again, where the test of the
             * step's start would do */
            if (!menu_works.work_05->swinging &&
                !menu_works.alpha_title->fading) {
                kart_race_start();
                menu_control.mode = MENU_MODE_KART_RACE;
            }
            break;
        case NEW_MENU_VERSUS_CHAO_RACE:
            sequence_start(SEQUENCE_NONE);
            stage_set_current(STAGE_CHAO);
            stage_set_character(0, PLAYER_SONIC);
            game_state.split_screen = 0;
            game_state.two_players = 0;
            game_state.demo = 0;
            game_state.next_mode = GAME_MODE_STAGE;
            game_state.chao_race_menu = CHAO_RACE_MENU_VERSUS;
            return TRUE;
        }
        break;
    case STEP_EXTRAS_SWING:
        if (menu_works.work_05->extras.swinging) {
            break;
        }
        menu_control.step = STEP_EXTRAS;
        menu_works.work_05->extras.lit = TRUE;
        break;
    case STEP_EXTRAS:
        switch (menu_works.work_05->extras.cursor) {
        case NEW_MENU_EXTRAS_RANKING:
            message_sprite_draw(HELP_RANKING, &help_pos, 1.0F);
            break;
        case NEW_MENU_EXTRAS_TUTORIAL:
            message_sprite_draw(HELP_TUTORIAL, &help_pos, 1.0F);
            break;
        case NEW_MENU_EXTRAS_EMBLEMS:
            message_sprite_draw(HELP_EMBLEMS, &help_pos, 1.0F);
            break;
        case NEW_MENU_EXTRAS_DOWNLOADS:
            message_sprite_draw(HELP_DOWNLOADS, &help_pos, 1.0F);
            break;
        }
        if (pad_press_repeat[0] & PDD_DGT_KR) {
            new_menu_screen_next(&menu_works.work_05->extras,
                                 NEW_MENU_EXTRAS_BUTTONS);
        } else if (pad_press_repeat[0] & PDD_DGT_KL) {
            new_menu_screen_previous(&menu_works.work_05->extras);
            /* @hack type: the pad's masks cast to int, where their own
             * Uint32 would do */
        } else if ((int)pad_press_repeat[0] & (int)pad_cancel_buttons) {
            menu_control.step = STEP_TOP_SWING;
            menu_works.work_05->extras.shown = FALSE;
            menu_works.work_05->extras.lit = FALSE;
            menu_works.work_05->top.shown = TRUE;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
        } else if (pad_press_repeat[0] & pad_choose_buttons) {
            switch (menu_works.work_05->extras.cursor) {
            case NEW_MENU_EXTRAS_RANKING:
                menu_works.work_05->shown = FALSE;
                menu_works.work_05->unk_04 = FALSE;
                title_screen_hide();
                menu_control.step = STEP_EXTRAS_CHOSEN;
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                break;
            case NEW_MENU_EXTRAS_TUTORIAL:
                menu_works.work_05->shown = FALSE;
                menu_works.work_05->unk_04 = FALSE;
                title_screen_hide();
                menu_control.step = STEP_EXTRAS_CHOSEN;
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                break;
            case NEW_MENU_EXTRAS_EMBLEMS:
                menu_works.work_05->shown = FALSE;
                menu_works.work_05->unk_04 = FALSE;
                title_screen_hide();
                menu_control.step = STEP_EXTRAS_CHOSEN;
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                break;
            case NEW_MENU_EXTRAS_DOWNLOADS:
                menu_works.work_05->shown = FALSE;
                menu_works.work_05->unk_04 = FALSE;
                title_screen_hide();
                menu_control.step = STEP_EXTRAS_CHOSEN;
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                break;
            }
        }
        break;
    case STEP_EXTRAS_CHOSEN:
        if (menu_works.work_05->swinging) {
            break;
        }
        switch (menu_works.work_05->extras.cursor) {
        case NEW_MENU_EXTRAS_RANKING:
            world_ranking_menu_start();
            menu_control.mode = MENU_MODE_WORLD_RANKING;
            break;
        case NEW_MENU_EXTRAS_TUTORIAL:
            tutorial_start();
            menu_control.mode = MENU_MODE_TUTORIAL;
            break;
        case NEW_MENU_EXTRAS_EMBLEMS:
            emblem_results_menu_start();
            menu_control.mode = MENU_MODE_EMBLEM_RESULTS;
            break;
        case NEW_MENU_EXTRAS_DOWNLOADS:
            download_start();
            menu_control.mode = MENU_MODE_DOWNLOAD;
            break;
        /* @hack redundant: a default that only breaks */
        default:
            break;
        }
        break;
    case STEP_OPTIONS:
        /* @hack spelling: an empty branch for the swing, where a test of its
         * negation would do */
        if (menu_works.work_05->swinging) {
        } else {
            options_start();
            menu_control.mode = MENU_MODE_OPTIONS;
        }
        break;
    case STEP_INTERNET:
        if (menu_works.work_05->swinging) {
            break;
        }
        game_state.next_mode = GAME_MODE_TO_MENU;
        menus_leave_game = TRUE;
        return TRUE;
    }
    return FALSE;
}

static void new_menu_screen_next(struct new_menu_group *group, int buttons)
{
    int found = -1;
    int i;

    for (i = group->cursor + 1; i < buttons; i++) {
        if (group->refused[i] == 0) {
            found = i;
            break;
        }
    }
    if (found >= 0) {
        sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
        group->cursor = found;
    }
}

static void new_menu_screen_previous(struct new_menu_group *group)
{
    int found = -1;
    int i;

    for (i = group->cursor - 1; i >= 0; i--) {
        if (group->refused[i] == 0) {
            found = i;
            break;
        }
    }
    if (found >= 0) {
        sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
        group->cursor = found;
    }
}

static Bool new_menu_screen_has_choice(struct new_menu_group *group,
                                       int buttons)
{
    int choices = 0;
    int i;

    for (i = 0; i < buttons; i++) {
        if (group->refused[i] == 0) {
            choices++;
        }
    }
    /* @hack spelling: the result picked by an if, where the comparison
     * returned would do */
    if (choices >= 2) {
        return TRUE;
    }
    return FALSE;
}

void new_menu_screen_resume(void)
{
    menu_snapshot_restore();
    new_menu_screen_start();
    menu_control.mode = MENU_MODE_NEW_MENU;
    menu_control.step = STEP_VERSUS_SWING;
}
