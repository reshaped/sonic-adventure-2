/* njCnkSimpleTextureNearClip.c compiled as njCnkSimpleTextureDbNearClip, the
 * near clip of njCnkSimpleTextureDbClip */

/* NOLINTBEGIN(readability-identifier-naming) */
#define njCnkSimpleTextureNearClip njCnkSimpleTextureDbNearClip
/* NOLINTEND(readability-identifier-naming) */
#define NEAR_CLIP_PARAMS UV_PARAMS, Float ambient, Float side
#define NEAR_CLIP_LOCALS
#define NEAR_CLIP_START
#define NEAR_CLIP_COLOR(k, entry)
#define NEAR_CLIP_UV PARAM_UV
#define NEAR_CLIP_FACING LIGHT_BY_FACING
#define NEAR_CLIP_CUT_COLOR LERP_LIT

/* the Ninja route searches sdk/include alone, so the path starts there */
#include "../ninja/njCnkSimpleTextureNearClip.c" /* NOLINT(bugprone-suspicious-include) */
