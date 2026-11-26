/* the simple chunk drawer's shape point functions, along a curve or a
 * straight line between keys */
#include <nj_cnk_shape.h>
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set the shape layout for a vertex chunk of type: NJD_CV, NJD_CV_D8,
 * NJD_CV_VN or NJD_CV_VN_D8. any other type gives a vertex of no words and
 * clears the shape type */
void njCnkInitSimpleShapeData(Sint32 type);
/* the shape point functions of _nj_cnk_s_shape_int_func_table_.
 * njCnkSimpleShapeSplinePoint moves the points along njCnkInterpolatesSF3's
 * curve between two keys. njCnkSimpleShapeLinearPoint moves them in a straight
 * line between the same two. both move the normals in a straight line by
 * njCnkInterpolatesNormal */
void njCnkSimpleShapeSplinePoint(NJS_CNK_OBJECT *object);
void njCnkSimpleShapeLinearPoint(NJS_CNK_OBJECT *object);
/* NOLINTEND(misc-use-internal-linkage) */

void (*_nj_cnk_s_shape_int_func_table_[2])(NJS_CNK_OBJECT *object) = {
    njCnkSimpleShapeLinearPoint,
    njCnkSimpleShapeSplinePoint,
};

void njCnkInitSimpleShapeData(Sint32 type)
{
    switch (type) {
    case NJD_CV:
        _nj_cnk_shape_nextstep_ = SHAPE_VECTOR;
        _nj_cnk_shape_v_ini_ = SHAPE_HEAD;
        _nj_cnk_shape_n_ini_ = SHAPE_NONE;
        _nj_cnk_shape_c_ini_ = SHAPE_NONE;
        _nj_mot_shape_data_type_ &=
            (Uint16) ~(SHAPE_MTYPE_COLOR | NJD_MTYPE_NORM_5);
        break;
    case NJD_CV_D8:
        _nj_cnk_shape_nextstep_ = SHAPE_VECTOR + SHAPE_COLOR;
        _nj_cnk_shape_v_ini_ = SHAPE_HEAD;
        _nj_cnk_shape_n_ini_ = SHAPE_NONE;
        _nj_cnk_shape_c_ini_ = SHAPE_HEAD + SHAPE_VECTOR;
        _nj_mot_shape_data_type_ &= (Uint16)~NJD_MTYPE_NORM_5;
        break;
    case NJD_CV_VN:
        _nj_cnk_shape_nextstep_ = 2 * SHAPE_VECTOR;
        _nj_cnk_shape_v_ini_ = SHAPE_HEAD;
        _nj_cnk_shape_n_ini_ = SHAPE_HEAD + SHAPE_VECTOR;
        _nj_cnk_shape_c_ini_ = SHAPE_NONE;
        _nj_mot_shape_data_type_ &= (Uint16)~SHAPE_MTYPE_COLOR;
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
        _nj_mot_shape_data_type_ = 0;
        break;
    }
}
void njCnkSimpleShapeSplinePoint(NJS_CNK_OBJECT *object)
{
    Sint32 *vlist;
    /* @hack spelling: csrc, a second cursor that holds src's value at each
     * use, where src would do */
    Sint32 *csrc;
    Sint32 *buf;
    Sint32 *colors;
    Float *src;
    Float *points;
    Float *normals;
    NJS_MKEY_P *keys;
    void *point_keys[4];
    /* @hack redundant: normal_keys's last two entries, which nothing writes */
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
    Sint32 count;
    Sint32 found;
    Sint32 i;
    Sint32 j;

#ifdef NONMATCHING
    /* GCC can't see that the locals below are set wherever they are read. p0
     * to p3 and point_t are read only where found holds SHAPE_POINTS. n0, n1
     * and normal_t are read only where it holds SHAPE_NORMALS. points, normals
     * and colors are read only where the layout gives their offsets */
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
    if (vlist != NULL) {
        src = (Float *)vlist;
        csrc = vlist;
        if (_nj_mot_shape_data_cnt_) {
            buf = (Sint32 *)_nj_shape_buf_;
            buf[0] = vlist[0];
            buf[1] = vlist[1];
            count = SHAPE_COUNT(buf);
            if (!_nj_cnk_shape_init_flag_) {
                njCnkInitSimpleShapeData((Uint16)buf[0]);
                _nj_cnk_shape_init_flag_ = TRUE;
            }
            found = 0;
            if (_nj_cnk_shape_v_ini_ != SHAPE_NONE) {
                points = &_nj_shape_buf_[_nj_cnk_shape_v_ini_];
            }
            if (_nj_cnk_shape_n_ini_ != SHAPE_NONE) {
                normals = &_nj_shape_buf_[_nj_cnk_shape_n_ini_];
            }
            if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                colors = (Sint32 *)&_nj_shape_buf_[_nj_cnk_shape_c_ini_];
            }
            if (_nj_mot_shape_data_type_ & NJD_MTYPE_VERT_4) {
                keys = _nj_shape_data_info_.p[_nj_shape_data_info_.factor];
                if (keys != NULL) {
                    nb = _nj_shape_data_info_.nb[_nj_shape_data_info_.factor];
                    low = 0;
                    high = nb;
                    frame = _nj_shape_data_info_.frame;
                    while (high - low > 1) {
                        middle = (low + high) >> 1;
                        if (frame >= keys[middle].keyframe) {
                            low = middle;
                        } else {
                            high = middle;
                        }
                    }
                    point_t = njCnkGetSplineKFdata(keys, low, nb, point_keys);
                    p0 = point_keys[0];
                    p1 = point_keys[1];
                    p2 = point_keys[2];
                    p3 = point_keys[3];
                    found = SHAPE_POINTS;
                }
                _nj_shape_data_info_.factor++;
            }
            if (_nj_mot_shape_data_type_ & NJD_MTYPE_NORM_5) {
                keys = _nj_shape_data_info_.p[_nj_shape_data_info_.factor];
                if (keys != NULL) {
                    nb = _nj_shape_data_info_.nb[_nj_shape_data_info_.factor];
                    low = 0;
                    high = nb;
                    frame = _nj_shape_data_info_.frame;
                    while (high - low > 1) {
                        middle = (low + high) >> 1;
                        if (frame >= keys[middle].keyframe) {
                            low = middle;
                        } else {
                            high = middle;
                        }
                    }
                    normal_t = njCnkGetLinearKFdata(keys, low, nb, normal_keys);
                    n0 = normal_keys[0];
                    n1 = normal_keys[1];
                    found += SHAPE_NORMALS;
                }
                _nj_shape_data_info_.factor++;
            }
            switch (found) {
            case SHAPE_POINTS:
                if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                    if (_nj_cnk_shape_n_ini_ != SHAPE_NONE) {
                        for (i = 0; i < count; i++) {
                            j = i * SHAPE_VECTOR;
                            njCnkInterpolatesSF3(
                                (NJS_POINT3 *)&p0[j], (NJS_POINT3 *)&p1[j],
                                (NJS_POINT3 *)&p2[j], (NJS_POINT3 *)&p3[j],
                                point_t, (NJS_POINT3 *)points);
                            points += _nj_cnk_shape_nextstep_;
                            /* @hack spelling: a vertex's second and third
                             * floats read past a pointer to its first,
                             * where src indexed would do */
                            normals[0] = src[_nj_cnk_shape_n_ini_];
                            normals[1] = (src + _nj_cnk_shape_n_ini_)[1];
                            normals[2] = (src + _nj_cnk_shape_n_ini_)[2];
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
                                (NJS_POINT3 *)&p2[j], (NJS_POINT3 *)&p3[j],
                                point_t, (NJS_POINT3 *)points);
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
                            njCnkInterpolatesSF3(
                                (NJS_POINT3 *)&p0[j], (NJS_POINT3 *)&p1[j],
                                (NJS_POINT3 *)&p2[j], (NJS_POINT3 *)&p3[j],
                                point_t, (NJS_POINT3 *)points);
                            points += _nj_cnk_shape_nextstep_;
                            /* @hack spelling: a vertex's second and third
                             * floats read past a pointer to its first,
                             * where src indexed would do */
                            normals[0] = src[_nj_cnk_shape_n_ini_];
                            normals[1] = (src + _nj_cnk_shape_n_ini_)[1];
                            normals[2] = (src + _nj_cnk_shape_n_ini_)[2];
                            normals += _nj_cnk_shape_nextstep_;
                            src += _nj_cnk_shape_nextstep_;
                        }
                    } else {
                        for (i = 0; i < count; i++) {
                            j = i * SHAPE_VECTOR;
                            njCnkInterpolatesSF3(
                                (NJS_POINT3 *)&p0[j], (NJS_POINT3 *)&p1[j],
                                (NJS_POINT3 *)&p2[j], (NJS_POINT3 *)&p3[j],
                                point_t, (NJS_POINT3 *)points);
                            points += _nj_cnk_shape_nextstep_;
                        }
                    }
                }
                break;
            case SHAPE_NORMALS:
                if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                    for (i = 0; i < count; i++) {
                        /* @hack spelling: a vertex's second and third
                         * floats read past a pointer to its first, where
                         * src indexed would do */
                        points[0] = src[_nj_cnk_shape_v_ini_];
                        points[1] = (src + _nj_cnk_shape_v_ini_)[1];
                        points[2] = (src + _nj_cnk_shape_v_ini_)[2];
                        points += _nj_cnk_shape_nextstep_;
                        j = i * SHAPE_VECTOR;
                        njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                                (NJS_VECTOR *)&n1[j], normals,
                                                normal_t);
                        normals += _nj_cnk_shape_nextstep_;
                        *colors = csrc[_nj_cnk_shape_c_ini_];
                        colors += _nj_cnk_shape_nextstep_;
                        src += _nj_cnk_shape_nextstep_;
                        csrc += _nj_cnk_shape_nextstep_;
                    }
                } else {
                    for (i = 0; i < count; i++) {
                        /* @hack spelling: a vertex's second and third
                         * floats read past a pointer to its first, where
                         * src indexed would do */
                        points[0] = src[_nj_cnk_shape_v_ini_];
                        points[1] = (src + _nj_cnk_shape_v_ini_)[1];
                        points[2] = (src + _nj_cnk_shape_v_ini_)[2];
                        points += _nj_cnk_shape_nextstep_;
                        j = i * SHAPE_VECTOR;
                        njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                                (NJS_VECTOR *)&n1[j], normals,
                                                normal_t);
                        normals += _nj_cnk_shape_nextstep_;
                        src += _nj_cnk_shape_nextstep_;
                    }
                }
                break;
            /* @hack spelling: the default arm before the last case, where
             * last would do */
            default:
                /* @hack spelling: a goto to the draw, where the draw in
                 * this arm would do */
                goto draw;
            case SHAPE_POINTS + SHAPE_NORMALS:
                if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                    for (i = 0; i < count; i++) {
                        j = i * SHAPE_VECTOR;
                        njCnkInterpolatesSF3(
                            (NJS_POINT3 *)&p0[j], (NJS_POINT3 *)&p1[j],
                            (NJS_POINT3 *)&p2[j], (NJS_POINT3 *)&p3[j], point_t,
                            (NJS_POINT3 *)points);
                        points += _nj_cnk_shape_nextstep_;
                        /* @hack redundant: j set again to the value it
                         * holds */
                        j = i * SHAPE_VECTOR;
                        njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                                (NJS_VECTOR *)&n1[j], normals,
                                                normal_t);
                        normals += _nj_cnk_shape_nextstep_;
                        *colors = csrc[_nj_cnk_shape_c_ini_];
                        colors += _nj_cnk_shape_nextstep_;
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
                        /* @hack redundant: j set again to the value it
                         * holds */
                        j = i * SHAPE_VECTOR;
                        njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                                (NJS_VECTOR *)&n1[j], normals,
                                                normal_t);
                        normals += _nj_cnk_shape_nextstep_;
                    }
                }
            }
            object->model->vlist = buf;
            /* @hack spelling: the end chunk's word written through a pointer
             * sum, where buf[SHAPE_WORDS(count)] would do */
            *(buf + (_nj_cnk_shape_nextstep_ * count) + SHAPE_HEAD) = NJD_CE;
            /* @hack redundant: a goto to the label that follows */
            goto draw;
        }
    }
