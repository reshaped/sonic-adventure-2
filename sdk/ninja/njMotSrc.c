/* the basic models' motion and shape steps, and their interpolations. they
 * are njCnkMotSrc's and njCnkShapeSrc's counterparts for NJS_OBJECT trees */
#include <machine.h>
#include <nj_cnk_shape.h>
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

static void linear_floats(NJS_MKEY_F *keys, Uint32 index, Uint32 nb,
                          Float *out);
static void linear_angles(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                          Angle *out);
static void overhauser_floats(NJS_MKEY_F *keys, Uint32 index, Uint32 nb,
                              Float *out);
static void overhauser_angles(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                              Angle *out);
static void spline_shape(NJS_OBJECT *object);
static void linear_shape(NJS_OBJECT *object);
static void motion_data0(NJS_OBJECT *object);
static void motion_data1(NJS_OBJECT *object);
static void motion_data2(NJS_OBJECT *object);
static void motion_data3(NJS_OBJECT *object);
static void motion_data4(NJS_OBJECT *object);
static void shape_data0(NJS_OBJECT *object);
static void shape_data1(NJS_OBJECT *object);
static void shape_data2(NJS_OBJECT *object);
static void linear_angles_short(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                                Angle *out);

/* the member exports these names, though no header of the SDK declares
 * them. each step does for an NJS_OBJECT what its chunk counterpart in
 * nj_motion.h does for a chunk object. a counterpart's name has Cnk after nj.
 * where a chunk step works on the current matrix, these pass
 * _nj_motion_data_info_.m to njTranslate, njRotateXYZ, njRotateZXY and
 * njScale.
 *
 * _nj_motion_data_func_table_ holds the node steps, indexed by the count of
 * factors in an entry. _nj_shape_data_func_table_ holds the shape node steps,
 * indexed by _nj_mot_shape_data_cnt_. the steps at SHAPE_POINTS and
 * SHAPE_NORMALS take an entry of one factor. the step at their sum takes an
 * entry of two.
 *
 * each interpolation table holds a straight line at 0 and an Overhauser curve
 * at 1. the angle tables hold overhauser_angles at 1, where nj_motion.h's
 * chunk angle tables hold the straight line twice.
 * _nj_motion_int_a_func_table_old_'s straight line is linear_angles_short,
 * which blends as njCnkLinearMotionAOld does */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void njMotionIntTranslate(NJS_OBJECT *object);
void njMotionIntScale(NJS_OBJECT *object);
void njMotionIntRotateXYZ(NJS_OBJECT *object);
void njMotionIntRotateZXY(NJS_OBJECT *object);
void njMotionObjTranslate(NJS_OBJECT *object);
void njMotionObjRotateXYZ(NJS_OBJECT *object);
void njMotionObjRotateZXY(NJS_OBJECT *object);
void njMotionObjScale(NJS_OBJECT *object);
void (*_nj_motion_data_func_table_[NODE_STEPS])(NJS_OBJECT *object) = {
    motion_data0, motion_data1, motion_data2, motion_data3, motion_data4,
};
void (*_nj_shape_data_func_table_[SHAPE_STEPS])(NJS_OBJECT *object) = {
    shape_data0, shape_data1, shape_data1, shape_data2};
void (*_nj_motion_int_f_func_table_[INTERPOLATIONS])(NJS_MKEY_F *keys,
                                                     Uint32 index, Uint32 nb,
                                                     Float *out) = {
    linear_floats, overhauser_floats};
void (*_nj_motion_int_a_func_table_[INTERPOLATIONS])(NJS_MKEY_A *keys,
                                                     Uint32 index, Uint32 nb,
                                                     Angle *out) = {
    linear_angles, overhauser_angles};
void (*_nj_shape_int_func_table_[INTERPOLATIONS])(NJS_OBJECT *object) = {
    linear_shape, spline_shape};
void (*_nj_motion_int_a_func_table_old_[INTERPOLATIONS])(
    NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
    Angle *out) = {linear_angles_short, overhauser_angles};
/* NOLINTEND(misc-use-internal-linkage) */

/* the interpolations, as njCnkMotSrc's njCnkLinearMotionF and
 * njCnkOverhauserMotionF are. an angle is blended as a float, then truncated
 * toward 0 */
