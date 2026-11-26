/* the karts' pads, read a delay behind the players'. each step a task copies
 * both players' pads into the next row of cart_pad_history. its
 * CART_PAD_FRAMES rows make a ring */
#ifndef CART_PADS_H
#define CART_PADS_H
#include <shinobi.h>

#define CART_PAD_FRAMES 100
#define CART_PLAYERS 2

/* cart_pad_frame is the row the karts read: the newest, or the one
 * cart_set_pad_delay's frames steps older */
extern PDS_PERIPHERAL cart_pad_history[CART_PAD_FRAMES][CART_PLAYERS];
extern int cart_pad_frame;
/* start the task with a delay of 0 */
void cart_pads_start(void);
/* end the task at its next step. no code calls it */
void cart_pads_stop(void);
/* read the pads frames steps late, from 0 to CART_PAD_FRAMES - 1, unchecked */
void cart_set_pad_delay(int frames);
#endif
