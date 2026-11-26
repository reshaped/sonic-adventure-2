/* the direct compile, which turns chunk models into the records that
 * nj_direct.h lays out, for njDirectDrawObject */
#include <kamui.h>
#include <nj_cnk.h>
#include <nj_cnk_set_material.h>
#include <nj_km.h>
#include <ninja.h>
#include <sg_xpt.h>

/* TRUE while the records take the D8 formats, and FALSE for the plain ones.
 * njCnkDirectVlist sets it from a model's vertex chunks. a model with no
 * chunk that sets it takes the formats of the model compiled before it */
/* @hack spelling: compile_d8 defined above the include of nj_direct.h, where
 * beside the member's other data would do */
static Bool compile_d8;

#include <nj_direct.h>

/* the passes of njCnkDirectPlist. the opaque pass compiles the strip chunks
 * without NJD_FST_UA, and the translucent pass those with it */
#define PASS_OPAQUE 0
#define PASS_TRANS 1

/* the head of the last model compiled. each of the four compile calls ends
 * its chain of heads there, with a null next */
static NJS_DIRECT_HEAD *last_head;
/* the member exports these functions and data, though no header of the SDK
 * declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* each global record writer writes a strip chunk's global record at ptr, and
 * returns the address after it. it counts the record in
 * _nj_direct_global_cnt_. the record takes its parameter words from
 * _nj_parameterkm_. a plain record also takes specular and the diffuse color
 * _nj_direct_col holds. the caller writes its mode and num */
void *njCnkDirectGlobalOpaqueD8(void *ptr);
void *njCnkDirectGlobalTransD8(void *ptr);
void *njCnkDirectGlobalOpaque(void *ptr);
void *njCnkDirectGlobalTrans(void *ptr);
/* compile plist's strip chunks for pass into ptr, and return the address
 * after the last record. pass is PASS_OPAQUE or PASS_TRANS. only NJD_CS,
 * NJD_CS_UVN and NJD_CS_UVH chunks compile. their strips index vbuf.
 * compile_d8 picks their formats. *global_count goes up by one for each global
 * record. each pass also reads the list's blend, texture and material chunks.
 * they set the parameter words and colors that the global records take */
void *njCnkDirectPlist(Sint16 *plist, NJS_DIRECT_BUF *vbuf, void *ptr,
                       Uint32 *global_count, Uint32 pass);
/* transform the vertices of list's vertex chunks into buf. a chunk's first
 * vertex goes to buf[n]. n sums its index offset and those of the chunks
 * before it. only NJD_CV, NJD_CV_VN, NJD_CV_D8 and NJD_CV_VN_D8 chunks write
 * vertices. a chunk of another type still adds its offset. each chunk of
 * those four types sets compile_d8, TRUE for the D8 ones. under
 * DIRECT_COMPILE_LIGHT, any chunk sets it TRUE */
void njCnkDirectVlist(Sint16 *list, NJS_DIRECT_BUF *buf);
/* compile model at ptr through the current matrix, and return the address
 * after its records. the model's head becomes last_head. a model without a
 * vertex list indexes the entries that _nj_vertex_buf_ holds already. the
 * caller sets _nj_direct_compile_mode_, and calls it between njGetkmPointer
 * and njSetkmPointer */
void *_njCnkDirectModelCompile(NJS_CNK_MODEL *model, void *ptr);
/* compile the models of object, its children and its siblings at
 * object_end, and move object_end past them. each node's model compiles
 * through the node's transform and its parents', as their evalflags give
 * them */
void _njCnkDirectObjectCompile(NJS_CNK_OBJECT *object);
/* _nj_direct_vertex_cnt_ and _nj_direct_global_cnt_ count the vertex records
 * and the global records that every compile so far has written. the member
 * never resets them */
Uint32 _nj_direct_vertex_cnt_;
Uint32 _nj_direct_global_cnt_;
/* NOLINTEND(misc-use-internal-linkage) */
static void *object_end;
Sint32 _nj_direct_compile_mode_;
NJS_DIRECT_COMPILE_LIGHT *_nj_direct_compile_light_;

