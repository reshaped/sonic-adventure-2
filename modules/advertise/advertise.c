/* ADVERTISE.PRS's init and exit, which its head names. the program runs the
 * fake title or the menus */
#include "advertise/advertise.h"
#include "advertise/fake_title.h"
#include "advertise/menus.h"
#include "hud/hud.h"
#include "save/card.h"
#include "shared/messenger.h"
#include "sound/adx.h"
#include "sound/lists.h"
#include "sound/sound.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "vmu/lcd.h"

#define FRAME_BLANKS 1
#define FRAME_STEPS 1

/* @hack type: the name in an array of 12 bytes, where an array sized by
 * its string would do */
static const char sound_file[12] = "menu.mlt";
static const struct sound menu_sound_list[9] = {
#include "advertise/advertise/menu_sound_list.inc"
};
static const struct sound_list menu_sounds = {
#include "advertise/advertise/menu_sounds.inc"
};

/* the menu_set of each of the program's two halves */
static struct menu_set *dat_8c55cd84[2] = {&fake_title_set.set, &menus_set};

void advertise_init(void)
{
    frame_pacing_init(FRAME_BLANKS, FRAME_STEPS);
    task_init();
    sound_task_create(NULL);
    sound_load_menus(sound_file, &menu_sounds);
    card_init();
    lcd_start();
    if (game_state.lives[0] == 0) {
        lives_reset();
    }
    if (game_state.display_pending) {
        menus_init();
        PROGRAM_OVERLAY->step = menus_step;
    } else {
        switch (game_state.edition) {
        case EDITION_FAKE_TITLE_2:
        case EDITION_FAKE_TITLE_5:
        case EDITION_TENTO:
            fake_title_init();
            PROGRAM_OVERLAY->step = fake_title_step;
            break;
        /* @hack redundant: the cases of 3, 4 and 7, which the default
         * covers */
        case 3:
        case 4:
        case 7:
        default:
            menus_init();
            PROGRAM_OVERLAY->step = menus_step;
            break;
        }
    }
    game_state.four_cameras = 0;
    game_state.single_view = 0;
    game_state.chao_race_menu = CHAO_RACE_MENU_NONE;
}

void advertise_exit(void)
{
    messenger_messages_free();
    card_exit();
    lcd_stop();
    fun_8c059fb2();
    sound_stop_all_and_unpause();
    sound_task_kill();
    task_destroy_all();
    game_state.idle_dim = 1;
    game_state.card_warning = 0;
}
