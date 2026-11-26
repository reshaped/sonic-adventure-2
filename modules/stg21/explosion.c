/* boss_golem/explosion.c for Sand Ocean, under names of its own. it puts
 * alpha blending back after each shell's draw */
#include "shared/ocean_util.h" /* IWYU pragma: keep -- names boss_golem/explosion.c takes */
#include "stg21/explosion.h" /* IWYU pragma: keep -- names boss_golem/explosion.c takes */

#define EXPLOSION_CREATE ocean_explosion_create
#define EXPLOSION_DRAW_SCALED ocean_model_draw_simple_scaled
#define EXPLOSION_TASK_NAME "EffectOpaqueExplosionExec"
#define EXPLOSION_RESTORE_BLEND

#include "boss_golem/explosion.c" /* NOLINT(bugprone-suspicious-include) */
