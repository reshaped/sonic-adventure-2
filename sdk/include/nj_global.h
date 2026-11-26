/* Ninja's own data and calls that no header of the SDK declares, as far as
 * the sources use them */
#ifndef NJ_GLOBAL_H
#define NJ_GLOBAL_H
#include <kamui.h>
#include <nj_cnk.h>
#include <nj_light_state.h> /* IWYU pragma: export */
#include <ninja.h>
#include <sg_xpt.h>

/* the rectangle 2D draws clip to: the least x and y on the screen, then the
 * greatest */
extern NJS_POINT2 _nj_2d_clip_[2];

/* the sprite starters use white where NJD_SPRITE_COLOR is clear */
extern const NJS_ARGB sprite_white;
/* the near-plane clipper indexes the eight orders by its outside corners */
extern const Uint8 sprite_clip_orders[8];

/* TRUE while the strip being drawn is flat shaded, as its NJD_FST_FL says,
 * else FALSE. the polygon walkers set it for each strip they draw */
extern Bool _nj_flat_model_flag_;
/* the base color, a packed ARGB word, that the easy drawer's bump functions
 * send with every vertex. njCnkSetEasyBumpLightColor sets it */
extern Uint32 _nj_cnk_easy_bump_lt_color_;
/* the base color, a packed ARGB word, that the simple drawer's bump functions
 * send with every vertex. njCnkSetSimpleBumpLightColor sets it */
extern Uint32 _nj_cnk_simple_bump_lt_color_;
/* the offsets that NJD_CONTROL_3D_ENV_UV_SCROLL adds to an
 * environment-mapped vertex's u and v */
extern Float _nj_uv_scroll_u_;
extern Float _nj_uv_scroll_v_;
/* the colors of the easy and the simple drawers' lights.
 * njCnkSetEasyLightColor and njCnkSetSimpleLightColor set their r, g and b.
 * the easy drawer's polygon walker copies a material's diffuse alpha into a
 * before each textured strip with NJD_FST_UA. the simple drawer's does so
 * before a bump-mapped one alone */
extern NJS_ARGB _nj_lcolor_easy;
extern NJS_ARGB _nj_lcolor_simple;

/* points to the current context's color mode. njSetCurrentContext points it
 * there, and njSetkmColorMode stores its mode there */
extern Uint32 *_nj_color_mode_;
/* TRUE between the start of a render and its end interrupt. */
extern volatile Bool _nj_eor_flag_;
/* the vertex buffer Ninja's draws send their polygons to */
extern KMVERTEXBUFFDESC _nj_vertex_buffer_;
/* Ninja's vertex contexts, of which njSetCurrentContext makes one current */
extern KMVERTEXCONTEXT _nj_context_[];
/* the works the easy and the simple drawers' vertex walkers hand their
 * vertex functions */
extern struct nj_cnk_easy_work _nj_vertex_easy;
extern struct nj_cnk_simple_work _nj_vertex_simple;
/* what njCnkEasyVlist returned for the last model whose polygon list the
 * easy drawer drew */
extern Sint32 _nj_cnk_easy_vtype_;
/* the two vectors of the last bump texture the easy and the simple drawers
 * set, as six floats. each float is a halfword of the texture's NJD_CM_BU
 * chunk over BUMP_SCALE */
extern Float _nj_cnk_easy_bump_tex_vector_[6];
extern Float _nj_cnk_simple_bump_tex_vector_[6];
/* where the second vector, t, starts in either array. the first, s, starts at
 * 0 */
#define BUMP_T 3
/* the store queue address where the open particle run's next block goes.
 * njPtclPolygonStart and njPtclSpriteStart set it. each draw of the run
 * moves it on. the run's End call closes the polygon there */
extern Uint32 *_nj_ptcl_sq_ptr_;
/* the store queue address of the next vertex: _nj_d3p_sq_ptr_ in the strips
 * njDrawPolygon3DExStart opened, and _nj_d3t_sq_ptr_ in those
 * njDrawTexture3DExStart opened */
extern Uint32 *_nj_d3p_sq_ptr_;
extern Uint32 *_nj_d3t_sq_ptr_;

/* the free texture memory in bytes, as njChangeTextureMemorySize and
 * njInitVertexBuffer last measured it by njCalcTexture(NJD_TEXMEM_FREESIZE).
 * njCalcTexture returns it for NJD_TEXMEM_MAXSIZE */
extern Uint32 _nj_tex_max_free_size;
/* a surface that holds no texture memory. njInitTexture and njExitTexture
 * empty it. njLoadFileTexture copies it into the record it enters for a file
 * it can't open. it does the same for a file whose header it can't read */
extern NJS_TEXSURFACE _nj_tex_err_surface;

/* empty _nj_light_control_ and _nj_light_queue_, zeroing each queue slot.
 * _nj_light_material_'s diffuse and specular become a gray of 0.5, and its
 * ambient black. each of the three takes an alpha of 1, and the material's
 * exponent becomes 6 */
void njLightSystemInit(void);

#endif
