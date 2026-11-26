/* starters that open polygons in Kamui's lists, and setters of the copy,
 * _nj_parameterkm_, whose words they send */
#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_global.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* @hack spelling: a parameter's words stored as sq steps back from its end,
 * where indexed stores from its start would do */
#define SQ_PUT(sq, value) (*--(sq) = (value))
/* the words from a parameter's first through the last one sent. a NonTex
 * starter leaves word 3, the texture word, unwritten */
#define NONTEX_WORDS 3
#define TEX_WORDS 4
#define NONTEX_2P_WORDS 5
#define TEX_2P_WORDS 6
#define MODIFIER_WORDS 2
/* list holds the next address of one of the copy's lists. point sq at the store
 * queue address that writes to it. have both queues write to its area.
 * _nj_sq_base_ keeps list's bits above SQ_OFFSET, for njEndVertex */
#define SQ_OPEN(sq, list)                                                      \
    {                                                                          \
        (sq) = SQ_ADDRESS(list);                                               \
        _nj_sq_base_ = (list) & ~SQ_OFFSET;                                    \
        SQ_MAP(list);                                                          \
    }
/* put color's COLOR_FLOATS words, last first, as sq steps back. color must
 * point at its last word, and is left at its first */
/* @hack spelling: color stepped back between its reads, where indexed reads
 * of its words would do */
#define SQ_PUT_COLOR(sq, color)                                                \
    {                                                                          \
        SQ_PUT(sq, *(Uint32 *)(color));                                        \
        (color)--;                                                             \
        SQ_PUT(sq, *(Uint32 *)(color));                                        \
        (color)--;                                                             \
        SQ_PUT(sq, *(Uint32 *)(color));                                        \
        (color)--;                                                             \
        SQ_PUT(sq, *(Uint32 *)(color));                                        \
    }

#define TSP_PUNCH(tsp)                                                         \
    (((tsp) & ~(TSP_BLEND | TSP_USE_ALPHA)) |                                  \
     ((Uint32)BLEND_SRC_ALPHA << TSP_SRC_SHIFT) |                              \
     ((Uint32)BLEND_INV_SRC_ALPHA << TSP_DST_SHIFT))
#define GLOBAL_PUNCH(global)                                                   \
    (((global) & ~GLOBAL_LIST) |                                               \
     ((Uint32)KM_PUNCHTHROUGH_POLYGON << GLOBAL_LIST_SHIFT))
/* a sprite's global word takes GLOBAL_SHADOW only while
 * NJD_CONTROL_3D_SHADOW is set */
#define SHADOWED(word)                                                         \
    ((_nj_control_3d_flag_ & NJD_CONTROL_3D_SHADOW) ? (word) | GLOBAL_SHADOW   \
                                                    : (word))

/* the TSP fields njSetkmModelAttr takes from attr, and njSetkmCnkTextureNum
 * from a texture chunk */
#define MODEL_ATTR_TSP                                                         \
    (TSP_BLEND | TSP_FLIP | TSP_CLAMP | TSP_FILTER | TSP_SUPER_SAMPLE |        \
     TSP_MIPMAP)
#define CNK_TEXTURE_TSP                                                        \
    (TSP_FLIP | TSP_CLAMP | TSP_FILTER | TSP_SUPER_SAMPLE | TSP_MIPMAP)
/* the texture's index in an NJD_CT_TID chunk's second halfword */
#define CNK_TEXTURE_INDEX 0x1fff
/* how far a texture chunk's bits lie from the TSP word's. its flip and clamp
 * bits lie CNK_FLIP_CLAMP_SHIFT bits below. its super-sample and filter bits
 * lie CNK_FILTER_SHIFT bits above. its NJD_FDA_MASK bits, the D adjust, lie
 * where TSP_MIPMAP's do. a chunk's NJD_FBS_ and NJD_FBD_ blend bits lie
 * CNK_BLEND_SHIFT bits below */
#define CNK_FLIP_CLAMP_SHIFT 3
#define CNK_FILTER_SHIFT 1
#define CNK_BLEND_SHIFT 18
/* the bit of njColorBlendingkmMode's mode that sets both factors */
#define BLEND_BOTH 1

struct nj_parameterkm _nj_parameterkm_;
Uint32 _nj_sq_base_;
Uint32 **_nj_listtype_;

/* the member exports these functions and data, though no header of the SDK
 * declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* a 2p starter opens a polygon with two volumes' words. it sends the copy's
 * global word as it stands, so the copy needs GLOBAL_VOLUME, as
 * njSetkmColorMode2p sets it. a Tex 2p starter sends _nj_TSPPARAMBUFFER_2_
 * and _nj_TexturePARAMBUFFER_2_ as the polygon's second TSP and texture words.
 * a NonTex 2p starter sends the copy's TSP word as both TSP words. an Opaque
 * 2p starter sends both as TSP_OPAQUE makes them */
