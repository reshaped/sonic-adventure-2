/* the easy chunk drawer. one light gives each vertex an intensity, which
 * scales its strip's polygon color */
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_cnk_set_material.h>
#include <nj_global.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* draw the polygon list plist from vbuf, which the model's vertex list
 * filled. vtype is what njCnkEasyVlist returned for that list. at
 * NJD_CV_D8, the strips take their vertices' colors. at any other vtype,
 * the vertices' intensities light them. a bump-mapped strip takes neither.
 * clipnum is _nj_cnk_clip_num_ after that list. a nonzero clipnum selects
 * the polygon functions' Clip forms */
void njCnkEasyPlist(Sint16 *plist, Sint32 vtype, Sint32 clipnum,
                    NJS_CNK_EASY_BUF *vbuf);
/* project each vertex chunk of vlist into vbuf by its type's easy vertex
 * function. a chunk of a type no function takes projects nothing. return
 * the last chunk's first halfword, sign-extended, with NJD_FV_CONT cleared.
 * where that chunk is NJD_CV_VN_D8, return NJD_CV_D8, as its entries hold
 * colors. for a vlist of NJD_CE alone, the result is unset */
Sint32 njCnkEasyVlist(Sint16 *vlist, NJS_CNK_EASY_BUF *vbuf);
/* draw object as njCnkEasyDrawObject draws it, between calls of
 * njGetkmPointer and njSetkmPointer that the caller makes */
void _njCnkEasyDrawObject(NJS_CNK_OBJECT *object);
/* NOLINTEND(misc-use-internal-linkage) */

/* the diffuse and specular colors that njCnkSetMaterial last wrote, as four
 * floats each */
static Float diffuse[4] = {1.0F, 1.0F, 1.0F, 1.0F};
static Float specular[4] = {0.0F, 0.0F, 0.0F, 0.0F};
/* TRUE where the walker's last texture chunk set a bump texture, and the
 * walker read the texture's vectors. each call of njCnkEasyPlist starts it
 * FALSE */
static Bool bump;

void njCnkSetEasyLight(Float x, Float y, Float z)
{
    _nj_vertex_easy.light.x = x;
    _nj_vertex_easy.light.y = y;
    _nj_vertex_easy.light.z = z;
    _nj_vertex_easy.dist_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    _nj_vertex_easy.dist_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    _nj_vertex_easy.cx = _nj_screen_.cx;
    _nj_vertex_easy.cy = _nj_screen_.cy;
}

void njCnkSetEasyLightIntensity(Float inten, Float ambient)
{
    _nj_vertex_easy.inten = -inten;
    _nj_vertex_easy.ambient = ambient;
}

void njCnkSetEasyLightColor(Float r, Float g, Float b)
{
    _nj_lcolor_easy.r = r;
    _nj_lcolor_easy.g = g;
    _nj_lcolor_easy.b = b;
}

