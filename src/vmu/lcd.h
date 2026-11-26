/* the memory cards' screens: animations a task plays on them */
#ifndef VMU_LCD_H
#define VMU_LCD_H
#include <sg_xpt.h>

/* an animation is an array of frames, ended by a frame of 0 steps or a null
 * image. its first frame must not be the end */
struct lcd_frame {
    Sint32 steps; /* the task's steps the frame shows for */
    Uint8 *image; /* LCD_SIZE bytes, packed as pdVmsLcdWrite1 takes them */
};

/* create the task that plays the animations, and clear every port's. from
 * then until lcd_stop or the task's destroy, lcd_start does nothing. the
 * animations are cleared even where task_create returns null, and the next
 * lcd_start tries again.
 *
 * each step of the task writes the frames that come due with
 * pdVmsLcdWrite1. it drops the result, so a frame the write refuses is
 * skipped. while game_state.card_state isn't CARD_IDLE, a step writes
 * nothing, and no frame's steps run down */
void lcd_start(void);
/* kill the task. the screens keep the frames they show. the ports keep their
 * animations until the next lcd_start clears them */
void lcd_stop(void);
/* the animation the screen on port plays over and over, or null for none. the
 * port keeps frames, not a copy, so they must stay in place while they play.
 * lcd_set_loop starts the port over: the next frame the task writes there is
 * the first of frames. a port outside 0 to 23 is ignored, and lcd_get_loop
 * returns null for it */
void lcd_set_loop(int port, const struct lcd_frame *frames);
const struct lcd_frame *lcd_get_loop(int port);

#endif
