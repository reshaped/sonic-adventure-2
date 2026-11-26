/* Pyramid Cave's registry of its sandglasses: shared/sandglass_registry.c
 * compiled again, under the cave_ names. its free of the empty table has no
 * test for null, which syFree accepts. sandglass_angle_turn and
 * sandglass_polar_xz are the stage's cave_angle_turn and cave_polar_xz */
/* NOLINTBEGIN(readability-identifier-naming) */
#define sandglasses cave_sandglasses
#define sandglass_count cave_sandglass_count
#define sandglass_take_mode cave_sandglass_take_mode
#define sandglass_set_mode cave_sandglass_set_mode
#define sandglass_register cave_sandglass_register
#define sandglass_unregister cave_sandglass_unregister
#define sandglass_find cave_sandglass_find
#define sandglass_running cave_sandglass_running
#define sandglass_time_left cave_sandglass_time_left
#define sandglass_pick_up cave_sandglass_pick_up
#define sandglass_carry cave_sandglass_carry
#define sandglass_throw cave_sandglass_throw
#define sandglass_angle_turn cave_angle_turn
#define sandglass_polar_xz cave_polar_xz
/* NOLINTEND(readability-identifier-naming) */
#define SANDGLASS_TABLE_FREE                                                   \
    syFree(sandglasses);                                                       \
    sandglasses = NULL;

#include "shared/sandglass_registry.c" /* NOLINT(bugprone-suspicious-include) */
