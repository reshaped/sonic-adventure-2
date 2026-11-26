/* the easy chunk drawer's shape link point function, along a straight line
 * between keys, and the slots' blend */
#include <nj_cnk_motlink.h>
#include <nj_cnk_shape.h>
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set the shape layout for type as njCnkInitEasyShapeData does, but in the
 * current slot's shape type alone */
void njCnkInitEasyShapeLinkData(Sint32 type);
/* NOLINTEND(misc-use-internal-linkage) */

void njCnkInitEasyShapeLinkData(Sint32 type)
{
    switch (type) {
    case NJD_CV:
        _nj_cnk_shape_nextstep_ = SHAPE_VECTOR;
        _nj_cnk_shape_v_ini_ = SHAPE_HEAD;
        _nj_cnk_shape_n_ini_ = SHAPE_NONE;
        _nj_cnk_shape_c_ini_ = SHAPE_NONE;
        _nj_motlink_shape_data_type_[_nj_mot_current_data_] &=
            (Uint16) ~(SHAPE_MTYPE_COLOR | NJD_MTYPE_NORM_5);
        break;
    case NJD_CV_D8:
        _nj_cnk_shape_nextstep_ = SHAPE_VECTOR + SHAPE_COLOR;
        _nj_cnk_shape_v_ini_ = SHAPE_HEAD;
        _nj_cnk_shape_n_ini_ = SHAPE_NONE;
        _nj_cnk_shape_c_ini_ = SHAPE_HEAD + SHAPE_VECTOR;
        _nj_motlink_shape_data_type_[_nj_mot_current_data_] &=
            (Uint16)~NJD_MTYPE_NORM_5;
        break;
    case NJD_CV_VN:
        _nj_cnk_shape_nextstep_ = 2 * SHAPE_VECTOR;
        _nj_cnk_shape_v_ini_ = SHAPE_HEAD;
        _nj_cnk_shape_n_ini_ = SHAPE_HEAD + SHAPE_VECTOR;
        _nj_cnk_shape_c_ini_ = SHAPE_NONE;
        _nj_motlink_shape_data_type_[_nj_mot_current_data_] &=
            (Uint16)~SHAPE_MTYPE_COLOR;
        break;
    default:
        _nj_cnk_shape_nextstep_ = 0;
        _nj_cnk_shape_v_ini_ = SHAPE_NONE;
        _nj_cnk_shape_n_ini_ = SHAPE_NONE;
        _nj_cnk_shape_c_ini_ = SHAPE_NONE;
        _nj_motlink_shape_data_type_[_nj_mot_current_data_] = 0;
        break;
    }
}

