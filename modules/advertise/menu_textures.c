/* the menus' textures: which works a menu keeps loaded, and their reload
 * across a change of display */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/alpha_title.h"
#include "advertise/boss_attack_panel.h"
#include "advertise/download_panel.h"
#include "advertise/emblem_results.h"
#include "advertise/file_select.h"
#include "advertise/kart_race_panel.h"
#include "advertise/main_menu.h"
#include "advertise/menu_textures.h"
#include "advertise/new_menu.h"
#include "advertise/options_draw.h"
#include "advertise/screen_effect.h"
#include "advertise/screen_fade.h"
#include "advertise/share_data.h"
#include "advertise/sound_test_panel.h"
#include "advertise/stage_map.h"
#include "advertise/story_select_draw.h"
#include "advertise/synopsis.h"
#include "advertise/tutorial_panel.h"
#include "advertise/tv_setting_draw.h"
#include "advertise/versus_select_draw.h"
#include "advertise/world_ranking.h"
#include "main.h"
#include "system/game_state.h"

/* the works, by their place in menu_works */
enum {
    WORK_ALPHA_TITLE,
    WORK_MAIN_MENU,
    WORK_STAGE_MAP,
    WORK_STORY_SELECT,
    WORK_VERSUS_SELECT,
    WORK_NEW_MENU,
    WORK_SYNOPSIS,
    WORK_SCREEN_FADE,
    WORK_OPTIONS,
    WORK_TV_SETTING,
    WORK_SOUND_TEST,
    WORK_SHARE_DATA,
    WORK_FILE_SELECT,
    WORK_KART_RACE,
    WORK_DOWNLOAD,
    WORK_BOSS_ATTACK,
    WORK_TUTORIAL,
    WORK_SCREEN_EFFECT,
    WORK_WORLD_RANKING,
    WORK_EMBLEM_RESULTS,
    WORKS
};

#define FRAME_BLANKS 1

/* a work's textures. selected is TRUE while the work is in the group last
 * selected */
struct work_textures {
    Bool selected;
    void (*load)(void);
    void (*release)(void);
};

static struct work_textures textures[WORKS] = {
    {0, alpha_title_load, alpha_title_release},
    {0, main_menu_load, main_menu_release},
    {0, stage_map_load, stage_map_release},
    {0, story_select_load, story_select_release},
    {0, versus_select_load, versus_select_release},
    {0, new_menu_load, new_menu_release},
    {0, synopsis_load, synopsis_release},
    {0, screen_fade_load, screen_fade_release},
    {0, options_draw_load, options_draw_release},
    {0, tv_setting_draw_load, tv_setting_draw_release},
    {0, menu_sound_test_panel_load, menu_sound_test_panel_release},
    {0, share_data_load, share_data_release},
    {0, file_select_load, file_select_release},
    {0, kart_race_panel_load, kart_race_panel_release},
    {0, download_panel_load, download_panel_release},
    {0, boss_attack_panel_load, boss_attack_panel_release},
    {0, tutorial_panel_load, tutorial_panel_release},
    {0, screen_effect_load, screen_effect_release},
    {0, world_ranking_load, world_ranking_release},
    {0, emblem_results_load, emblem_results_release},
};

