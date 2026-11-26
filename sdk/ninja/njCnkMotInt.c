/* the chunk motion walk, which poses a chunk object's nodes one at a time */
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* take the entry at mdata: point _nj_motion_data_info_.p and .nb at its
 * arrays. the entry is an NJS_MDATA1 to NJS_MDATA4 by _nj_mot_data_cnt_.
 * any other count takes nothing */
#define TAKE_ENTRY(mdata)                                                      \
    switch (_nj_mot_data_cnt_) {                                               \
    case 1: {                                                                  \
        NJS_MDATA1 *entry = (mdata);                                           \
        _nj_motion_data_info_.p = entry->p;                                    \
        _nj_motion_data_info_.nb = entry->nb;                                  \
    } break;                                                                   \
    case 2: {                                                                  \
        NJS_MDATA2 *entry = (mdata);                                           \
        _nj_motion_data_info_.p = entry->p;                                    \
        _nj_motion_data_info_.nb = entry->nb;                                  \
    } break;                                                                   \
    case 3: {                                                                  \
        NJS_MDATA3 *entry = (mdata);                                           \
        _nj_motion_data_info_.p = entry->p;                                    \
        _nj_motion_data_info_.nb = entry->nb;                                  \
    } break;                                                                   \
    case 4: {                                                                  \
        NJS_MDATA4 *entry = (mdata);                                           \
        _nj_motion_data_info_.p = entry->p;                                    \
        _nj_motion_data_info_.nb = entry->nb;                                  \
    } break;                                                                   \
    }

/* pose object as njCnkMotionData1 to njCnkMotionData4 do, but with rotate as
 * the angle step. the entry at _nj_motion_data_info_.mdata is an NJS_MDATAn of
 * n factors. mdata then moves to the next entry */
#define NODE_STEP(n, object, rotate)                                           \
    {                                                                          \
        NJS_MDATA##n *entry = _nj_motion_data_info_.mdata;                     \
                                                                               \
        /* @hack spelling: &entry->p[0], where entry->p would do */            \
        _nj_motion_data_info_.p = &entry->p[0];                                \
        _nj_motion_data_info_.nb = entry->nb;                                  \
        _nj_motion_data_info_.factor = 0;                                      \
        _nj_cnk_motion_translate_(object);                                     \
        rotate(object);                                                        \
        _nj_cnk_motion_scale_(object);                                         \
        /* @hack spelling: entry stepped on, then stored, where a store of     \
         * entry + 1 would do */                                               \
        entry++;                                                               \
        _nj_motion_data_info_.mdata = entry;                                   \
    }

void njSetCnkCurrentMotion(NJS_MOTION *motion, Float frame)
{
    if (motion) {
        if (frame >= (Float)motion->nbFrame) {
            return;
        }
        TAKE_MOTION(motion)
    } else {
        _nj_mot_data_cnt_ = 0;
        _nj_mot_spline_cnt_ = 0;
        _nj_mot_data_type_ = 0;
    }
    _nj_motion_data_info_.frame = frame;
    _nj_motion_data_info_.factor = 0;
    _nj_motion_data_info_.m = NULL;
    if (_nj_mot_data_type_ & NJD_MTYPE_POS_0) {
        _nj_cnk_motion_translate_ = njCnkMotionIntTranslate;
    } else {
        _nj_cnk_motion_translate_ = njCnkMotionObjTranslate;
    }
    if (_nj_mot_data_type_ & NJD_MTYPE_ANG_1) {
        _nj_cnk_motion_rotate_zxy_ = njCnkMotionIntRotateZXY;
        _nj_cnk_motion_rotate_ = njCnkMotionIntRotateXYZ;
    } else if (_nj_mot_data_type_ & NJD_MTYPE_QUAT_1) {
        _nj_cnk_motion_rotate_qua_ = njCnkMotionIntQuaternion;
    } else {
        _nj_cnk_motion_rotate_zxy_ = njCnkMotionObjRotateZXY;
        _nj_cnk_motion_rotate_ = njCnkMotionObjRotateXYZ;
        _nj_cnk_motion_rotate_qua_ = njCnkMotionObjRotate;
    }
    if (_nj_mot_data_type_ & NJD_MTYPE_SCL_2) {
        _nj_cnk_motion_scale_ = njCnkMotionIntScale;
    } else {
        _nj_cnk_motion_scale_ = njCnkMotionObjScale;
    }
}

void njCnkMotionTransform(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                          Int spline_flag)
{
    _nj_motion_int_f_func_ = _nj_cnk_motion_int_f_func_table_[spline_flag];
    _nj_motion_int_a_func_ = _nj_cnk_motion_int_a_func_table_[spline_flag];
    _nj_motion_data_info_.m = m;
    switch (_nj_mot_data_cnt_) {
    case 0:
        _nj_cnk_motion_translate_(object);
        _nj_cnk_motion_rotate_(object);
        _nj_cnk_motion_scale_(object);
        break;
    case 1:
        NODE_STEP(1, object, _nj_cnk_motion_rotate_)
        break;
    case 2:
        NODE_STEP(2, object, _nj_cnk_motion_rotate_)
        break;
    case 3:
        NODE_STEP(3, object, _nj_cnk_motion_rotate_)
        break;
    case 4:
        NODE_STEP(4, object, _nj_cnk_motion_rotate_)
        break;
    }
    if (_nj_motion_data_info_.m) {
        njMultiMatrix(NULL, _nj_motion_data_info_.m);
    }
}

