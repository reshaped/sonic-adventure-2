/* the fade: a color drawn over the whole screen */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "draw/fade.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"

#define ALPHA_MAX 255
#define BLACK 0xff000000

/* how far a step moves the alpha or a channel, out of ALPHA_MAX */
#define FADE_STEP 5
#define FADE_STEP_FAST 6

/* the idle dim raises the alpha toward IDLE_ALPHA past IDLE_CALLS calls in a
 * row without input. until then each call lowers it by IDLE_WAKE_STEP */
#define IDLE_CALLS 7200
#define IDLE_ALPHA 192
#define IDLE_WAKE_STEP 4

/* the buttons fade_idle_step takes for input */
#define INPUT_BUTTONS                                                          \
    (PDD_DGT_TL | PDD_DGT_TR | PDD_DGT_TX | PDD_DGT_TY | PDD_DGT_KR |          \
     PDD_DGT_KL | PDD_DGT_KD | PDD_DGT_KU | PDD_DGT_ST | PDD_DGT_TA |          \
     PDD_DGT_TB)

/* nothing calls these. none is static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the fade's color, as fade_set_color takes it */
Uint32 fade_get_color(void);
/* fade_set_color(0), which zeroes the alpha and every channel */
void fade_clear(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* the whole screen, which fade_draw draws in fade_color */
static NJS_POLYGON_VTX fade_quad[4] = {
#include "draw/fade/fade_quad.inc"
};
static NJS_COLOR fade_color = {BLACK};

void fade_set_color(Uint32 color)
{
    fade_color.color = color;
    game_state.idle_calls = 0;
}

Uint32 fade_get_color(void)
{
    return fade_color.color;
}

void fade_draw(void)
{
    Bool trans;

    /* @hack redundant: the else after the return */
    if (fade_color.argb.a == 0) {
        return;
    } else if (fade_color.argb.a == ALPHA_MAX) {
        trans = FALSE;
    } else {
        trans = TRUE;
    }
    /* @hack spelling: the four colors in one chained assignment, where an
     * assignment for each, from fade_quad[0] to fade_quad[3], would do */
    fade_quad[0].col = fade_quad[1].col = fade_quad[2].col = fade_quad[3].col =
        fade_color.color;
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njDrawPolygon(fade_quad, 4, trans);
    njFogEnable();
}

void fade_idle_step(void)
{
    if (game_state.idle_dim == 0) {
        return;
    }
    if ((pad_held[0] & INPUT_BUTTONS) == 0 &&
        (pad_held[1] & INPUT_BUTTONS) == 0 &&
        (player_pads[0]->x1 | player_pads[1]->x1 | player_pads[0]->y1 |
         player_pads[1]->y1) == 0 &&
        (player_pads[0]->l | player_pads[1]->l | player_pads[0]->r |
         player_pads[1]->r) == 0) {
        game_state.idle_calls++;
    } else {
        game_state.idle_calls = 0;
    }
    if (game_state.idle_calls <= IDLE_CALLS) {
        if (fade_color.argb.a >= IDLE_WAKE_STEP) {
            fade_color.argb.a -= IDLE_WAKE_STEP;
        } else {
            fade_color.argb.a = 0;
        }
    } else {
        /* @bug logic: the dim doesn't hold IDLE_ALPHA. the test lets an alpha
         * of 192 rise to 193, and the next call sets it back to 192. CMP/GT
         * R5,R6 at 0x8c056d24 tests the alpha against the 192 in R5, and ADD
         * #1,R0 at 0x8c056d2c raises it */
        if (fade_color.argb.a <= IDLE_ALPHA) {
            fade_color.argb.a++;
        } else {
            fade_color.argb.a = IDLE_ALPHA;
        }
    }
}

void fade_set_black(void)
{
    fade_set_color(BLACK);
}

Bool fade_in_step(void)
{
    Uint8 alpha = fade_color.argb.a;
    Int step;

    if (game_state.display == DISPLAY_PAL && task_current_call == NULL) {
        step = FADE_STEP_FAST;
    } else {
        step = FADE_STEP;
    }
    if (alpha == 0) {
        return TRUE;
    }
    if (alpha < step) {
        alpha = 0;
    } else {
        alpha -= step;
    }
    fade_color.argb.a = alpha;
    return FALSE;
}

void fade_clear(void)
{
    fade_set_color(0);
    /* @hack redundant: a second store of 0 to idle_calls, which
     * fade_set_color has just zeroed */
    game_state.idle_calls = 0;
}

void fade_reset_idle(void)
{
    game_state.idle_calls = 0;
}

Bool fade_out_step(void)
{
    Uint8 alpha = fade_color.argb.a;
    Int step;

    if (game_state.display == DISPLAY_PAL && task_current_call == NULL) {
        step = FADE_STEP_FAST;
    } else {
        step = FADE_STEP;
    }
    /* @hack redundant: the else after the return */
    if (alpha == ALPHA_MAX) {
        return TRUE;
    } else if (alpha > ALPHA_MAX - step) {
        alpha = ALPHA_MAX;
    } else {
        alpha += step;
    }
    fade_color.argb.a = alpha;
    return FALSE;
}

Bool fade_to_alpha_step(Uint32 alpha)
{
    Uint32 now = fade_color.argb.a;
    Int step;

    if (game_state.display == DISPLAY_PAL && task_current_call == NULL) {
        step = FADE_STEP_FAST;
    } else {
        step = FADE_STEP;
    }
    /* @hack redundant: the else after the return */
    /* @bug arithmetic: a fade toward an alpha below the step never lands on
     * it. alpha - step, unsigned, wraps to a large value. so the fade's
     * alpha rises a step from any value up to alpha + step but alpha itself.
     * it swings between two values a step apart, and every call returns
     * FALSE. SUB R6,R3 at 0x8c056e3a subtracts, and CMP/HS R3,R5 at
     * 0x8c056e3c tests */
    if (now == alpha) {
        return TRUE;
    } else if (now > alpha + step) {
        now -= step;
    } else if (now < alpha - step) {
        now += step;
    } else {
        now = alpha;
    }
    fade_color.argb.a = now;
    return FALSE;
}

Bool fade_to_color_step(Uint8 alpha, Uint8 red, Uint8 green, Uint8 blue)
{
    Bool done = TRUE;
    Int step;
    Int now;

    if (game_state.display == DISPLAY_PAL && task_current_call == NULL) {
        step = FADE_STEP_FAST;
    } else {
        step = FADE_STEP;
    }
    now = fade_color.argb.a;
    if (now > alpha + step) {
        now -= step;
        done = FALSE;
    } else if (now < alpha - step) {
        now += step;
        done = FALSE;
    } else {
        now = alpha;
    }
    fade_color.argb.a = now;
    now = fade_color.argb.r;
    if (now > red + step) {
        now -= step;
        done = FALSE;
    } else if (now < red - step) {
        now += step;
        done = FALSE;
    } else {
        now = red;
    }
    fade_color.argb.r = now;
    now = fade_color.argb.g;
    if (now > green + step) {
        now -= step;
        done = FALSE;
    } else if (now < green - step) {
        now += step;
        done = FALSE;
    } else {
        now = green;
    }
    fade_color.argb.g = now;
    now = fade_color.argb.b;
    if (now > blue + step) {
        now -= step;
        done = FALSE;
    } else if (now < blue - step) {
        now += step;
        done = FALSE;
    } else {
        now = blue;
    }
    fade_color.argb.b = now;
    return done;
}
