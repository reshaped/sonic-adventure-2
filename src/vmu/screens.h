/* the memory cards' screens: the animation each one shows */
#ifndef VMU_SCREENS_H
#define VMU_SCREENS_H

/* give each screen without a loop the one game_state.mode picks, where it
 * picks one. the screens are those of the memory cards in each pad's first
 * socket. nothing changes while game_state.card_state isn't CARD_IDLE */
void screens_fill(void);

#endif