Uint32 _nj_direct_culling_mode_ = NJD_POLYGON_CULLINGSMALL;
Float _nj_direct_col[4] = {1.0F, 1.0F, 1.0F, 1.0F};
/* the specular color that njCnkSetMaterial last gave any compile, 0 in each
 * channel before the first. _nj_direct_col holds the diffuse color */
static NJS_ARGB specular = {0.0F, 0.0F, 0.0F, 0.0F};

void njCnkDirectCullingMode(Uint32 mode)
{
    switch (mode) {
    case KM_CULLCCW:
        _nj_direct_culling_mode_ = NJD_POLYGON_CULLINGACW;
        break;
    case KM_CULLCW:
        _nj_direct_culling_mode_ = NJD_POLYGON_CULLINGCW;
        break;
    default:
        _nj_direct_culling_mode_ = NJD_POLYGON_CULLINGSMALL;
        break;
    }
}

void *njCnkDirectGlobalOpaqueD8(void *ptr)
{
    NJS_DIRECT_GLOBAL_D8 *global;

    global = ptr;
    /* @hack spelling: ptr set past the record before the record is written,
     * where returning global + 1 would do */
    ptr = global + 1;
    global->TEXTUREADDR = _nj_parameterkm_.texture;
    global->TSPPARAM = TSP_OPAQUE(_nj_parameterkm_.tsp);
    global->ISPPARAM =
        (_nj_parameterkm_.isp & ~ISP_CULL) | _nj_direct_culling_mode_;
    global->GLOBALPARAM = _nj_parameterkm_.global;
    global->GP = global->GLOBALPARAM;
    _nj_direct_global_cnt_++;
    return ptr;
}

void *njCnkDirectGlobalTransD8(void *ptr)
{
    NJS_DIRECT_GLOBAL_D8 *global;

    global = ptr;
    /* @hack spelling: ptr set past the record before the record is written,
     * where returning global + 1 would do */
    ptr = global + 1;
    global->TEXTUREADDR = _nj_parameterkm_.texture;
    global->TSPPARAM = _nj_parameterkm_.tsp;
    global->ISPPARAM =
        (_nj_parameterkm_.isp & ~ISP_CULL) | _nj_direct_culling_mode_;
    global->GLOBALPARAM = _nj_parameterkm_.global;
    global->GP = global->GLOBALPARAM;
    _nj_direct_global_cnt_++;
    return ptr;
}

void *njCnkDirectGlobalOpaque(void *ptr)
{
    NJS_DIRECT_GLOBAL *global;

    global = ptr;
    global->sa = specular.a;
    global->sr = specular.r;
    global->sg = specular.g;
    global->sb = specular.b;
    global->da = _nj_direct_col[ARGB_A];
    global->dr = _nj_direct_col[ARGB_R];
    global->dg = _nj_direct_col[ARGB_G];
    global->db = _nj_direct_col[ARGB_B];
    global->TEXTUREADDR = _nj_parameterkm_.texture;
    global->TSPPARAM = TSP_OPAQUE(_nj_parameterkm_.tsp);
    global->ISPPARAM =
        (_nj_parameterkm_.isp & ~ISP_CULL) | _nj_direct_culling_mode_;
    global->GLOBALPARAM = _nj_parameterkm_.global;
    global->GP = global->GLOBALPARAM;
    _nj_direct_global_cnt_++;
    /* @hack spelling: ptr stepped past the record through a cast, where
     * returning global + 1 would do */
    ptr = (NJS_DIRECT_GLOBAL *)ptr + 1;
    return ptr;
}

void *njCnkDirectGlobalTrans(void *ptr)
{
    NJS_DIRECT_GLOBAL *global;

    global = ptr;
    global->sa = specular.a;
    global->sr = specular.r;
    global->sg = specular.g;
    global->sb = specular.b;
    global->da = _nj_direct_col[ARGB_A];
    global->dr = _nj_direct_col[ARGB_R];
    global->dg = _nj_direct_col[ARGB_G];
    global->db = _nj_direct_col[ARGB_B];
    global->TEXTUREADDR = _nj_parameterkm_.texture;
    global->TSPPARAM = _nj_parameterkm_.tsp;
    global->ISPPARAM =
        (_nj_parameterkm_.isp & ~ISP_CULL) | _nj_direct_culling_mode_;
    global->GLOBALPARAM = _nj_parameterkm_.global;
    global->GP = global->GLOBALPARAM;
    _nj_direct_global_cnt_++;
    /* @hack spelling: ptr stepped past the record through a cast, where
     * returning global + 1 would do */
    ptr = (NJS_DIRECT_GLOBAL *)ptr + 1;
    return ptr;
}