void njCnkEasyShapeLinkLinearPoint(NJS_CNK_OBJECT *object)
{
    Sint32 *vlist;
    /* @hack type: src, points and normals typed Sint32 *, their floats moved
     * through casts, where Float * would do */
    Sint32 *src;
    /* @hack spelling: csrc, a second cursor that holds src's value at each
     * use, where src would do */
    Sint32 *csrc;
    Sint32 *buf;
    Sint32 *points;
    Sint32 *normals;
    Sint32 *colors;
    NJS_MKEY_P *keys;
    /* @hack redundant: the last two entries of point_keys and
     * normal_keys, which nothing writes */
    void *point_keys[4];
    void *normal_keys[4];
    /* @hack redundant: eight bytes of the frame, which nothing reads */
    Uint8 unused[8];
    Float *p0;
    Float *p1;
    Float *n0;
    Float *n1;
    Float point_t;
    Float normal_t;
    Uint32 frame;
    Uint32 low;
    Uint32 high;
    Uint32 middle;
    Uint32 nb;
    Uint32 factor;
    Sint32 count;
    Sint32 found;
    Sint32 i;
    Sint32 j;
    Sint32 k;
    Sint32 offset;

#ifdef NONMATCHING
    /* GCC can't see that the locals below are set wherever they are read. p0,
     * p1 and point_t are read only where found holds SHAPE_POINTS. n0, n1 and
     * normal_t are read only where it holds SHAPE_NORMALS. colors is read only
     * where the layout gives its offset. points and normals are too, but for
     * the path the bug tag on their setting names. on that path this side
     * writes through null, where the shipped code writes through what their
     * registers held */
    p0 = NULL;
    p1 = NULL;
    n0 = NULL;
    n1 = NULL;
    points = NULL;
    normals = NULL;
    colors = NULL;
    point_t = 0.0F;
    normal_t = 0.0F;
#endif
    if (object->model == NULL) {
        return;
    }
    if (object->evalflags & NJD_EVAL_HIDE) {
        return;
    }
    vlist = object->model->vlist;
    if (vlist == NULL) {
        return;
    }
    csrc = vlist;
    _nj_cnk_shapelink_vlist_[_nj_mot_current_data_] = NULL;
    src = vlist;
    if (_nj_motlink_shape_data_cnt_[_nj_mot_current_data_]) {
        count = SHAPE_COUNT(vlist);
        if (!_nj_cnk_shape_init_flag_) {
            njCnkInitEasyShapeLinkData((Uint16)vlist[0]);
            _nj_cnk_shape_init_flag_ = TRUE;
        }
        found = 0;
        offset = SHAPE_WORDS(count) * _nj_mot_current_data_;
        buf = (Sint32 *)&_nj_shape_buf_[offset];
        buf[0] = vlist[0];
        buf[1] = vlist[1];
        /* @bug uninitialized: leaves points and normals unset where the layout
         * lacks them. for the slot that didn't run the setup, this function can
         * then write through them, as nj_cnk_motlink.h says (MOV R10,R6 at
         * 0x8c13ea50 passes the unset normals) */
        if (_nj_cnk_shape_v_ini_ != SHAPE_NONE) {
            points = buf + _nj_cnk_shape_v_ini_;
        }
        if (_nj_cnk_shape_n_ini_ != SHAPE_NONE) {
            normals = buf + _nj_cnk_shape_n_ini_;
        }
        if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
            colors = buf + _nj_cnk_shape_c_ini_;
        }
        if (_nj_motlink_shape_data_type_[_nj_mot_current_data_] &
            NJD_MTYPE_VERT_4) {
            factor = _nj_shapelink_data_info_[_nj_mot_current_data_].factor;
            keys = _nj_shapelink_data_info_[_nj_mot_current_data_].p[factor];
            if (keys != NULL) {
                nb = _nj_shapelink_data_info_[_nj_mot_current_data_].nb[factor];
                low = 0;
                high = nb;
                frame = _nj_shapelink_data_info_[_nj_mot_current_data_].frame;
                while (high - low > 1) {
                    middle = (low + high) >> 1;
                    if (frame >= keys[middle].keyframe) {
                        low = middle;
                    } else {
                        high = middle;
                    }
                }
                point_t = njCnkGetLinkLinearKFdata(keys, low, nb, point_keys);
                p0 = point_keys[0];
                p1 = point_keys[1];
                found = SHAPE_POINTS;
            }
            _nj_shapelink_data_info_[_nj_mot_current_data_].factor++;
        }
        if (_nj_motlink_shape_data_type_[_nj_mot_current_data_] &
            NJD_MTYPE_NORM_5) {
            factor = _nj_shapelink_data_info_[_nj_mot_current_data_].factor;
            keys = _nj_shapelink_data_info_[_nj_mot_current_data_].p[factor];
            if (keys != NULL) {
                nb = _nj_shapelink_data_info_[_nj_mot_current_data_].nb[factor];
                low = 0;
                high = nb;
                frame = _nj_shapelink_data_info_[_nj_mot_current_data_].frame;
                while (high - low > 1) {
                    middle = (low + high) >> 1;
                    if (frame >= keys[middle].keyframe) {
                        low = middle;
                    } else {
                        high = middle;
                    }
                }
                normal_t = njCnkGetLinkLinearKFdata(keys, low, nb, normal_keys);
                n0 = normal_keys[0];
                n1 = normal_keys[1];
                found += SHAPE_NORMALS;
            }
            _nj_shapelink_data_info_[_nj_mot_current_data_].factor++;
        }
        switch (found) {
        /* @hack spelling: the default arm first, where last would do */
        default:
            return;
        case SHAPE_POINTS:
            if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                for (i = 0; i < count; i++) {
                    j = i * SHAPE_VECTOR;
                    njCnkInterpolatesF3((NJS_POINT3 *)&p0[j],
                                        (NJS_POINT3 *)&p1[j],
                                        (NJS_POINT3 *)points, point_t);
                    points += _nj_cnk_shape_nextstep_;
                    *colors = csrc[_nj_cnk_shape_c_ini_];
                    colors += _nj_cnk_shape_nextstep_;
                    csrc += _nj_cnk_shape_nextstep_;
                }
            } else if (_nj_cnk_shape_n_ini_ != SHAPE_NONE) {
                for (i = 0; i < count; i++) {
                    j = i * SHAPE_VECTOR;
                    njCnkInterpolatesF3((NJS_POINT3 *)&p0[j],
                                        (NJS_POINT3 *)&p1[j],
                                        (NJS_POINT3 *)points, point_t);
                    points += _nj_cnk_shape_nextstep_;
                    /* @hack spelling: a vertex's second and third floats
                     * read past a pointer to its first, where src indexed
                     * would do */
                    ((Float *)normals)[0] =
                        ((Float *)src)[_nj_cnk_shape_n_ini_];
                    ((Float *)normals)[1] =
                        ((Float *)&src[_nj_cnk_shape_n_ini_])[1];
                    ((Float *)normals)[2] =
                        ((Float *)&src[_nj_cnk_shape_n_ini_])[2];
                    normals += _nj_cnk_shape_nextstep_;
                    src += _nj_cnk_shape_nextstep_;
                }
            } else {
                for (i = 0; i < count; i++) {
                    j = i * SHAPE_VECTOR;
                    njCnkInterpolatesF3((NJS_POINT3 *)&p0[j],
                                        (NJS_POINT3 *)&p1[j],
                                        (NJS_POINT3 *)points, point_t);
                    points += _nj_cnk_shape_nextstep_;
                }
            }
            break;
        case SHAPE_NORMALS:
            for (i = 0; i < count; i++) {
                /* @hack spelling: a vertex's second and third floats read
                 * past a pointer to its first, where src indexed would do */
                ((Float *)points)[0] = ((Float *)src)[_nj_cnk_shape_v_ini_];
                ((Float *)points)[1] = ((Float *)&src[_nj_cnk_shape_v_ini_])[1];
                ((Float *)points)[2] = ((Float *)&src[_nj_cnk_shape_v_ini_])[2];
                points += _nj_cnk_shape_nextstep_;
                j = i * SHAPE_VECTOR;
                njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                        (NJS_VECTOR *)&n1[j], (Float *)normals,
                                        normal_t);
                normals += _nj_cnk_shape_nextstep_;
                src += _nj_cnk_shape_nextstep_;
            }
            break;
        case SHAPE_POINTS + SHAPE_NORMALS:
            for (i = 0; i < count; i++) {
                j = i * SHAPE_VECTOR;
                njCnkInterpolatesF3((NJS_POINT3 *)&p0[j], (NJS_POINT3 *)&p1[j],
                                    (NJS_POINT3 *)points, point_t);
                points += _nj_cnk_shape_nextstep_;
                /* @hack spelling: k, which holds i * SHAPE_VECTOR as j does,
                 * where j would do */
                k = i * SHAPE_VECTOR;
                njCnkInterpolatesNormal((NJS_VECTOR *)&n0[k],
                                        (NJS_VECTOR *)&n1[k], (Float *)normals,
                                        normal_t);
                normals += _nj_cnk_shape_nextstep_;
            }
        }
        _nj_cnk_shapelink_vlist_[_nj_mot_current_data_] = buf;
        /* @hack spelling: the end chunk's word written through a pointer sum,
         * where buf[SHAPE_WORDS(count)] would do */
        *(buf + (_nj_cnk_shape_nextstep_ * count) + SHAPE_HEAD) = NJD_CE;
    }
}

