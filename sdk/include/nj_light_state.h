/* the light and clipping state Ninja's chunk drawers share */
#ifndef NJ_LIGHT_STATE_H
#define NJ_LIGHT_STATE_H
#include <ninja.h>
#include <sg_xpt.h>

/* the easy multi drawer's light buffers, in this order.
 * _nj_cnk_easy_multilight_buffer_ holds MULTILIGHT_FLOATS floats: each of six
 * lights' vector or point, 3 floats each, then 2 more. _mdst_ follows with
 * the mode word and the ambient color. _dst_ then holds the six lights as the
 * drawer reads them, 8 floats each. the simple drawer's buffers lie alike */
#define MULTILIGHT_FLOATS 20
/* ambient red's index from the start of _nj_cnk_easy_multilight_buffer_,
 * past its MULTILIGHT_FLOATS and the mode word */
#define MULTILIGHT_AMBIENT (MULTILIGHT_FLOATS + 1)
#define MULTILIGHT_OUTPUT_FLOATS 48
struct nj_multilight_parameters {
    Uint32 modes;
    Float ambient[3];
};

extern Float _nj_cnk_easy_multilight_buffer_[];
extern struct nj_multilight_parameters _nj_cnk_easy_multilight_buffer_mdst_;
extern Float _nj_cnk_easy_multilight_buffer_dst_[MULTILIGHT_OUTPUT_FLOATS];
extern Float _nj_cnk_simple_multilight_buffer_[];
extern struct nj_multilight_parameters _nj_cnk_simple_multilight_buffer_mdst_;
extern Float _nj_cnk_simple_multilight_buffer_dst_[MULTILIGHT_OUTPUT_FLOATS];

/* how many vertices of the model's current run of vertex chunks its
 * drawer's vertex functions count as near. the easy drawer's count a vertex
 * whose z is 1 or less. a vertex chunk with NJD_FV_CONT clear starts a run,
 * and the count, from 0. where the count isn't 0, a polygon walker draws
 * with the Clip forms of its functions */
extern Sint32 _nj_cnk_clip_num_;
/* how many vertices the same run holds. a polygon walker draws no strip unless
 * _nj_cnk_clip_num_ is below this. so a model whose every vertex is near draws
 * nothing */
extern Sint32 _nj_cnk_clip_vtx_;
/* the polygon lists that NJD_CB_CP chunks have cached and no NJD_CB_DP
 * chunk has drawn yet */
extern Sint32 _nj_cnk_plist_stack_cnt_;
/* the rest of the polygon list after each NJD_CB_CP chunk, which stops the
 * list there. an NJD_CB_DP chunk of the same number draws it. the number,
 * the chunk's flags, must be below POLYGON_CACHE_ENTRIES */
#define POLYGON_CACHE_ENTRIES 125
extern Sint16 *_nj_cnk_polygon_cache_tbl_[POLYGON_CACHE_ENTRIES];
extern NJS_LIGHT_MATERIAL _nj_light_material_;

/* Ninja's lights: entries[0] to entries[count - 1] */
struct nj_light_control {
    NJS_LIGHT *entries[NJD_LIGHT_MAX];
    Int count;
};
extern struct nj_light_control _nj_light_control_;
/* the lights njSetChunkLightQueue found on, which the chunk lights shine with.
 * slots[0] to slots[count - 1] index _nj_light_control_'s entries */
struct nj_light_queue {
    Int slots[NJD_LIGHT_MAX];
    Int count;
};
extern struct nj_light_queue _nj_light_queue_;

extern NJS_MATRIX _nj_camera_matrix_;

#endif