static void linear_floats(NJS_MKEY_F *keys, Uint32 index, Uint32 nb, Float *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_F *key = &keys[index];
    NJS_MKEY_F *next = &keys[(index + 1) % nb];
    Float t;
    Float s;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) /
        divisor[(Sint32)(next->keyframe - key->keyframe) > 0];
    s = 1.0F - t;
    out[0] = (key->key[0] * s) + (next->key[0] * t);
    out[1] = (key->key[1] * s) + (next->key[1] * t);
    out[2] = (key->key[2] * s) + (next->key[2] * t);
}

static void linear_angles(NJS_MKEY_A *keys, Uint32 index, Uint32 nb, Angle *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_A *key = &keys[index];
    NJS_MKEY_A *next = &keys[(index + 1) % nb];
    Float t;
    Float s;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) /
        divisor[(Sint32)(next->keyframe - key->keyframe) > 0];
    s = 1.0F - t;
    out[0] = (Angle)(((Float)key->key[0] * s) + ((Float)next->key[0] * t));
    out[1] = (Angle)(((Float)key->key[1] * s) + ((Float)next->key[1] * t));
    out[2] = (Angle)(((Float)key->key[2] * s) + ((Float)next->key[2] * t));
}

static void overhauser_floats(NJS_MKEY_F *keys, Uint32 index, Uint32 nb,
                              Float *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_F *before = &keys[(index + nb - 1) % nb];
    NJS_MKEY_F *key = &keys[index];
    NJS_MKEY_F *next = &keys[(index + 1) % nb];
    NJS_MKEY_F *after = &keys[(index + 2) % nb];
    Float t;
    Float t2;
    Float t3;
    /* the Catmull-Rom weights of before, key, next and after */
    Float w0;
    Float w1;
    Float w2;
    Float w3;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) /
        divisor[(Sint32)(next->keyframe - key->keyframe) > 0];
    t2 = t * t;
    t3 = t * t2;
    w0 = ((2.0F * t2) - t - t3) * 0.5F;
    w1 = (1.5F * t3) - (2.5F * t2) + 1.0F;
    w2 = (0.5F * t) - (1.5F * t3) + (2.0F * t2);
    w3 = (t3 - t2) * 0.5F;
    out[0] = (w0 * before->key[0]) + (w1 * key->key[0]) + (w2 * next->key[0]) +
             (w3 * after->key[0]);
    out[1] = (w0 * before->key[1]) + (w1 * key->key[1]) + (w2 * next->key[1]) +
             (w3 * after->key[1]);
    out[2] = (w0 * before->key[2]) + (w1 * key->key[2]) + (w2 * next->key[2]) +
             (w3 * after->key[2]);
}

static void overhauser_angles(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                              Angle *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_A *before = &keys[(index + nb - 1) % nb];
    NJS_MKEY_A *key = &keys[index];
    NJS_MKEY_A *next = &keys[(index + 1) % nb];
    NJS_MKEY_A *after = &keys[(index + 2) % nb];
    Float t;
    Float t2;
    Float t3;
    /* the Catmull-Rom weights of before, key, next and after */
    Float w0;
    Float w1;
    Float w2;
    Float w3;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) /
        divisor[(Sint32)(next->keyframe - key->keyframe) > 0];
    t2 = t * t;
    t3 = t * t2;
    w0 = ((2.0F * t2) - t - t3) * 0.5F;
    w1 = (1.5F * t3) - (2.5F * t2) + 1.0F;
    w2 = (0.5F * t) - (1.5F * t3) + (2.0F * t2);
    w3 = (t3 - t2) * 0.5F;
    out[0] = (Angle)((w0 * (Float)before->key[0]) + (w1 * (Float)key->key[0]) +
                     (w2 * (Float)next->key[0]) + (w3 * (Float)after->key[0]));
    out[1] = (Angle)((w0 * (Float)before->key[1]) + (w1 * (Float)key->key[1]) +
                     (w2 * (Float)next->key[1]) + (w3 * (Float)after->key[1]));
    out[2] = (Angle)((w0 * (Float)before->key[2]) + (w1 * (Float)key->key[2]) +
                     (w2 * (Float)next->key[2]) + (w3 * (Float)after->key[2]));
}

void njMotionIntTranslate(NJS_OBJECT *object)
{
    NJS_MKEY_F *keys;
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;
    Float value[3];

    keys = _nj_motion_data_info_.p[_nj_motion_data_info_.factor];
    if (keys) {
        nb = _nj_motion_data_info_.nb[_nj_motion_data_info_.factor];
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
        _nj_motion_int_f_func_(keys, lower, nb, value);
        njTranslate(_nj_motion_data_info_.m, value[0], value[1], value[2]);
    } else {
        njTranslate(_nj_motion_data_info_.m, object->pos[0], object->pos[1],
                    object->pos[2]);
    }
    _nj_motion_data_info_.factor++;
}

