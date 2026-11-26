/* the cart's debug menu of play modes. no code calls cart_debug_menu */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "cart/course.h"
#include "cart/stage.h"
#include "hud/message_window.h"
#include "system/game_state.h"
#include "system/pad.h"

/* the menu's rows, in names' order. a choice writes its row to
 * cart_setup.play_mode. enum cart_play_mode numbers the play modes otherwise:
 * there 0 is a kart race and 1 a story race */
enum debug_mode { NORMAL_STAGE, ONE_PLAYER, TWO_PLAYERS, NETWORK, MODES };
/* the first row stands at MENU_COLUMN and MENU_ROW of njPrint's grid */
#define MENU_COLUMN 5
#define MENU_ROW 5
#define SELECTED_COLOR 0xff44ccff
#define TEXT_COLOR 0xffffffff
/* NETWORK opens network_message for MESSAGE_STEPS steps */
#define MESSAGE_STEPS 120

static const char name_format[4] = "%s";
/* the kana of "dame na no...", it won't do, in Shift JIS */
static const char network_message[16] =
    "\x82\xbe\x82\xdf\x82\xc8\x82\xcc\x81\x45\x81\x45\x81\x45";
static const char normal_name[24] = "1 PLAYER(NORMAL STAGE)";
static const char mini_name[] = "1 PLAYER(MINI GAME)";
static const char versus_name[] = "2 PLAYER(MINI GAME)";
static const char network_name[] = "NETWORK(MINI GAME) mitya dame!";
static const char *names[] = {normal_name, mini_name, versus_name,
                              network_name};
static Uint32 back_colors[] = {0x80ff0000, 0x8000ff00, 0x800000ff};
static Sint32 selection;

static void draw_modes(void)
{
    int i;
    for (i = 0; i < MODES; i++) {
        if (i == selection) {
            njPrintColor(SELECTED_COLOR);
        }
        njPrint(NJM_LOCATION(MENU_COLUMN, i + MENU_ROW), name_format, names[i]);
        njPrintColor(TEXT_COLOR);
    }
}
void cart_debug_menu(void)
{
    if (cart_setup.frames == 0) {
        selection = ONE_PLAYER;
    }
    if (player_pads[0]->press & PDD_DGT_KD) {
        selection++;
        if (selection >= MODES) {
            selection = 0;
        }
    } else if (player_pads[0]->press & PDD_DGT_KU) {
        selection--;
        if (selection < 0) {
            selection = MODES - 1;
        }
    }
    draw_modes();
    njSetBackColor(back_colors[0], back_colors[1], back_colors[2]);
    if (player_pads[0]->press & PDD_DGT_TA) {
        cart_setup.play_mode = selection;
        switch (selection) {
        /* @hack spelling: a call for each of the first three rows, where one
         * call after their three labels would do */
        case NORMAL_STAGE:
            cart_set_mode(CART_MODE_CHOSEN);
            break;
        case ONE_PLAYER:
            cart_set_mode(CART_MODE_CHOSEN);
            break;
        case TWO_PLAYERS:
            cart_set_mode(CART_MODE_CHOSEN);
            break;
        case NETWORK:
            message_window_open_unowned(MESSAGE_WINDOW_LOW, network_message,
                                        MESSAGE_STEPS,
                                        game_state.text_language);
            break;
        }
    }
}
