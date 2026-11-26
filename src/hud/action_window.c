/* the action window: the box at the top right of a player's screen that
 * names the player's current action. the cart window is a box of the same
 * look, which shows a text its owner gives */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "draw/text_sprite.h"
#include "hud/action_window.h"
#include "player/action.h"
#include "player/knuckles.h"
#include "player/player.h"
#include "player/sonic.h"
#include "player/walker.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/text.h"
#include "system/texture.h"

/* the last numbers of the boss stages' range and of the cart stages'. they
 * lie past STAGE_BOSS_GOLEM_E and STAGE_CART_2. no stage has these numbers:
 * loader_read_stage_overlay reads no overlay for 58, or for 72 to 79 */
#define STAGE_BOSS_BOUND 58
#define STAGE_CART_BOUND 79
/* the boss stages and the cart stages. each macro reads stage twice */
#define STAGE_BOSS(stage)                                                      \
    ((stage) >= STAGE_BOSS_BIGFOOT && (stage) <= STAGE_BOSS_BOUND)
#define STAGE_CART(stage)                                                      \
    ((stage) >= STAGE_CART_1 && (stage) <= STAGE_CART_BOUND)

/* the icon's left edge and top, and the text's right edge and top, in pixels.
 * in a two-player game, the first player's window takes the X_HALF values,
 * and both take the Y_HALF ones */
#define ICON_X 592.0F
#define ICON_X_HALF 272.0F
#define ICON_Y 40.0F
#define ICON_Y_HALF 32.0F
#define TEXT_X 576.0F
#define TEXT_X_HALF 256.0F
#define TEXT_Y 48.0F
#define TEXT_Y_HALF 40.0F
/* how far a window drops down the screen, in pixels. it drops in the
 * STAGE_BOSS and STAGE_VERSUS stages, and where MISSION_DROPS holds */
#define WINDOW_DROP 32.0F
/* whether a window drops in a stage other than the STAGE_BOSS, STAGE_CART
 * and STAGE_VERSUS stages. it drops in MISSION_TIMED. in MISSION_FIRST it
 * drops on STAGE_WHITE_JUNGLE and STAGE_SECURITY_HALL, and with one player on
 * STAGE_GREEN_FOREST. these are the three stages where time_limit_start can
 * give MISSION_FIRST a time limit */
#define MISSION_DROPS(state, stage)                                            \
    ((state)->mission == MISSION_TIMED ||                                      \
     ((state)->mission == MISSION_FIRST &&                                     \
      (((stage) == STAGE_GREEN_FOREST && (state)->two_players == 0) ||         \
       (stage) == STAGE_WHITE_JUNGLE || (stage) == STAGE_SECURITY_HALL)))
/* the icon's width and height, in pixels */
#define ICON_SIZE 32.0F
/* in the carts' stages, how many pixels above its text's top the icon stands */
#define CART_ICON_RISE 7.0F
/* a cart window's text top, in pixels down the screen. CART_TEXT_Y_HIGH is
 * on CART_WINDOW_UPPER's screen, and CART_TEXT_Y_LOW on the others */
#define CART_TEXT_Y_LOW 400.0F
#define CART_TEXT_Y_HIGH 200.0F

#define CART_WINDOW_LEVEL 2 /* a cart window's task level */

/* a window's icon gains FADE_STEP of alpha, out of ALPHA_MAX, at each count
 * of its timer. its text shows once the timer passes TEXT_DELAY, and then
 * grows WIDTH_STEP pixels a count */
#define ALPHA_MAX 255
#define FADE_STEP 16
#define TEXT_DELAY 40
#define WIDTH_STEP 8
/* the value a cart window's step holds its timer at once the text shows.
 * TIMER_HELD is above TEXT_DELAY, so the text keeps showing. TIMER_HELD times
 * FADE_STEP is above ALPHA_MAX, so the icon stays unfaded */
#define TIMER_HELD 100

/* the scale the window gives each sprite, across and down */
#define TEXT_SCALE 0.7F
/* the color and the shadow color the window gives each sprite, as ARGB.
 * they serve text_sprite.h's draws, which nothing here calls: the window
 * draws the sprite's texture itself */
#define SPRITE_COLOR 0xff000000
#define SPRITE_SHADOW_COLOR 0x404000
/* the pixels between two characters of the text a player's window makes */
#define SPRITE_PITCH_X 3

