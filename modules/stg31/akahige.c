/* Green Hill's Akahige, the hornet under GREEN_HILL_AKAHIGE with its own
 * models. it carries three bombs or six */
#define GREEN_HILL_AKAHIGE
#define BOMBS_MAX BOMBS_6
#define HORNET_ASSET(name) akahige_##name
#define BOMB_ASSET(name) akahige_bomb_##name
#define HORNET_STEP green_hill_akahige_step
#define HORNET_UNUSED green_hill_akahige_unused

#include "shared/hornet_shapes.h" /* IWYU pragma: keep -- the data it defines here */
#include "shared/akahige_debris.h" /* IWYU pragma: keep -- the data it defines here */
#include "shared/hornet_actor.h" /* IWYU pragma: keep -- the code it compiles here */
