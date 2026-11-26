/* njCnkSimpleTextureNearClip.c compiled as njCnkSimpleTextureD8EnvDbNearClip,
 * the near clip of njCnkSimplePolygonD8EnvDbClip and
 * njCnkSimpleTextureD8EnvDbClip */
#include <nj_cnk.h> /* IWYU pragma: keep -- names ../ninja/njCnkSimpleTextureNearClip.c takes */

/* NOLINTBEGIN(readability-identifier-naming) */
#define njCnkSimpleTextureNearClip njCnkSimpleTextureD8EnvDbNearClip
/* NOLINTEND(readability-identifier-naming) */
#define NEAR_CLIP_PARAMS
#define NEAR_CLIP_LOCALS ENV_LOCALS ARGB_LOCALS
#define NEAR_CLIP_START UV_OFFSETS
#define NEAR_CLIP_COLOR PACKED_COLOR
#define NEAR_CLIP_UV NORMAL_UV
#define NEAR_CLIP_FACING
#define NEAR_CLIP_CUT_COLOR LERP_PACKED

/* the Ninja route searches sdk/include alone, so the path starts there */
#include "../ninja/njCnkSimpleTextureNearClip.c" /* NOLINT(bugprone-suspicious-include) */