Uint32 *njStartVertexNonTexOpaque2p(void);
Uint32 *njStartVertexNonTexTrans2p(void);
Uint32 *njStartVertexTexOpaque2p(void);
Uint32 *njStartVertexTexTrans2p(void);
/* a Punch starter opens a polygon as a Tex starter does, but in the copy's
 * punch list. TSP_PUNCH and GLOBAL_PUNCH make its TSP and global words. face
 * and offset go out as an Intens or IntensH starter's do */
Uint32 *njStartVertexTexPunch(void);
Uint32 *njStartVertexTexPunchIntens(Float *face);
Uint32 *njStartVertexTexPunchIntensH(Float *face, Float *offset);
/* an Intens2p starter opens a two-volume polygon as a 2p starter does. its
 * parameter is 64 bytes, with face as words 8 to 11. face2, the polygon's
 * color inside a modifier volume, goes out as words 12 to 15 */
Uint32 *njStartVertexNonTexOpaqueIntens2p(Float *face, Float *face2);
Uint32 *njStartVertexNonTexTransIntens2p(Float *face, Float *face2);
Uint32 *njStartVertexTexOpaqueIntens2p(Float *face, Float *face2);
Uint32 *njStartVertexTexTransIntens2p(Float *face, Float *face2);
/* njEndModifier, with KM_MODIFIER_EXCLUDE_LAST_POLY as its parameter's
 * second word */
Uint32 *njEndModifierClip(Uint32 *sq);
/* njSetkmColorMode for a two-volume polygon. _nj_TSPPARAMBUFFER_2_ takes
 * mode's TSP bits too. the copy's global word takes GLOBAL_SHADOW and
 * GLOBAL_VOLUME, whatever _nj_control_3d_flag_ holds */
void njSetkmColorMode2p(Uint32 mode);
/* njSetkmTextureNum for a two-volume polygon's second texture, whether or
 * not texture n is current. _nj_TSPPARAMBUFFER_2_ and _nj_TexturePARAMBUFFER_2_
 * take the words njSetkmTextureNum would give the copy. the copy and the
 * current texture stay as they were, but _nj_texglobal_color_ takes texture n's
 * format */
Sint32 njSetkmTextureNum2p(Uint32 n);
/* njSetkmTextureNum for record n of _nj_texmemlist, whether or not it is
 * current */
Sint32 njSetkmTextureMemList(Uint32 n);
void njSetkmModelAttr(Uint32 attr);
/* give the hardware's culling register size, through kmSetCullingRegister */
void njkmPolygonCullingSize(Float size);
/* each kmMode setter and njMipmapkmAdjust sets one field of the copy's ISP, TSP
 * or global word. a Uint32 mode or level must hold that field's bits where the
 * word holds them, and no others. the setter ORs it in whole. a setter of one
 * bit takes TRUE, which turns on what its name says, or FALSE. it ignores any
 * other value. njZBufferkmMode's TRUE lets polygons write their depth */
void njZBufferkmMode(Int mode);
void njMipmapkmAdjust(Uint32 level);
void njTextureFilterkmMode(Uint32 mode);
void njSuperSamplekmMode(Int mode);
void njTextureClampkmMode(Uint32 mode);
void njTextureFlipkmMode(Uint32 mode);
void njPolygonCullingkmMode(Uint32 mode);
/* mode holds both blend factors where the TSP word holds them, and may hold
 * BLEND_BOTH too. under BLEND_BOTH, the call ORs mode in whole. that sets both
 * factors, and the TSP word's bit 0 with them. otherwise it sets the source's
 * factor for a target of NJD_SOURCE_COLOR, and the destination's for any other
 * target */
void njColorBlendingkmMode(Int target, Uint32 mode);
void njSpecularkmMode(Int mode);
void njAlphakmMode(Int mode);
void njIgnoreTextureAlphakmMode(Int mode);
void njTextureShadingkmMode(Uint32 mode);
/* njSetkmCnkBlendMode, but from attr even while
 * NJD_CONTROL_3D_CNK_BLEND_MODE is set */
void njSetkmCellBlendMode(Uint32 attr);
/* a two-volume polygon's second TSP and texture words, which the Tex 2p
 * starters send. njSetkmTextureNum2p sets both, and njSetkmColorMode2p the TSP
 * word's color mode bits */
Uint32 _nj_TSPPARAMBUFFER_2_;
Uint32 _nj_TexturePARAMBUFFER_2_;
/* NOLINTEND(misc-use-internal-linkage) */

