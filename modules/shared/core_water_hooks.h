/* the lending of player hooks on Rouge's and Knuckles's routes, as player_hooks
 * says a stage lends them. CORE_INSTALL_EFFECT sets a hook only where it is
 * null. CORE_REMOVE_EFFECT clears it only where it still holds the route's
 * effect */
#ifndef SHARED_CORE_WATER_HOOKS_H
#define SHARED_CORE_WATER_HOOKS_H
#include "player/action.h"

#define CORE_INSTALL_EFFECT(field, effect)                                     \
    if (!player_hooks.field) {                                                 \
        player_hooks.field = (effect);                                         \
    }
#define CORE_REMOVE_EFFECT(field, effect)                                      \
    if (player_hooks.field == (effect)) {                                      \
        player_hooks.field = NULL;                                             \
    }

#endif