draw:
    _nj_cnk_draw_model_(object->model);
    object->model->vlist = vlist;
}

void njCnkSimpleShapeLinearPoint(NJS_CNK_OBJECT *object)
{
    Sint32 *vlist;
    /* @hack spelling: csrc, a second cursor that holds src's value at each
     * use, where src would do */
    Sint32 *csrc;
    Sint32 *buf;
    Sint32 *colors;
    Float *src;
    Float *points;
    Float *normals;
    NJS_MKEY_P *keys;
    void *point_keys[2];
    void *normal_keys[2];
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
    Sint32 count;
    Sint32 found;
    Sint32 i;
    Sint32 j;

#ifdef NONMATCHING
    /* GCC can't see that the locals below are set wherever they are read. p0,
     * p1 and point_t are read only where found holds SHAPE_POINTS. n0, n1 and
     * normal_t are read only where it holds SHAPE_NORMALS. points, normals and
     * colors are read only where the layout gives their offsets */
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
    if (vlist != NULL) {
        csrc = vlist;
        src = (Float *)vlist;
        if (_nj_mot_shape_data_cnt_) {
            buf = (Sint32 *)_nj_shape_buf_;
            buf[0] = vlist[0];
            buf[1] = vlist[1];
            count = SHAPE_COUNT(buf);
            if (!_nj_cnk_shape_init_flag_) {
                njCnkInitSimpleShapeData((Uint16)buf[0]);
                _nj_cnk_shape_init_flag_ = TRUE;
            }
            found = 0;
            if (_nj_cnk_shape_v_ini_ != SHAPE_NONE) {
                points = &_nj_shape_buf_[_nj_cnk_shape_v_ini_];
            }
            if (_nj_cnk_shape_n_ini_ != SHAPE_NONE) {
                normals = &_nj_shape_buf_[_nj_cnk_shape_n_ini_];
            }
            if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                colors = (Sint32 *)&_nj_shape_buf_[_nj_cnk_shape_c_ini_];
            }
            if (_nj_mot_shape_data_type_ & NJD_MTYPE_VERT_4) {
                keys = _nj_shape_data_info_.p[_nj_shape_data_info_.factor];
                if (keys != NULL) {
                    nb = _nj_shape_data_info_.nb[_nj_shape_data_info_.factor];
                    low = 0;
                    high = nb;
                    frame = _nj_shape_data_info_.frame;
                    while (high - low > 1) {
                        middle = (low + high) >> 1;
                        if (frame >= keys[middle].keyframe) {
                            low = middle;
                        } else {
                            high = middle;
                        }
                    }
                    point_t = njCnkGetLinearKFdata(keys, low, nb, point_keys);
                    p0 = point_keys[0];
                    p1 = point_keys[1];
                    found = SHAPE_POINTS;
                }
                _nj_shape_data_info_.factor++;
            }
            if (_nj_mot_shape_data_type_ & NJD_MTYPE_NORM_5) {
                keys = _nj_shape_data_info_.p[_nj_shape_data_info_.factor];
                if (keys != NULL) {
                    nb = _nj_shape_data_info_.nb[_nj_shape_data_info_.factor];
                    low = 0;
                    high = nb;
                    frame = _nj_shape_data_info_.frame;
                    while (high - low > 1) {
                        middle = (low + high) >> 1;
                        if (frame >= keys[middle].keyframe) {
                            low = middle;
                        } else {
                            high = middle;
                        }
                    }
                    normal_t = njCnkGetLinearKFdata(keys, low, nb, normal_keys);
                    n0 = normal_keys[0];
                    n1 = normal_keys[1];
                    found += SHAPE_NORMALS;
                }
                _nj_shape_data_info_.factor++;
            }
            switch (found) {
            case SHAPE_POINTS:
                if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                    if (_nj_cnk_shape_n_ini_ != SHAPE_NONE) {
                        for (i = 0; i < count; i++) {
                            j = i * SHAPE_VECTOR;
                            njCnkInterpolatesF3((NJS_POINT3 *)&p0[j],
                                                (NJS_POINT3 *)&p1[j],
                                                (NJS_POINT3 *)points, point_t);
                            points += _nj_cnk_shape_nextstep_;
                            /* @hack spelling: a vertex's second and third
                             * floats read past a pointer to its first,
                             * where src indexed would do */
                            normals[0] = src[_nj_cnk_shape_n_ini_];
                            normals[1] = (src + _nj_cnk_shape_n_ini_)[1];
                            normals[2] = (src + _nj_cnk_shape_n_ini_)[2];
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
                            /* @hack spelling: a vertex's second and third
                             * floats read past a pointer to its first,
                             * where src indexed would do */
                            normals[0] = src[_nj_cnk_shape_n_ini_];
                            normals[1] = (src + _nj_cnk_shape_n_ini_)[1];
                            normals[2] = (src + _nj_cnk_shape_n_ini_)[2];
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
                }
                break;
            case SHAPE_NORMALS:
                if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                    for (i = 0; i < count; i++) {
                        /* @hack spelling: a vertex's second and third
                         * floats read past a pointer to its first, where
                         * src indexed would do */
                        points[0] = src[_nj_cnk_shape_v_ini_];
                        points[1] = (src + _nj_cnk_shape_v_ini_)[1];
                        points[2] = (src + _nj_cnk_shape_v_ini_)[2];
                        points += _nj_cnk_shape_nextstep_;
                        j = i * SHAPE_VECTOR;
                        njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                                (NJS_VECTOR *)&n1[j], normals,
                                                normal_t);
                        normals += _nj_cnk_shape_nextstep_;
                        *colors = csrc[_nj_cnk_shape_c_ini_];
                        colors += _nj_cnk_shape_nextstep_;
                        src += _nj_cnk_shape_nextstep_;
                        csrc += _nj_cnk_shape_nextstep_;
                    }
                } else {
                    for (i = 0; i < count; i++) {
                        /* @hack spelling: a vertex's second and third
                         * floats read past a pointer to its first, where
                         * src indexed would do */
                        points[0] = src[_nj_cnk_shape_v_ini_];
                        points[1] = (src + _nj_cnk_shape_v_ini_)[1];
                        points[2] = (src + _nj_cnk_shape_v_ini_)[2];
                        points += _nj_cnk_shape_nextstep_;
                        j = i * SHAPE_VECTOR;
                        njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                                (NJS_VECTOR *)&n1[j], normals,
                                                normal_t);
                        normals += _nj_cnk_shape_nextstep_;
                        src += _nj_cnk_shape_nextstep_;
                    }
                }
                break;
            /* @hack spelling: the default arm before the last case, where
             * last would do */
            default:
                /* @hack spelling: a goto to the draw, where the draw in
                 * this arm would do */
                goto draw;
            case SHAPE_POINTS + SHAPE_NORMALS:
                if (_nj_cnk_shape_c_ini_ != SHAPE_NONE) {
                    for (i = 0; i < count; i++) {
                        j = i * SHAPE_VECTOR;
                        njCnkInterpolatesF3((NJS_POINT3 *)&p0[j],
                                            (NJS_POINT3 *)&p1[j],
                                            (NJS_POINT3 *)points, point_t);
                        points += _nj_cnk_shape_nextstep_;
                        /* @hack redundant: j set again to the value it
                         * holds */
                        j = i * SHAPE_VECTOR;
                        njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                                (NJS_VECTOR *)&n1[j], normals,
                                                normal_t);
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
                        /* @hack redundant: j set again to the value it
                         * holds */
                        j = i * SHAPE_VECTOR;
                        njCnkInterpolatesNormal((NJS_VECTOR *)&n0[j],
                                                (NJS_VECTOR *)&n1[j], normals,
                                                normal_t);
                        normals += _nj_cnk_shape_nextstep_;
                    }
                }
            }
            object->model->vlist = buf;
            /* @hack spelling: the end chunk's word written through a pointer
             * sum, where buf[SHAPE_WORDS(count)] would do */
            *(buf + (_nj_cnk_shape_nextstep_ * count) + SHAPE_HEAD) = NJD_CE;
            /* @hack redundant: a goto to the label that follows */
            goto draw;
        }
    }
draw:
    _nj_cnk_draw_model_(object->model);
    object->model->vlist = vlist;
}