void njMotionIntScale(NJS_OBJECT *object)
{
    NJS_MKEY_F *keys;
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;
    Float value[3];

    keys = _nj_motion_data_info_.p[_nj_motion_data_info_.factor];
    if (keys) {
        nb = _nj_motion_data_info_.nb[_nj_motion_data_info_.factor];
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
        _nj_motion_int_f_func_(keys, lower, nb, value);
        njScale(_nj_motion_data_info_.m, value[0], value[1], value[2]);
    } else {
        njScale(_nj_motion_data_info_.m, object->scl[0], object->scl[1],
                object->scl[2]);
    }
    _nj_motion_data_info_.factor++;
}

void njMotionIntRotateXYZ(NJS_OBJECT *object)
{
    NJS_MKEY_A *keys;
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;
    Angle value[3];

    keys = _nj_motion_data_info_.p[_nj_motion_data_info_.factor];
    if (keys) {
        nb = _nj_motion_data_info_.nb[_nj_motion_data_info_.factor];
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
        _nj_motion_int_a_func_(keys, lower, nb, value);
        njRotateXYZ(_nj_motion_data_info_.m, value[0], value[1], value[2]);
    } else {
        njRotateXYZ(_nj_motion_data_info_.m, object->ang[0], object->ang[1],
                    object->ang[2]);
    }
    _nj_motion_data_info_.factor++;
}

void njMotionIntRotateZXY(NJS_OBJECT *object)
{
    NJS_MKEY_A *keys;
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;
    Angle value[3];

    keys = _nj_motion_data_info_.p[_nj_motion_data_info_.factor];
    if (keys) {
        nb = _nj_motion_data_info_.nb[_nj_motion_data_info_.factor];
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
        _nj_motion_int_a_func_(keys, lower, nb, value);
        njRotateZXY(_nj_motion_data_info_.m, value[0], value[1], value[2]);
    } else {
        njRotateZXY(_nj_motion_data_info_.m, object->ang[0], object->ang[1],
                    object->ang[2]);
    }
    _nj_motion_data_info_.factor++;
}

/* the shape steps' key readers, as nj_cnk_shape.h's njCnkGetSplineKFdata
 * and njCnkGetLinearKFdata are */
static Float spline_key_data(NJS_MKEY_P *keys, Uint32 index, Uint32 nb,
                             void **out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_P *before = &keys[(index + nb - 1) % nb];
    NJS_MKEY_P *key = &keys[index];
    NJS_MKEY_P *next = &keys[(index + 1) % nb];
    NJS_MKEY_P *after = &keys[(index + 2) % nb];
    Float t;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_shape_data_info_.frame - (Float)key->keyframe) /
        divisor[(Sint32)(next->keyframe - key->keyframe) > 0];
    out[0] = before->key;
    out[1] = key->key;
    out[2] = next->key;
    out[3] = after->key;
    return t;
}

static Float linear_key_data(NJS_MKEY_P *keys, Uint32 index, Uint32 nb,
                             void **out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_P *key = &keys[index];
    NJS_MKEY_P *next = &keys[(index + 1) % nb];
    Float t;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_shape_data_info_.frame - (Float)key->keyframe) /
        divisor[(Sint32)(next->keyframe - key->keyframe) > 0];
    out[0] = key->key;
    out[1] = next->key;
    return t;
}

/* the shape steps' blends, as nj_cnk_shape.h's njCnkInterpolatesNormal and
 * njCnkInterpolatesF3 are */
/* @hack type: out a Float *, where an NJS_VECTOR * would do */
static void blend_normal(NJS_VECTOR *v1, NJS_VECTOR *v2, Float *out, Float rate)
{
    Float s = 1.0F - rate;
    Float length2;

    /* @hack spelling: s first in x's product, where v1->x * s as in y's and
     * z's would do */
    out[0] = (s * v1->x) + (v2->x * rate);
    out[1] = (v1->y * s) + (v2->y * rate);
    out[2] = (v1->z * s) + (v2->z * rate);
    length2 = (out[0] * out[0]) + (out[1] * out[1]) + (out[2] * out[2]);
    if (length2 > MIN_LENGTH2) {
        /* @hack spelling: length2 reused for 1 over its square root, where a
         * scale of its own would do */
        length2 = fsrra(length2);
        out[0] *= length2;
        out[1] *= length2;
        out[2] *= length2;
    } else {
        out[0] = 0.0F;
        out[1] = 0.0F;
        out[2] = 1.0F;
    }
}