void *njCnkDirectPlist(Sint16 *plist, NJS_DIRECT_BUF *vbuf, void *ptr,
                       Uint32 *global_count, Uint32 pass)
{
    Sint16 head;
    Sint16 type;
    Uint32 size;
    Uint32 strips;
    Uint32 vertices;
    Uint32 highlight;
    NJS_DIRECT_GLOBAL *global;
    NJS_DIRECT_GLOBAL_D8 *global_d8;

    while ((head = *plist++) != NJD_CE) {
        type = CNK_TYPE(head);
        if (type < NJD_TINYOFF) {
            /* @hack redundant: the cases of NJD_CB_DA and NJD_CB_EXP, which
             * do nothing */
            switch (type) {
            case NJD_CB_BA:
                njSetkmCnkBlendMode(head);
                break;
            case NJD_CB_DA:
                break;
            case NJD_CB_EXP:
                break;
            }
        } else if (type < NJD_MATOFF) {
            njSetkmCnkTextureNum(head, *plist++);
        } else if (type < NJD_STRIPOFF) {
            size = *plist++;
            njCnkSetMaterial(head, (Uint32 *)plist, _nj_direct_col, NULL,
                             &specular.a);
            plist += size;
        } else {
            size = (Uint16)*plist++;
            strips = *plist++;
            if (_nj_control_3d_flag_ & NJD_CONTROL_3D_CNK_CONSTANT_ATTR) {
                head &= _nj_constant_attr_and_;
                head |= _nj_constant_attr_or_;
            }
            /* @hack spelling: head shifted down to the strip's flags and kept
             * for them, where a local of their own would do */
            head >>= NJD_FST_SHIFT;
            if (!compile_d8) {
                /* a textured strip's polygons take an offset color for their
                 * specular light, unless the strip has NJD_FST_IS. NJD_FST_ENV
                 * gives an NJD_CS strip the texture color modes, and that
                 * offset color with them */
                highlight = NJD_ARC1_HIGHLIGHT;
                if (head & CNK_FST(NJD_FST_IS)) {
                    highlight = 0;
                }
                switch (type) {
                case NJD_CS_UVN:
                case NJD_CS_UVH:
                    if (head & CNK_FST(NJD_FST_UA)) {
                        if (pass == PASS_TRANS) {
                            (*global_count)++;
                            global = ptr;
                            if (head & CNK_FST(NJD_FST_ENV)) {
                                if (head & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        highlight |
                                        NJD_COLOR_MODE_FLAT_INTENSITY_TEXTURE_TRANS);
                                } else {
                                    njSetkmColorMode(
                                        highlight |
                                        NJD_COLOR_MODE_INTENSITY_TEXTURE_TRANS);
                                }
                                global->mode = TRUE;
                            } else {
                                if (head & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        highlight |
                                        NJD_COLOR_MODE_FLAT_INTENSITY_TEXTURE_TRANS |
                                        NJD_ARC1_UV16_FLOAT);
                                } else {
                                    njSetkmColorMode(
                                        highlight |
                                        NJD_COLOR_MODE_INTENSITY_TEXTURE_TRANS |
                                        NJD_ARC1_UV16_FLOAT);
                                }
                                global->mode = FALSE;
                            }
                            ptr = njCnkDirectGlobalTrans(ptr);
                            ptr = njCnkDirectTexture(
                                plist, vbuf, strips, ptr,
                                type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE,
                                &vertices);
                            global->num = vertices;
                            _nj_direct_vertex_cnt_ += vertices;
                        }
                    } else {
                        if (pass == PASS_OPAQUE) {
                            (*global_count)++;
                            global = ptr;
                            if (head & CNK_FST(NJD_FST_ENV)) {
                                if (head & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        highlight |
                                        NJD_COLOR_MODE_FLAT_INTENSITY_TEXTURE);
                                } else {
                                    njSetkmColorMode(
                                        highlight |
                                        NJD_COLOR_MODE_INTENSITY_TEXTURE);
                                }
                                global->mode = TRUE;
                            } else {
                                if (head & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        highlight |
                                        NJD_COLOR_MODE_FLAT_INTENSITY_TEXTURE |
                                        NJD_ARC1_UV16_FLOAT);
                                } else {
                                    njSetkmColorMode(
                                        highlight |
                                        NJD_COLOR_MODE_INTENSITY_TEXTURE |
                                        NJD_ARC1_UV16_FLOAT);
                                }
                                global->mode = FALSE;
                            }
                            ptr = njCnkDirectGlobalOpaque(ptr);
                            ptr = njCnkDirectTexture(
                                plist, vbuf, strips, ptr,
                                type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE,
                                &vertices);
                            global->num = vertices;
                            _nj_direct_vertex_cnt_ += vertices;
                        }
                    }
                    break;
                case NJD_CS:
                    if (head & CNK_FST(NJD_FST_UA)) {
                        if (pass == PASS_TRANS) {
                            (*global_count)++;
                            global = ptr;
                            if (head & CNK_FST(NJD_FST_ENV)) {
                                if (head & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        highlight |
                                        NJD_COLOR_MODE_FLAT_INTENSITY_TEXTURE_TRANS);
                                } else {
                                    njSetkmColorMode(
                                        highlight |
                                        NJD_COLOR_MODE_INTENSITY_TEXTURE_TRANS);
                                }
                                global->mode = TRUE;
                            } else {
                                if (head & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_FLAT_INTENSITY_NORMAL_TRANS);
                                } else {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_INTENSITY_NORMAL_TRANS);
                                }
                                global->mode = FALSE;
                            }
                            ptr = njCnkDirectGlobalTrans(ptr);
                            ptr = njCnkDirectPolygon(plist, vbuf, strips, ptr,
                                                     &vertices);
                            global->num = vertices;
                            _nj_direct_vertex_cnt_ += vertices;
                        }
                    } else {
                        if (pass == PASS_OPAQUE) {
                            (*global_count)++;
                            global = ptr;
                            if (head & CNK_FST(NJD_FST_ENV)) {
                                if (head & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        highlight |
                                        NJD_COLOR_MODE_FLAT_INTENSITY_TEXTURE);
                                } else {
                                    njSetkmColorMode(
                                        highlight |
                                        NJD_COLOR_MODE_INTENSITY_TEXTURE);
                                }
                                global->mode = TRUE;
                            } else {
                                if (head & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_FLAT_INTENSITY_NORMAL);
                                } else {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_INTENSITY_NORMAL);
                                }
                                global->mode = FALSE;
                            }
                            ptr = njCnkDirectGlobalOpaque(ptr);
                            ptr = njCnkDirectPolygon(plist, vbuf, strips, ptr,
                                                     &vertices);
                            global->num = vertices;
                            _nj_direct_vertex_cnt_ += vertices;
                        }
                    }
                    break;
                }
            } else {
                switch (type) {
                case NJD_CS_UVN:
                case NJD_CS_UVH:
                    if (head & CNK_FST(NJD_FST_UA)) {
                        if (pass == PASS_TRANS) {
                            (*global_count)++;
                            global_d8 = ptr;
                            if (head & CNK_FST(NJD_FST_FL)) {
                                njSetkmColorMode(
                                    NJD_COLOR_MODE_FLAT_PACKED_TEXTURE_TRANS |
                                    NJD_ARC1_UV16_FLOAT);
                            } else {
                                njSetkmColorMode(
                                    NJD_COLOR_MODE_PACKED_TEXTURE_TRANS |
                                    NJD_ARC1_UV16_FLOAT);
                            }
                            ptr = njCnkDirectGlobalTransD8(ptr);
                            if (head & CNK_FST(NJD_FST_ENV)) {
                                ptr = njCnkDirectTextureEnvD8(
                                    plist, vbuf, strips, ptr, &vertices);
                                global_d8->mode = TRUE;
                            } else {
                                ptr = njCnkDirectTextureD8(
                                    plist, vbuf, strips, ptr,
                                    type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE,
                                    &vertices);
                                global_d8->mode = FALSE;
                            }
                            global_d8->num = vertices;
                            _nj_direct_vertex_cnt_ += vertices;
                        }
                    } else {
                        if (pass == PASS_OPAQUE) {
                            (*global_count)++;
                            global_d8 = ptr;
                            if (head & CNK_FST(NJD_FST_FL)) {
                                njSetkmColorMode(
                                    NJD_COLOR_MODE_FLAT_PACKED_TEXTURE |
                                    NJD_ARC1_UV16_FLOAT);
                            } else {
                                njSetkmColorMode(NJD_COLOR_MODE_PACKED_TEXTURE |
                                                 NJD_ARC1_UV16_FLOAT);
                            }
                            ptr = njCnkDirectGlobalOpaqueD8(ptr);
                            if (head & CNK_FST(NJD_FST_ENV)) {
                                ptr = njCnkDirectTextureEnvD8(
                                    plist, vbuf, strips, ptr, &vertices);
                                global_d8->mode = TRUE;
                            } else {
                                ptr = njCnkDirectTextureD8(
                                    plist, vbuf, strips, ptr,
                                    type == NJD_CS_UVN ? UVN_SCALE : UVH_SCALE,
                                    &vertices);
                                global_d8->mode = FALSE;
                            }
                            global_d8->num = vertices;
                            _nj_direct_vertex_cnt_ += vertices;
                        }
                    }
                    break;
                case NJD_CS:
                    if (head & CNK_FST(NJD_FST_ENV)) {
                        if (head & CNK_FST(NJD_FST_UA)) {
                            if (pass == PASS_TRANS) {
                                (*global_count)++;
                                global_d8 = ptr;
                                if (head & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_FLAT_PACKED_TEXTURE_TRANS |
                                        NJD_ARC1_UV16_FLOAT);
                                } else {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_PACKED_TEXTURE_TRANS |
                                        NJD_ARC1_UV16_FLOAT);
                                }
                                ptr = njCnkDirectGlobalTransD8(ptr);
                                ptr = njCnkDirectPolygonEnvD8(
                                    plist, vbuf, strips, ptr, &vertices);
                                global_d8->mode = TRUE;
                                global_d8->num = vertices;
                                _nj_direct_vertex_cnt_ += vertices;
                            }
                        } else {
                            if (pass == PASS_OPAQUE) {
                                (*global_count)++;
                                global_d8 = ptr;
                                if (head & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_FLAT_PACKED_TEXTURE |
                                        NJD_ARC1_UV16_FLOAT);
                                } else {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_PACKED_TEXTURE |
                                        NJD_ARC1_UV16_FLOAT);
                                }
                                ptr = njCnkDirectGlobalOpaqueD8(ptr);
                                ptr = njCnkDirectPolygonEnvD8(
                                    plist, vbuf, strips, ptr, &vertices);
                                global_d8->mode = TRUE;
                                global_d8->num = vertices;
                                _nj_direct_vertex_cnt_ += vertices;
                            }
                        }
                    } else {
                        if (head & CNK_FST(NJD_FST_UA)) {
                            if (pass == PASS_TRANS) {
                                (*global_count)++;
                                global_d8 = ptr;
                                if (head & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_FLAT_PACKED_NORMAL_TRANS);
                                } else {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_PACKED_NORMAL_TRANS);
                                }
                                global_d8->mode = FALSE;
                                ptr = njCnkDirectGlobalTransD8(ptr);
                                ptr = njCnkDirectPolygonD8(plist, vbuf, strips,
                                                           ptr, &vertices);
                                global_d8->num = vertices;
                                _nj_direct_vertex_cnt_ += vertices;
                            }
                        } else {
                            if (pass == PASS_OPAQUE) {
                                (*global_count)++;
                                global_d8 = ptr;
                                if (head & CNK_FST(NJD_FST_FL)) {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_FLAT_PACKED_NORMAL);
                                } else {
                                    njSetkmColorMode(
                                        NJD_COLOR_MODE_PACKED_NORMAL);
                                }
                                global_d8->mode = FALSE;
                                ptr = njCnkDirectGlobalOpaqueD8(ptr);
                                ptr = njCnkDirectPolygonD8(plist, vbuf, strips,
                                                           ptr, &vertices);
                                global_d8->num = vertices;
                                _nj_direct_vertex_cnt_ += vertices;
                            }
                        }
                    }
                    break;
                }
            }
            /* size counts the chunk's halfwords after the size halfword.
             * plist has passed the first, which holds strips */
            plist += size - 1;
        }
    }
    return ptr;
}

