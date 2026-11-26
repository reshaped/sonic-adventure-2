/* the action window: the box at the top right of a player's screen that
 * names the player's current action. the cart window is a box of the same
 * look, which shows a text its owner gives */
#ifndef HUD_ACTION_WINDOW_H
#define HUD_ACTION_WINDOW_H
#include <ninja.h>
#include <sg_xpt.h>

/* the part of a texture that the icon draws in the carts' stages. u0 and v0
 * give its top left corner, and u1 and v1 its bottom right */
struct icon_rect {
    Float u0, v0;
    Float u1, v1;
    NJS_TEXLIST *texlist;
};

struct text_sprite;

/* a window's record. a player's window keeps it in its task's extra. a cart
 * window's record is its owner's, which the task reaches through motion */
struct action_window {
    Sint8 player; /* the slot of the player whose window it is */
    /* a player's window sets it to 1 at a press of pad_next_action_buttons
     * that steps the action. the window clears it as it makes the next
     * text's sprite. that text's icon then shows unfaded at once, and the
     * text itself shows from the next draw on. a cart window shows only while
     * its owner keeps it set */
    Sint8 pressed;
    /* counts the window's draws, or a cart window's steps, for the icon's
     * fade and the text's delay */
    Sint16 timer;
    /* the width of the text shown so far, in pixels. it grows to text_width
     * times the sprite's scale_x */
    Sint16 width;
    /* the text's size, before the sprite's scale */
    Uint16 text_width;
    Uint16 text_height;
    /* in a player's window, the index in text_file of the text the sprite
     * shows */
    Sint32 text;
    struct text_sprite *sprite; /* the text's sprite, or null */
    /* in the carts' stages, the window's place on the screen. x is both the
     * icon's left edge and the text's right edge. y is the text's top */
    Float x;
    Float y;
    Uint8 unk_1c[4]; /* nothing reads these 4 bytes */
};

/* start the window of the player in slot player, as a child of the player's
 * task. the slot must hold a task, unchecked. on the STAGE_VERSUS stages only
 * player 0 gets a window. nothing checks that the heap gives the task and its
 * record.
 *
 * the window's step moves the player's action through the actions its record
 * offers. a press of pad_next_action_buttons moves it to the next while none
 * of pad_action_buttons is held. ACTION_SKIPPED moves to the next without a
 * press. the last action, or one the record doesn't offer, moves to the first.
 * the step sets the action to 0 while the record offers none or its task has
 * no step. on STAGE_CHAO, while stage_hooks.chao_area is set and returns other
 * than 1, 2 or 3, the window neither steps nor draws */
void action_window_start(int player);

/* cart_window_start's modes. each is a screen the window shows on: a
 * one-player race's, or a two-player race's upper or lower */
#define CART_WINDOW_WHOLE 0
#define CART_WINDOW_UPPER 1
#define CART_WINDOW_LOWER 2
/* start a task named cartAWindowExec, in level 2, that shows window in the
 * carts' stages. window stays its owner's: it must outlast the task, which
 * never frees it. the owner must give window its sprite, text_width and
 * text_height before the task's first step. nothing checks that task_create
 * gives the task. a mode outside CART_WINDOW_ leaves window's y as it is.
 *
 * the first step sets the sprite's scale and colors and the window's x. it
 * clears timer, width and pressed. while the owner then keeps pressed set, the
 * icon fades in. after a delay the text grows in too. clearing pressed hides
 * the window and starts it over */
void cart_window_start(int mode, struct action_window *window);

#endif
