/* the easy multi chunk drawer. several lights give each vertex a color. a
 * strip's material scales the color and adds its ambient light */
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_cnk_set_material.h>
#include <nj_global.h>
#include <nj_km.h>
#include <nj_light_state.h>
#include <sg_xpt.h>

/* AMBIENT(0), AMBIENT(1) and AMBIENT(2) are the red, green and blue of a
 * strip's ambient light. each is the material's ambient channel times the
 * lights' */
#ifdef NONMATCHING
/* this side reads the lights' ambient by its own name, the plain form the hack
 * tag below gives. GCC needn't place _nj_cnk_easy_multilight_buffer_mdst_
 * right after the buffer, so an index past the buffer's end needn't reach it */
#define AMBIENT(i)                                                             \
    (material[MATERIAL_AMBIENT + ARGB_R + (i)] *                               \
     _nj_cnk_easy_multilight_buffer_mdst_.ambient[(i)])
#else
/* @hack spelling: the lights' ambient indexed from
 * _nj_cnk_easy_multilight_buffer_, past its end, where words 1 to 3 of
 * _nj_cnk_easy_multilight_buffer_mdst_ would do */
#define AMBIENT(i)                                                             \
    (material[MATERIAL_AMBIENT + ARGB_R + (i)] *                               \
     _nj_cnk_easy_multilight_buffer_[MULTILIGHT_AMBIENT + (i)])
#endif

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* draw the polygon list plist from vbuf, which the model's vertex list
 * filled. clipnum is _nj_cnk_clip_num_ after that list. a nonzero clipnum
 * selects the polygon functions' Clip forms */
void njCnkEasyMultiPlist(Sint16 *plist, Sint32 clipnum,
                         NJS_CNK_EASYMULTI_BUF *vbuf);
/* project each vertex chunk of vlist into vbuf by its type's easy multi
 * vertex function. while _nj_depth_enable_ is set, the functions are the
 * DepthQueue forms. return 0, or -1 at the first chunk whose type isn't
 * NJD_CV_VN, NJD_CV_VN_NF or NJD_CV_VN_D8 */
Sint32 njCnkEasyMultiVlist(Sint16 *vlist, NJS_CNK_EASYMULTI_BUF *vbuf);
/* draw object as njCnkEasyMultiDrawObject does. the caller calls
 * njGetkmPointer and njCnkInitEasyMultiVertex before it, and njSetkmPointer
 * after */
void _njCnkEasyMultiDrawObject(NJS_CNK_OBJECT *object);
/* NOLINTEND(misc-use-internal-linkage) */

static struct nj_cnk_easy_multi_work work;
/* the diffuse color, then the ambient color, that njCnkSetMaterial last
 * wrote, as four floats each */
static Float material[8];

/* @hack redundant: the inline pragma, which expands njCnkInitEasyMultiVertex
 * at each call */
#pragma inline(njCnkInitEasyMultiVertex)

