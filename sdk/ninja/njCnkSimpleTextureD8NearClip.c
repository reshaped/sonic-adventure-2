/* njCnkSimpleTextureNearClip.c compiled as njCnkSimpleTextureD8NearClip, the
 * near clip of njCnkSimpleTextureD8Clip */
#include <nj_cnk.h> /* IWYU pragma: keep -- names ../ninja/njCnkSimpleTextureNearClip.c takes */

/* NOLINTBEGIN(readability-identifier-naming) */
#define njCnkSimpleTextureNearClip njCnkSimpleTextureD8NearClip
/* NOLINTEND(readability-identifier-naming) */
#define NEAR_CLIP_PARAMS UV_PARAMS, Float side
#define NEAR_CLIP_LOCALS ARGB_LOCALS
#define NEAR_CLIP_START
#define NEAR_CLIP_COLOR PACKED_COLOR
#define NEAR_CLIP_UV PARAM_UV
#define NEAR_CLIP_FACING DROP_BACK
#define NEAR_CLIP_CUT_COLOR LERP_PACKED

/* the Ninja route searches sdk/include alone, so the path starts there */
#include "../ninja/njCnkSimpleTextureNearClip.c" /* NOLINT(bugprone-suspicious-include) */