void njCnkEasyPlist(Sint16 *plist, Sint32 vtype, Sint32 clipnum,
                    NJS_CNK_EASY_BUF *vbuf)
{
    /* @hack redundant: register on head, type, cache, size, nbstrip, format,
     * end, vector, sq and face */
    register Sint16 head;
    Sint16 flags;
    register Sint16 type;
    register Sint16 cache;
    register Sint32 size;
    register Sint32 nbstrip;
    register Sint32 format;
    register Uint32 *end;
    register Float *vector;
    register Uint32 *sq;
    register NJS_ARGB *face;
    Float color[4];

    bump = FALSE;
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
                njCnkEasyPlist(_nj_cnk_polygon_cache_tbl_[cache], vtype,
                               clipnum, vbuf);
                _nj_cnk_plist_stack_cnt_--;
                break;
            }
            continue;
        }
        if (type < NJD_MATOFF) {
            format = njSetkmCnkTextureNum(head, *plist++);
            bump = FALSE;
            /* an NJD_CM_BU chunk follows a bump texture's chunk. the six
             * halfwords of the texture's vectors follow that chunk's head and
             * size */
            if ((format & NJD_TEXFMT_COLOR_MASK) == NJD_TEXFMT_BUMP) {
                /* @bug logic: tests the next chunk's type for any bit of
                 * NJD_CM_BU, as nj_cnk.h says (TST R4,R3 at 0x8c127aee) */
                if (*plist & NJD_CM_BU) {
                    plist += CNK_MATERIAL_LEAD;
                    vector = _nj_cnk_easy_bump_tex_vector_;
                    *vector++ = (Float)*plist++ / BUMP_SCALE;
                    *vector++ = (Float)*plist++ / BUMP_SCALE;
                    *vector++ = (Float)*plist++ / BUMP_SCALE;
                    *vector++ = (Float)*plist++ / BUMP_SCALE;
                    *vector++ = (Float)*plist++ / BUMP_SCALE;
                    *vector = (Float)*plist++ / BUMP_SCALE;
                    bump = TRUE;
                }
            }
            continue;
        }
        if (type < NJD_STRIPOFF) {
            size = *plist++;
            njCnkSetMaterial(head, (Uint32 *)plist, diffuse, NULL, specular);
            plist += size;
            prefetch(plist);
            continue;
        }
        flags = head;
        size = (Uint16)*plist++;
        nbstrip = *plist++;
        if (clipnum < _nj_cnk_clip_vtx_) {
            if (_nj_control_3d_flag_ & NJD_CONTROL_3D_CNK_CONSTANT_ATTR) {
                flags &= _nj_constant_attr_and_;
                flags |= _nj_constant_attr_or_;
            }
            flags >>= NJD_FST_SHIFT;
            end = NULL;
            _nj_flat_model_flag_ = FALSE;
            if (flags & CNK_FST(NJD_FST_FL)) {
                _nj_flat_model_flag_ = TRUE;
            }
            switch (type) {
            case NJD_CS_UVN:
            case NJD_CS_UVH:
                if (bump) {
                    /* @hack redundant: a call of njStartVertexTexTrans that
                     * never runs */
                    if (0) {
                        njStartVertexTexTrans();
                    }
                    if (flags & CNK_FST(NJD_FST_UA)) {
                        njSetkmColorMode(
                            NJD_COLOR_MODE_HIGHLIGHT_FLAT_PACKED_TEXTURE_TRANS);
                        _nj_lcolor_easy.a = diffuse[ARGB_A];
                        sq = njStartVertexTexTrans();
                    } else {
                        njSetkmColorMode(
                            NJD_COLOR_MODE_HIGHLIGHT_FLAT_PACKED_TEXTURE);
                        sq = njStartVertexTexOpaque();
                    }
                    if (clipnum) {
                        end = njCnkEasyTextureBumpClip(
                            plist, vbuf, nbstrip, sq,
                            type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE);
                    } else {
                        end = njCnkEasyTextureBump(
                            plist, vbuf, nbstrip, sq,
                            type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE);
                    }
                } else {
                    if (vtype != NJD_CV_D8) {
                        /* a textured strip's polygon color is the light's,
                         * _nj_lcolor_easy, not the material's diffuse. under
                         * NJD_FST_UA, the walker sets the light's alpha to the
                         * diffuse alpha. another strip takes the alpha the
                         * light holds already */
                        face = &_nj_lcolor_easy;
                        if (flags & CNK_FST(NJD_FST_UA)) {
                            if (flags & CNK_FST(NJD_FST_IS)) {
                                if (flags & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_FLAT_INTENSITY_TEXTURE_TRANS);
                                } else {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_INTENSITY_TEXTURE_TRANS);
                                }
                                face->a = diffuse[ARGB_A];
                                sq = njStartVertexTexTransIntens(&face->a);
                            } else {
                                if (flags & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_HIGHLIGHT_FLAT_INTENSITY_TEXTURE_TRANS);
                                } else {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_HIGHLIGHT_INTENSITY_TEXTURE_TRANS);
                                }
                                face->a = diffuse[ARGB_A];
                                sq = njStartVertexTexTransIntensH(&face->a,
                                                                  specular);
                            }
                        } else {
                            if (flags & CNK_FST(NJD_FST_IS)) {
                                if (flags & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_FLAT_INTENSITY_TEXTURE);
                                } else {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_INTENSITY_TEXTURE);
                                }
                                sq = njStartVertexTexOpaqueIntens(&face->a);
                            } else {
                                if (flags & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_HIGHLIGHT_FLAT_INTENSITY_TEXTURE);
                                } else {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_HIGHLIGHT_INTENSITY_TEXTURE);
                                }
                                sq = njStartVertexTexOpaqueIntensH(&face->a,
                                                                   specular);
                            }
                        }
                    } else {
                        if (flags & CNK_FST(NJD_FST_UA)) {
                            if (flags & CNK_FST(NJD_FST_FL)) {
                                njSetkmColorMode(
                                    NJD_COLOR_MODE_FLAT_PACKED_TEXTURE_TRANS);
                            } else {
                                njSetkmColorMode(
                                    NJD_COLOR_MODE_PACKED_TEXTURE_TRANS);
                            }
                            _nj_lcolor_easy.a = diffuse[ARGB_A];
                            sq = njStartVertexTexTrans();
                        } else {
                            if (flags & CNK_FST(NJD_FST_FL)) {
                                njSetkmColorMode(
                                    NJD_COLOR_MODE_FLAT_PACKED_TEXTURE);
                            } else {
                                njSetkmColorMode(NJD_COLOR_MODE_PACKED_TEXTURE);
                            }
                            sq = njStartVertexTexOpaque();
                        }
                    }
                    if (clipnum) {
                        end = njCnkEasyTextureClip(
                            plist, vbuf, nbstrip, sq,
                            type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE);
                    } else {
                        end = njCnkEasyTexture(plist, vbuf, nbstrip, sq,
                                               type == NJD_CS_UVN ? UVN_SCALE
                                                                  : UVH_SCALE);
                    }
                }
                break;
            case NJD_CS:
                if (vtype != NJD_CV_D8) {
                    color[ARGB_A] = diffuse[ARGB_A];
                    color[ARGB_R] = diffuse[ARGB_R] * _nj_lcolor_easy.r;
                    color[ARGB_G] = diffuse[ARGB_G] * _nj_lcolor_easy.g;
                    color[ARGB_B] = diffuse[ARGB_B] * _nj_lcolor_easy.b;
                    /* @hack spelling: face pointed at color, where color
                     * passed to the starters would do */
                    face = (NJS_ARGB *)color;
                    if (flags & CNK_FST(NJD_FST_UA)) {
                        if (flags & CNK_FST(NJD_FST_FL)) {
                            njSetkmColorMode(
                                NJD_COLOR_MODE_FLAT_INTENSITY_NORMAL_TRANS);
                        } else {
                            njSetkmColorMode(
                                NJD_COLOR_MODE_INTENSITY_NORMAL_TRANS);
                        }
                        sq = njStartVertexNonTexTransIntens(&face->a);
                    } else {
                        if (flags & CNK_FST(NJD_FST_FL)) {
                            njSetkmColorMode(
                                NJD_COLOR_MODE_FLAT_INTENSITY_NORMAL);
                        } else {
                            njSetkmColorMode(NJD_COLOR_MODE_INTENSITY_NORMAL);
                        }
                        sq = njStartVertexNonTexOpaqueIntens(&face->a);
                    }
                } else {
                    if (flags & CNK_FST(NJD_FST_UA)) {
                        if (flags & CNK_FST(NJD_FST_FL)) {
                            njSetkmColorMode(
                                NJD_COLOR_MODE_FLAT_PACKED_NORMAL_TRANS);
                        } else {
                            njSetkmColorMode(
                                NJD_COLOR_MODE_PACKED_NORMAL_TRANS);
                        }
                        sq = njStartVertexNonTexTrans();
                    } else {
                        if (flags & CNK_FST(NJD_FST_FL)) {
                            njSetkmColorMode(NJD_COLOR_MODE_FLAT_PACKED_NORMAL);
                        } else {
                            njSetkmColorMode(NJD_COLOR_MODE_PACKED_NORMAL);
                        }
                        sq = njStartVertexNonTexOpaque();
                    }
                }
                if (clipnum) {
                    end = njCnkEasyPolygonClip(plist, vbuf, nbstrip, sq);
                } else {
                    end = njCnkEasyPolygon(plist, vbuf, nbstrip, sq);
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
    /* @hack redundant: a test of clipnum that never runs */
    if (0) {
        if (clipnum) {
        }
    }
}