void njGetkmPointer(void)
{
    _nj_parameterkm_.global = _nj_current_ctx_->GLOBALPARAMBUFFER;
    _nj_parameterkm_.isp = _nj_current_ctx_->ISPPARAMBUFFER;
    _nj_parameterkm_.tsp = _nj_current_ctx_->TSPPARAMBUFFER;
    _nj_parameterkm_.texture = _nj_current_ctx_->TexturePARAMBUFFER;
    _nj_parameterkm_.opaque = _nj_vertex_buffer_.pCurrentPtr[KM_OPAQUE_POLYGON];
    _nj_parameterkm_.opaque_modifier =
        _nj_vertex_buffer_.pCurrentPtr[KM_OPAQUE_MODIFIER];
    _nj_parameterkm_.trans_modifier =
        _nj_vertex_buffer_.pCurrentPtr[KM_TRANS_MODIFIER];
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_USE_PUNCHTHROUGH) {
        _nj_parameterkm_.trans =
            _nj_vertex_buffer_.pCurrentPtr[KM_PUNCHTHROUGH_POLYGON];
        _nj_parameterkm_.punch =
            _nj_vertex_buffer_.pCurrentPtr[KM_TRANS_POLYGON];
    } else {
        _nj_parameterkm_.trans =
            _nj_vertex_buffer_.pCurrentPtr[KM_TRANS_POLYGON];
        _nj_parameterkm_.punch =
            _nj_vertex_buffer_.pCurrentPtr[KM_PUNCHTHROUGH_POLYGON];
    }
}

void njSetkmPointer(void)
{
    _nj_current_ctx_->GLOBALPARAMBUFFER = _nj_parameterkm_.global;
    _nj_current_ctx_->ISPPARAMBUFFER = _nj_parameterkm_.isp;
    _nj_current_ctx_->TSPPARAMBUFFER = _nj_parameterkm_.tsp;
    _nj_current_ctx_->TexturePARAMBUFFER = _nj_parameterkm_.texture;
    _nj_vertex_buffer_.pCurrentPtr[KM_OPAQUE_POLYGON] = _nj_parameterkm_.opaque;
    _nj_vertex_buffer_.pCurrentPtr[KM_OPAQUE_MODIFIER] =
        _nj_parameterkm_.opaque_modifier;
    _nj_vertex_buffer_.pCurrentPtr[KM_TRANS_MODIFIER] =
        _nj_parameterkm_.trans_modifier;
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_USE_PUNCHTHROUGH) {
        _nj_vertex_buffer_.pCurrentPtr[KM_PUNCHTHROUGH_POLYGON] =
            _nj_parameterkm_.trans;
        _nj_vertex_buffer_.pCurrentPtr[KM_TRANS_POLYGON] =
            _nj_parameterkm_.punch;
    } else {
        _nj_vertex_buffer_.pCurrentPtr[KM_TRANS_POLYGON] =
            _nj_parameterkm_.trans;
        _nj_vertex_buffer_.pCurrentPtr[KM_PUNCHTHROUGH_POLYGON] =
            _nj_parameterkm_.punch;
    }
}

Uint32 *njStartVertexNonTexOpaque(void)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.opaque;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += NONTEX_WORDS;
    SQ_PUT(sq, TSP_OPAQUE(_nj_parameterkm_.tsp));
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.opaque;
    return sq;
}

Uint32 *njStartVertexNonTexOpaque2p(void)
{
    /* @hack redundant: register on list */
    register Uint32 list = (Uint32)_nj_parameterkm_.opaque;
    Uint32 *sq;
    Uint32 tsp;

    SQ_OPEN(sq, list)
    sq += NONTEX_2P_WORDS;
    tsp = TSP_OPAQUE(_nj_parameterkm_.tsp);
    SQ_PUT(sq, tsp);
    sq--;
    SQ_PUT(sq, tsp);
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.opaque;
    return sq;
}

Uint32 *njStartVertexNonTexTrans(void)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.trans;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += NONTEX_WORDS;
    SQ_PUT(sq, _nj_parameterkm_.tsp);
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.trans;
    return sq;
}

Uint32 *njStartVertexNonTexTrans2p(void)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.trans;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += NONTEX_2P_WORDS;
    SQ_PUT(sq, _nj_parameterkm_.tsp);
    sq--;
    SQ_PUT(sq, _nj_parameterkm_.tsp);
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.trans;
    return sq;
}

Uint32 *njStartVertexTexOpaque(void)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.opaque;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += TEX_WORDS;
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, TSP_OPAQUE(_nj_parameterkm_.tsp));
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.opaque;
    return sq;
}

Uint32 *njStartVertexTexOpaque2p(void)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.opaque;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += TEX_2P_WORDS;
    SQ_PUT(sq, _nj_TexturePARAMBUFFER_2_);
    SQ_PUT(sq, TSP_OPAQUE(_nj_TSPPARAMBUFFER_2_));
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, TSP_OPAQUE(_nj_parameterkm_.tsp));
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.opaque;
    return sq;
}

Uint32 *njStartVertexTexTrans(void)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.trans;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += TEX_WORDS;
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, _nj_parameterkm_.tsp);
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.trans;
    return sq;
}

Uint32 *njStartVertexTexTrans2p(void)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.trans;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += TEX_2P_WORDS;
    SQ_PUT(sq, _nj_TexturePARAMBUFFER_2_);
    SQ_PUT(sq, _nj_TSPPARAMBUFFER_2_);
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, _nj_parameterkm_.tsp);
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.trans;
    return sq;
}

