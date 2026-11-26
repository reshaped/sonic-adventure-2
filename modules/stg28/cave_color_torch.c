/* the colored torch, LIGHT in Pyramid Cave's object list: shared/color_torch.c
 * compiled again. this copy sets two values of the third preset */
#include "stg28/cave_color_torch.h"

/* NOLINTBEGIN(readability-identifier-naming) */
#define color_torch_init cave_color_torch_init
/* NOLINTEND(readability-identifier-naming) */
/* the orange light's green, which Death Chamber's torch takes as 0.75 */
#define COLOR_TORCH_THIRD_GREEN 0.85F
#define COLOR_TORCH_COLOR_LABEL "<- COLOR(      )"
#define COLOR_TORCH_LAST_NAME "ORANGE"
/* a float no code reads, which Death Chamber's torch takes as 0.125 */
#define COLOR_TORCH_THIRD_UNREAD 0.2F

#include "shared/color_torch.c" /* NOLINT(bugprone-suspicious-include) */