/* the global indexes of the icon's texture and the frame's, both in
 * console_texlist. CART_ICON_TEXTURE is the icon's in the carts' stages. the
 * frame's texture holds the frame's left end in u from 0 to FRAME_MIDDLE_U. it
 * holds the middle from there to 1 */
#define ICON_TEXTURE 5024
#define FRAME_TEXTURE 5023
#define CART_ICON_TEXTURE 10000000
#define FRAME_MIDDLE_U 0.5F
/* the frame's left end is FRAME_END_WIDTH pixels wide. its middle is
 * FRAME_PAD wider than the text shown, and ends at the icon's center. the
 * frame reaches FRAME_RIM above the text's top and below its bottom */
#define FRAME_END_WIDTH 16.0F
#define FRAME_PAD 24.0F
#define FRAME_RIM 3.0F

/* the tints of the text and of its shadow, as ARGB. the shadow stands
 * SHADOW_OFFSET pixels right of the text and below it. the text draws in
 * TEXT_PASSES passes, the text and then its shadow */
#define TEXT_COLOR 0xffc0c0c0
#define SHADOW_COLOR 0xff000000
#define SHADOW_OFFSET 2.0F
#define TEXT_PASSES 2
/* the icon's color, white, under the alpha its fade gives it */
#define ICON_WHITE 0x00ffffff
/* The text draws in front of its shadow. */
#define TEXT_PRIORITY 0.909F
#define SHADOW_PRIORITY 0.908F

/* a cart window's modes, in its task's work. CART_WINDOW_START sets the
 * window up for CART_WINDOW_RUN */
enum cart_window_mode { CART_WINDOW_START = 0, CART_WINDOW_RUN = 1 };

/* the first action whose text comes from the character's own table */
#define ACTION_CHARACTER 59
/* the action that ends a table of action_text */
#define ACTION_END (-1)

/* an action, and the index in text_file of the text that names it */
struct action_text {
    Sint32 action;
    Sint32 text;
};

/* the actions' texts. shared_texts holds those of the actions below
 * ACTION_CHARACTER, and each character's table those of its own. a mech with
 * the look PLAYER_KIND_BWALK or PLAYER_KIND_CWALK has a table of its own */