Uint32 *njStartVertexTexPunch(void)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.punch;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += TEX_WORDS;
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, TSP_PUNCH(_nj_parameterkm_.tsp));
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, GLOBAL_PUNCH(_nj_parameterkm_.global));
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.punch;
    return sq;
}

Uint32 *njStartVertexTexPunchIntens(Float *face)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.punch;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    face += COLOR_FLOATS - 1;
    sq += SQ_WORDS;
    SQ_PUT_COLOR(sq, face)
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, TSP_PUNCH(_nj_parameterkm_.tsp));
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, GLOBAL_PUNCH(_nj_parameterkm_.global));
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.punch;
    return sq;
}

Uint32 *njStartVertexTexPunchIntensH(Float *face, Float *offset)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.punch;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    face += COLOR_FLOATS - 1;
    offset += COLOR_FLOATS - 1;
    sq += TEX_WORDS;
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, TSP_PUNCH(_nj_parameterkm_.tsp));
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, GLOBAL_PUNCH(_nj_parameterkm_.global));
    prefetch(sq);
    sq += 2 * SQ_WORDS;
    SQ_PUT_COLOR(sq, offset)
    SQ_PUT_COLOR(sq, face)
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.punch;
    return sq;
}

Uint32 *njStartVertexNonTexOpaqueIntens(Float *face)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.opaque;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    face += COLOR_FLOATS - 1;
    sq += SQ_WORDS;
    SQ_PUT_COLOR(sq, face)
    sq--;
    SQ_PUT(sq, TSP_OPAQUE(_nj_parameterkm_.tsp));
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.opaque;
    return sq;
}

Uint32 *njStartVertexNonTexOpaqueIntens2p(Float *face, Float *face2)
{
    /* @hack redundant: register on list */
    register Uint32 list = (Uint32)_nj_parameterkm_.opaque;
    Uint32 *sq;
    Uint32 tsp;

    SQ_OPEN(sq, list)
    face += COLOR_FLOATS - 1;
    face2 += COLOR_FLOATS - 1;
    sq += NONTEX_2P_WORDS;
    tsp = TSP_OPAQUE(_nj_parameterkm_.tsp);
    SQ_PUT(sq, tsp);
    sq--;
    SQ_PUT(sq, tsp);
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += 2 * SQ_WORDS;
    SQ_PUT_COLOR(sq, face2)
    SQ_PUT_COLOR(sq, face)
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.opaque;
    return sq;
}

Uint32 *njStartVertexNonTexTransIntens(Float *face)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.trans;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    face += COLOR_FLOATS - 1;
    sq += SQ_WORDS;
    SQ_PUT_COLOR(sq, face)
    sq--;
    SQ_PUT(sq, _nj_parameterkm_.tsp);
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.trans;
    return sq;
}

Uint32 *njStartVertexNonTexTransIntens2p(Float *face, Float *face2)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.trans;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    face += COLOR_FLOATS - 1;
    face2 += COLOR_FLOATS - 1;
    sq += NONTEX_2P_WORDS;
    SQ_PUT(sq, _nj_parameterkm_.tsp);
    sq--;
    SQ_PUT(sq, _nj_parameterkm_.tsp);
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += 2 * SQ_WORDS;
    SQ_PUT_COLOR(sq, face2)
    SQ_PUT_COLOR(sq, face)
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.trans;
    return sq;
}

Uint32 *njStartVertexTexOpaqueIntens(Float *face)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.opaque;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    face += COLOR_FLOATS - 1;
    sq += SQ_WORDS;
    SQ_PUT_COLOR(sq, face)
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, TSP_OPAQUE(_nj_parameterkm_.tsp));
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.opaque;
    return sq;
}

Uint32 *njStartVertexTexOpaqueIntens2p(Float *face, Float *face2)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.opaque;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    face += COLOR_FLOATS - 1;
    face2 += COLOR_FLOATS - 1;
    sq += TEX_2P_WORDS;
    SQ_PUT(sq, _nj_TexturePARAMBUFFER_2_);
    SQ_PUT(sq, TSP_OPAQUE(_nj_TSPPARAMBUFFER_2_));
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, TSP_OPAQUE(_nj_parameterkm_.tsp));
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += 2 * SQ_WORDS;
    SQ_PUT_COLOR(sq, face2)
    SQ_PUT_COLOR(sq, face)
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.opaque;
    return sq;
}

Uint32 *njStartVertexTexTransIntens(Float *face)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.trans;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    face += COLOR_FLOATS - 1;
    sq += SQ_WORDS;
    SQ_PUT_COLOR(sq, face)
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, _nj_parameterkm_.tsp);
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.trans;
    return sq;
}

Uint32 *njStartVertexTexTransIntens2p(Float *face, Float *face2)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.trans;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    face += COLOR_FLOATS - 1;
    face2 += COLOR_FLOATS - 1;
    sq += TEX_2P_WORDS;
    SQ_PUT(sq, _nj_TexturePARAMBUFFER_2_);
    SQ_PUT(sq, _nj_TSPPARAMBUFFER_2_);
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, _nj_parameterkm_.tsp);
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += 2 * SQ_WORDS;
    SQ_PUT_COLOR(sq, face2)
    SQ_PUT_COLOR(sq, face)
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.trans;
    return sq;
}

