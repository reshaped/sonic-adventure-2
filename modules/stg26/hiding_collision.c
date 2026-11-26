/* HIDECOLI, a hiding volume that changes no light: global_light_switch.c
 * with VOLUME_PRESERVES_LIGHT */
#include "stg26/hiding_spot.h" /* IWYU pragma: keep -- names shared/global_light_switch.c takes */

#define VOLUME_INIT hiding_collision_init
#define VOLUME_CONTACTS hiding_collision_contacts
#define VOLUME_MANAGER_INIT hiding_collision_manager_init
#define VOLUME_PREVIOUS hiding_collision_previous
#define VOLUME_PRESERVES_LIGHT

#include "shared/global_light_switch.c" /* NOLINT(bugprone-suspicious-include) */