/* @hack redundant: register on list and buf */
void njCnkDirectVlist(register Sint16 *list, register NJS_DIRECT_BUF *buf)
{
    Int head;
    Int type;
    Sint16 *vlist;
    NJS_DIRECT_BUF *vbuf;
    Int size;
    /* @hack redundant: register on offset */
    register Int offset;
    /* @hack type: count, a Uint32, where Int, the type the vertex compilers
     * take, would do */
    Uint32 count;

    /* @hack spelling: vlist and vbuf, copies of list and buf, where the
     * parameters themselves would do */
    vlist = list;
    vbuf = buf;
    while ((head = *vlist++) != NJD_CE) {
        /* @hack spelling: type, a copy of head that the switch tests, where
         * head itself would do */
        type = head;
        /* size counts the chunk's 4-byte words after the size halfword.
         * vlist has passed the first, which holds offset and count */
        size = (Uint16)*vlist++;
        offset = *vlist++;
        count = *vlist++;
        /* @bug logic: adds each chunk's index offset to those of the chunks
         * before it, where the offset counts from buf itself. a chunk after
         * one with a nonzero offset lands past the entries its strips index
         * (ADD R4,R10 at 0x8c1269e2) */
        vbuf += offset;
        switch (CNK_TYPE(type)) {
        case NJD_CV_VN:
            njCnkDirectVertexVN((Float *)vlist, vbuf, count);
            compile_d8 = FALSE;
            break;
        case NJD_CV:
            njCnkDirectVertexV((Float *)vlist, vbuf, count);
            compile_d8 = FALSE;
            break;
        case NJD_CV_D8:
            njCnkDirectVertexVD8((Float *)vlist, vbuf, count);
            compile_d8 = TRUE;
            break;
        case NJD_CV_VN_D8:
            njCnkDirectVertexVND8((Float *)vlist, vbuf, count);
            compile_d8 = TRUE;
            break;
        }
        if (_nj_direct_compile_mode_ == DIRECT_COMPILE_LIGHT) {
            compile_d8 = TRUE;
        }
        vlist += (size * CNK_WORD_HALFWORDS) - CNK_WORD_HALFWORDS;
    }
}