Uint32 *njStartVertexTexOpaqueIntensH(Float *face, Float *offset)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.opaque;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    face += COLOR_FLOATS - 1;
    offset += COLOR_FLOATS - 1;
    sq += TEX_WORDS;
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, TSP_OPAQUE(_nj_parameterkm_.tsp));
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += 2 * SQ_WORDS;
    SQ_PUT_COLOR(sq, offset)
    SQ_PUT_COLOR(sq, face)
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.opaque;
    return sq;
}

Uint32 *njStartVertexTexTransIntensH(Float *face, Float *offset)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.trans;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    face += COLOR_FLOATS - 1;
    offset += COLOR_FLOATS - 1;
    sq += TEX_WORDS;
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, _nj_parameterkm_.tsp);
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, _nj_parameterkm_.global);
    prefetch(sq);
    sq += 2 * SQ_WORDS;
    SQ_PUT_COLOR(sq, offset)
    SQ_PUT_COLOR(sq, face)
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.trans;
    return sq;
}

Uint32 *njStartVertexNonTexSpriteOpaque(Uint32 color)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.opaque;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += SPRITE_WORDS;
    SQ_PUT(sq, color);
    sq--;
    SQ_PUT(sq, TSP_OPAQUE(_nj_parameterkm_.tsp));
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, (_nj_parameterkm_.global & GLOBAL_USER_CLIP) |
                   SHADOWED(GLOBAL_SPRITE));
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.opaque;
    return sq;
}

Uint32 *njStartVertexTexSpriteOpaque(Uint32 color)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.opaque;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += SPRITE_WORDS;
    SQ_PUT(sq, color);
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, TSP_OPAQUE(_nj_parameterkm_.tsp));
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, (_nj_parameterkm_.global & GLOBAL_USER_CLIP) |
                   SHADOWED(GLOBAL_SPRITE | GLOBAL_TEXTURE | GLOBAL_UV16));
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.opaque;
    return sq;
}

Uint32 *njStartVertexTexSpriteTrans(Uint32 color)
{
    /* @hack redundant: register on list and sq */
    register Uint32 list = (Uint32)_nj_parameterkm_.trans;
    register Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += SPRITE_WORDS;
    SQ_PUT(sq, color);
    SQ_PUT(sq, _nj_parameterkm_.texture);
    SQ_PUT(sq, (_nj_parameterkm_.tsp & ~TSP_IGNORE_TEX_ALPHA) | TSP_USE_ALPHA);
    SQ_PUT(sq, _nj_parameterkm_.isp);
    SQ_PUT(sq, (_nj_parameterkm_.global & GLOBAL_USER_CLIP) |
                   SHADOWED(GLOBAL_SPRITE | GLOBAL_TRANS | GLOBAL_TEXTURE |
                            GLOBAL_UV16));
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.trans;
    return sq;
}

Uint32 *njStartModifier(void)
{
    Uint32 list = (Uint32)_nj_parameterkm_.opaque_modifier;
    Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += MODIFIER_WORDS;
    SQ_PUT(sq, KM_MODIFIER_NORMAL_POLY);
    SQ_PUT(sq, (_nj_parameterkm_.global & GLOBAL_USER_CLIP) | GLOBAL_MODIFIER);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.opaque_modifier;
    return sq;
}

Uint32 *njStartTransModifier(void)
{
    Uint32 list = (Uint32)_nj_parameterkm_.trans_modifier;
    Uint32 *sq;

    SQ_OPEN(sq, list)
    sq += MODIFIER_WORDS;
    SQ_PUT(sq, KM_MODIFIER_NORMAL_POLY);
    SQ_PUT(sq, (_nj_parameterkm_.global & GLOBAL_USER_CLIP) | GLOBAL_MODIFIER);
    prefetch(sq);
    sq += SQ_WORDS;
    _nj_listtype_ = &_nj_parameterkm_.trans_modifier;
    return sq;
}

Uint32 *njEndModifier(Uint32 *sq)
{
    sq += MODIFIER_WORDS;
    SQ_PUT(sq, KM_MODIFIER_INCLUDE_LAST_POLY);
    SQ_PUT(sq, (_nj_parameterkm_.global & GLOBAL_USER_CLIP) | GLOBAL_MODIFIER |
                   GLOBAL_VOLUME);
    prefetch(sq);
    sq += SQ_WORDS;
    return sq;
}

Uint32 *njEndModifierClip(Uint32 *sq)
{
    sq += MODIFIER_WORDS;
    SQ_PUT(sq, KM_MODIFIER_EXCLUDE_LAST_POLY);
    SQ_PUT(sq, (_nj_parameterkm_.global & GLOBAL_USER_CLIP) | GLOBAL_MODIFIER |
                   GLOBAL_VOLUME);
    prefetch(sq);
    sq += SQ_WORDS;
    return sq;
}