static void blend_point(NJS_POINT3 *p1, NJS_POINT3 *p2, NJS_POINT3 *out,
                        Float rate)
{
    Float s = 1.0F - rate;

    /* @hack spelling: s first in x's product, where p1->x * s as in y's and
     * z's would do */
    out->x = (s * p1->x) + (p2->x * rate);
    out->y = (p1->y * s) + (p2->y * rate);
    out->z = (p1->z * s) + (p2->z * rate);
}

/* the shape steps' curves. spline_normal blends four normals as spline_point
 * blends four points. it then scales the blend to length 1, as blend_normal
 * does */
/* @hack type: out a Float *, where an NJS_VECTOR * would do */
static void spline_normal(NJS_VECTOR *p0, NJS_VECTOR *p1, NJS_VECTOR *p2,
                          NJS_VECTOR *p3, Float t, Float *out)
{
    Float t2 = t * t;
    Float t3 = t * t2;
    /* the Catmull-Rom weights of p0, p1, p2 and p3 */
    Float w0 = ((2.0F * t2) - t - t3) * 0.5F;
    Float w1 = (1.5F * t3) - (2.5F * t2) + 1.0F;
    Float w2 = (0.5F * t) - (1.5F * t3) + (2.0F * t2);
    Float w3 = (t3 - t2) * 0.5F;
    Float length2;
    Float scale;

    out[0] = (p0->x * w0) + (p1->x * w1) + (p2->x * w2) + (p3->x * w3);
    out[1] = (p0->y * w0) + (p1->y * w1) + (p2->y * w2) + (p3->y * w3);
    out[2] = (p0->z * w0) + (p1->z * w1) + (p2->z * w2) + (p3->z * w3);
    length2 = (out[0] * out[0]) + (out[1] * out[1]) + (out[2] * out[2]);
    if (length2 > MIN_LENGTH2) {
        scale = fsrra(length2);
        out[0] *= scale;
        out[1] *= scale;
        out[2] *= scale;
    } else {
        out[0] = 0.0F;
        out[1] = 0.0F;
        out[2] = 1.0F;
    }
}

/* set out to the point at t on the curve from p1 to p2, as nj_cnk_shape.h's
 * njCnkInterpolatesSF3 does */
static void spline_point(NJS_POINT3 *p0, NJS_POINT3 *p1, NJS_POINT3 *p2,
                         NJS_POINT3 *p3, Float t, NJS_POINT3 *out)
{
    Float t2 = t * t;
    Float t3 = t * t2;
    /* the Catmull-Rom weights of p0, p1, p2 and p3 */
    Float w0 = ((2.0F * t2) - t - t3) * 0.5F;
    Float w1 = (1.5F * t3) - (2.5F * t2) + 1.0F;
    Float w2 = (0.5F * t) - (1.5F * t3) + (2.0F * t2);
    Float w3 = (t3 - t2) * 0.5F;

    out->x = (p0->x * w0) + (p1->x * w1) + (p2->x * w2) + (p3->x * w3);
    out->y = (p0->y * w0) + (p1->y * w1) + (p2->y * w2) + (p3->y * w3);
    out->z = (p0->z * w0) + (p1->z * w1) + (p2->z * w2) + (p3->z * w3);
}

/* the shape steps. each blends the keys of the entry a shape node step took
 * last into _nj_shape_buf_. it points the points or normals of object's model
 * at the blends while _nj_draw_model_sub_ draws the model. then it points
 * them back at the model's own. where object's model is null, or object is
 * under NJD_EVAL_HIDE, the step does nothing.
 *
 * where both factors have keys, the normals' blends lie nbPoint points past
 * the points'. a factor without keys leaves the model's own points or
 * normals */
