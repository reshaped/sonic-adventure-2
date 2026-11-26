/* the fade: a color drawn over the whole screen */
#ifndef DRAW_FADE_H
#define DRAW_FADE_H
#include <sg_xpt.h>

/* set the fade's color, a packed ARGB color. it starts black and opaque,
 * 0xff000000, as fade_set_black sets it. each zeroes game_state.idle_calls */
void fade_set_color(Uint32 color);
void fade_set_black(void);
/* draw the color over the whole screen, blended by its alpha. an alpha of 0
 * draws nothing and changes nothing. any other alpha draws the color, opaque
 * at 255, and leaves the fog enabled, whatever it was. it leaves the blending
 * modes at NJD_COLOR_BLENDING_SRCALPHA and INVSRCALPHA too */
void fade_draw(void);

/* the steps of a fade, one a call. a step moves the alpha, or a channel, by 5
 * out of 255. it moves 6 instead while task_current_call is null under
 * DISPLAY_PAL, which shows 50 frames a second. fade_in_step lowers the alpha
 * toward 0, and fade_out_step raises it toward 255. fade_to_alpha_step moves it
 * toward alpha, which must be 0 to 255. a step that would pass its end lands on
 * it.
 *
 * each of the three returns TRUE where the alpha is at its end already, and
 * changes nothing. otherwise it returns FALSE, even where the step brings the
 * alpha to its end. so a fade is over at the first call that returns TRUE. a
 * shipped bug keeps fade_to_alpha_step from landing on an alpha below the step.
 * its alpha swings between two values a step apart instead, and every call
 * returns FALSE.
 *
 * fade_to_color_step steps each channel of the color toward its argument. it
 * returns FALSE where a channel lies more than a step from its argument.
 * otherwise every channel lands on its argument, and it returns TRUE. so unlike
 * the others, it returns TRUE on the call that ends the fade */
Bool fade_in_step(void);
Bool fade_out_step(void);
Bool fade_to_alpha_step(Uint32 alpha);
Bool fade_to_color_step(Uint8 alpha, Uint8 red, Uint8 green, Uint8 blue);

/* the idle dim. fade_idle_step does nothing while game_state.idle_dim is 0.
 * game_state.idle_calls counts its calls in a row that find no input. a call
 * that finds input zeroes the count. input is a button or direction held on
 * port A or B. a trigger or an axis of the first stick off 0 on either port
 * is input too. up to a count of 7200, a call lowers the alpha by 4, to no
 * less than 0. past 7200, a call raises the alpha by 1 toward 192. a higher
 * alpha drops to 192 at once. a shipped bug lets 192 rise to 193, which the
 * next call sets back. fade_reset_idle zeroes the count */
void fade_idle_step(void);
void fade_reset_idle(void);

#endif