void njCnkEasyMultiPlist(Sint16 *plist, Sint32 clipnum,
                         NJS_CNK_EASYMULTI_BUF *vbuf)
{
    /* @hack redundant: register on head, type, cache, size, nbstrip, end
     * and sq */
    register Sint16 head;
    register Sint16 type;
    register Sint16 cache;
    register Sint32 size;
    register Sint32 nbstrip;
    register Uint32 *end;
    register Uint32 *sq;

    while ((head = *plist++) != NJD_CE) {
        type = CNK_TYPE(head);
        if (type < NJD_TINYOFF) {
            switch (type) {
            case NJD_CB_BA:
                njSetkmCnkBlendMode(head);
                break;
            case NJD_CB_DA:
                break;
            case NJD_CB_EXP:
                break;
            case NJD_CB_CP:
                cache = CNK_FLAGS(head);
                _nj_cnk_polygon_cache_tbl_[cache] = plist;
                _nj_cnk_plist_stack_cnt_++;
                return;
            case NJD_CB_DP:
                cache = CNK_FLAGS(head);
                njCnkEasyMultiPlist(_nj_cnk_polygon_cache_tbl_[cache], clipnum,
                                    vbuf);
                _nj_cnk_plist_stack_cnt_--;
                break;
            }
            continue;
        }
        if (type < NJD_MATOFF) {
            njSetkmCnkTextureNum(head, *plist++);
            continue;
        }
        if (type < NJD_STRIPOFF) {
            size = *plist++;
            /* @hack spelling: &material[0], where material would do */
            njCnkSetMaterial(head, (Uint32 *)plist, &material[0],
                             &material[MATERIAL_AMBIENT], NULL);
            plist += size;
            prefetch(plist);
            continue;
        }
        size = (Uint16)*plist++;
        nbstrip = *plist++;
        if (clipnum < _nj_cnk_clip_vtx_) {
            if (_nj_control_3d_flag_ & NJD_CONTROL_3D_CNK_CONSTANT_ATTR) {
                head &= _nj_constant_attr_and_;
                head |= _nj_constant_attr_or_;
            }
            /* @hack spelling: head shifted down to the strip's flags and kept
             * for them, where a local of their own would do */
            head >>= NJD_FST_SHIFT;
            end = NULL;
            /* the depth queue fades through alpha, which NJD_FST_UA turns
             * on */
            if (_nj_depth_enable_) {
                head |= CNK_FST(NJD_FST_UA);
            }
            _nj_flat_model_flag_ = FALSE;
            if (head & CNK_FST(NJD_FST_FL)) {
                _nj_flat_model_flag_ = TRUE;
            }
            switch (type) {
            case NJD_CS_UVN:
            case NJD_CS_UVH:
                if (head & CNK_FST(NJD_FST_UA)) {
                    if (head & CNK_FST(NJD_FST_IS)) {
                        if (head & CNK_FST(NJD_FST_FL)) {
                            njSetkmColorMode(NJD_COLOR_MODE_FLAT_TEXTURE_TRANS);
                        } else {
                            njSetkmColorMode(NJD_COLOR_MODE_TEXTURE_TRANS);
                        }
                    } else {
                        if (head & CNK_FST(NJD_FST_FL)) {
                            njSetkmColorMode(
                                NJD_COLOR_MODE_HIGHLIGHT_FLAT_TEXTURE_TRANS);
                        } else {
                            njSetkmColorMode(
                                NJD_COLOR_MODE_HIGHLIGHT_TEXTURE_TRANS);
                        }
                    }
                    sq = njStartVertexTexTrans();
                } else {
                    if (head & CNK_FST(NJD_FST_IS)) {
                        if (head & CNK_FST(NJD_FST_FL)) {
                            njSetkmColorMode(NJD_COLOR_MODE_FLAT_TEXTURE);
                        } else {
                            njSetkmColorMode(NJD_COLOR_MODE_TEXTURE);
                        }
                    } else {
                        if (head & CNK_FST(NJD_FST_FL)) {
                            njSetkmColorMode(
                                NJD_COLOR_MODE_HIGHLIGHT_FLAT_TEXTURE);
                        } else {
                            njSetkmColorMode(NJD_COLOR_MODE_HIGHLIGHT_TEXTURE);
                        }
                    }
                    sq = njStartVertexTexOpaque();
                }
                if (clipnum) {
                    if (head & CNK_FST(NJD_FST_IA)) {
                        end = njCnkEasyMultiTextureClip(
                            plist, vbuf, nbstrip, sq, material[ARGB_A], 0.0F,
                            0.0F, 0.0F,
                            type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE);
                    } else {
                        end = njCnkEasyMultiTextureClip(
                            plist, vbuf, nbstrip, sq, material[ARGB_A],
                            AMBIENT(0), AMBIENT(1), AMBIENT(2),
                            type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE);
                    }
                } else {
                    if (head & CNK_FST(NJD_FST_IA)) {
                        end = njCnkEasyMultiTexture(
                            plist, vbuf, nbstrip, sq, material[ARGB_A], 0.0F,
                            0.0F, 0.0F,
                            type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE);
                    } else {
                        end = njCnkEasyMultiTexture(
                            plist, vbuf, nbstrip, sq, material[ARGB_A],
                            AMBIENT(0), AMBIENT(1), AMBIENT(2),
                            type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE);
                    }
                }
                break;
            case NJD_CS:
                if (head & CNK_FST(NJD_FST_UA)) {
                    if (head & CNK_FST(NJD_FST_FL)) {
                        njSetkmColorMode(NJD_COLOR_MODE_FLAT_NORMAL_TRANS);
                    } else {
                        njSetkmColorMode(NJD_COLOR_MODE_NORMAL_TRANS);
                    }
                    sq = njStartVertexNonTexTrans();
                } else {
                    if (head & CNK_FST(NJD_FST_FL)) {
                        njSetkmColorMode(NJD_COLOR_MODE_FLAT_NORMAL);
                    } else {
                        njSetkmColorMode(NJD_COLOR_MODE_NORMAL);
                    }
                    sq = njStartVertexNonTexOpaque();
                }
                if (clipnum) {
                    if (head & CNK_FST(NJD_FST_IA)) {
                        end = njCnkEasyMultiPolygonClip(
                            plist, vbuf, nbstrip, sq, material[ARGB_A],
                            material[ARGB_R], material[ARGB_G],
                            material[ARGB_B], 0.0F, 0.0F, 0.0F);
                    } else {
                        end = njCnkEasyMultiPolygonClip(
                            plist, vbuf, nbstrip, sq, material[ARGB_A],
                            material[ARGB_R], material[ARGB_G],
                            material[ARGB_B], AMBIENT(0), AMBIENT(1),
                            AMBIENT(2));
                    }
                } else {
                    if (head & CNK_FST(NJD_FST_IA)) {
                        end = njCnkEasyMultiPolygon(
                            plist, vbuf, nbstrip, sq, material[ARGB_A],
                            material[ARGB_R], material[ARGB_G],
                            material[ARGB_B], 0.0F, 0.0F, 0.0F);
                    } else {
                        end = njCnkEasyMultiPolygon(
                            plist, vbuf, nbstrip, sq, material[ARGB_A],
                            material[ARGB_R], material[ARGB_G],
                            material[ARGB_B], AMBIENT(0), AMBIENT(1),
                            AMBIENT(2));
                    }
                }
                break;
            }
            if (end) {
                njEndVertex(end);
            }
        }
        /* size counts the chunk's halfwords after the size halfword. plist
         * has passed the first, which holds nbstrip */
        plist += size - 1;
        prefetch(plist);
    }
}

