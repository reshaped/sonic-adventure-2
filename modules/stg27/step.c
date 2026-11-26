/* Lost Colony's STEP, which carries a light volume. the stage has no STEPB */
#include "object/set.h" /* IWYU pragma: keep -- names shared/engine_step.c takes */
#include "stg27/light.h" /* IWYU pragma: keep -- names shared/engine_step.c takes */
#include "stg27/lost_colony.h" /* IWYU pragma: keep -- names shared/engine_step.c takes */
#define COLONY_STEP
#include "shared/engine_step.c" /* NOLINT(bugprone-suspicious-include) */