static void spline_shape(NJS_OBJECT *object)
{
    NJS_POINT3 *points;
    NJS_VECTOR *normals;
    Sint32 nbpoint;
    Uint32 found;
    Float point_t;
    Float normal_t;
    void *point_keys[4];
    void *normal_keys[4];
    NJS_MKEY_P *keys;
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;
    Float *buf;
    /* @hack redundant: point_buf, a copy of buf, where buf would do */
    /* @hack type: point_buf, normal_buf and the keys' vertices as floats,
     * three to a vertex, where NJS_POINT3 and NJS_VECTOR pointers indexed by
     * vertex would do */
    Float *point_buf;
    Float *normal_buf;
    Float *p0;
    Float *p1;
    Float *p2;
    Float *p3;
    Float *n0;
    Float *n1;
    Float *n2;
    Float *n3;
    Sint32 i;

    found = 0;
    if (object->model && !(object->evalflags & NJD_EVAL_HIDE)) {
        points = object->model->points;
        normals = object->model->normals;
        nbpoint = object->model->nbPoint;
        switch (_nj_mot_shape_data_cnt_) {
        case SHAPE_POINTS:
            keys = _nj_shape_data_info_.p[_nj_shape_data_info_.factor];
            if (keys) {
                nb = _nj_shape_data_info_.nb[_nj_shape_data_info_.factor];
                lower = 0;
                upper = nb;
                frame = (Uint32)_nj_shape_data_info_.frame;
                while (upper - lower > 1) {
                    middle = (lower + upper) >> 1;
                    if (frame >= keys[middle].keyframe) {
                        lower = middle;
                    } else {
                        upper = middle;
                    }
                }
                point_t = spline_key_data(keys, lower, nb, point_keys);
                found += SHAPE_POINTS;
            }
            break;
        case SHAPE_NORMALS:
            keys = _nj_shape_data_info_.p[_nj_shape_data_info_.factor];
            if (keys) {
                nb = _nj_shape_data_info_.nb[_nj_shape_data_info_.factor];
                lower = 0;
                upper = nb;
                frame = (Uint32)_nj_shape_data_info_.frame;
                while (upper - lower > 1) {
                    middle = (lower + upper) >> 1;
                    if (frame >= keys[middle].keyframe) {
                        lower = middle;
                    } else {
                        upper = middle;
                    }
                }
                normal_t = spline_key_data(keys, lower, nb, normal_keys);
                found += SHAPE_NORMALS;
            }
            break;
        case SHAPE_POINTS + SHAPE_NORMALS:
            keys = _nj_shape_data_info_.p[_nj_shape_data_info_.factor];
            if (keys) {
                nb = _nj_shape_data_info_.nb[_nj_shape_data_info_.factor];
                lower = 0;
                upper = nb;
                frame = (Uint32)_nj_shape_data_info_.frame;
                while (upper - lower > 1) {
                    middle = (lower + upper) >> 1;
                    if (frame >= keys[middle].keyframe) {
                        lower = middle;
                    } else {
                        upper = middle;
                    }
                }
                point_t = spline_key_data(keys, lower, nb, point_keys);
                found += SHAPE_POINTS;
            }
            _nj_shape_data_info_.factor++;
            keys = _nj_shape_data_info_.p[_nj_shape_data_info_.factor];
            if (keys) {
                nb = _nj_shape_data_info_.nb[_nj_shape_data_info_.factor];
                lower = 0;
                upper = nb;
                frame = (Uint32)_nj_shape_data_info_.frame;
                while (upper - lower > 1) {
                    middle = (lower + upper) >> 1;
                    if (frame >= keys[middle].keyframe) {
                        lower = middle;
                    } else {
                        upper = middle;
                    }
                }
                normal_t = spline_key_data(keys, lower, nb, normal_keys);
                found += SHAPE_NORMALS;
            }
            break;
        }
        buf = _nj_shape_buf_;
        switch (found) {
        case SHAPE_POINTS:
            point_buf = buf;
            p0 = point_keys[0];
            p1 = point_keys[1];
            p2 = point_keys[2];
            p3 = point_keys[3];
            for (i = 0; i < nbpoint; i++) {
                spline_point((NJS_POINT3 *)&p0[i * SHAPE_VECTOR],
                             (NJS_POINT3 *)&p1[i * SHAPE_VECTOR],
                             (NJS_POINT3 *)&p2[i * SHAPE_VECTOR],
                             (NJS_POINT3 *)&p3[i * SHAPE_VECTOR], point_t,
                             (NJS_POINT3 *)&point_buf[i * SHAPE_VECTOR]);
            }
            object->model->points = (NJS_POINT3 *)point_buf;
            break;
        case SHAPE_NORMALS:
            normal_buf = buf;
            n0 = normal_keys[0];
            n1 = normal_keys[1];
            n2 = normal_keys[2];
            n3 = normal_keys[3];
            for (i = 0; i < nbpoint; i++) {
                spline_normal((NJS_VECTOR *)&n0[i * SHAPE_VECTOR],
                              (NJS_VECTOR *)&n1[i * SHAPE_VECTOR],
                              (NJS_VECTOR *)&n2[i * SHAPE_VECTOR],
                              (NJS_VECTOR *)&n3[i * SHAPE_VECTOR], normal_t,
                              &normal_buf[i * SHAPE_VECTOR]);
            }
            object->model->normals = (NJS_VECTOR *)normal_buf;
            break;
        case SHAPE_POINTS + SHAPE_NORMALS:
            point_buf = buf;
            normal_buf = &buf[nbpoint * SHAPE_VECTOR];
            p0 = point_keys[0];
            p1 = point_keys[1];
            p2 = point_keys[2];
            p3 = point_keys[3];
            n0 = normal_keys[0];
            n1 = normal_keys[1];
            n2 = normal_keys[2];
            n3 = normal_keys[3];
            for (i = 0; i < nbpoint; i++) {
                spline_point((NJS_POINT3 *)&p0[i * SHAPE_VECTOR],
                             (NJS_POINT3 *)&p1[i * SHAPE_VECTOR],
                             (NJS_POINT3 *)&p2[i * SHAPE_VECTOR],
                             (NJS_POINT3 *)&p3[i * SHAPE_VECTOR], point_t,
                             (NJS_POINT3 *)&point_buf[i * SHAPE_VECTOR]);
                spline_normal((NJS_VECTOR *)&n0[i * SHAPE_VECTOR],
                              (NJS_VECTOR *)&n1[i * SHAPE_VECTOR],
                              (NJS_VECTOR *)&n2[i * SHAPE_VECTOR],
                              (NJS_VECTOR *)&n3[i * SHAPE_VECTOR], normal_t,
                              &normal_buf[i * SHAPE_VECTOR]);
            }
            object->model->points = (NJS_POINT3 *)point_buf;
            object->model->normals = (NJS_VECTOR *)normal_buf;
            break;
        }
        _nj_draw_model_sub_(object->model);
        object->model->points = points;
        object->model->normals = normals;
    }
}

