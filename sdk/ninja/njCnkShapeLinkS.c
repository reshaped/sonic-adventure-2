/* the simple chunk drawer's shape link point functions and the slots' blend */
#include <nj_cnk_motlink.h>
#include <nj_cnk_shape.h>
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the member exports these functions and data, though no header of the SDK
 * declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set the shape layout for type as njCnkInitSimpleShapeData does, but in the
 * current slot's shape type alone */
void njCnkInitSimpleShapeLinkData(Sint32 type);
/* the current slot's point functions. each writes a copy of the model's first
 * vertex chunk to the slot's place in _nj_shape_buf_. it moves the copy's
 * vertices by the slot's shape motion. it points the slot's
 * _nj_cnk_shapelink_vlist_ at the copy, or at null where the motion moves no
 * vertex. a null model leaves that pointer as it was. so does a node under
 * NJD_EVAL_HIDE, and so does a model without a vertex list.
 *
 * njCnkSimpleShapeLinkSplinePoint moves the points along
 * njCnkInterpolatesSF3's curve between two keys.
 * njCnkSimpleShapeLinkLinearPoint moves them in a straight line between the
 * same two. on a chunk of points alone it calls njCnkInterpolatesSF3 instead,
 * as its bug tag says. both move the normals in a straight line by
 * njCnkInterpolatesNormal */
void njCnkSimpleShapeLinkSplinePoint(NJS_CNK_OBJECT *object);
void njCnkSimpleShapeLinkLinearPoint(NJS_CNK_OBJECT *object);

void (*_nj_cnk_s_shapelink_int_func_table[INTERPOLATIONS])(
    NJS_CNK_OBJECT *object) = {
    njCnkSimpleShapeLinkLinearPoint,
    njCnkSimpleShapeLinkSplinePoint,
};
/* NOLINTEND(misc-use-internal-linkage) */

void njCnkInitSimpleShapeLinkData(Sint32 type)
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
    case NJD_CV_VN_D8:
        _nj_cnk_shape_nextstep_ = (2 * SHAPE_VECTOR) + SHAPE_COLOR;
        _nj_cnk_shape_v_ini_ = SHAPE_HEAD;
        _nj_cnk_shape_n_ini_ = SHAPE_HEAD + SHAPE_VECTOR;
        _nj_cnk_shape_c_ini_ = SHAPE_HEAD + (2 * SHAPE_VECTOR);
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

void njCnkSimpleShapeLinkSplinePoint(NJS_CNK_OBJECT *object)
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
    void *point_keys[4];
    /* @hack redundant: normal_keys's last two entries, which nothing
     * writes */
    void *normal_keys[4];
    /* @hack redundant: eight bytes of the frame, which nothing reads */
    Uint8 unused[8];
    Float *p0;
    Float *p1;
    Float *p2;
    Float *p3;
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
    Sint32 m;
    Sint32 offset;