static struct action_text shared_texts[] = {
    {PLAYER_MODE_PICK_UP, 5},
    {PLAYER_MODE_GRAB_PUSHABLE, 6},
    {PLAYER_MODE_UNK_17, 7},
    {PLAYER_MODE_THROW, 8},
    {PLAYER_MODE_AIR_THROW, 9},
    {PLAYER_MODE_THROW_AT_TOUCH, 10},
    {PLAYER_MODE_UNK_14, 12},
    {PLAYER_MODE_LET_GO, 13},
    {PLAYER_MODE_UNK_2A, 14},
    {PLAYER_MODE_PULL_HEAVY, 5},
    {PLAYER_MODE_ASK_HINT, 15},
    {PLAYER_MODE_PLAY_FLUTE, 16},
    {PLAYER_MODE_PLAY_FLUTE_AGAIN, 16},
    {PLAYER_MODE_CANT_PLAY_FLUTE, 15},
    {PLAYER_MODE_PULL_BURIED, 17},
    {PLAYER_MODE_PET_CHAO, 11},
    {ACTION_END, 0},
};
/* clang-format off */
static struct action_text sonic_texts[] = {
    {PLAYER_MODE_SONIC_LIGHT_ATTACK, 18},
    {PLAYER_MODE_SONIC_LIGHT_DASH, 19},
    {PLAYER_MODE_SONIC_RAIL, 42},
    {PLAYER_MODE_SONIC_MAGIC_HAND, 40},
    {SONIC_ACTION_SPEED_UP, 20},
    {SONIC_ACTION_WIND_CUTTER, 21},
    {SONIC_ACTION_TIME_STOP, 22},
    {PLAYER_MODE_SONIC_BOARD_BRAKE, 34},
    {PLAYER_MODE_HOLD_HANDLE, 43},
    {ACTION_END, 0},
};
/* clang-format on */
static struct action_text shadow_texts[] = {
    {PLAYER_MODE_SONIC_LIGHT_ATTACK, 18},
    {PLAYER_MODE_SONIC_LIGHT_DASH, 19},
    {PLAYER_MODE_SONIC_RAIL, 42},
    {PLAYER_MODE_SONIC_MAGIC_HAND, 40},
    {SONIC_ACTION_SPEED_UP, 20},
    {SONIC_ACTION_WIND_CUTTER, 47},
    {SONIC_ACTION_TIME_STOP, 23},
    {PLAYER_MODE_SONIC_BOARD_BRAKE, 34},
    {ACTION_END, 0},
};
static struct action_text knuckles_texts[] = {
    {PLAYER_MODE_HUNTER_DIVE, 24},
    {PLAYER_MODE_HUNTER_ATTACK_1, 25},
    {PLAYER_MODE_HUNTER_SPIRAL, 26},
    {PLAYER_MODE_HUNTER_DIG, 27},
    {PLAYER_MODE_HUNTER_DIG_FAIL, 27},
    {PLAYER_MODE_HUNTER_WALL_DIG, 27},
    {PLAYER_MODE_HUNTER_WALL_DIG_FAIL, 27},
    {ACTION_SUNGLASSES, 28},
    {ACTION_EARTHQUAKE, 29},
    {ACTION_EARTHFLAME, 30},
    {ACTION_POWER_FLASH, 31},
    {ACTION_END, 0},
};
static struct action_text rouge_texts[] = {
    {PLAYER_MODE_HUNTER_DIVE, 48},
    {PLAYER_MODE_HUNTER_ATTACK_1, 49},
    {PLAYER_MODE_HUNTER_SPIRAL, 50},
    {PLAYER_MODE_HUNTER_DIG, 27},
    {PLAYER_MODE_HUNTER_DIG_FAIL, 27},
    {PLAYER_MODE_HUNTER_WALL_DIG, 27},
    {PLAYER_MODE_HUNTER_WALL_DIG_FAIL, 27},
    {ACTION_SUNGLASSES, 28},
    {ACTION_EARTHQUAKE, 45},
    {ACTION_EARTHFLAME, 46},
    {ACTION_POWER_FLASH, 44},
    {PLAYER_MODE_ROUGE_RIDE_FERRY, 38},
    {PLAYER_MODE_ROUGE_RIDE_SWIMMER, 39},
    {ACTION_END, 0},
};
static struct action_text eggman_mech_texts[] = {
    {ACTION_RELEASE, 32},
    {PLAYER_MODE_EGGMAN_MECH_STRIKE, 25},
    {PLAYER_MODE_MECH_SPECIAL_1, 35},
    {PLAYER_MODE_MECH_SPECIAL_2, 36},
    {PLAYER_MODE_MECH_LASER, 37},
    {ACTION_END, 0},
};
static struct action_text bwalk_texts[] = {
    {ACTION_RELEASE, 32},
    {PLAYER_MODE_EGGMAN_MECH_STRIKE, 51},
    {PLAYER_MODE_MECH_SPECIAL_1, 35},
    {PLAYER_MODE_MECH_SPECIAL_2, 36},
    {PLAYER_MODE_MECH_LASER, 37},
    {ACTION_END, 0},
};
static struct action_text tails_mech_texts[] = {
    {ACTION_RELEASE, 32},
    {PLAYER_MODE_TAILS_MECH_STRIKE, 33},
    {PLAYER_MODE_MECH_SPECIAL_1, 35},
    {PLAYER_MODE_MECH_SPECIAL_2, 36},
    {PLAYER_MODE_MECH_LASER, 37},
    {ACTION_END, 0},
};
static struct action_text cwalk_texts[] = {
    {ACTION_RELEASE, 32},
    {PLAYER_MODE_TAILS_MECH_STRIKE, 52},
    {PLAYER_MODE_MECH_SPECIAL_1, 35},
    {PLAYER_MODE_MECH_SPECIAL_2, 36},
    {PLAYER_MODE_MECH_LASER, 37},
    {ACTION_END, 0},
};
static struct action_text eggman_texts[] = {
    {PLAYER_MODE_EGGMAN_ATTACK, 41},
    {ACTION_END, 0},
};
static struct action_text tails_texts[] = {
    {PLAYER_MODE_TAILS_TAIL_ATTACK, 41},
    {ACTION_END, 0},
};

/* the text, drawn again as its shadow. the draws set every field of each
 * corner */
static NJS_TEXTURE_VTX text_polygon[4] = {
#include "hud/action_window/text_polygon.inc"
};
/* the icon outside the carts' stages. the draws set each corner's place and
 * color */
static NJS_TEXTURE_VTX icon_polygon[4] = {
#include "hud/action_window/icon_polygon.inc"
};
/* the icon in the carts' stages. the draws set each corner's place, color, u
 * and v */