void menu_textures_select(Int group)
{
    Int i;

    for (i = 0; i < WORKS; i++) {
        textures[i].selected = FALSE;
    }
    /* @hack spelling: the new menu's work marked before the story select's,
     * in each case that takes both, where the works' order would do */
    switch (group) {
    case MENU_TEXTURES_TITLE:
        textures[WORK_ALPHA_TITLE].selected = TRUE;
        textures[WORK_NEW_MENU].selected = TRUE;
        textures[WORK_STORY_SELECT].selected = TRUE;
        textures[WORK_FILE_SELECT].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_MAIN_MENU:
        textures[WORK_ALPHA_TITLE].selected = TRUE;
        textures[WORK_MAIN_MENU].selected = TRUE;
        textures[WORK_STORY_SELECT].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_STAGE_MAP:
        textures[WORK_ALPHA_TITLE].selected = TRUE;
        textures[WORK_STAGE_MAP].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_STORY_SELECT:
        textures[WORK_ALPHA_TITLE].selected = TRUE;
        textures[WORK_NEW_MENU].selected = TRUE;
        textures[WORK_STORY_SELECT].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_VERSUS_SELECT:
        textures[WORK_ALPHA_TITLE].selected = TRUE;
        textures[WORK_VERSUS_SELECT].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    /* @hack spelling: the title's case copied under the new menu's and the
     * file select's, where their labels beside the title's would do */
    case MENU_TEXTURES_NEW_MENU:
        textures[WORK_ALPHA_TITLE].selected = TRUE;
        textures[WORK_NEW_MENU].selected = TRUE;
        textures[WORK_STORY_SELECT].selected = TRUE;
        textures[WORK_FILE_SELECT].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_SYNOPSIS:
        textures[WORK_STORY_SELECT].selected = TRUE;
        textures[WORK_SYNOPSIS].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_OPTIONS:
        textures[WORK_ALPHA_TITLE].selected = TRUE;
        textures[WORK_OPTIONS].selected = TRUE;
        textures[WORK_FILE_SELECT].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_TV_SETTING:
        textures[WORK_OPTIONS].selected = TRUE;
        textures[WORK_TV_SETTING].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_SOUND_TEST:
        textures[WORK_ALPHA_TITLE].selected = TRUE;
        textures[WORK_OPTIONS].selected = TRUE;
        textures[WORK_SOUND_TEST].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_FILE_SELECT:
        textures[WORK_ALPHA_TITLE].selected = TRUE;
        textures[WORK_NEW_MENU].selected = TRUE;
        textures[WORK_STORY_SELECT].selected = TRUE;
        textures[WORK_FILE_SELECT].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_KART_RACE:
        textures[WORK_KART_RACE].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    /* @hack spelling: the kart race's case copied under the download's,
     * where its label beside the kart race's would do */
    case MENU_TEXTURES_DOWNLOAD:
        textures[WORK_KART_RACE].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_BOSS_ATTACK:
        textures[WORK_BOSS_ATTACK].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_TUTORIAL:
        textures[WORK_TUTORIAL].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    case MENU_TEXTURES_WORLD_RANKING:
        textures[WORK_WORLD_RANKING].selected = TRUE;
        textures[WORK_EMBLEM_RESULTS].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    /* @hack spelling: the world ranking's case copied under the emblem
     * results', where its label beside the world ranking's would do */
    case MENU_TEXTURES_EMBLEM_RESULTS:
        textures[WORK_WORLD_RANKING].selected = TRUE;
        textures[WORK_EMBLEM_RESULTS].selected = TRUE;
        textures[WORK_SCREEN_EFFECT].selected = TRUE;
        break;
    }
    textures[WORK_SHARE_DATA].selected = TRUE;
    for (i = 0; i < WORKS; i++) {
        if (!textures[i].selected) {
            textures[i].release();
        }
    }
    njGarbageTexture(texmemlist, TEXTURE_COUNT);
    for (i = 0; i < WORKS; i++) {
        if (textures[i].selected) {
            textures[i].load();
        }
    }
}

void menu_textures_change_display(Int display)
{
    Int i;
    Int mode;

    njWaitVSync();
    njWaitVSync();
    njWaitVSync();
    for (i = 0; i < WORKS; i++) {
        if (textures[i].selected) {
            textures[i].release();
        }
    }
    njExitPrint();
#ifdef NONMATCHING
    /* GCC can't see that every path sets mode. display is a DISPLAY_ value,
     * as menu_textures.h requires */
    mode = NJD_RESOLUTION_640x480_NTSCNI;
#endif
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    switch (display) {
    case DISPLAY_VGA:
        mode = NJD_RESOLUTION_VGA;
        break;
    case DISPLAY_PAL:
        mode = RESOLUTION_PAL;
        break;
    case DISPLAY_NTSC:
        mode = NJD_RESOLUTION_640x480_NTSCNI;
        break;
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
    njWaitVSync();
    njWaitVSync();
    njChangeSystem(mode, NJD_FRAMEBUFFER_MODE_RGB565, FRAME_BLANKS);
    njInitVertexBuffer(OPAQUE_WORDS, OPAQUE_MODIFIER_WORDS, TRANSLUCENT_WORDS,
                       TRANSLUCENT_MODIFIER_WORDS, PUNCH_THROUGH_WORDS);
    njInitPrint(NULL, 0, 0);
    njPrintSize(PRINT_SIZE);
    for (i = 0; i < WORKS; i++) {
        if (textures[i].selected) {
            textures[i].load();
        }
    }
}
