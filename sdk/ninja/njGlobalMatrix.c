/* the light colors, the matrices and the view, each on a 32-byte boundary,
 * the SH-4's cache line */
#include <nj_render_types.h>

#ifdef NONMATCHING
/* this side aligns each to 32 bytes by GCC's attribute, in place of SHC's
 * #pragma aligndata32 */
#define CACHE_ALIGNED __attribute__((aligned(32)))
#else
#define CACHE_ALIGNED
#pragma section 32
#pragma aligndata32(_nj_lcolor_easy, _nj_lcolor_simple, _nj_unit_matrix_)
#pragma aligndata32(_nj_view_, _nj_view_matrix_, _nj_tmp_local_mtrx_)
#endif

/* the library exports each of these */
/* NOLINTBEGIN(misc-use-internal-linkage) */
NJS_ARGB _nj_lcolor_easy CACHE_ALIGNED = {1.0F, 1.0F, 1.0F, 1.0F};
NJS_ARGB _nj_lcolor_simple CACHE_ALIGNED = {1.0F, 1.0F, 1.0F, 1.0F};
NJS_MATRIX _nj_unit_matrix_ CACHE_ALIGNED = {
    1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F,
    0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F,
};

struct nj_view *_nj_view_ CACHE_ALIGNED;
NJS_MATRIX _nj_view_matrix_ CACHE_ALIGNED;
NJS_MATRIX _nj_tmp_local_mtrx_ CACHE_ALIGNED;
/* NOLINTEND(misc-use-internal-linkage) */
#pragma section
