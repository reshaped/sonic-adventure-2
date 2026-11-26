/* the motion callback, and the work the chunk drawers share */
#include <ninja.h>
#include <nj_cnk.h>
#include <sg_xpt.h>

/* the library exports each of these */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void (*_nj_motion_callback_)(NJS_OBJECT *object);
Uint32 *_nj_d3p_sq_ptr_;
Uint32 *_nj_d3t_sq_ptr_;
struct nj_cnk_easy_work _nj_vertex_easy;
struct nj_cnk_simple_work _nj_vertex_simple;
Sint32 _nj_cnk_easy_vtype_;
Float _nj_cnk_easy_bump_tex_vector_[6];
Float _nj_cnk_simple_bump_tex_vector_[6];
/* NOLINTEND(misc-use-internal-linkage) */
