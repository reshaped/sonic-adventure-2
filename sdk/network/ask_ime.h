#ifndef ASK_IME_H
#define ASK_IME_H
#include <sg_xpt.h>
/* ASK's open sets this, so that the next init_univmem rebuilds the cell
 * pool's free list. init_univmem clears it, and while it is clear skips a
 * pool whose cells are all free */
extern Sint32 first_init;
#endif