void njEndVertex(const Uint32 *sq)
{
    *_nj_listtype_ = (Uint32 *)(((Uint32)sq & SQ_OFFSET) | _nj_sq_base_);
}

void njSetkmColorMode(Uint32 mode)
{
    /* @hack redundant: register on global, tsp and isp */
    register Uint32 global = _nj_parameterkm_.global & ~COLOR_MODE_GLOBAL;
    register Uint32 tsp = _nj_parameterkm_.tsp & ~COLOR_MODE_TSP;
    /* @hack redundant: the copy's ISP word read and written back unchanged */
    register Uint32 isp = _nj_parameterkm_.isp;

    global |= mode & COLOR_MODE_GLOBAL;
    tsp |= mode & COLOR_MODE_TSP;
    /* @hack spelling: an arm of its own for each of the two tests, where one
     * arm under their || would do */
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_SHADOW) {
        global |= GLOBAL_SHADOW;
    } else if ((_nj_control_3d_flag_ & NJD_CONTROL_3D_SHADOW_OPAQUE) &&
               !(mode & NJD_ARC1_TRANS)) {
        global |= GLOBAL_SHADOW;
    } else {
        global &= ~GLOBAL_SHADOW;
    }
    _nj_parameterkm_.global = global;
    _nj_parameterkm_.isp = isp;
    _nj_parameterkm_.tsp = tsp;
    *_nj_color_mode_ = mode;
}

void njSetkmColorMode2p(Uint32 mode)
{
    /* @hack redundant: register on global, isp, tsp and tsp2 */
    register Uint32 global = _nj_parameterkm_.global & ~COLOR_MODE_GLOBAL;
    /* @hack redundant: the copy's ISP word read and written back unchanged */
    register Uint32 isp = _nj_parameterkm_.isp;
    register Uint32 tsp = _nj_parameterkm_.tsp & ~COLOR_MODE_TSP;
    register Uint32 tsp2 = _nj_TSPPARAMBUFFER_2_ & ~COLOR_MODE_TSP;

    global |= mode & COLOR_MODE_GLOBAL;
    tsp |= mode & COLOR_MODE_TSP;
    tsp2 |= mode & COLOR_MODE_TSP;
    /* @hack spelling: the two bits set here, after tsp and tsp2 take mode's
     * bits, where the statement that gives global mode's bits would set them
     * too */
    global |= GLOBAL_SHADOW | GLOBAL_VOLUME;
    _nj_parameterkm_.global = global;
    _nj_parameterkm_.isp = isp;
    _nj_parameterkm_.tsp = tsp;
    _nj_TSPPARAMBUFFER_2_ = tsp2;
    *_nj_color_mode_ = mode;
}

Sint32 njSetkmTextureNum(Uint32 n)
{
    NJS_TEXMEMLIST *tex =
        (NJS_TEXMEMLIST *)_nj_current_texlist->textures[n].texaddr;
    /* @hack spelling: tsp and texture, locals for the two words, where a
     * statement that sets each word of the copy would do */
    Uint32 tsp;
    /* @hack redundant: register on texture */
    register Uint32 texture;

    if (tex->globalIndex != _nj_current_texture ||
        _nj_current_texture_bank != tex->bank) {
        _nj_current_texture = tex->globalIndex;
        _nj_current_texture_bank = tex->bank;
        tsp = (_nj_parameterkm_.tsp & ~TSP_SIZE) |
              (tex->tspparambuffer & TSP_SIZE);
        _nj_parameterkm_.tsp = tsp;
        texture = tex->texparambuffer;
        _nj_parameterkm_.texture = texture;
        _nj_texglobal_color_ =
            tex->texinfo.texsurface.Type >> TEXSURFACE_FORMAT_SHIFT;
    }
    return _nj_texglobal_color_;
}

Sint32 njSetkmTextureNum2p(Uint32 n)
{
    NJS_TEXMEMLIST *tex =
        (NJS_TEXMEMLIST *)_nj_current_texlist->textures[n].texaddr;
    /* @hack spelling: tsp and texture, locals for the two words, where a
     * statement that sets each second word would do */
    Uint32 tsp;
    Uint32 texture;

    tsp = (_nj_parameterkm_.tsp & ~TSP_SIZE) | (tex->tspparambuffer & TSP_SIZE);
    _nj_TSPPARAMBUFFER_2_ = tsp;
    texture = tex->texparambuffer;
    _nj_TexturePARAMBUFFER_2_ = texture;
    _nj_texglobal_color_ =
        tex->texinfo.texsurface.Type >> TEXSURFACE_FORMAT_SHIFT;
    return _nj_texglobal_color_;
}

