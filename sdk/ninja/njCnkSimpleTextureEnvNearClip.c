/* njCnkSimpleTextureNearClip.c compiled as njCnkSimpleTextureEnvNearClip, the
 * near clip of njCnkSimplePolygonEnvClip and njCnkSimpleTextureEnvClip */
#include <nj_cnk.h> /* IWYU pragma: keep -- names ../ninja/njCnkSimpleTextureNearClip.c takes */

/* NOLINTBEGIN(readability-identifier-naming) */
#define njCnkSimpleTextureNearClip njCnkSimpleTextureEnvNearClip
/* NOLINTEND(readability-identifier-naming) */
#define NEAR_CLIP_PARAMS , Float ambient, Float side
#define NEAR_CLIP_LOCALS ENV_LOCALS
#define NEAR_CLIP_START UV_OFFSETS
#define NEAR_CLIP_COLOR LIT_COLOR
#define NEAR_CLIP_UV NORMAL_UV
#define NEAR_CLIP_FACING DROP_BACK
#define NEAR_CLIP_CUT_COLOR LERP_LIT

/* the Ninja route searches sdk/include alone, so the path starts there */
#include "../ninja/njCnkSimpleTextureNearClip.c" /* NOLINT(bugprone-suspicious-include) */
