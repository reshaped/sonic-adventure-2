/* HIDELIGHT, a hiding volume that gives light 5 to a player inside it:
 * global_light_switch.c under its own names */
#include "stg26/hiding_spot.h" /* IWYU pragma: keep -- names shared/global_light_switch.c takes */

#define VOLUME_INIT hiding_light_init
#define VOLUME_CONTACTS hiding_light_contacts
#define VOLUME_MANAGER_INIT hiding_light_manager_init
#define VOLUME_PREVIOUS hiding_light_previous

#include "shared/global_light_switch.c" /* NOLINT(bugprone-suspicious-include) */
