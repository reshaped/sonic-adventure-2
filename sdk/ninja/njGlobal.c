/* the Ninja library's display, vertex buffer, texture and Kamui state */
#include <kamui.h>
#include <nj_render_types.h>
#include <sg_xpt.h>

/* the library exports each of these. SHC places uninitialized variables in
 * the order it first meets their names. so this source includes only the
 * headers of their types, and its definitions set the order */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Uint32 _nj_texture_memory_size = 1024UL * 1024;
Bool _nj_flat_model_flag_ = FALSE;
Sint32 _nj_vertex_buffer_op_ = 0;
Sint32 _nj_vertex_buffer_om_ = 0;
Sint32 _nj_vertex_buffer_tp_ = 0;
Sint32 _nj_vertex_buffer_tm_ = 0;
Sint32 _nj_vertex_buffer_pt_ = 0;
Int _nj_flip_on_ = TRUE;
void (*_nj_eor_function_)(void) = NULL;
void (*_nj_eor_function2_)(void) = NULL;

/* the easy and simple drawers' bump light colors, which start opaque black */
Uint32 _nj_cnk_easy_bump_lt_color_ = 0xff000000;
Uint32 _nj_cnk_simple_bump_lt_color_ = 0xff000000;
Float _nj_uv_scroll_u_ = 0.0F;
Float _nj_uv_scroll_v_ = 0.0F;
Sint32 _nj_rotate_func_flag_ = 0;

NJS_TEXLIST *_nj_current_texlist = NULL;
NJS_TEXMEMLIST *_nj_texmemlist = NULL;
Uint32 _nj_texmemsize = 0;
Uint32 _nj_current_texture = TEXMEM_FREE;
Uint32 _nj_current_texture_bank = TEXMEM_FREE;
/* the sixteen global indices above this one never go to a texture without
 * a GBIX chunk */
Uint32 _nj_user_globalIndex = 0xffffffef;
Sint32 _nj_texglobal_color_ = 0;
Int _nj_texload_wait_ = TRUE;
Int _nj_loadtexreq_mode_ = 2;
Int _nj_loadtexreq_i_ = 0;

Uint32 _nj_vertex_buffer_size_ = 0;
Uint32 *_nj_usr_vertex_buffer_ptr_ = NULL;
Uint32 _nj_usr_vertex_buffer_size_ = 0;

NJS_SCREEN _nj_screen_;
NJS_MATRIX *_nj_current_matrix_ptr_;
NJS_MATRIX *_nj_base_matrix_ptr_;
NJS_POINT2 _nj_2d_clip_[2];
Float _nj_near_clip_;
Float _nj_far_clip_;
Float _nj_scr_aspect_x_;
Float _nj_scr_aspect_y_;
Int _nj_display_mode_;
Int _nj_frame_mode_;
Int _nj_waitvsync_count;
Uint32 _nj_control_3d_flag_;

/* each vertex list's pCurrentPtr in _nj_vertex_buffer_, as njWaitVSync or
 * njFlipFrameBuffer last left it */
Uint32 _nj_start_vertex_op;
Uint32 _nj_start_vertex_om;
Uint32 _nj_start_vertex_tp;
Uint32 _nj_start_vertex_tm;
Uint32 _nj_start_vertex_pt;
Int _nj_frame_size_w;
Int _nj_frame_size_h;
NJS_VERTEX_BUF *_nj_vertex_buf_;
Int _nj_vertex_num_;
Int _nj_calc_vertex_;
Int _nj_calc_polygon_;
Int _nj_draw_polygon_;
Uint32 _nj_constant_attr_and_;
Uint32 _nj_constant_attr_or_;
NJS_ARGB _nj_constant_material_;
Uint32 _nj_cnk_blend_mode_;
Uint32 _nj_vsync_count_;
Float _nj_depth_near_;
Float _nj_depth_far_;
Float _nj_depth_range_;
Float _nj_vertex_offset_;
Int _nj_depth_enable_;
volatile Int _nj_fatal_err_callback_;
NJS_CAMERA _nj_motion_camera_;
Sint32 _nj_max_stack_size_;
Sint32 _nj_stack_size_;

Sint32 _nj_tex_count;
char _nj_current_texture_path[TEXTURE_PATH_SIZE];
Uint32 _nj_tex_max_free_size;
NJS_TEXERRSTAT _nj_texerr_;
Sint8 *_nj_texture_buffer_st;
Uint32 _nj_texture_buffer_max_size;

volatile Bool _nj_eor_flag_;
volatile Bool _nj_eov_flag_;
volatile Bool _nj_render_flag_;
volatile Int _nj_palext_height_ratio_;
NJS_TEXSURFACE _nj_framebuffer_surface0_;
NJS_TEXSURFACE _nj_framebuffer_surface1_;
Uint32 _nj_frame_addr_0_;
Uint32 _nj_frame_addr_1_;
KMVERTEXCONTEXT _nj_context_[6];
KMVERTEXCONTEXT *_nj_current_ctx_;
Uint32 _nj_ctx_color_mode_[6];
Uint32 *_nj_color_mode_;
KMVERTEXBUFFDESC _nj_vertex_buffer_;
KMVERTEXCONTEXT _nj_background_ctx_;
KMSYSTEMCONFIGSTRUCT _nj_SystemConfigStruct_;
PKMSURFACEDESC _nj_ppSurfaceArray[2];
Uint32 _nj_wait_vsync_in_;
Uint32 _nj_wait_vsync_out_;
/* NOLINTEND(misc-use-internal-linkage) */
