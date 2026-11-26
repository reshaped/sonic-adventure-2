/* the simple chunk drawer. one light gives each vertex a diffuse and a
 * specular intensity. they scale its strip's polygon and offset colors */
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
 * filled. clipnum is _nj_cnk_clip_num_ after that list. a nonzero clipnum
 * selects _njCnkFunctionClipS and _njCnkBumpFunctionClipS */
void njCnkSimplePlist(Sint16 *plist, Sint32 clipnum, NJS_CNK_SIMPLE_BUF *vbuf);
/* project each vertex chunk of vlist into vbuf by the function in its
 * type's row of _njCnkVertexFuncTableS. an NJD_CV_VN_NF chunk goes to
 * njCnkSimpleVertexVNNF instead */
void njCnkSimpleVlist(Sint16 *vlist, NJS_CNK_SIMPLE_BUF *vbuf);
/* draw object as njCnkSimpleDrawObject draws it, between calls of
 * njGetkmPointer and njSetkmPointer that the caller makes */
void _njCnkSimpleDrawObject(NJS_CNK_OBJECT *object);
/* NOLINTEND(misc-use-internal-linkage) */

/* the diffuse color that njCnkSetMaterial last wrote, as four floats */
static Float diffuse[4] = {1.0F, 1.0F, 1.0F, 1.0F};
/* four floats, all 0, that the member's data holds between diffuse and
 * specular */
/* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
static Float unk_10[4] = {0.0F, 0.0F, 0.0F, 0.0F};
/* the specular color that njCnkSetMaterial last wrote, as four floats */
static Float specular[4] = {0.0F, 0.0F, 0.0F, 0.0F};
/* whether the last vertex chunk's function lights each vertex by an
 * intensity, from the chunk's row of _njCnkVertexFuncTableS */
static Bool vertex_inten;
/* TRUE where the walker's last texture chunk set a bump texture, and the
 * walker read the texture's vectors. it carries from one polygon list into
 * the next */
static Bool bump;

void njCnkSetSimpleLight(Float x, Float y, Float z)
{
    _nj_vertex_simple.light.x = x;
    _nj_vertex_simple.light.y = y;
    _nj_vertex_simple.light.z = z;
    _nj_vertex_simple.dist_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    _nj_vertex_simple.dist_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    _nj_vertex_simple.cx = _nj_screen_.cx;
    _nj_vertex_simple.cy = _nj_screen_.cy;
}

void njCnkSetSimpleLightIntensity(Float inten, Float ambient)
{
    _nj_vertex_simple.inten = -inten;
    _nj_vertex_simple.ambient = ambient;
}

void njCnkSetSimpleLightColor(Float r, Float g, Float b)
{
    _nj_lcolor_simple.r = r;
    _nj_lcolor_simple.g = g;
    _nj_lcolor_simple.b = b;
}