Sint32 njCnkEasyVlist(Sint16 *vlist, NJS_CNK_EASY_BUF *vbuf)
{
    /* @hack redundant: register on head, size and nbvertex */
    register Sint32 head;
    register Sint32 size;
    Sint32 type;
    Sint32 offset;
    register Sint32 nbvertex;
    Sint32 near;
    NJS_CNK_EASY_BUF *buf;

#ifdef NONMATCHING
    /* a list of NJD_CE alone leaves type unset, and the image returns the
     * caller's R12. this side returns NJD_CE, which is no vertex chunk's
     * type */
    type = NJD_CE;
#endif
    while ((head = *vlist++) != NJD_CE) {
        size = (Uint16)*vlist++;
        type = head;
        offset = *vlist++;
        nbvertex = *vlist++;
        if (!(type & NJD_FV_CONT)) {
            _nj_cnk_clip_num_ = 0;
            _nj_cnk_clip_vtx_ = 0;
        }
        type &= ~NJD_FV_CONT;
        _nj_vertex_easy.nbvertex = nbvertex;
        _nj_vertex_easy.vertices = vlist;
        buf = &vbuf[offset];
        _nj_vertex_easy.buf = buf;
        _nj_vertex_easy.sq = SQ_ADDRESS((Uint32)buf);
        SQ_MAP((Uint32)buf);
        _nj_calc_vertex_ += nbvertex;
        switch (CNK_TYPE(type)) {
        case NJD_CV_VN:
            near = njCnkEasyVertexVN(&_nj_vertex_easy);
            break;
        case NJD_CV_VN_D8:
            near = njCnkEasyVertexVND8(&_nj_vertex_easy);
            type = NJD_CV_D8;
            break;
        case NJD_CV_VN_NF:
            near = njCnkEasyVertexVNNF(&_nj_vertex_easy, type);
            break;
        case NJD_CV_VN_UF:
        case NJD_CV_VN_S5:
        case NJD_CV_VN_S4:
        case NJD_CV_VN_IN:
            near = njCnkEasyVertexVN1(&_nj_vertex_easy);
            break;
        case NJD_CV:
            near = njCnkEasyVertexV(&_nj_vertex_easy);
            break;
        case NJD_CV_UF:
        case NJD_CV_NF:
        case NJD_CV_S5:
        case NJD_CV_S4:
            near = njCnkEasyVertexV1(&_nj_vertex_easy);
            break;
        case NJD_CV_VNX:
            near = njCnkEasyVertexVNX(&_nj_vertex_easy);
            break;
        case NJD_CV_VNX_D8:
        case NJD_CV_VNX_UF:
            near = njCnkEasyVertexVNX1(&_nj_vertex_easy);
            break;
        case NJD_CV_SH:
            near = njCnkEasyVertexSHV(&_nj_vertex_easy);
            break;
        case NJD_CV_VN_SH:
            near = njCnkEasyVertexSHVN(&_nj_vertex_easy);
            break;
        case NJD_CV_IN:
            near = njCnkEasyVertexVI(&_nj_vertex_easy);
            break;
        case NJD_CV_D8:
            near = njCnkEasyVertexVD8(&_nj_vertex_easy);
            break;
        default:
            near = 0;
            break;
        }
        _nj_cnk_clip_num_ += near;
        /* the chunks of an NJD_CV_VN_NF series weight the same vertices.
         * only the series' NJD_FW_END chunk adds them to _nj_cnk_clip_vtx_ */
        if ((type & CNK_FLAGS_MASK) == NJD_FW_END ||
            CNK_TYPE(type) != NJD_CV_VN_NF) {
            _nj_cnk_clip_vtx_ += nbvertex;
        }
        /* size counts the chunk's 4-byte words after the size halfword.
         * offset and nbvertex fill the first, which vlist has passed */
        vlist += (size * CNK_WORD_HALFWORDS) - CNK_WORD_HALFWORDS;
    }
    /* @bug uninitialized: returns type unset where vlist holds only NJD_CE.
     * the result is R12 as the caller left it (MOV R12,R0 at 0x8c128134) */
    return type;
}

