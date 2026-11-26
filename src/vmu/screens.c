/* the memory cards' screens: the animation each one shows */
#include <sg_xpt.h>
#include <shinobi.h>

#include "main.h"
#include "player/player.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/util.h"
#include "vmu/lcd.h"
#include "vmu/screens.h"

/* indexes in animations. a character's second animation is the one for
 * its other look, which game_state.alt_character asks for */
#define SONIC_SECOND 8
#define SHADOW_SECOND 12
#define TAILS_SECOND 13
#define EGGMAN_SECOND 14
#define KNUCKLES_SECOND 15
#define ROUGE_SECOND 16
/* player_animation_entry's result for a screen that gets no animation */
#define NO_ANIMATION (-1)

/* VMU screens store 48 one-bit pixels on each of 32 rows. Each row holds
 * six bytes in the order pdVmsLcdWrite1 sends them. */
#define LCD_PICTURE_BYTES (48 * 32 / 8)

static Uint8 sonic_second_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/sonic_second_picture.inc"
};

static Uint8 eggman_second_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/eggman_second_picture.inc"
};

static Uint8 tails_second_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/tails_second_picture.inc"
};

static Uint8 rouge_second_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/rouge_second_picture.inc"
};

static Uint8 eggman_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/eggman_picture.inc"
};

static Uint8 knuckles_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/knuckles_picture.inc"
};

static Uint8 shadow_second_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/shadow_second_picture.inc"
};

static Uint8 rouge_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/rouge_picture.inc"
};

static Uint8 shadow_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/shadow_picture.inc"
};

static Uint8 sonic_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/sonic_picture.inc"
};

static Uint8 tails_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/tails_picture.inc"
};

static Uint8 knuckles_second_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/knuckles_second_picture.inc"
};

static Uint8 other_screens_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/other_screens_picture.inc"
};

static Uint8 other_print_first_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/other_print_first_picture.inc"
};

static Uint8 other_print_last_picture[LCD_PICTURE_BYTES] = {
#include "vmu/screens/other_print_last_picture.inc"
};

/* A still shows for 0xffffffff steps. Each sequence ends with a zero-step
 * frame with a null picture. */
static const struct lcd_frame sonic_second[2] = {
#include "vmu/screens/sonic_second.inc"
};

static const struct lcd_frame eggman_second[2] = {
#include "vmu/screens/eggman_second.inc"
};

static const struct lcd_frame tails_second[2] = {
#include "vmu/screens/tails_second.inc"
};

static const struct lcd_frame rouge_second[2] = {
#include "vmu/screens/rouge_second.inc"
};

static const struct lcd_frame eggman[2] = {
#include "vmu/screens/eggman.inc"
};

static const struct lcd_frame knuckles[2] = {
#include "vmu/screens/knuckles.inc"
};

static const struct lcd_frame shadow_second[2] = {
#include "vmu/screens/shadow_second.inc"
};

static const struct lcd_frame rouge[2] = {
#include "vmu/screens/rouge.inc"
};

static const struct lcd_frame shadow[2] = {
#include "vmu/screens/shadow.inc"
};

static const struct lcd_frame sonic[2] = {
#include "vmu/screens/sonic.inc"
};

static const struct lcd_frame tails[2] = {
#include "vmu/screens/tails.inc"
};

static const struct lcd_frame knuckles_second[2] = {
#include "vmu/screens/knuckles_second.inc"
};

/* the animation of a screen that shows no player's */
static const struct lcd_frame other_screens[2] = {
#include "vmu/screens/other_screens.inc"
};

/* other_print_run's pictures, each alone. nothing plays these two */
static const struct lcd_frame other_print_first[2] = {
#include "vmu/screens/other_print_first.inc"
};

static const struct lcd_frame other_print_last[2] = {
#include "vmu/screens/other_print_last.inc"
};

static const struct lcd_frame other_print_run[3] = {
#include "vmu/screens/other_print_run.inc"
};

static const struct lcd_frame characters_in_turn[8] = {
#include "vmu/screens/characters_in_turn.inc"
};

/* player_animation_entry selects a character or its second look. The mechs
 * use their pilots' animations. Entries 9 through 11 have no selector. The
 * table itself is writable, as in the original D section. */