static void linear_shape(NJS_OBJECT *object)
{
    NJS_POINT3 *points;
    NJS_VECTOR *normals;
    Sint32 nbpoint;
    Uint32 found;
    Float point_t;
    Float normal_t;
    void *point_keys[2];
    void *normal_keys[2];
    NJS_MKEY_P *keys;
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;
    Float *buf;
    /* @hack type: normal_buf and the keys' vertices as floats, three to a
     * vertex, where NJS_POINT3 and NJS_VECTOR pointers indexed by vertex
     * would do */
    Float *normal_buf;
    Float *p0;
    Float *p1;
    Float *n0;
    Float *n1;
    Sint32 i;

    found = 0;
    if (object->model && !(object->evalflags & NJD_EVAL_HIDE)) {
        points = object->model->points;
        normals = object->model->normals;
        nbpoint = object->model->nbPoint;
        switch (_nj_mot_shape_data_cnt_) {
        case SHAPE_POINTS:
            keys = _nj_shape_data_info_.p[_nj_shape_data_info_.factor];
            if (keys) {
                nb = _nj_shape_data_info_.nb[_nj_shape_data_info_.factor];
                lower = 0;
                upper = nb;
                frame = (Uint32)_nj_shape_data_info_.frame;
                while (upper - lower > 1) {
                    middle = (lower + upper) >> 1;
                    if (frame >= keys[middle].keyframe) {
                        lower = middle;
                    } else {
                        upper = middle;
                    }
                }
                point_t = linear_key_data(keys, lower, nb, point_keys);
                found += SHAPE_POINTS;
            }
            break;
        case SHAPE_NORMALS:
            keys = _nj_shape_data_info_.p[_nj_shape_data_info_.factor];
            if (keys) {
                nb = _nj_shape_data_info_.nb[_nj_shape_data_info_.factor];
                lower = 0;
                upper = nb;
                frame = (Uint32)_nj_shape_data_info_.frame;
                while (upper - lower > 1) {
                    middle = (lower + upper) >> 1;
                    if (frame >= keys[middle].keyframe) {
                        lower = middle;
                    } else {
                        upper = middle;
                    }
                }
                normal_t = linear_key_data(keys, lower, nb, normal_keys);
                found += SHAPE_NORMALS;
            }
            break;
        case SHAPE_POINTS + SHAPE_NORMALS:
            keys = _nj_shape_data_info_.p[_nj_shape_data_info_.factor];
            if (keys) {
                nb = _nj_shape_data_info_.nb[_nj_shape_data_info_.factor];
                lower = 0;
                upper = nb;
                frame = (Uint32)_nj_shape_data_info_.frame;
                while (upper - lower > 1) {
                    middle = (lower + upper) >> 1;
                    if (frame >= keys[middle].keyframe) {
                        lower = middle;
                    } else {
                        upper = middle;
                    }
                }
                point_t = linear_key_data(keys, lower, nb, point_keys);
                found += SHAPE_POINTS;
            }
            _nj_shape_data_info_.factor++;
            keys = _nj_shape_data_info_.p[_nj_shape_data_info_.factor];
            if (keys) {
                nb = _nj_shape_data_info_.nb[_nj_shape_data_info_.factor];
                lower = 0;
                upper = nb;
                frame = (Uint32)_nj_shape_data_info_.frame;
                while (upper - lower > 1) {
                    middle = (lower + upper) >> 1;
                    if (frame >= keys[middle].keyframe) {
                        lower = middle;
                    } else {
                        upper = middle;
                    }
                }
                normal_t = linear_key_data(keys, lower, nb, normal_keys);
                found += SHAPE_NORMALS;
            }
            break;
        }
        buf = _nj_shape_buf_;
        switch (found) {
        case SHAPE_POINTS:
            p0 = point_keys[0];
            p1 = point_keys[1];
            for (i = 0; i < nbpoint; i++) {
                blend_point((NJS_POINT3 *)&p0[i * SHAPE_VECTOR],
                            (NJS_POINT3 *)&p1[i * SHAPE_VECTOR],
                            (NJS_POINT3 *)&buf[i * SHAPE_VECTOR], point_t);
            }
            object->model->points = (NJS_POINT3 *)buf;
            break;
        case SHAPE_NORMALS:
            normal_buf = buf;
            n0 = normal_keys[0];
            n1 = normal_keys[1];
            for (i = 0; i < nbpoint; i++) {
                blend_normal((NJS_VECTOR *)&n0[i * SHAPE_VECTOR],
                             (NJS_VECTOR *)&n1[i * SHAPE_VECTOR],
                             &normal_buf[i * SHAPE_VECTOR], normal_t);
            }
            object->model->normals = (NJS_VECTOR *)normal_buf;
            break;
        case SHAPE_POINTS + SHAPE_NORMALS:
            normal_buf = &buf[nbpoint * SHAPE_VECTOR];
            p0 = point_keys[0];
            p1 = point_keys[1];
            n0 = normal_keys[0];
            n1 = normal_keys[1];
            for (i = 0; i < nbpoint; i++) {
                blend_point((NJS_POINT3 *)&p0[i * SHAPE_VECTOR],
                            (NJS_POINT3 *)&p1[i * SHAPE_VECTOR],
                            (NJS_POINT3 *)&buf[i * SHAPE_VECTOR], point_t);
                blend_normal((NJS_VECTOR *)&n0[i * SHAPE_VECTOR],
                             (NJS_VECTOR *)&n1[i * SHAPE_VECTOR],
                             &normal_buf[i * SHAPE_VECTOR], normal_t);
            }
            object->model->points = (NJS_POINT3 *)buf;
            object->model->normals = (NJS_VECTOR *)normal_buf;
            break;
        }
        _nj_draw_model_sub_(object->model);
        object->model->points = points;
        object->model->normals = normals;
    }
}