void njCnkMotionTransformZXY(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                             Int spline_flag)
{
    _nj_motion_int_f_func_ = _nj_cnk_motion_int_f_func_table_[spline_flag];
    _nj_motion_int_a_func_ = _nj_cnk_motion_int_a_func_table_[spline_flag];
    _nj_motion_data_info_.m = m;
    switch (_nj_mot_data_cnt_) {
    case 0:
        _nj_cnk_motion_translate_(object);
        _nj_cnk_motion_rotate_zxy_(object);
        _nj_cnk_motion_scale_(object);
        break;
    case 1:
        NODE_STEP(1, object, _nj_cnk_motion_rotate_zxy_)
        break;
    case 2:
        NODE_STEP(2, object, _nj_cnk_motion_rotate_zxy_)
        break;
    case 3:
        NODE_STEP(3, object, _nj_cnk_motion_rotate_zxy_)
        break;
    case 4:
        NODE_STEP(4, object, _nj_cnk_motion_rotate_zxy_)
        break;
    }
    if (_nj_motion_data_info_.m) {
        njMultiMatrix(NULL, _nj_motion_data_info_.m);
    }
}

void njCnkMotionTransformQuaternion(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                                    Int spline_flag)
{
    _nj_motion_int_f_func_ = _nj_cnk_motion_int_f_func_table_[spline_flag];
    _nj_motion_int_a_func_ = _nj_cnk_motion_int_a_func_table_[spline_flag];
    _nj_motion_data_info_.m = m;
    switch (_nj_mot_data_cnt_) {
    case 0:
        _nj_cnk_motion_translate_(object);
        _nj_cnk_motion_rotate_qua_(object);
        _nj_cnk_motion_scale_(object);
        break;
    case 1:
        NODE_STEP(1, object, _nj_cnk_motion_rotate_qua_)
        break;
    case 2:
        NODE_STEP(2, object, _nj_cnk_motion_rotate_qua_)
        break;
    case 3:
        NODE_STEP(3, object, _nj_cnk_motion_rotate_qua_)
        break;
    case 4:
        NODE_STEP(4, object, _nj_cnk_motion_rotate_qua_)
        break;
    }
    if (_nj_motion_data_info_.m) {
        njMultiMatrix(NULL, _nj_motion_data_info_.m);
    }
}

void njCnkMotionTranslate(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                          Int spline_flag)
{
    _nj_motion_int_f_func_ = _nj_cnk_motion_int_f_func_table_[spline_flag];
    _nj_motion_data_info_.factor = 0;
    _nj_motion_data_info_.m = m;
    TAKE_ENTRY(_nj_motion_data_info_.mdata)
    _nj_cnk_motion_translate_(object);
}

void njCnkMotionRotateXYZ(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                          Int spline_flag)
{
    _nj_motion_int_a_func_ = _nj_cnk_motion_int_a_func_table_[spline_flag];
    _nj_motion_data_info_.factor =
        (_nj_mot_data_type_ & NJD_MTYPE_POS_0) ? 1 : 0;
    _nj_motion_data_info_.m = m;
    TAKE_ENTRY(_nj_motion_data_info_.mdata)
    _nj_cnk_motion_rotate_(object);
}

void njCnkMotionRotateZXY(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                          Int spline_flag)
{
    _nj_motion_int_a_func_ = _nj_cnk_motion_int_a_func_table_[spline_flag];
    _nj_motion_data_info_.factor =
        (_nj_mot_data_type_ & NJD_MTYPE_POS_0) ? 1 : 0;
    _nj_motion_data_info_.m = m;
    TAKE_ENTRY(_nj_motion_data_info_.mdata)
    _nj_cnk_motion_rotate_zxy_(object);
}

void njCnkMotionQuaternion(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                           Int spline_flag)
{
    _nj_motion_data_info_.factor =
        (_nj_mot_data_type_ & NJD_MTYPE_POS_0) ? 1 : 0;
    _nj_motion_data_info_.m = m;
    TAKE_ENTRY(_nj_motion_data_info_.mdata)
    _nj_cnk_motion_rotate_qua_(object);
}