Sint32 njCnkEasyMultiVlist(Sint16 *vlist, NJS_CNK_EASYMULTI_BUF *vbuf)
{
    /* @hack redundant: register on head, size, offset, nbvertex, near and
     * buf */
    register Sint32 head;
    register Sint32 size;
    register Sint32 offset;
    register Sint32 nbvertex;
    register Sint32 near;
    Sint32 type;
    register NJS_CNK_EASYMULTI_BUF *buf;

    while ((head = *vlist++) != NJD_CE) {
        size = (Uint16)*vlist++;
        offset = *vlist++;
        nbvertex = *vlist++;
        if (!(head & NJD_FV_CONT)) {
            _nj_cnk_clip_num_ = 0;
            _nj_cnk_clip_vtx_ = 0;
        }
        head &= ~NJD_FV_CONT;
        work.nbvertex = nbvertex;
        work.vertices = vlist;
        buf = &vbuf[offset];
        work.buf = buf;
        work.sq = SQ_ADDRESS((Uint32)buf);
        SQ_MAP((Uint32)buf);
        _nj_calc_vertex_ += nbvertex;
        type = CNK_TYPE(head);
        if (_nj_depth_enable_) {
            if (type == NJD_CV_VN) {
                near = njCnkEasyMultiVertexVNDepthQueue(&work);
            } else if (type == NJD_CV_VN_NF) {
                near = njCnkEasyMultiVertexVNNFDepthQueue(&work, head);
            } else if (type == NJD_CV_VN_D8) {
                near = njCnkEasyMultiVertexVND8DepthQueue(&work);
            } else {
                return -1;
            }
        } else {
            if (type == NJD_CV_VN) {
                near = njCnkEasyMultiVertexVN(&work);
            } else if (type == NJD_CV_VN_NF) {
                near = njCnkEasyMultiVertexVNNF(&work, head);
            } else if (type == NJD_CV_VN_D8) {
                near = njCnkEasyMultiVertexVND8(&work);
            } else {
                return -1;
            }
        }
        _nj_cnk_clip_num_ += near;
        /* the chunks of an NJD_CV_VN_NF series weight the same vertices.
         * only the series' NJD_FW_END chunk adds them to _nj_cnk_clip_vtx_ */
        if ((head & CNK_FLAGS_MASK) == NJD_FW_END || type != NJD_CV_VN_NF) {
            _nj_cnk_clip_vtx_ += nbvertex;
        }
        /* size counts the chunk's 4-byte words after the size halfword.
         * offset and nbvertex fill the first, which vlist has passed */
        vlist += (size * CNK_WORD_HALFWORDS) - CNK_WORD_HALFWORDS;
    }
    return 0;
}