void njMotionObjTranslate(NJS_OBJECT *object)
{
    njTranslate(_nj_motion_data_info_.m, object->pos[0], object->pos[1],
                object->pos[2]);
}

void njMotionObjRotateXYZ(NJS_OBJECT *object)
{
    njRotateXYZ(_nj_motion_data_info_.m, object->ang[0], object->ang[1],
                object->ang[2]);
}

void njMotionObjRotateZXY(NJS_OBJECT *object)
{
    njRotateZXY(_nj_motion_data_info_.m, object->ang[0], object->ang[1],
                object->ang[2]);
}

void njMotionObjScale(NJS_OBJECT *object)
{
    njScale(_nj_motion_data_info_.m, object->scl[0], object->scl[1],
            object->scl[2]);
}

/* the node steps, as njCnkMotSrc's njCnkMotionData0 to njCnkMotionData4 are.
 * motion_data0 takes no entry. each other step takes the entry at
 * _nj_motion_data_info_.mdata, of as many factors as its number */
static void motion_data0(NJS_OBJECT *object)
{
    _nj_motion_translate_(object);
    _nj_motion_rotate_(object);
    _nj_motion_scale_(object);
}

static void motion_data1(NJS_OBJECT *object)
{
    NJS_MDATA1 *entry = _nj_motion_data_info_.mdata;

    _nj_motion_data_info_.p = entry->p;
    _nj_motion_data_info_.nb = entry->nb;
    _nj_motion_data_info_.factor = 0;
    _nj_motion_translate_(object);
    _nj_motion_rotate_(object);
    _nj_motion_scale_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_motion_data_info_.mdata = entry;
}