static const struct lcd_frame *animations[17] = {
#include "vmu/screens/animations.inc"
};

/* the port of each pad's first socket, from pad A to pad D. leaving out
 * const puts the table in D, where the image holds it */
static Sint32 screen_ports[] = {PDD_PORT_A1, PDD_PORT_B1, PDD_PORT_C1,
                                PDD_PORT_D1};
#define SCREENS ARRAY_COUNT(screen_ports)

/* the index in animations of player's animation, or NO_ANIMATION for none */
static int player_animation_entry(int player)
{
    /* @hack spelling: entry, a local that each case sets for one return,
     * where a return in each case would do */
    int entry;

    if (stage_current_id() == STAGE_CHAO) {
        return NO_ANIMATION;
    }
    if (game_state.alt_character[player] != 0 && game_state.two_players != 0 &&
        stage_current_id() != STAGE_CART_1 &&
        stage_current_id() != STAGE_CART_2) {
        switch (stage_character(player)) {
        case PLAYER_SONIC:
            entry = SONIC_SECOND;
            break;
        case PLAYER_SHADOW:
            entry = SHADOW_SECOND;
            break;
        /* @hack spelling: the labels of PLAYER_TAILS_MECH and
         * PLAYER_EGGMAN_MECH ahead of PLAYER_TAILS's and PLAYER_EGGMAN's,
         * not the other way round */
        case PLAYER_TAILS_MECH:
        case PLAYER_TAILS:
            entry = TAILS_SECOND;
            break;
        case PLAYER_EGGMAN_MECH:
        case PLAYER_EGGMAN:
            entry = EGGMAN_SECOND;
            break;
        case PLAYER_KNUCKLES:
            entry = KNUCKLES_SECOND;
            break;
        case PLAYER_ROUGE:
            entry = ROUGE_SECOND;
            break;
        default:
            entry = NO_ANIMATION;
            break;
        }
        return entry;
    }
    return stage_character(player);
}

void screens_fill(void)
{
    int i;
    int entry;

    if (game_state.card_state != CARD_IDLE) {
        return;
    }
    /* @hack spelling: the cases in this order, not in the order of their
     * values */
    switch (game_state.mode) {
    case GAME_MODE_OTHER_PRINT_RUN:
        for (i = 0; (Uint32)i < SCREENS; i++) {
            if (lcd_get_loop(screen_ports[i]) == NULL) {
                lcd_set_loop(screen_ports[i], other_print_run);
            }
        }
        break;
    case GAME_MODE_STAGE_RUN:
        for (i = 0; (Uint32)i < SCREENS; i++) {
            if (lcd_get_loop(screen_ports[i]) == NULL) {
                if (i == player_pad_ports[0]) {
                    if (stage_current_id() == STAGE_BOSS_LAST1 ||
                        stage_current_id() == STAGE_BOSS_LAST2) {
                        lcd_set_loop(screen_ports[i], characters_in_turn);
                    } else {
                        entry = player_animation_entry(0);
                        if (entry != NO_ANIMATION) {
                            lcd_set_loop(screen_ports[i], animations[entry]);
                        }
                    }
                } else if (game_state.two_players != 0 &&
                           i == player_pad_ports[1]) {
                    entry = player_animation_entry(1);
                    if (entry != NO_ANIMATION) {
                        lcd_set_loop(screen_ports[i], animations[entry]);
                    }
                } else {
                    lcd_set_loop(screen_ports[i], other_screens);
                }
            }
        }
        break;
    /* @hack spelling: the labels in this order, not in the order of their
     * values */
    case GAME_MODE_MENU_RUN:
    case GAME_MODE_EVENT_RUN:
    case 3: /* a mode main.h doesn't name, and game_mode_step doesn't run */
    case GAME_MODE_DEMO_RUN:
    case GAME_MODE_ENDING_RUN:
        for (i = 0; (Uint32)i < SCREENS; i++) {
            if (lcd_get_loop(screen_ports[i]) == NULL) {
                lcd_set_loop(screen_ports[i], other_screens);
            }
        }
        break;
    }
}
