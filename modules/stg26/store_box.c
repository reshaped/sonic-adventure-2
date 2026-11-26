#include "stg26/quarters_util.h" /* IWYU pragma: keep -- names shared/store_box.c takes */

#define STORE_BOX_DRAW_MODEL quarters_model_draw_scaled
#define STORE_BOX_DRAW_BUILT quarters_direct_draw_scaled

#include "shared/store_box.c" /* NOLINT(bugprone-suspicious-include) */