Sint32 njSetkmTextureNumG(Uint32 globalIndex)
{
    Bool changed = FALSE;
    /* @hack redundant: register on i */
    register Sint32 i;
    NJS_TEXMEMLIST *tex;
    /* @hack spelling: tsp and texture, locals for the two words, where a
     * statement that sets each word of the copy would do */
    /* @hack redundant: register on tsp */
    register Uint32 tsp;
    Uint32 texture;

    if (globalIndex != _nj_current_texture) {
        changed = TRUE;
        /* @hack redundant: a search whose result nothing reads */
        njSearchTexList(_nj_texmemlist, _nj_texmemsize, globalIndex);
    } else {
        i = njSearchTexList(_nj_texmemlist, _nj_texmemsize, globalIndex);
        /* @hack redundant: i stepped up and back down, which leaves it as the
         * search gave it */
        i++;
        i--;
        if (_nj_current_texture_bank != _nj_texmemlist[i].bank) {
            changed = TRUE;
        }
    }
    if (changed) {
        i = njSearchTexList(_nj_texmemlist, _nj_texmemsize, globalIndex);
        tex = &_nj_texmemlist[i];
        _nj_current_texture = globalIndex;
        _nj_current_texture_bank = tex->bank;
        tsp = _nj_parameterkm_.tsp;
        /* @hack redundant: a test of globalIndex that never runs */
        if (0) {
            if (globalIndex) {
            }
        }
        tsp &= ~TSP_SIZE;
        tsp |= tex->tspparambuffer & TSP_SIZE;
        _nj_parameterkm_.tsp = tsp;
        texture = tex->texparambuffer;
        _nj_parameterkm_.texture = texture;
        _nj_texglobal_color_ =
            tex->texinfo.texsurface.Type >> TEXSURFACE_FORMAT_SHIFT;
    }
    return _nj_texglobal_color_;
}

Sint32 njSetkmTextureMemList(Uint32 n)
{
    /* @hack spelling: tsp and texture, locals for the two words, where a
     * statement that sets each word of the copy would do */
    /* @hack redundant: register on tsp and texture */
    register Uint32 tsp;
    register Uint32 texture;

    _nj_current_texture = _nj_texmemlist[n].globalIndex;
    _nj_current_texture_bank = _nj_texmemlist[n].bank;
    tsp = _nj_parameterkm_.tsp & ~TSP_SIZE;
    tsp |= _nj_texmemlist[n].tspparambuffer & TSP_SIZE;
    _nj_parameterkm_.tsp = tsp;
    texture = _nj_texmemlist[n].texparambuffer;
    _nj_parameterkm_.texture = texture;
    _nj_texglobal_color_ =
        _nj_texmemlist[n].texinfo.texsurface.Type >> TEXSURFACE_FORMAT_SHIFT;
    return _nj_texglobal_color_;
}

void njSetkmModelAttr(Uint32 attr)
{
    /* @hack spelling: tsp, a local for the word, where one statement that
     * sets the copy's TSP word would do */
    Uint32 tsp = _nj_parameterkm_.tsp & ~MODEL_ATTR_TSP;

    tsp |= attr & MODEL_ATTR_TSP;
    _nj_parameterkm_.tsp = tsp;
}

void njZBufferkmMode(Int mode)
{
    /* @hack spelling: a switch on mode, where if (mode == TRUE) and else if
     * (mode == FALSE) would do */
    switch (mode) {
    case TRUE:
        _nj_parameterkm_.isp &= ~ISP_Z_WRITE_DISABLE;
        break;
    case FALSE:
        _nj_parameterkm_.isp |= ISP_Z_WRITE_DISABLE;
        break;
    }
}

void njMipmapkmAdjust(Uint32 level)
{
    _nj_parameterkm_.tsp = (_nj_parameterkm_.tsp & ~TSP_MIPMAP) | level;
}

void njTextureFilterkmMode(Uint32 mode)
{
    _nj_parameterkm_.tsp = (_nj_parameterkm_.tsp & ~TSP_FILTER) | mode;
}

void njSuperSamplekmMode(Int mode)
{
    /* @hack spelling: a switch on mode, where if (mode == TRUE) and else if
     * (mode == FALSE) would do */
    switch (mode) {
    case TRUE:
        _nj_parameterkm_.tsp |= TSP_SUPER_SAMPLE;
        break;
    case FALSE:
        _nj_parameterkm_.tsp &= ~TSP_SUPER_SAMPLE;
        break;
    }
}

void njTextureClampkmMode(Uint32 mode)
{
    _nj_parameterkm_.tsp = (_nj_parameterkm_.tsp & ~TSP_CLAMP) | mode;
}

void njTextureFlipkmMode(Uint32 mode)
{
    _nj_parameterkm_.tsp = (_nj_parameterkm_.tsp & ~TSP_FLIP) | mode;
}

void njkmPolygonCullingSize(Float size)
{
    kmSetCullingRegister(size);
}

void njPolygonCullingkmMode(Uint32 mode)
{
    _nj_parameterkm_.isp = (_nj_parameterkm_.isp & ~ISP_CULL) | mode;
}