#ifdef NONMATCHING
    /* GCC can't see that the locals below are set wherever they are read. p0
     * to p3 and point_t are read only where found holds SHAPE_POINTS. n0, n1
     * and normal_t are read only where it holds SHAPE_NORMALS. colors, points
     * and normals are read only where the layout gives their offsets, once
     * this side's repair under the bug tag on their setting has run */
    p0 = NULL;
    p1 = NULL;
    p2 = NULL;
    p3 = NULL;
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
    csrc = src = vlist;
    _nj_cnk_shapelink_vlist_[_nj_mot_current_data_] = NULL;
    if (_nj_motlink_shape_data_cnt_[_nj_mot_current_data_]) {
        count = SHAPE_COUNT(vlist);
        if (!_nj_cnk_shape_init_flag_) {
            njCnkInitSimpleShapeLinkData((Uint16)vlist[0]);
            _nj_cnk_shape_init_flag_ = TRUE;
        }
        found = 0;
        offset = SHAPE_WORDS(count) * _nj_mot_current_data_;
        buf = (Sint32 *)&_nj_shape_buf_[offset];
        buf[0] = vlist[0];
        buf[1] = vlist[1];
        /* @bug uninitialized: leaves points and normals unset where the layout
         * lacks them. for the slot that didn't run the setup, this function can
         * then write through them, as nj_cnk_motlink.h says (MOV R11,R6 at
         * 0x8c124702 passes the unset normals) */
#ifdef NONMATCHING
        /* this side repairs the tag above: the slot's shape type drops the
         * normals where the layout has none, and every bit where it has no
         * points, as njCnkInitSimpleShapeLinkData drops them for the slot it
         * runs for */
        if (_nj_cnk_shape_v_ini_ == SHAPE_NONE) {
            _nj_motlink_shape_data_type_[_nj_mot_current_data_] = 0;
        } else if (_nj_cnk_shape_n_ini_ == SHAPE_NONE) {
            _nj_motlink_shape_data_type_[_nj_mot_current_data_] &=
                (Uint16)~NJD_MTYPE_NORM_5;
        }
#endif
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
                point_t = njCnkGetLinkSplineKFdata(keys, low, nb, point_keys);
                p0 = point_keys[0];
                p1 = point_keys[1];
                p2 = point_keys[2];
                p3 = point_keys[3];
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
                if (_nj_cnk_shape_n_ini_ != SHAPE_NONE) {
                    for (i = 0; i < count; i++) {
                        /* @hack spelling: j set in the call's statement, by
                         * a comma, where a statement of its own would do */
                        j = i * SHAPE_VECTOR,
                        njCnkInterpolatesSF3(
                            (NJS_POINT3 *)&p0[j], (NJS_POINT3 *)&p1[j],
                            (NJS_POINT3 *)&p2[j], (NJS_POINT3 *)&p3[j], point_t,
                            (NJS_POINT3 *)points);
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
                        *colors = csrc[_nj_cnk_shape_c_ini_];
                        colors += _nj_cnk_shape_nextstep_;
                        src += _nj_cnk_shape_nextstep_;
                        csrc += _nj_cnk_shape_nextstep_;
                    }
                } else {
                    for (i = 0; i < count; i++) {
                        j = i * SHAPE_VECTOR;
                        njCnkInterpolatesSF3(
                            (NJS_POINT3 *)&p0[j], (NJS_POINT3 *)&p1[j],
                            (NJS_POINT3 *)&p2[j], (NJS_POINT3 *)&p3[j], point_t,
                            (NJS_POINT3 *)points);
                        points += _nj_cnk_shape_nextstep_;
                        *colors = csrc[_nj_cnk_shape_c_ini_];
                        colors += _nj_cnk_shape_nextstep_;
                        csrc += _nj_cnk_shape_nextstep_;
                    }
                }
            } else {
                if (_nj_cnk_shape_n_ini_ != SHAPE_NONE) {
                    for (i = 0; i < count; i++) {
                        /* @hack spelling: j set in the call's statement, by
                         * a comma, where a statement of its own would do */
                        j = i * SHAPE_VECTOR,
                        njCnkInterpolatesSF3(
                            (NJS_POINT3 *)&p0[j], (NJS_POINT3 *)&p1[j],
                            (NJS_POINT3 *)&p2[j], (NJS_POINT3 *)&p3[j], point_t,
                            (NJS_POINT3 *)points);
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
                    /* @hack spelling: m, stepped by SHAPE_VECTOR beside i,
                     * where i * SHAPE_VECTOR would do */
                    for (i = 0, m = 0; i < count; i++, m += SHAPE_VECTOR) {
                        j = m;
                        njCnkInterpolatesSF3(
                            (NJS_POINT3 *)&p0[j], (NJS_POINT3 *)&p1[j],
                            (NJS_POINT3 *)&p2[j], (NJS_POINT3 *)&p3[j], point_t,
                            (NJS_POINT3 *)points);
                        points += _nj_cnk_shape_nextstep_;
                    }
                }
            }
            break;
        case SHAPE_NORMALS:
            if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                /* @hack spelling: m, stepped by SHAPE_VECTOR beside i, where
                 * i * SHAPE_VECTOR would do */
                for (i = 0, m = 0; i < count; i++, m += SHAPE_VECTOR) {
                    /* @hack spelling: a vertex's second and third floats read
                     * past a pointer to its first, where src indexed would
                     * do */
                    ((Float *)points)[0] = ((Float *)src)[_nj_cnk_shape_v_ini_];
                    ((Float *)points)[1] =
                        ((Float *)&src[_nj_cnk_shape_v_ini_])[1];
                    ((Float *)points)[2] =
                        ((Float *)&src[_nj_cnk_shape_v_ini_])[2];
                    points += _nj_cnk_shape_nextstep_;
                    j = m;
                    njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                            (NJS_VECTOR *)&n1[j],
                                            (Float *)normals, normal_t);
                    normals += _nj_cnk_shape_nextstep_;
                    *colors = csrc[_nj_cnk_shape_c_ini_];
                    colors += _nj_cnk_shape_nextstep_;
                    src += _nj_cnk_shape_nextstep_;
                    csrc += _nj_cnk_shape_nextstep_;
                }
            } else {
                for (i = 0; i < count; i++) {
                    /* @hack spelling: a vertex's second and third floats read
                     * past a pointer to its first, where src indexed would
                     * do */
                    ((Float *)points)[0] = ((Float *)src)[_nj_cnk_shape_v_ini_];
                    ((Float *)points)[1] =
                        ((Float *)&src[_nj_cnk_shape_v_ini_])[1];
                    ((Float *)points)[2] =
                        ((Float *)&src[_nj_cnk_shape_v_ini_])[2];
                    points += _nj_cnk_shape_nextstep_;
                    j = i * SHAPE_VECTOR;
                    njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                            (NJS_VECTOR *)&n1[j],
                                            (Float *)normals, normal_t);
                    normals += _nj_cnk_shape_nextstep_;
                    src += _nj_cnk_shape_nextstep_;
                }
            }
            break;
        case SHAPE_POINTS + SHAPE_NORMALS:
            if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                for (i = 0; i < count; i++) {
                    j = i * SHAPE_VECTOR;
                    njCnkInterpolatesSF3(
                        (NJS_POINT3 *)&p0[j], (NJS_POINT3 *)&p1[j],
                        (NJS_POINT3 *)&p2[j], (NJS_POINT3 *)&p3[j], point_t,
                        (NJS_POINT3 *)points);
                    points += _nj_cnk_shape_nextstep_;
                    /* @hack spelling: k, which holds i * SHAPE_VECTOR as j
                     * does, where j would do */
                    k = i * SHAPE_VECTOR;
                    njCnkInterpolatesNormal((NJS_VECTOR *)&n0[k],
                                            (NJS_VECTOR *)&n1[k],
                                            (Float *)normals, normal_t);
                    normals += _nj_cnk_shape_nextstep_;
                    *colors = csrc[_nj_cnk_shape_c_ini_];
                    colors += _nj_cnk_shape_nextstep_;
                    csrc += _nj_cnk_shape_nextstep_;
                }
            } else {
                /* @hack spelling: m, stepped by SHAPE_VECTOR beside i, where
                 * i * SHAPE_VECTOR would do */
                for (i = 0, m = 0; i < count; i++, m += SHAPE_VECTOR) {
                    j = m;
                    njCnkInterpolatesSF3(
                        (NJS_POINT3 *)&p0[j], (NJS_POINT3 *)&p1[j],
                        (NJS_POINT3 *)&p2[j], (NJS_POINT3 *)&p3[j], point_t,
                        (NJS_POINT3 *)points);
                    points += _nj_cnk_shape_nextstep_;
                    /* @hack spelling: k, which holds m as j does, where j
                     * would do */
                    k = m;
                    njCnkInterpolatesNormal((NJS_VECTOR *)&n0[k],
                                            (NJS_VECTOR *)&n1[k],
                                            (Float *)normals, normal_t);
                    normals += _nj_cnk_shape_nextstep_;
                }
            }
        }
        _nj_cnk_shapelink_vlist_[_nj_mot_current_data_] = buf;
        /* @hack spelling: the end chunk's word written through a pointer sum,
         * where buf[SHAPE_WORDS(count)] would do */
        *(buf + (_nj_cnk_shape_nextstep_ * count) + SHAPE_HEAD) = NJD_CE;
    }
}

