/* the easy chunk drawer's shape point function, along a straight line
 * between keys */
#include <nj_cnk_shape.h>
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set the shape layout for type as njCnkInitSimpleShapeData does, for NJD_CV,
 * NJD_CV_D8 and NJD_CV_VN alone. any other type, NJD_CV_VN_D8
 * among them, gives a vertex of no words and clears the shape type */
void njCnkInitEasyShapeData(Sint32 type);
/* NOLINTEND(misc-use-internal-linkage) */

void njCnkInitEasyShapeData(Sint32 type)
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
    default:
        _nj_cnk_shape_nextstep_ = 0;
        _nj_cnk_shape_v_ini_ = SHAPE_NONE;
        _nj_cnk_shape_n_ini_ = SHAPE_NONE;
        _nj_cnk_shape_c_ini_ = SHAPE_NONE;
        _nj_mot_shape_data_type_ = 0;
        break;
    }
}

void njCnkEasyShapeLinearPoint(NJS_CNK_OBJECT *object)
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
    Sint32 k;

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
        src = vlist;
        if (_nj_mot_shape_data_cnt_) {
            buf = (Sint32 *)_nj_shape_buf_;
            buf[0] = vlist[0];
            buf[1] = vlist[1];
            count = SHAPE_COUNT(buf);
            if (!_nj_cnk_shape_init_flag_) {
                njCnkInitEasyShapeData((Uint16)buf[0]);
                _nj_cnk_shape_init_flag_ = TRUE;
            }
            found = 0;
            if (_nj_cnk_shape_v_ini_ != SHAPE_NONE) {
                points = (Sint32 *)&_nj_shape_buf_[_nj_cnk_shape_v_ini_];
            }
            if (_nj_cnk_shape_n_ini_ != SHAPE_NONE) {
                normals = (Sint32 *)&_nj_shape_buf_[_nj_cnk_shape_n_ini_];
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
                        /* @hack spelling: a vertex's second and third
                         * floats read past a pointer to its first, where
                         * src indexed would do */
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
                    /* @hack spelling: a vertex's second and third floats
                     * read past a pointer to its first, where src indexed
                     * would do */
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
                break;
            case SHAPE_POINTS + SHAPE_NORMALS:
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
                break;
            default:
                /* @hack spelling: a goto to the draw, where the draw in
                 * this arm would do */
                goto draw;
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