void njCnkMakeEasyShapeLink(NJS_CNK_OBJECT *object)
{
    Sint32 *vlist;
    Sint32 *buf;
    Sint32 *slot0;
    Sint32 *slot1;
    Sint32 *out;
    Sint32 count;
    Sint32 type;
    Sint32 i;
    Sint32 size;

    if (object->model == NULL) {
        return;
    }
    if (object->evalflags & NJD_EVAL_HIDE) {
        return;
    }
    vlist = object->model->vlist;
    if (vlist != NULL) {
        type = (Uint16)vlist[0];
        count = SHAPE_COUNT(vlist);
        if (_nj_cnk_shapelink_vlist_[0] != NULL ||
            _nj_cnk_shapelink_vlist_[1] != NULL) {
            size = SHAPE_WORDS(count);
            /* @hack spelling: size doubled in a statement of its own, where
             * 2 * size as the index would do */
            size += size;
            buf = (Sint32 *)&_nj_shape_buf_[size];
            buf[0] = vlist[0];
            buf[1] = vlist[1];
            out = buf;
            if (_nj_cnk_shapelink_vlist_[0] != NULL) {
                slot0 = _nj_cnk_shapelink_vlist_[0];
            } else {
                slot0 = vlist;
            }
            if (_nj_cnk_shapelink_vlist_[1] != NULL) {
                slot1 = _nj_cnk_shapelink_vlist_[1];
            } else {
                slot1 = vlist;
            }
            /* @hack spelling: out stepped past the head here, where buf +
             * SHAPE_HEAD as its start would do */
            /* @hack spelling: out = out + SHAPE_HEAD, where out += SHAPE_HEAD
             * would do */
            out = out + SHAPE_HEAD;
            slot0 += SHAPE_HEAD;
            slot1 += SHAPE_HEAD;
            switch (type) {
            case NJD_CV:
                for (i = 0; i < count; i++) {
                    njCnkInterpolatesF3((NJS_POINT3 *)slot0,
                                        (NJS_POINT3 *)slot1, (NJS_POINT3 *)out,
                                        _nj_mot_current_frame_);
                    slot0 += SHAPE_VECTOR;
                    slot1 += SHAPE_VECTOR;
                    out += SHAPE_VECTOR;
                }
                break;
            case NJD_CV_D8:
                for (i = 0; i < count; i++) {
                    njCnkInterpolatesF3((NJS_POINT3 *)slot0,
                                        (NJS_POINT3 *)slot1, (NJS_POINT3 *)out,
                                        _nj_mot_current_frame_);
                    slot0 += SHAPE_VECTOR;
                    out += SHAPE_VECTOR;
                    /* @bug logic: steps slot1 past slot 1's point alone, as
                     * nj_cnk_motlink.h says (ADD #12,R12 at 0x8c13ec4c) */
                    slot1 += SHAPE_VECTOR;
                    *out++ = *slot0++;
                }
                break;
            case NJD_CV_VN:
                for (i = 0; i < count; i++) {
                    njCnkInterpolatesF3((NJS_POINT3 *)slot0,
                                        (NJS_POINT3 *)slot1, (NJS_POINT3 *)out,
                                        _nj_mot_current_frame_);
                    slot0 += SHAPE_VECTOR;
                    slot1 += SHAPE_VECTOR;
                    out += SHAPE_VECTOR;
                    njCnkInterpolatesNormal((NJS_VECTOR *)slot0,
                                            (NJS_VECTOR *)slot1, (Float *)out,
                                            _nj_mot_current_frame_);
                    slot0 += SHAPE_VECTOR;
                    slot1 += SHAPE_VECTOR;
                    out += SHAPE_VECTOR;
                }
                break;
            default:
                /* @hack spelling: a goto to the draw, where the draw in this
                 * arm would do */
                goto draw;
            }
            object->model->vlist = buf;
            /* @hack spelling: the end chunk's word written through a pointer
             * sum, where buf[SHAPE_WORDS(count)] would do */
            *(buf + (_nj_cnk_shape_nextstep_ * count) + SHAPE_HEAD) = NJD_CE;
        }
    }
draw:
    _nj_cnk_draw_model_(object->model);
    object->model->vlist = vlist;
}