void njColorBlendingkmMode(Int target, Uint32 mode)
{
    if (mode & BLEND_BOTH) {
        /* @bug logic: sets the TSP word's bit 0, the low bit of the texture's
         * height field, along with the blend factors. mode goes in whole,
         * BLEND_BOTH with it (OR R5,R6 at 0x8c11453e). the game never gets
         * here: no word in any image holds this function's address */
        _nj_parameterkm_.tsp = (_nj_parameterkm_.tsp & ~TSP_BLEND) | mode;
    } else if (target == NJD_SOURCE_COLOR) {
        _nj_parameterkm_.tsp =
            (_nj_parameterkm_.tsp & ~TSP_SRC) | (mode & TSP_SRC);
    } else {
        _nj_parameterkm_.tsp =
            (_nj_parameterkm_.tsp & ~TSP_DST) | (mode & TSP_DST);
    }
}

void njSpecularkmMode(Int mode)
{
    /* @hack spelling: a switch on mode, where if (mode == TRUE) and else if
     * (mode == FALSE) would do */
    switch (mode) {
    case TRUE:
        _nj_parameterkm_.global |= GLOBAL_OFFSET;
        break;
    case FALSE:
        _nj_parameterkm_.global &= ~GLOBAL_OFFSET;
        break;
    }
}

void njAlphakmMode(Int mode)
{
    /* @hack spelling: a switch on mode, where if (mode == TRUE) and else if
     * (mode == FALSE) would do */
    switch (mode) {
    case TRUE:
        _nj_parameterkm_.tsp |= TSP_USE_ALPHA;
        break;
    case FALSE:
        _nj_parameterkm_.tsp &= ~TSP_USE_ALPHA;
        break;
    }
}

void njIgnoreTextureAlphakmMode(Int mode)
{
    /* @hack spelling: a switch on mode, where if (mode == TRUE) and else if
     * (mode == FALSE) would do */
    switch (mode) {
    case TRUE:
        _nj_parameterkm_.tsp |= TSP_IGNORE_TEX_ALPHA;
        break;
    case FALSE:
        _nj_parameterkm_.tsp &= ~TSP_IGNORE_TEX_ALPHA;
        break;
    }
}

void njTextureShadingkmMode(Uint32 mode)
{
    _nj_parameterkm_.tsp = (_nj_parameterkm_.tsp & ~TSP_SHADING) | mode;
}

Sint32 njSetkmCnkTextureNum(Uint32 head, Uint32 texid)
{
    NJS_TEXMEMLIST *tex = (NJS_TEXMEMLIST *)_nj_current_texlist
                              ->textures[texid & CNK_TEXTURE_INDEX]
                              .texaddr;
    /* @hack redundant: register on tsp */
    register Uint32 tsp;

    if (tex->globalIndex != _nj_current_texture ||
        _nj_current_texture_bank != tex->bank) {
        _nj_current_texture = tex->globalIndex;
        _nj_current_texture_bank = tex->bank;
        /* @hack spelling: the texture's size set through tsp, where a
         * statement on the copy's TSP word would do */
        tsp = _nj_parameterkm_.tsp;
        tsp &= ~TSP_SIZE;
        tsp |= tex->tspparambuffer & TSP_SIZE;
        _nj_parameterkm_.tsp = tsp;
        _nj_parameterkm_.texture = tex->texparambuffer;
        _nj_texglobal_color_ =
            tex->texinfo.texsurface.Type >> TEXSURFACE_FORMAT_SHIFT;
    }
    tsp = _nj_parameterkm_.tsp & ~CNK_TEXTURE_TSP;
    tsp |= (head & (NJD_FFL_U | NJD_FFL_V | NJD_FCL_U | NJD_FCL_V))
           << CNK_FLIP_CLAMP_SHIFT;
    tsp |= head & NJD_FDA_MASK;
    tsp |= (texid & NJD_FSS) >> CNK_FILTER_SHIFT;
    tsp |= (texid & NJD_FFM_BF) >> CNK_FILTER_SHIFT;
    _nj_parameterkm_.tsp = tsp;
    return _nj_texglobal_color_;
}

void njSetkmCnkBlendMode(Uint32 head)
{
    /* @hack spelling: tsp, a local masked above the flag's test, where one
     * statement below it that sets the copy's TSP word would do */
    Uint32 tsp = _nj_parameterkm_.tsp & ~TSP_BLEND;

    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_CNK_BLEND_MODE) {
        head = _nj_cnk_blend_mode_;
    }
    tsp |= (head << CNK_BLEND_SHIFT) & TSP_BLEND;
    _nj_parameterkm_.tsp = tsp;
}

void njSetCnkBlendMode(Uint32 attr)
{
    _nj_cnk_blend_mode_ = attr;
}

void njSetkmCellBlendMode(Uint32 attr)
{
    /* @hack spelling: tsp, a local for the word, where one statement that
     * sets the copy's TSP word would do */
    Uint32 tsp = _nj_parameterkm_.tsp & ~TSP_BLEND;

    tsp |= (attr << CNK_BLEND_SHIFT) & TSP_BLEND;
    _nj_parameterkm_.tsp = tsp;
}
