/* the screen's width and height, in the pixels of Ninja's screen and its 2D
 * draws, and its halves in a two-player game */
#ifndef DRAW_SCREEN_H
#define DRAW_SCREEN_H

#define SCREEN_WIDTH 640.0F
#define SCREEN_HEIGHT 480.0F
/* player 0's half ends at SPLIT_LEFT_END, and player 1's starts at
 * SPLIT_RIGHT_START */
#define SPLIT_LEFT_END (SCREEN_WIDTH / 2)
#define SPLIT_RIGHT_START ((SCREEN_WIDTH / 2) + 1.0F)

#endif
