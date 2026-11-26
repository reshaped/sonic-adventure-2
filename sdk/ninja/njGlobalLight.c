/* the chunk drawers' light banks, clipping counts and polygon cache, and
 * Ninja's light state */
#ifdef NONMATCHING
/* a drawer's vertex functions read its _mdst_ mode word. they then step 16
 * bytes on to its _dst_'s first light. so each drawer's three arrays share a
 * section, in source order, which no_reorder holds under LTO too */
#define LIGHT_BANK(name, alignment)                                            \
    __attribute__((section(".bss.nj_lights_" #name), no_reorder,               \
                   aligned(alignment)))
#define CACHE_ALIGNED __attribute__((aligned(32)))
#else
#pragma section 32
#pragma aligndata32(_nj_cnk_easy_multilight_buffer_,                           \
                    _nj_cnk_easy_multilight_buffer_dst_)
#pragma aligndata32(_nj_cnk_simple_multilight_buffer_,                         \
                    _nj_cnk_simple_multilight_buffer_dst_)
#pragma aligndata32(_nj_cnk_clip_num_, _nj_light_material_, _nj_light_control_)
#pragma aligndata32(_nj_light_queue_, _nj_camera_matrix_, _nj_diff0_,          \
                    _nj_diff1_)
#define LIGHT_BANK(name, alignment)
#define CACHE_ALIGNED
#endif

#include <nj_light_state.h>
#include <ninja.h>
#include <sg_xpt.h>

Float _nj_cnk_easy_multilight_buffer_[MULTILIGHT_FLOATS] LIGHT_BANK(easy, 32);
struct nj_multilight_parameters
    _nj_cnk_easy_multilight_buffer_mdst_ LIGHT_BANK(easy, 4);
Float _nj_cnk_easy_multilight_buffer_dst_[MULTILIGHT_OUTPUT_FLOATS] LIGHT_BANK(
    easy, 32);
Float _nj_cnk_simple_multilight_buffer_[MULTILIGHT_FLOATS] LIGHT_BANK(simple,
                                                                      32);
struct nj_multilight_parameters
    _nj_cnk_simple_multilight_buffer_mdst_ LIGHT_BANK(simple, 4);
Float
    _nj_cnk_simple_multilight_buffer_dst_[MULTILIGHT_OUTPUT_FLOATS] LIGHT_BANK(
        simple, 32);

Sint32 _nj_cnk_clip_num_ CACHE_ALIGNED;
Sint32 _nj_cnk_clip_vtx_;
Sint32 _nj_cnk_plist_stack_cnt_;
Sint16 *_nj_cnk_polygon_cache_tbl_[POLYGON_CACHE_ENTRIES];
NJS_LIGHT_MATERIAL _nj_light_material_ CACHE_ALIGNED;
struct nj_light_control _nj_light_control_ CACHE_ALIGNED;
struct nj_light_queue _nj_light_queue_ CACHE_ALIGNED;
NJS_MATRIX _nj_camera_matrix_ CACHE_ALIGNED;

/* the library exports these two diffuse and ambient pairs, which no code
 * reads */
/* NOLINTBEGIN(misc-use-internal-linkage) */
NJS_ARGB _nj_diff0_ CACHE_ALIGNED;
NJS_ARGB _nj_ambi0_;
NJS_ARGB _nj_diff1_ CACHE_ALIGNED;
NJS_ARGB _nj_ambi1_;
/* NOLINTEND(misc-use-internal-linkage) */
#pragma section