void *_njCnkDirectModelCompile(NJS_CNK_MODEL *model, void *ptr)
{
    /* @hack redundant: register on head, vbuf, plist and end */
    register NJS_DIRECT_HEAD *head;
    Uint32 opaque;
    Uint32 trans;
    register NJS_DIRECT_BUF *vbuf;
    Sint32 *vlist;
    register Sint16 *plist;
    register void *end;
    void *records;

    opaque = 0;
    trans = 0;
    head = ptr;
    last_head = head;
    njCalcPoint(NULL, &model->center, &head->center);
    head->r = model->r;
    /* @hack spelling: records, a local that end is set from, where end set
     * directly would do */
    /* @hack spelling: ptr stepped past the head through a cast, where head + 1
     * would do */
    records = (NJS_DIRECT_HEAD *)ptr + 1;
    end = records;
    vbuf = (NJS_DIRECT_BUF *)_nj_vertex_buf_;
    vlist = model->vlist;
    if (vlist != NULL) {
        njCnkDirectVlist((Sint16 *)vlist, vbuf);
    }
    plist = model->plist;
    if (plist != NULL) {
        /* @hack spelling: ptr, the parameter, reused for the end of the
         * opaque pass, where end would do */
        ptr = njCnkDirectPlist(plist, vbuf, end, &opaque, PASS_OPAQUE);
        end = njCnkDirectPlist(plist, vbuf, ptr, &trans, PASS_TRANS);
    }
    head->next = end;
    head->nOP = opaque;
    head->nTR = trans;
    head->mode = compile_d8;
    return end;
}

