#include <kamui.h> /* IWYU pragma: keep -- GLOBAL_SPRITE uses KM_PARAM_SPRITE */
#include <machine.h>
#include <ninja.h>
#include <nj_km.h>
#include <sg_xpt.h>

Uint32 *njStartVertexNonTexSpriteTrans(Uint32 color)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.trans;
    register Uint32 *sq;

    sq = SQ_ADDRESS(list);
    _nj_sq_base_ = list & ~SQ_OFFSET;
    SQ_MAP(list);
    sq += SPRITE_WORDS;
    /* @hack spelling: the parameter's words stored as sq steps back from its
     * end, where indexed stores from its start would do */
    *--sq = color;
    /* word 3, the texture word, stays unwritten */
    --sq;
    *--sq = (_nj_parameterkm_.tsp & ~TSP_IGNORE_TEX_ALPHA) | TSP_USE_ALPHA;
    *--sq = _nj_parameterkm_.isp;
    /* the global word's list field stays 0, the opaque list's, where
     * njStartVertexTexSpriteTrans's holds GLOBAL_TRANS */
    *--sq = (_nj_parameterkm_.global & GLOBAL_USER_CLIP) |
            ((_nj_control_3d_flag_ & NJD_CONTROL_3D_SHADOW)
                 ? GLOBAL_SPRITE | GLOBAL_SHADOW
                 : GLOBAL_SPRITE);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.trans;
    return sq;
}