Sint32 _njCnkEasyDrawModel(NJS_CNK_MODEL *model)
{
    /* @hack redundant: register on vbuf, vlist and clipnum */
    register NJS_CNK_EASY_BUF *vbuf;
    register Sint32 *vlist;
    Sint16 *plist;
    Sint32 vtype;
    /* @hack type: clipnum a Uint32, where an Sint32 would do */
    register Uint32 clipnum;

    if ((_nj_control_3d_flag_ & NJD_CONTROL_3D_MODEL_CLIP) && model->r > 0.0F &&
        njCnkModelClip(model)) {
        return -1;
    }
    vbuf = (NJS_CNK_EASY_BUF *)_nj_vertex_buf_;
    vlist = model->vlist;
#ifdef NONMATCHING
    /* a model without a vertex list leaves vtype unset, and the image passes
     * the caller's R5. this side passes NJD_CE, as an empty list does here */
    vtype = NJD_CE;
#endif
    if (vlist != NULL) {
        vtype = njCnkEasyVlist((Sint16 *)vlist, vbuf);
    }
    clipnum = _nj_cnk_clip_num_;
    plist = model->plist;
    if (plist != NULL) {
        /* @bug uninitialized: a model with no vertex list stores and passes
         * a vtype that no path set: R5 as the caller left it (MOV.L R5,@R1
         * at 0x8c1281d6) */
        _nj_cnk_easy_vtype_ = vtype;
        njCnkEasyPlist(plist, vtype, clipnum, vbuf);
    }
    return 0;
}

void _njCnkEasyDrawObject(NJS_CNK_OBJECT *object)
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
                _njCnkEasyDrawModel(object->model);
            }
            if (!(flags & NJD_EVAL_BREAK)) {
                _njCnkEasyDrawObject(object->child);
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
                clipped = _njCnkEasyDrawModel(object->model);
            }
            if (!(flags & NJD_EVAL_BREAK)) {
                /* @bug uninitialized: tests clipped for a hidden node, as
                 * nj_cnk.h says (TST R13,R13 at 0x8c128284) */
                if (!(flags & NJD_EVAL_CLIP) || !clipped) {
                    _njCnkEasyDrawObject(object->child);
                }
            }
            njPopMatrixEx();
        }
        object = object->sibling;
    } while (object);
}

void njCnkEasyDrawModel(NJS_CNK_MODEL *model)
{
    njGetkmPointer();
    _njCnkEasyDrawModel(model);
    njSetkmPointer();
}

void njCnkEasyDrawObject(NJS_CNK_OBJECT *object)
{
    njGetkmPointer();
    prefetch(object);
    _njCnkEasyDrawObject(object);
    njSetkmPointer();
}