void _njCnkDirectObjectCompile(NJS_CNK_OBJECT *object)
{
    /* @hack redundant: register on flags and zxy */
    register Uint32 flags;
    /* @hack spelling: zxy, a local for the rotation order, where the
     * expression passed to njRotateEx would do */
    register Sint32 zxy;

    do {
        flags = object->evalflags;
        if ((flags & CNK_EVAL_UNIT) == CNK_EVAL_UNIT) {
            if (!(flags & NJD_EVAL_HIDE)) {
                object_end =
                    _njCnkDirectModelCompile(object->model, object_end);
            }
            if (!(flags & NJD_EVAL_BREAK)) {
                _njCnkDirectObjectCompile(object->child);
            }
        } else {
            njPushMatrixEx();
            if (!(flags & NJD_EVAL_UNIT_POS)) {
                njTranslateEx((NJS_VECTOR *)object->pos);
            }
            if (!(flags & NJD_EVAL_UNIT_ANG)) {
                zxy = flags & NJD_EVAL_ZXY_ANG;
                njRotateEx(object->ang, zxy);
            }
            if (!(flags & NJD_EVAL_UNIT_SCL)) {
                njScaleEx((NJS_VECTOR *)object->scl);
            }
            if (!(flags & NJD_EVAL_HIDE)) {
                object_end =
                    _njCnkDirectModelCompile(object->model, object_end);
            }
            if (!(flags & NJD_EVAL_BREAK)) {
                _njCnkDirectObjectCompile(object->child);
            }
            njPopMatrixEx();
        }
        object = object->sibling;
    } while (object != NULL);
}

