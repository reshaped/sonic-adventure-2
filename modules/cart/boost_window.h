#ifndef CART_BOOST_WINDOW_H
#define CART_BOOST_WINDOW_H

#include "cart/pads.h"
#include "hud/action_window.h"

extern struct action_window cart_boost_windows[CART_PLAYERS];
void cart_boost_windows_start(void);
void cart_boost_windows_stop(void);
void cart_boost_window_set(int player, int pressed);
#endif