static void motion_data2(NJS_OBJECT *object)
{
    NJS_MDATA2 *entry = _nj_motion_data_info_.mdata;

    _nj_motion_data_info_.p = entry->p;
    _nj_motion_data_info_.nb = entry->nb;
    _nj_motion_data_info_.factor = 0;
    _nj_motion_translate_(object);
    _nj_motion_rotate_(object);
    _nj_motion_scale_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_motion_data_info_.mdata = entry;
}

static void motion_data3(NJS_OBJECT *object)
{
    NJS_MDATA3 *entry = _nj_motion_data_info_.mdata;

    _nj_motion_data_info_.p = entry->p;
    _nj_motion_data_info_.nb = entry->nb;
    _nj_motion_data_info_.factor = 0;
    _nj_motion_translate_(object);
    _nj_motion_rotate_(object);
    _nj_motion_scale_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_motion_data_info_.mdata = entry;
}

static void motion_data4(NJS_OBJECT *object)
{
    NJS_MDATA4 *entry = _nj_motion_data_info_.mdata;

    _nj_motion_data_info_.p = entry->p;
    _nj_motion_data_info_.nb = entry->nb;
    _nj_motion_data_info_.factor = 0;
    _nj_motion_translate_(object);
    _nj_motion_rotate_(object);
    _nj_motion_scale_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_motion_data_info_.mdata = entry;
}

/* the shape node steps, as nj_cnk_shape.h says of the chunk ones. shape_data1
 * takes an entry of one factor, and shape_data2 an entry of two */
static void shape_data0(NJS_OBJECT *object)
{
    if (object->model && !(object->evalflags & NJD_EVAL_HIDE)) {
        _nj_draw_model_sub_(object->model);
    }
}

static void shape_data1(NJS_OBJECT *object)
{
    NJS_MDATA1 *entry = _nj_shape_data_info_.mdata;

    _nj_shape_data_info_.p = entry->p;
    _nj_shape_data_info_.nb = entry->nb;
    _nj_shape_data_info_.factor = 0;
    _nj_motion_shape_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_shape_data_info_.mdata = entry;
}

static void shape_data2(NJS_OBJECT *object)
{
    NJS_MDATA2 *entry = _nj_shape_data_info_.mdata;

    _nj_shape_data_info_.p = entry->p;
    _nj_shape_data_info_.nb = entry->nb;
    _nj_shape_data_info_.factor = 0;
    _nj_motion_shape_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_shape_data_info_.mdata = entry;
}

void njInitShape(Float *buf)
{
    _nj_shape_buf_ = buf;
}

void njSetMotionCallback(void (*func)(NJS_OBJECT *object))
{
    _nj_motion_callback_ = func;
}

static void linear_angles_short(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                                Angle *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_A *key = &keys[index];
    NJS_MKEY_A *next = &keys[(index + 1) % nb];
    Float t;
    Float s;
    /* the next key's angles. each is moved a turn toward key's where the two
     * lie over half a turn apart. so keys less than a turn and a half apart
     * blend the shorter way */
    Angle x;
    Angle y;
    Angle z;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) /
        divisor[(Sint32)(next->keyframe - key->keyframe) > 0];
    s = 1.0F - t;
    x = next->key[0] - key->key[0];
    if (x > HALF_TURN) {
        x -= TURN;
    } else if (x < -HALF_TURN) {
        x += TURN;
    }
    x += key->key[0];
    y = next->key[1] - key->key[1];
    if (y > HALF_TURN) {
        y -= TURN;
    } else if (y < -HALF_TURN) {
        y += TURN;
    }
    y += key->key[1];
    z = next->key[2] - key->key[2];
    if (z > HALF_TURN) {
        z -= TURN;
    } else if (z < -HALF_TURN) {
        z += TURN;
    }
    z += key->key[2];
    out[0] = (Angle)(((Float)key->key[0] * s) + ((Float)x * t));
    out[1] = (Angle)(((Float)key->key[1] * s) + ((Float)y * t));
    out[2] = (Angle)(((Float)key->key[2] * s) + ((Float)z * t));
}