void *njCnkDirectModelCompile(NJS_CNK_MODEL *model, void *buf)
{
    void *end;

    njGetkmPointer();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    _nj_direct_compile_mode_ = DIRECT_COMPILE_PLAIN;
    end = _njCnkDirectModelCompile(model, buf);
    njPopMatrixEx();
    last_head->next = NULL;
    njSetkmPointer();
    return end;
}

void *njCnkDirectObjectCompile(NJS_CNK_OBJECT *object, void *buf)
{
    njGetkmPointer();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    _nj_direct_compile_mode_ = DIRECT_COMPILE_PLAIN;
    object_end = buf;
    _njCnkDirectObjectCompile(object);
    njPopMatrixEx();
    /* @bug memory: writes null through last_head where no model compiled.
     * last_head is then the head an earlier compile wrote last, whose buffer
     * may be gone, or null before any compile (MOV.L R1,@(16,R2) at
     * 0x8c126c22) */
#ifdef NONMATCHING
    /* this side repairs the tag above: a compile that left object_end at buf
     * wrote no head, and writes through none */
    if (object_end != buf) {
        last_head->next = NULL;
    }
#else
    last_head->next = NULL;
#endif
    njSetkmPointer();
    return object_end;
}

void *njCnkDirectModelCompileLight(NJS_CNK_MODEL *model, void *buf,
                                   NJS_DIRECT_COMPILE_LIGHT *light)
{
    void *end;

    njGetkmPointer();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    _nj_direct_compile_mode_ = DIRECT_COMPILE_LIGHT;
    _nj_direct_compile_light_ = light;
    end = _njCnkDirectModelCompile(model, buf);
    njPopMatrixEx();
    last_head->next = NULL;
    njSetkmPointer();
    return end;
}

void *njCnkDirectObjectCompileLight(NJS_CNK_OBJECT *object, void *buf,
                                    NJS_DIRECT_COMPILE_LIGHT *light)
{
    njGetkmPointer();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    _nj_direct_compile_mode_ = DIRECT_COMPILE_LIGHT;
    _nj_direct_compile_light_ = light;
    object_end = buf;
    _njCnkDirectObjectCompile(object);
    njPopMatrixEx();
    /* @bug memory: writes null through last_head where no model compiled, as
     * njCnkDirectObjectCompile does (MOV.L R1,@(16,R3) at 0x8c126cbc) */
#ifdef NONMATCHING
    /* this side repairs the tag above: a compile that left object_end at buf
     * wrote no head, and writes through none */
    if (object_end != buf) {
        last_head->next = NULL;
    }
#else
    last_head->next = NULL;
#endif
    njSetkmPointer();
    return object_end;
}