void njCnkMotionScale(NJS_MATRIX *m, NJS_CNK_OBJECT *object, Int spline_flag)
{
    _nj_motion_int_f_func_ = _nj_cnk_motion_int_f_func_table_[spline_flag];
    _nj_motion_data_info_.factor = 0;
    _nj_motion_data_info_.m = m;
    if (_nj_mot_data_type_ & NJD_MTYPE_POS_0) {
        _nj_motion_data_info_.factor++;
    }
    if (_nj_mot_data_type_ & NJD_MTYPE_ANG_1) {
        _nj_motion_data_info_.factor++;
    }
    if (_nj_mot_data_type_ & NJD_MTYPE_QUAT_1) {
        _nj_motion_data_info_.factor++;
    }
    TAKE_ENTRY(_nj_motion_data_info_.mdata)
    _nj_cnk_motion_scale_(object);
}

void njGetCnkMotionTranslate(Float *pos, Int spline_flag)
{
    NJS_MKEY_F *keys;
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;

    TAKE_ENTRY(_nj_motion_data_info_.mdata)
    keys = _nj_motion_data_info_.p[0];
    nb = _nj_motion_data_info_.nb[0];
    lower = 0;
    upper = nb;
    frame = (Uint32)_nj_motion_data_info_.frame;
    while (upper - lower > 1) {
        middle = (lower + upper) >> 1;
        if (frame >= keys[middle].keyframe) {
            lower = middle;
        } else {
            upper = middle;
        }
    }
    _nj_cnk_motion_int_f_func_table_[spline_flag](keys, lower, nb, pos);
}

void njGetCnkMotionRotate(Angle *ang, Int spline_flag)
{
    NJS_MKEY_A *keys;
    Uint32 factor;
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;

    factor = 0;
    if (_nj_mot_data_type_ & NJD_MTYPE_POS_0) {
        factor++;
    }
    TAKE_ENTRY(_nj_motion_data_info_.mdata)
    keys = _nj_motion_data_info_.p[factor];
    nb = _nj_motion_data_info_.nb[factor];
    lower = 0;
    upper = nb;
    frame = (Uint32)_nj_motion_data_info_.frame;
    while (upper - lower > 1) {
        middle = (lower + upper) >> 1;
        if (frame >= keys[middle].keyframe) {
            lower = middle;
        } else {
            upper = middle;
        }
    }
    _nj_cnk_motion_int_a_func_table_[spline_flag](keys, lower, nb, ang);
}

void njGetCnkMotionQuaternion(NJS_QUATERNION *qua, Int spline_flag)
{
    NJS_MKEY_QUAT *keys;
    Uint32 factor;
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;

    factor = 0;
    if (_nj_mot_data_type_ & NJD_MTYPE_POS_0) {
        factor++;
    }
    TAKE_ENTRY(_nj_motion_data_info_.mdata)
    keys = _nj_motion_data_info_.p[factor];
    nb = _nj_motion_data_info_.nb[factor];
    lower = 0;
    upper = nb;
    frame = (Uint32)_nj_motion_data_info_.frame;
    while (upper - lower > 1) {
        middle = (lower + upper) >> 1;
        if (frame >= keys[middle].keyframe) {
            lower = middle;
        } else {
            upper = middle;
        }
    }
    njCnkLinearMotionQ(keys, lower, nb, (Float *)qua);
}

void njGetCnkMotionScale(Float *scl, Int spline_flag)
{
    NJS_MKEY_F *keys;
    Uint32 factor;
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;

    factor = 0;
    if (_nj_mot_data_type_ & NJD_MTYPE_POS_0) {
        factor++;
    }
    if (_nj_mot_data_type_ & NJD_MTYPE_ANG_1) {
        factor++;
    }
    if (_nj_mot_data_type_ & NJD_MTYPE_QUAT_1) {
        factor++;
    }
    TAKE_ENTRY(_nj_motion_data_info_.mdata)
    keys = _nj_motion_data_info_.p[factor];
    nb = _nj_motion_data_info_.nb[factor];
    lower = 0;
    upper = nb;
    frame = (Uint32)_nj_motion_data_info_.frame;
    while (upper - lower > 1) {
        middle = (lower + upper) >> 1;
        if (frame >= keys[middle].keyframe) {
            lower = middle;
        } else {
            upper = middle;
        }
    }
    _nj_cnk_motion_int_f_func_table_[spline_flag](keys, lower, nb, scl);
}

void njSetCnkNextMotionNode(void)
{
    struct nj_motion_info *info = &_nj_motion_data_info_;
    void *mdata;
    NJS_MATRIX *m;

    /* @hack spelling: mdata, a local copy of info->mdata, where info->mdata
     * read in each case would do */
    mdata = info->mdata;
    switch (_nj_mot_data_cnt_) {
    case 1:
        info->mdata = (NJS_MDATA1 *)mdata + 1;
        break;
    case 2:
        info->mdata = (NJS_MDATA2 *)mdata + 1;
        break;
    case 3:
        info->mdata = (NJS_MDATA3 *)mdata + 1;
        break;
    case 4:
        info->mdata = (NJS_MDATA4 *)mdata + 1;
        break;
    }
    /* @hack spelling: m, a local copy of info->m, where info->m read twice
     * would do */
    m = info->m;
    if (m) {
        njMultiMatrix(NULL, m);
    }
}