void njCnkSimplePlist(Sint16 *plist, Sint32 clipnum, NJS_CNK_SIMPLE_BUF *vbuf)
{
    Sint16 head;
    /* @hack redundant: register on type, cache, size, nbstrip, vector and
     * sq */
    register Sint16 type;
    Sint16 flags;
    register Sint16 cache;
    register Sint32 size;
    register Uint32 nbstrip;
    Sint32 format;
    Uint32 *end;
    register Float *vector;
    register Uint32 *sq;
    Float strip_ambient;
    Float uvscale;
    Float color[4];

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
                njCnkSimplePlist(_nj_cnk_polygon_cache_tbl_[cache], clipnum,
                                 vbuf);
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
                 * NJD_CM_BU, as nj_cnk.h says (TST R4,R2 at 0x8c128f50) */
                if (*plist & NJD_CM_BU) {
                    plist += CNK_MATERIAL_LEAD;
                    vector = _nj_cnk_simple_bump_tex_vector_;
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
            if (flags & CNK_FST(NJD_FST_IA)) {
                strip_ambient = 0.0F;
            } else {
                strip_ambient = _nj_vertex_simple.ambient;
            }
            if (flags & CNK_FST(NJD_FST_IL)) {
                strip_ambient = 1.0F;
            }
            _nj_flat_model_flag_ = FALSE;
            end = NULL;
            if (flags & CNK_FST(NJD_FST_FL)) {
                _nj_flat_model_flag_ = TRUE;
            }
            switch (type) {
            case NJD_CS_UVN:
            case NJD_CS_UVH:
                if (bump) {
                    /* @hack redundant: this block's own sq and register
                     * uvscale, where the function's would do */
                    Uint32 *sq;
                    register Float uvscale;

                    if (flags & CNK_FST(NJD_FST_UA)) {
                        njSetkmColorMode(
                            NJD_COLOR_MODE_HIGHLIGHT_FLAT_PACKED_TEXTURE_TRANS);
                        _nj_lcolor_simple.a = diffuse[ARGB_A];
                        sq = njStartVertexTexTrans();
                    } else {
                        njSetkmColorMode(
                            NJD_COLOR_MODE_HIGHLIGHT_FLAT_PACKED_TEXTURE);
                        sq = njStartVertexTexOpaque();
                    }
                    uvscale = type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE;
                    if (clipnum) {
                        end = _njCnkBumpFunctionClipS[0](
                            plist, vbuf, nbstrip, sq, strip_ambient, uvscale);
                    } else {
                        end = _njCnkBumpFunctionS[0](plist, vbuf, nbstrip, sq,
                                                     strip_ambient, uvscale);
                    }
                } else if (vertex_inten) {
                    if (_nj_control_3d_flag_ &
                        NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL) {
                        if (flags & CNK_FST(NJD_FST_IL)) {
                            color[ARGB_A] = diffuse[ARGB_A];
                            color[ARGB_R] = 1.0F;
                            color[ARGB_G] = 1.0F;
                            color[ARGB_B] = 1.0F;
                        } else {
                            color[ARGB_A] = diffuse[ARGB_A];
                            color[ARGB_R] = _nj_lcolor_simple.r;
                            color[ARGB_G] = _nj_lcolor_simple.g;
                            color[ARGB_B] = _nj_lcolor_simple.b;
                        }
                    } else {
                        if (flags & CNK_FST(NJD_FST_IL)) {
                            color[ARGB_A] = diffuse[ARGB_A];
                            color[ARGB_R] = diffuse[ARGB_R];
                            color[ARGB_G] = diffuse[ARGB_G];
                            color[ARGB_B] = diffuse[ARGB_B];
                        } else {
                            color[ARGB_A] = diffuse[ARGB_A];
                            color[ARGB_R] =
                                diffuse[ARGB_R] * _nj_lcolor_simple.r;
                            color[ARGB_G] =
                                diffuse[ARGB_G] * _nj_lcolor_simple.g;
                            color[ARGB_B] =
                                diffuse[ARGB_B] * _nj_lcolor_simple.b;
                        }
                    }
                    njSetkmColorMode(_njCnkFunctionS[flags].inten_texture_mode);
                    if (flags & CNK_FST(NJD_FST_UA)) {
                        if (flags & CNK_FST(NJD_FST_IS)) {
                            sq = njStartVertexTexTransIntens(color);
                        } else {
                            sq = njStartVertexTexTransIntensH(color, specular);
                        }
                    } else {
                        if (flags & CNK_FST(NJD_FST_IS)) {
                            sq = njStartVertexTexOpaqueIntens(color);
                        } else {
                            sq = njStartVertexTexOpaqueIntensH(color, specular);
                        }
                    }
                    uvscale = type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE;
                    if (clipnum) {
                        end = _njCnkFunctionClipS[flags].inten_texture(
                            plist, vbuf, nbstrip, sq, strip_ambient, uvscale);
                    } else {
                        end = _njCnkFunctionS[flags].inten_texture(
                            plist, vbuf, nbstrip, sq, strip_ambient, uvscale);
                    }
                } else {
                    njSetkmColorMode(_njCnkFunctionS[flags].texture_mode);
                    if (flags & CNK_FST(NJD_FST_UA)) {
                        sq = njStartVertexTexTrans();
                    } else {
                        sq = njStartVertexTexOpaque();
                    }
                    uvscale = type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE;
                    if (clipnum) {
                        end = _njCnkFunctionClipS[flags].texture(
                            plist, vbuf, nbstrip, sq, uvscale);
                    } else {
                        end = _njCnkFunctionS[flags].texture(
                            plist, vbuf, nbstrip, sq, uvscale);
                    }
                }
                break;
            case NJD_CS:
                if (flags & CNK_FST(NJD_FST_ENV)) {
                    if (vertex_inten) {
                        if (flags & CNK_FST(NJD_FST_IL)) {
                            color[ARGB_A] = diffuse[ARGB_A];
                            color[ARGB_R] = diffuse[ARGB_R];
                            color[ARGB_G] = diffuse[ARGB_G];
                            color[ARGB_B] = diffuse[ARGB_B];
                        } else {
                            color[ARGB_A] = diffuse[ARGB_A];
                            color[ARGB_R] =
                                diffuse[ARGB_R] * _nj_lcolor_simple.r;
                            color[ARGB_G] =
                                diffuse[ARGB_G] * _nj_lcolor_simple.g;
                            color[ARGB_B] =
                                diffuse[ARGB_B] * _nj_lcolor_simple.b;
                        }
                        njSetkmColorMode(_njCnkFunctionS[flags].inten_mode);
                        if (flags & CNK_FST(NJD_FST_UA)) {
                            if (flags & CNK_FST(NJD_FST_IS)) {
                                sq = njStartVertexTexTransIntens(color);
                            } else {
                                sq = njStartVertexTexTransIntensH(color,
                                                                  specular);
                            }
                        } else {
                            if (flags & CNK_FST(NJD_FST_IS)) {
                                sq = njStartVertexTexOpaqueIntens(color);
                            } else {
                                sq = njStartVertexTexOpaqueIntensH(color,
                                                                   specular);
                            }
                        }
                        if (clipnum) {
                            end = _njCnkFunctionClipS[flags].inten_polygon(
                                plist, vbuf, nbstrip, sq, strip_ambient);
                        } else {
                            end = _njCnkFunctionS[flags].inten_polygon(
                                plist, vbuf, nbstrip, sq, strip_ambient);
                        }
                    } else {
                        njSetkmColorMode(_njCnkFunctionS[flags].mode);
                        if (flags & CNK_FST(NJD_FST_UA)) {
                            sq = njStartVertexTexTrans();
                        } else {
                            sq = njStartVertexTexOpaque();
                        }
                        if (clipnum) {
                            end = _njCnkFunctionClipS[flags].polygon(
                                plist, vbuf, nbstrip, sq);
                        } else {
                            end = _njCnkFunctionS[flags].polygon(plist, vbuf,
                                                                 nbstrip, sq);
                        }
                    }
                } else {
                    if (vertex_inten) {
                        /* @bug logic: tests NJD_FST_IA where the walker's
                         * other strips test NJD_FST_IL. a strip under
                         * NJD_FST_IA alone loses the light's color. a strip
                         * under NJD_FST_IL alone keeps the color it should
                         * lose (TST R12,R11 at 0x8c12943a) */
                        if (flags & CNK_FST(NJD_FST_IA)) {
                            color[ARGB_A] = diffuse[ARGB_A];
                            color[ARGB_R] = diffuse[ARGB_R];
                            color[ARGB_G] = diffuse[ARGB_G];
                            color[ARGB_B] = diffuse[ARGB_B];
                        } else {
                            color[ARGB_A] = diffuse[ARGB_A];
                            color[ARGB_R] =
                                diffuse[ARGB_R] * _nj_lcolor_simple.r;
                            color[ARGB_G] =
                                diffuse[ARGB_G] * _nj_lcolor_simple.g;
                            color[ARGB_B] =
                                diffuse[ARGB_B] * _nj_lcolor_simple.b;
                        }
                        njSetkmColorMode(_njCnkFunctionS[flags].inten_mode);
                        if (flags & CNK_FST(NJD_FST_UA)) {
                            sq = njStartVertexNonTexTransIntens(color);
                        } else {
                            sq = njStartVertexNonTexOpaqueIntens(color);
                        }
                        if (clipnum) {
                            end = _njCnkFunctionClipS[flags].inten_polygon(
                                plist, vbuf, nbstrip, sq, strip_ambient);
                        } else {
                            end = _njCnkFunctionS[flags].inten_polygon(
                                plist, vbuf, nbstrip, sq, strip_ambient);
                        }
                    } else {
                        njSetkmColorMode(_njCnkFunctionS[flags].mode);
                        if (flags & CNK_FST(NJD_FST_UA)) {
                            sq = njStartVertexNonTexTrans();
                        } else {
                            sq = njStartVertexNonTexOpaque();
                        }
                        if (clipnum) {
                            end = _njCnkFunctionClipS[flags].polygon(
                                plist, vbuf, nbstrip, sq);
                        } else {
                            end = _njCnkFunctionS[flags].polygon(plist, vbuf,
                                                                 nbstrip, sq);
                        }
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

void njCnkSimpleVlist(Sint16 *vlist, NJS_CNK_SIMPLE_BUF *vbuf)
{
    /* @hack redundant: register on head, size, nbvertex and type */
    register Sint32 head;
    register Sint32 size;
    Sint32 offset;
    register Sint32 nbvertex;
    register Sint32 type;
    Sint32 near;
    NJS_CNK_SIMPLE_BUF *buf;

    while ((head = *vlist++) != NJD_CE) {
        size = (Uint16)*vlist++;
        offset = *vlist++;
        nbvertex = *vlist++;
        if (!(head & NJD_FV_CONT)) {
            _nj_cnk_clip_num_ = 0;
            _nj_cnk_clip_vtx_ = 0;
        }
        head &= ~NJD_FV_CONT;
        _nj_vertex_simple.nbvertex = nbvertex;
        _nj_vertex_simple.vertices = vlist;
        buf = &vbuf[offset];
        _nj_vertex_simple.buf = buf;
        _nj_vertex_simple.sq = SQ_ADDRESS((Uint32)buf);
        SQ_MAP((Uint32)buf);
        type = CNK_TYPE(head);
        _nj_calc_vertex_ += nbvertex;
        if (type != NJD_CV_VN_NF) {
            near = _njCnkVertexFuncTableS[type - NJD_VERTOFF].func(
                &_nj_vertex_simple);
        } else {
            near = njCnkSimpleVertexVNNF(&_nj_vertex_simple, head);
        }
        vertex_inten = _njCnkVertexFuncTableS[type - NJD_VERTOFF].inten;
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
}

Sint32 _njCnkSimpleDrawModel(NJS_CNK_MODEL *model)
{
    /* @hack redundant: register on vlist */
    register Sint32 *vlist;
    NJS_CNK_SIMPLE_BUF *vbuf;
    Sint16 *plist;
    Sint32 clipnum;

    vbuf = (NJS_CNK_SIMPLE_BUF *)_nj_vertex_buf_;
    vlist = model->vlist;
    if ((_nj_control_3d_flag_ & NJD_CONTROL_3D_MODEL_CLIP) && model->r > 0.0F &&
        njCnkModelClip(model)) {
        return -1;
    }
    if (vlist) {
        njCnkSimpleVlist((Sint16 *)vlist, vbuf);
    }
    plist = model->plist;
    clipnum = _nj_cnk_clip_num_;
    if (plist) {
        njCnkSimplePlist(plist, clipnum, vbuf);
    }
    return 0;
}

void _njCnkSimpleDrawObject(NJS_CNK_OBJECT *object)
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
                _njCnkSimpleDrawModel(object->model);
            }
            if (!(flags & NJD_EVAL_BREAK)) {
                _njCnkSimpleDrawObject(object->child);
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
                clipped = _njCnkSimpleDrawModel(object->model);
            }
            if (!(flags & NJD_EVAL_BREAK)) {
                /* @bug uninitialized: tests clipped for a hidden node, as
                 * nj_cnk.h says (TST R13,R13 at 0x8c1297c4) */
                if (!(flags & NJD_EVAL_CLIP) || !clipped) {
                    _njCnkSimpleDrawObject(object->child);
                }
            }
            njPopMatrixEx();
        }
        object = object->sibling;
    } while (object);
}

void njCnkSimpleDrawModel(NJS_CNK_MODEL *model)
{
    njGetkmPointer();
    _njCnkSimpleDrawModel(model);
    njSetkmPointer();
}

void njCnkSimpleDrawObject(NJS_CNK_OBJECT *object)
{
    njGetkmPointer();
    prefetch(object);
    _njCnkSimpleDrawObject(object);
    njSetkmPointer();
}