static NJS_TEXTURE_VTX cart_icon_polygon[4] = {
#include "hud/action_window/cart_icon_polygon.inc"
};
/* the frame, drawn for its left end and again for its middle. the draws set
 * each corner's place, u and v */
static NJS_TEXTURE_VTX frame_polygon[4] = {
#include "hud/action_window/frame_polygon.inc"
};

static void action_window_step(struct task *task);
static void action_window_draw(struct task *task);
static void action_window_destroy(struct task *task);
static void cart_window_step(struct task *task);
static void cart_window_draw(struct task *task);
static void cart_window_destroy(struct task *task);

/* a child task of parent that runs step, with a cleared struct action_window
 * in its extra. it returns null where the heap can't give both */
static struct task *window_task_create(void (*step)(struct task *task),
                                       struct task *parent)
{
    struct task *task;
    struct action_window *window;

    task = task_create_child(0, step, parent);
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((window = syCalloc(1, sizeof(struct action_window))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = window;
    }
    return task;
}

void action_window_start(int player)
{
    struct task *task;
    struct action_window *window;
    struct task *parent = player_tasks[player];

    if (player != 0 && STAGE_VERSUS(stage_current_id())) {
        return;
    }
    task = window_task_create(action_window_step, parent);
    task->draw = action_window_draw;
    task->destructor = action_window_destroy;
    window = task->extra.p;
    window->player = player;
}

static void action_window_step(struct task *task)
{
    struct action_window *window = task->extra.p;
    struct player *record = player_records[window->player];
    int count = record->action_count;
    /* @hack spelling: current, a local for record->action, where
     * record->action read in the loop would do */
    int current;
    /* @hack spelling: action, a pointer stepped beside i, where
     * record->actions[i] would do */
    Sint8 *action;
    int i;

    if (stage_current_id() == STAGE_CHAO) {
        /* @hack type: stage_hooks.chao_area called as chao_area_hook, where the
         * member would do */
        if (chao_area_hook[0] != NULL) {
            switch (chao_area_hook[0]()) {
            case 1:
            case 2:
            case 3:
                break;
            default:
                return;
            }
        }
    }
    if (count >= 1 && player_tasks[window->player]->step != NULL) {
        if (((player_pad_records[window->player].press &
              pad_next_action_buttons) &&
             !(player_pad_records[window->player].on & pad_action_buttons)) ||
            record->action == ACTION_SKIPPED) {
            action = record->actions;
            current = record->action;
            for (i = 0; i < count; i++, action++) {
                if (current == *action) {
                    break;
                }
            }
            i++;
            /* @hack spelling: count <= i, where i >= count would do */
            if (count <= i) {
                i = 0;
            }
            record->action = record->actions[i];
            if (player_pad_records[window->player].press &
                pad_next_action_buttons) {
                window->pressed = 1;
            }
        }
    } else {
        record->action = 0;
    }
}

/* draw the text, cut to window's width, over its shadow */
static void action_window_draw_text(struct action_window *window)
{
    /* @hack type: stage as a Sint16, where an int would do: every stage
     * number fits a Sint16 */
    Sint16 stage = stage_current_id();
    Float x;
    Float y;
    /* @hack redundant: a read of the sprite's scale_x that nothing uses */
    Float scale_x = window->sprite->scale_x;
    /* @hack redundant: a copy of the sprite that nothing uses */
    struct text_sprite *sprite = window->sprite;
    Float width = window->width;
    Float height = window->sprite->scale_y * window->text_height;
    int i;
    /* @hack spelling: the record through a pointer, where game_state itself
     * would do */
    struct game_state *state = &game_state;

    /* @hack spelling: three arms whose first and last are alike, where one
     * test of two_players != 0 && player == 0 would do */
    if (state->two_players == 0) {
        x = TEXT_X - width;
    } else if (window->player == 0) {
        x = TEXT_X_HALF - width;
    } else {
        x = TEXT_X - width;
    }
    y = TEXT_Y;
    if (state->two_players) {
        y = TEXT_Y_HALF;
    }
    if (STAGE_BOSS(stage) || STAGE_VERSUS(stage)) {
        y += WINDOW_DROP;
    } else if (STAGE_CART(stage)) {
        x = window->x - width;
        y = window->y;
    } else if (MISSION_DROPS(state, stage)) {
        y += WINDOW_DROP;
    }
    njSetTexture(&window->sprite->texlist);
    text_polygon[0].z = text_polygon[1].z = text_polygon[2].z =
        text_polygon[3].z = TEXT_PRIORITY;
    text_polygon[0].col = text_polygon[1].col = text_polygon[2].col =
        text_polygon[3].col = TEXT_COLOR;
    for (i = 0; i < TEXT_PASSES; i++) {
        text_polygon[0].x = text_polygon[1].x = x;
        text_polygon[2].x = text_polygon[3].x = x + width;
        text_polygon[0].y = text_polygon[2].y = y;
        text_polygon[1].y = text_polygon[3].y = y + height;
        text_polygon[0].u = text_polygon[1].u = 0.0F;
        text_polygon[2].u = text_polygon[3].u =
            width / (window->sprite->texture_width * window->sprite->scale_x);
        text_polygon[0].v = text_polygon[2].v = 0.0F;
        text_polygon[1].v = text_polygon[3].v =
            height / (window->sprite->texture_height * window->sprite->scale_y);
        njDrawTexture(text_polygon, 4, window->sprite->texture, 1);
        /* @hack spelling: x = x + SHADOW_OFFSET and y = y + SHADOW_OFFSET,
         * where x += SHADOW_OFFSET and y += SHADOW_OFFSET would do */
        x = x + SHADOW_OFFSET;
        y = y + SHADOW_OFFSET;
        text_polygon[0].z = text_polygon[1].z = text_polygon[2].z =
            text_polygon[3].z = SHADOW_PRIORITY;
        text_polygon[0].col = text_polygon[1].col = text_polygon[2].col =
            text_polygon[3].col = SHADOW_COLOR;
    }
}

/* draw the icon, faded in by window's timer. in the carts' stages, it draws
 * the part of a texture that stage_hooks.action_icon gives */
static void action_window_draw_icon(struct action_window *window)
{
    /* @hack type: stage as Sint16, as in action_window_draw_text, where int
     * would do */
    Sint16 stage = stage_current_id();
    Uint32 alpha = window->timer * FADE_STEP;
    Uint32 color;
    Float x;
    Float y;
    struct icon_rect rect;
    /* @hack spelling: the record through a pointer, where game_state itself
     * would do */
    struct game_state *state;
    Float unused; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    if (alpha > ALPHA_MAX) {
        alpha = ALPHA_MAX;
    }
    color = (alpha << 24) | ICON_WHITE;
    /* @hack spelling: state set in the test's comma, not as a statement
     * before the if */
    /* @hack spelling: three arms whose first and last are alike, where one
     * test of two_players != 0 && player == 0 would do */
    if (state = &game_state, state->two_players == 0) {
        x = ICON_X;
    } else if (window->player == 0) {
        x = ICON_X_HALF;
    } else {
        x = ICON_X;
    }
    y = ICON_Y;
    if (state->two_players) {
        y = ICON_Y_HALF;
    }
    if (STAGE_BOSS(stage) || STAGE_VERSUS(stage)) {
        y += WINDOW_DROP;
    } else if (STAGE_CART(stage)) {
        x = window->x;
        y = window->y - CART_ICON_RISE;
        /* @hack type: stage_hooks.action_icon called as action_icon_hook,
         * where the member would do */
        action_icon_hook[0](&rect);
    } else if (MISSION_DROPS(state, stage)) {
        y += WINDOW_DROP;
    }
    /* @hack spelling: the stage read again through stage_current_id, where
     * stage would do */
    if (STAGE_CART(stage_current_id())) {
        njSetTexture(rect.texlist);
        cart_icon_polygon[0].y = cart_icon_polygon[2].y = y;
        cart_icon_polygon[1].y = cart_icon_polygon[3].y = y + ICON_SIZE;
        cart_icon_polygon[0].col = cart_icon_polygon[1].col =
            cart_icon_polygon[2].col = cart_icon_polygon[3].col = color;
        cart_icon_polygon[0].x = cart_icon_polygon[1].x = x;
        cart_icon_polygon[2].x = cart_icon_polygon[3].x = x + ICON_SIZE;
        cart_icon_polygon[0].u = rect.u0;
        cart_icon_polygon[0].v = rect.v0;
        cart_icon_polygon[1].u = rect.u0;
        cart_icon_polygon[1].v = rect.v1;
        cart_icon_polygon[2].u = rect.u1;
        cart_icon_polygon[2].v = rect.v0;
        cart_icon_polygon[3].u = rect.u1;
        cart_icon_polygon[3].v = rect.v1;
    } else {
        njSetTexture(&console_texlist);
        icon_polygon[0].y = icon_polygon[2].y = y;
        icon_polygon[1].y = icon_polygon[3].y = y + ICON_SIZE;
        /* @hack redundant: a read of icon_polygon[1] that nothing uses */
        unused = icon_polygon[1].u;
        icon_polygon[0].col = icon_polygon[1].col = icon_polygon[2].col =
            icon_polygon[3].col = color;
        /* @hack redundant: a read of icon_polygon[3] that nothing uses */
        unused = icon_polygon[3].u;
        icon_polygon[0].x = icon_polygon[1].x = x;
        icon_polygon[2].x = icon_polygon[3].x = x + ICON_SIZE;
    }
    /* @hack spelling: the stage read again through stage_current_id, where
     * stage would do */
    if (STAGE_CART(stage_current_id())) {
        njDrawTexture(cart_icon_polygon, 4, CART_ICON_TEXTURE, 1);
    } else {
        njDrawTexture(icon_polygon, 4, ICON_TEXTURE, 1);
    }
}

/* draw the frame's left end, then its middle. window's width is never below
 * 0, so the middle is never narrower than FRAME_PAD. the left end therefore
 * always draws whole, and the middle always draws */
static void action_window_draw_frame(struct action_window *window)
{
    /* @hack type: stage as Sint16, as in action_window_draw_text, where int
     * would do */
    Sint16 stage = stage_current_id();
    Float x;
    Float y;
    /* @hack redundant: a read of the sprite's scale_x that nothing uses */
    Float scale_x = window->sprite->scale_x;
    Float middle = window->width + FRAME_PAD;
    Float height = window->sprite->scale_y * window->text_height;
    /* @hack redundant: a copy of the sprite that nothing uses */
    struct text_sprite *sprite = window->sprite;
    /* @hack spelling: the record through a pointer, where game_state itself
     * would do */
    struct game_state *state = &game_state;
    Sint8 two_players; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    /* @hack spelling: three arms whose first and last are alike, where one
     * test of two_players != 0 && player == 0 would do */
    if (state->two_players == 0) {
        x = ICON_X - middle;
    } else if (window->player == 0) {
        x = ICON_X_HALF - middle;
    } else {
        x = ICON_X - middle;
    }
    y = TEXT_Y;
    if (state->two_players) {
        y = TEXT_Y_HALF;
    }
    if (STAGE_BOSS(stage) || STAGE_VERSUS(stage)) {
        y += WINDOW_DROP;
    } else if (STAGE_CART(stage)) {
        x = window->x - middle;
        y = window->y;
    } else if (MISSION_DROPS(state, stage)) {
        y += WINDOW_DROP;
    }
    /* @hack redundant: a read of two_players that nothing uses */
    two_players = state->two_players;
    njSetTexture(&console_texlist);
    if (middle >= FRAME_END_WIDTH) {
        frame_polygon[2].x = frame_polygon[3].x = x + FRAME_END_WIDTH;
        frame_polygon[2].u = frame_polygon[3].u = FRAME_MIDDLE_U;
    } else {
        frame_polygon[2].x = frame_polygon[3].x = x + middle;
        /* @hack spelling: a product by FRAME_MIDDLE_U / FRAME_END_WIDTH,
         * where middle / FRAME_END_WIDTH * FRAME_MIDDLE_U would do */
        frame_polygon[2].u = frame_polygon[3].u =
            middle * (FRAME_MIDDLE_U / FRAME_END_WIDTH);
    }
    frame_polygon[0].x = frame_polygon[1].x = x;
    frame_polygon[0].u = frame_polygon[1].u = 0.0F;
    frame_polygon[0].y = frame_polygon[2].y = y - FRAME_RIM;
    frame_polygon[1].y = frame_polygon[3].y = y + height + FRAME_RIM;
    frame_polygon[0].v = frame_polygon[2].v = 0.0F;
    frame_polygon[1].v = frame_polygon[3].v = 1.0F;
    njDrawTexture(frame_polygon, 4, FRAME_TEXTURE, 1);
    if (middle > FRAME_END_WIDTH) {
        frame_polygon[0].x = frame_polygon[1].x = x + FRAME_END_WIDTH;
        frame_polygon[2].x = frame_polygon[3].x = x + middle + FRAME_END_WIDTH;
        frame_polygon[0].y = frame_polygon[2].y = y - FRAME_RIM;
        frame_polygon[1].y = frame_polygon[3].y = y + height + FRAME_RIM;
        frame_polygon[0].u = frame_polygon[1].u = FRAME_MIDDLE_U;
        frame_polygon[2].u = frame_polygon[3].u = 1.0F;
        frame_polygon[0].v = frame_polygon[2].v = 0.0F;
        frame_polygon[1].v = frame_polygon[3].v = 1.0F;
        njDrawTexture(frame_polygon, 4, FRAME_TEXTURE, 1);
    }
}

/* a new text takes two draws. the first frees the old text's sprite and
 * clears the timer. the second makes the new sprite. the text and its frame
 * show once the timer passes TEXT_DELAY. a window with no text to show clears
 * its timer and width. under DEMO_ON the window draws nothing */
static void action_window_draw(struct task *task)
{
    struct action_window *window = task->extra.p;
    struct player *record = player_records[window->player];
    int count = record->action_count;
    int current;
    /* @hack spelling: action, a pointer stepped beside i, where
     * record->actions[i] would do */
    Sint8 *action;
    int i;
    struct action_text *table;

    if (game_state.unk_0b == 0) {
        return;
    }
    if (game_state.split_screen == 1 &&
        camera_current_index != window->player) {
        return;
    }
    if (stage_current_id() == STAGE_CHAO) {
        /* @hack type: stage_hooks.chao_area called as chao_area_hook, where the
         * member would do */
        if (chao_area_hook[0] != NULL) {
            switch (chao_area_hook[0]()) {
            case 1:
            case 2:
            case 3:
                break;
            default:
                return;
            }
        }
    }
    if (count < 1 || player_tasks[window->player]->step == NULL) {
        goto reset;
    }
    current = record->action;
    for (action = record->actions, i = 0; i < count; i++, action++) {
        if (current == *action) {
            break;
        }
    }
    /* an action the player doesn't offer shows as the first it offers */
    /* @hack spelling: count == i, where i == count would do */
    if (count == i) {
        current = record->actions[0];
        if ((record->character == PLAYER_KNUCKLES ||
             record->character == PLAYER_ROUGE) &&
            current == PLAYER_MODE_HUNTER_ATTACK_1) {
            goto reset;
        }
        if (record->character == PLAYER_EGGMAN &&
            current == PLAYER_MODE_EGGMAN_ATTACK) {
            goto reset;
        }
    }
    if (player_pad_records[window->player].on & pad_action_buttons) {
        /* @hack spelling: an empty arm and a goto in the else, where if
         * (!(...)) goto reset would do */
        if (((record->character == PLAYER_SONIC ||
              record->character == PLAYER_SHADOW) &&
             current == PLAYER_MODE_SONIC_LIGHT_ATTACK) ||
            ((record->character == PLAYER_EGGMAN_MECH ||
              record->character == PLAYER_TAILS_MECH) &&
             current == ACTION_RELEASE)) {
        } else {
            goto reset;
        }
    }
    if (current < ACTION_CHARACTER) {
        table = shared_texts;
    } else {
        switch (record->character) {
        case PLAYER_SONIC:
            table = sonic_texts;
            break;
        case PLAYER_SHADOW:
            table = shadow_texts;
            break;
        case PLAYER_TAILS:
            table = tails_texts;
            break;
        case PLAYER_KNUCKLES:
            table = knuckles_texts;
            break;
        case PLAYER_ROUGE:
            table = rouge_texts;
            break;
        case PLAYER_EGGMAN:
            table = eggman_texts;
            break;
        case PLAYER_EGGMAN_MECH:
            if (record->kind == PLAYER_KIND_BWALK) {
                table = bwalk_texts;
            } else {
                table = eggman_mech_texts;
            }
            break;
        case PLAYER_TAILS_MECH:
            if (record->kind == PLAYER_KIND_CWALK) {
                table = cwalk_texts;
            } else {
                table = tails_mech_texts;
            }
            break;
        default:
            goto reset;
        }
    }
    for (; table->action != ACTION_END; table++) {
        if (table->action == current) {
            if (window->text != table->text && window->sprite != NULL) {
                text_sprite_destroy(window->sprite);
                window->sprite = NULL;
                window->width = 0;
                window->timer = 0;
                return;
            }
            if (window->sprite == NULL) {
                window->sprite = text_sprite_create_styled(
                    TEXT(table->text), game_state.text_language, TEXT_WHITE,
                    SPRITE_PITCH_X, TEXT_PITCH_DEFAULT);
                text_sprite_measure(window->sprite, TEXT(table->text),
                                    game_state.text_language,
                                    &window->text_width, &window->text_height);
                window->sprite->scale_x = TEXT_SCALE;
                window->sprite->scale_y = TEXT_SCALE;
                window->sprite->color.color = SPRITE_COLOR;
                window->sprite->shadow_color.color = SPRITE_SHADOW_COLOR;
                window->text = table->text;
                if (window->pressed == 1) {
                    window->timer = TEXT_DELAY;
                    window->pressed = 0;
                }
            }
            njFogDisable();
            njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
            if (game_state.demo != DEMO_ON) {
                action_window_draw_icon(window);
            }
            /* @bug arithmetic: the text and its frame vanish for 32809 draws
             * once timer passes 32767. the store at 0x8c010e08 wraps timer to
             * -32768, and the test at 0x8c010e0e fails until it climbs back
             * past TEXT_DELAY */
            if (window->timer++ > TEXT_DELAY) {
                if (window->width <
                    window->text_width * window->sprite->scale_x) {
                    window->width += WIDTH_STEP;
                    if (window->width >=
                        window->text_width * window->sprite->scale_x) {
                        window->width =
                            window->text_width * window->sprite->scale_x;
                    }
                } else {
                    window->width =
                        window->text_width * window->sprite->scale_x;
                }
                if (game_state.demo != DEMO_ON) {
                    action_window_draw_text(window);
                    action_window_draw_frame(window);
                }
            }
            njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
            njFogEnable();
            return;
        }
    }
reset:
    window->timer = 0;
    window->width = 0;
}

static void action_window_destroy(struct task *task)
{
    struct action_window *window = task->extra.p;

    if (window->sprite != NULL) {
        text_sprite_destroy(window->sprite);
        window->sprite = NULL;
    }
}

void cart_window_start(int mode, struct action_window *window)
{
    struct task *task;

    task = task_create(TASK_WORK, CART_WINDOW_LEVEL, cart_window_step,
                       "cartAWindowExec");
    task->motion.p = window;
    switch (mode) {
    case CART_WINDOW_WHOLE:
    case CART_WINDOW_LOWER:
        window->y = CART_TEXT_Y_LOW;
        break;
    case CART_WINDOW_UPPER:
        window->y = CART_TEXT_Y_HIGH;
        break;
    }
    task->destructor = cart_window_destroy;
    task->draw = cart_window_draw;
}

static void cart_window_step(struct task *task)
{
    struct action_window *window = task->motion.p;

    if (task->work->mode != CART_WINDOW_START) {
        if (window->pressed != 0) {
            window->timer++;
            if (window->timer > TEXT_DELAY) {
                if (window->width <
                    window->text_width * window->sprite->scale_x) {
                    window->width += WIDTH_STEP;
                    if (window->width >=
                        window->text_width * window->sprite->scale_x) {
                        window->width =
                            window->text_width * window->sprite->scale_x;
                    }
                } else {
                    window->width =
                        window->text_width * window->sprite->scale_x;
                }
                window->timer = TIMER_HELD;
            }
        } else {
            window->width = 0;
            window->timer = 0;
        }
    } else {
        window->timer = 0;
        window->width = 0;
        window->x = ICON_X;
        window->pressed = 0;
        window->sprite->scale_x = TEXT_SCALE;
        window->sprite->scale_y = TEXT_SCALE;
        window->sprite->color.color = SPRITE_COLOR;
        window->sprite->shadow_color.color = SPRITE_SHADOW_COLOR;
        task->work->mode = CART_WINDOW_RUN;
    }
}

static void cart_window_draw(struct task *task)
{
    struct action_window *window = task->motion.p;

    if (window->pressed) {
        njFogDisable();
        njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
        action_window_draw_icon(window);
        if (window->timer > TEXT_DELAY) {
            action_window_draw_text(window);
            action_window_draw_frame(window);
        }
        njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
        njFogEnable();
    }
}

/* task_destroy frees a task's motion after its destructor runs. motion holds
 * the owner's window, so the destructor clears it */
static void cart_window_destroy(struct task *task)
{
    task->motion.p = NULL;
}