Sint32 _njCnkEasyMultiDrawModel(NJS_CNK_MODEL *model)
{
    NJS_POINT3 center;
    /* @hack redundant: register on vbuf */
    register NJS_CNK_EASYMULTI_BUF *vbuf;
    Sint32 *vlist;
    Sint16 *plist;
    Sint32 clipnum;

    if ((_nj_control_3d_flag_ & NJD_CONTROL_3D_MODEL_CLIP) && model->r > 0.0F &&
        njCnkModelClip(model)) {
        return -1;
    }
    /* the depth queue's fade is 1 at a z of _nj_depth_near_ or less. so the
     * drawer turns the queue on only for a model whose bounding sphere
     * reaches farther */
    _nj_depth_enable_ = FALSE;
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_DEPTH_QUEUE) {
        njCalcPoint(NULL, &model->center, &center);
        if (center.z + model->r > _nj_depth_near_) {
            _nj_depth_enable_ = TRUE;
        }
    }
    vbuf = (NJS_CNK_EASYMULTI_BUF *)_nj_vertex_buf_;
    vlist = model->vlist;
    if (vlist != NULL) {
        if (njCnkEasyMultiVlist((Sint16 *)vlist, vbuf) == -1) {
            return -1;
        }
    }
    clipnum = _nj_cnk_clip_num_;
    plist = model->plist;
    if (plist != NULL) {
        njCnkEasyMultiPlist(plist, clipnum, vbuf);
    }
    return 0;
}

void _njCnkEasyMultiDrawObject(NJS_CNK_OBJECT *object)
{
    /* @hack redundant: register on flags and clipped */
    register Uint32 flags;
    register Sint32 clipped;

#ifdef NONMATCHING
    /* clipped starts at 0 in place of the caller's R13, as nj_cnk.h says */
    clipped = 0;
#endif
    do {
        flags = object->evalflags;
        if ((flags & CNK_EVAL_UNIT) == CNK_EVAL_UNIT) {
            if (!(flags & NJD_EVAL_HIDE)) {
                _njCnkEasyMultiDrawModel(object->model);
            }
            if (!(flags & NJD_EVAL_BREAK)) {
                _njCnkEasyMultiDrawObject(object->child);
            }
        } else {
#ifdef NONMATCHING
            /* This subtree needs a transform of its own. Its siblings can
             * still draw in the parent coordinate system. */
            if (!njPushMatrixEx()) {
                object = object->sibling;
                continue;
            }
#else
            njPushMatrixEx();
#endif
            if (!(flags & NJD_EVAL_UNIT_POS)) {
                njTranslateEx((NJS_VECTOR *)object->pos);
            }
            if (!(flags & NJD_EVAL_UNIT_ANG)) {
                /* @hack spelling: the rotation order passed in clipped,
                 * where flags & NJD_EVAL_ZXY_ANG passed in place would do */
                clipped = flags & NJD_EVAL_ZXY_ANG;
                njRotateEx(object->ang, clipped);
            }
            if (!(flags & NJD_EVAL_UNIT_SCL)) {
                njScaleEx((NJS_VECTOR *)object->scl);
            }
            if (!(flags & NJD_EVAL_HIDE)) {
                clipped = _njCnkEasyMultiDrawModel(object->model);
            }
            if (!(flags & NJD_EVAL_BREAK)) {
                /* @bug uninitialized: tests clipped for a hidden node, as
                 * nj_cnk.h says (TST R13,R13 at 0x8c128a20) */
                if (!(flags & NJD_EVAL_CLIP) || !clipped) {
                    _njCnkEasyMultiDrawObject(object->child);
                }
            }
            njPopMatrixEx();
        }
        object = object->sibling;
    } while (object);
}

void njCnkEasyMultiDrawModel(NJS_CNK_MODEL *model)
{
    njGetkmPointer();
    njCnkInitEasyMultiVertex();
    _njCnkEasyMultiDrawModel(model);
    njSetkmPointer();
}

void njCnkEasyMultiDrawObject(NJS_CNK_OBJECT *object)
{
    njGetkmPointer();
    njCnkInitEasyMultiVertex();
    prefetch(object);
    _njCnkEasyMultiDrawObject(object);
    njSetkmPointer();
}

void njCnkInitEasyMultiVertex(void)
{
    work.dist_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    work.dist_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    work.cx = _nj_screen_.cx;
    work.cy = _nj_screen_.cy;
}