void njCnkSimpleShapeLinkLinearPoint(NJS_CNK_OBJECT *object)
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
    Float *p2;
    Float *p3;
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
     * normal_t are read only where it holds SHAPE_NORMALS. colors, points and
     * normals are read only where the layout gives their offsets, once this
     * side's repair under the bug tag on their setting has run */
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
    csrc = src = vlist;
    _nj_cnk_shapelink_vlist_[_nj_mot_current_data_] = NULL;
    if (_nj_motlink_shape_data_cnt_[_nj_mot_current_data_]) {
        count = SHAPE_COUNT(vlist);
        if (!_nj_cnk_shape_init_flag_) {
            njCnkInitSimpleShapeLinkData((Uint16)vlist[0]);
            _nj_cnk_shape_init_flag_ = TRUE;
        }
        found = 0;
        offset = SHAPE_WORDS(count) * _nj_mot_current_data_;
        buf = (Sint32 *)&_nj_shape_buf_[offset];
        buf[0] = vlist[0];
        buf[1] = vlist[1];
        /* @bug uninitialized: leaves points and normals unset where the layout
         * lacks them, as njCnkSimpleShapeLinkSplinePoint does (MOV R12,R6 at
         * 0x8c124d44 passes the unset normals) */
#ifdef NONMATCHING
        /* this side repairs the tag above: the slot's shape type drops the
         * normals where the layout has none, and every bit where it has no
         * points, as njCnkInitSimpleShapeLinkData drops them for the slot it
         * runs for */
        if (_nj_cnk_shape_v_ini_ == SHAPE_NONE) {
            _nj_motlink_shape_data_type_[_nj_mot_current_data_] = 0;
        } else if (_nj_cnk_shape_n_ini_ == SHAPE_NONE) {
            _nj_motlink_shape_data_type_[_nj_mot_current_data_] &=
                (Uint16)~NJD_MTYPE_NORM_5;
        }
#endif
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
            /* @hack spelling: factor set inside the subscript, where a
             * statement of its own would do */
            keys =
                _nj_shapelink_data_info_[_nj_mot_current_data_]
                    .p[factor = _nj_shapelink_data_info_[_nj_mot_current_data_]
                                    .factor];
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
                if (_nj_cnk_shape_n_ini_ != SHAPE_NONE) {
                    for (i = 0; i < count; i++) {
                        /* @hack spelling: j set in the call's statement, by
                         * a comma, where a statement of its own would do */
                        j = i * SHAPE_VECTOR,
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
                        *colors = csrc[_nj_cnk_shape_c_ini_];
                        colors += _nj_cnk_shape_nextstep_;
                        src += _nj_cnk_shape_nextstep_;
                        csrc += _nj_cnk_shape_nextstep_;
                    }
                } else {
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
                }
            } else {
                if (_nj_cnk_shape_n_ini_ != SHAPE_NONE) {
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
                        /* @bug uninitialized: moves a chunk of points alone
                         * along njCnkInterpolatesSF3's curve, whose last two
                         * points, p2 and p3, no path set. each other arm
                         * blends p0 and p1 in a straight line. SHC's code
                         * takes the stack pointer for both, so the curve reads
                         * words of the frame (ADD R15,R7 at 0x8c124d06 and ADD
                         * R15,R6 at 0x8c124d0a, before JSR @R3 at
                         * 0x8c124d0e) */
#ifdef NONMATCHING
                        /* this side repairs the defect the tag above names.
                         * it blends p0 and p1 in a straight line, as each
                         * other arm does */
                        njCnkInterpolatesF3((NJS_POINT3 *)&p0[j],
                                            (NJS_POINT3 *)&p1[j],
                                            (NJS_POINT3 *)points, point_t);
#else
                        /* NOLINTBEGIN(clang-diagnostic-uninitialized) */
                        njCnkInterpolatesSF3(
                            (NJS_POINT3 *)&p0[j], (NJS_POINT3 *)&p1[j],
                            (NJS_POINT3 *)&p2[j], (NJS_POINT3 *)&p3[j], point_t,
                            (NJS_POINT3 *)points);
                        /* NOLINTEND(clang-diagnostic-uninitialized) */
#endif
                        points += _nj_cnk_shape_nextstep_;
                    }
                }
            }
            break;
        case SHAPE_NORMALS:
            if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                for (i = 0; i < count; i++) {
                    /* @hack spelling: a vertex's second and third floats read
                     * past a pointer to its first, where src indexed would
                     * do */
                    ((Float *)points)[0] = ((Float *)src)[_nj_cnk_shape_v_ini_];
                    ((Float *)points)[1] =
                        ((Float *)&src[_nj_cnk_shape_v_ini_])[1];
                    ((Float *)points)[2] =
                        ((Float *)&src[_nj_cnk_shape_v_ini_])[2];
                    points += _nj_cnk_shape_nextstep_;
                    j = i * SHAPE_VECTOR;
                    njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                            (NJS_VECTOR *)&n1[j],
                                            (Float *)normals, normal_t);
                    normals += _nj_cnk_shape_nextstep_;
                    *colors = csrc[_nj_cnk_shape_c_ini_];
                    colors += _nj_cnk_shape_nextstep_;
                    src += _nj_cnk_shape_nextstep_;
                    csrc += _nj_cnk_shape_nextstep_;
                }
            } else {
                for (i = 0; i < count; i++) {
                    /* @hack spelling: a vertex's second and third floats read
                     * past a pointer to its first, where src indexed would
                     * do */
                    ((Float *)points)[0] = ((Float *)src)[_nj_cnk_shape_v_ini_];
                    ((Float *)points)[1] =
                        ((Float *)&src[_nj_cnk_shape_v_ini_])[1];
                    ((Float *)points)[2] =
                        ((Float *)&src[_nj_cnk_shape_v_ini_])[2];
                    points += _nj_cnk_shape_nextstep_;
                    j = i * SHAPE_VECTOR;
                    njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                            (NJS_VECTOR *)&n1[j],
                                            (Float *)normals, normal_t);
                    normals += _nj_cnk_shape_nextstep_;
                    src += _nj_cnk_shape_nextstep_;
                }
            }
            break;
        case SHAPE_POINTS + SHAPE_NORMALS:
            if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                for (i = 0; i < count; i++) {
                    j = i * SHAPE_VECTOR;
                    njCnkInterpolatesF3((NJS_POINT3 *)&p0[j],
                                        (NJS_POINT3 *)&p1[j],
                                        (NJS_POINT3 *)points, point_t);
                    points += _nj_cnk_shape_nextstep_;
                    /* @hack spelling: k, which holds i * SHAPE_VECTOR as j
                     * does, where j would do */
                    k = i * SHAPE_VECTOR;
                    njCnkInterpolatesNormal((NJS_VECTOR *)&n0[k],
                                            (NJS_VECTOR *)&n1[k],
                                            (Float *)normals, normal_t);
                    normals += _nj_cnk_shape_nextstep_;
                    *colors = csrc[_nj_cnk_shape_c_ini_];
                    colors += _nj_cnk_shape_nextstep_;
                    csrc += _nj_cnk_shape_nextstep_;
                }
            } else {
                for (i = 0; i < count; i++) {
                    j = i * SHAPE_VECTOR;
                    njCnkInterpolatesF3((NJS_POINT3 *)&p0[j],
                                        (NJS_POINT3 *)&p1[j],
                                        (NJS_POINT3 *)points, point_t);
                    points += _nj_cnk_shape_nextstep_;
                    /* @hack spelling: k, which holds i * SHAPE_VECTOR as j
                     * does, where j would do */
                    k = i * SHAPE_VECTOR;
                    njCnkInterpolatesNormal((NJS_VECTOR *)&n0[k],
                                            (NJS_VECTOR *)&n1[k],
                                            (Float *)normals, normal_t);
                    normals += _nj_cnk_shape_nextstep_;
                }
            }
        }
        _nj_cnk_shapelink_vlist_[_nj_mot_current_data_] = buf;
        /* @hack spelling: the end chunk's word written through a pointer sum,
         * where buf[SHAPE_WORDS(count)] would do */
        *(buf + (_nj_cnk_shape_nextstep_ * count) + SHAPE_HEAD) = NJD_CE;
    }
}

void njCnkMakeSimpleShapeLink(NJS_CNK_OBJECT *object)
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
            out += SHAPE_HEAD;
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
                     * nj_cnk_motlink.h says (ADD #12,R12 at 0x8c12502a) */
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
            case NJD_CV_VN_D8:
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
                    out += SHAPE_VECTOR;
                    /* @bug logic: steps slot1 past slot 1's normal alone, as
                     * nj_cnk_motlink.h says (ADD #12,R12 at 0x8c1250b4) */
                    slot1 += SHAPE_VECTOR;
                    /* @hack spelling: out stepped in a statement of its
                     * own, where *out++ as in the NJD_CV_D8 arm would do */
                    *out = *slot0++;
                    out++;
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
