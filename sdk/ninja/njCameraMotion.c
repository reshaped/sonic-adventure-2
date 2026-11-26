/* the camera motions, their channels and their interpolations */
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the camera motions' interpolations. each reads its keys as nj_motion.h's
 * interpolations do, but at _nj_cmotion_data_info_.frame. linear_angle and
 * linear_vector take the straight line from key index to the next.
 * overhauser_vector takes njOverhauserSpline's curve. its four points are
 * the key before index, key index, the next key and the one after */
static void linear_angle(NJS_MKEY_A1 *keys, Uint32 index, Uint32 nb, Angle *out)
{
    Float span[2] = {1.0F, 1.0F};
    NJS_MKEY_A1 *key = &keys[index];
    NJS_MKEY_A1 *next = &keys[(index + 1) % nb];
    Float t;
    Float s;

    /* @hack spelling: the divisor read from span at the index the test
     * gives, where an if would choose it */
    span[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_cmotion_data_info_.frame - (Float)key->keyframe) /
        span[(Sint32)(next->keyframe - key->keyframe) > 0];
    s = 1.0F - t;
#ifdef NONMATCHING
    /* SHC rounds next->key * t on its own, by FMUL. it then fuses
     * key->key * s into the sum by FMAC, which rounds once */
    *out = (Angle)__builtin_fmaf((Float)key->key, s, (Float)next->key * t);
#else
    *out = (Angle)(((Float)key->key * s) + ((Float)next->key * t));
#endif
}

static void linear_vector(NJS_MKEY_F *keys, Uint32 index, Uint32 nb, Float *out)
{
    Float span[2] = {1.0F, 1.0F};
    NJS_MKEY_F *key = &keys[index];
    NJS_MKEY_F *next = &keys[(index + 1) % nb];
    Float t;
    Float s;

    /* @hack spelling: the divisor read from span at the index the test
     * gives, where an if would choose it */
    span[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_cmotion_data_info_.frame - (Float)key->keyframe) /
        span[(Sint32)(next->keyframe - key->keyframe) > 0];
    s = 1.0F - t;
#ifdef NONMATCHING
    /* fused as linear_angle's sum is */
    out[0] = __builtin_fmaf(key->key[0], s, next->key[0] * t);
    out[1] = __builtin_fmaf(key->key[1], s, next->key[1] * t);
    out[2] = __builtin_fmaf(key->key[2], s, next->key[2] * t);
#else
    out[0] = (key->key[0] * s) + (next->key[0] * t);
    out[1] = (key->key[1] * s) + (next->key[1] * t);
    out[2] = (key->key[2] * s) + (next->key[2] * t);
#endif
}

static void overhauser_vector(NJS_MKEY_F *keys, Uint32 index, Uint32 nb,
                              Float *out)
{
    Float span[2] = {1.0F, 1.0F};
    NJS_MKEY_F *before = &keys[(index + nb - 1) % nb];
    NJS_MKEY_F *key = &keys[index];
    NJS_MKEY_F *next = &keys[(index + 1) % nb];
    NJS_MKEY_F *after = &keys[(index + 2) % nb];
    Float t;
    Float square;
    Float cube;
    Float w0;
    Float w1;
    Float w2;
    Float w3;

    /* @hack spelling: the divisor read from span at the index the test
     * gives, where an if would choose it */
    span[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_cmotion_data_info_.frame - (Float)key->keyframe) /
        span[(Sint32)(next->keyframe - key->keyframe) > 0];
    square = t * t;
    cube = square * t;
    w0 = ((2.0F * square) - t - cube) * 0.5F;
    w1 = (1.5F * cube) - (2.5F * square) + 1.0F;
    w2 = (0.5F * t) - (1.5F * cube) + (2.0F * square);
    w3 = (cube - square) * 0.5F;
#ifdef NONMATCHING
    /* SHC rounds w1's product with key's on its own, by FMUL. it then adds
     * w0's, w2's and w3's products to it in that order. each goes by an FMAC,
     * which rounds once */
    out[0] = __builtin_fmaf(
        w3, after->key[0],
        __builtin_fmaf(w2, next->key[0],
                       __builtin_fmaf(w0, before->key[0], w1 * key->key[0])));
    out[1] = __builtin_fmaf(
        w3, after->key[1],
        __builtin_fmaf(w2, next->key[1],
                       __builtin_fmaf(w0, before->key[1], w1 * key->key[1])));
    out[2] = __builtin_fmaf(
        w3, after->key[2],
        __builtin_fmaf(w2, next->key[2],
                       __builtin_fmaf(w0, before->key[2], w1 * key->key[2])));
#else
    out[0] = (w0 * before->key[0]) + (w1 * key->key[0]) + (w2 * next->key[0]) +
             (w3 * after->key[0]);
    out[1] = (w0 * before->key[1]) + (w1 * key->key[1]) + (w2 * next->key[1]) +
             (w3 * after->key[1]);
    out[2] = (w0 * before->key[2]) + (w1 * key->key[2]) + (w2 * next->key[2]) +
             (w3 * after->key[2]);
#endif
}

void (*_nj_cmotion_int_f_func_table_[INTERPOLATIONS])(NJS_MKEY_F *keys,
                                                      Uint32 index, Uint32 nb,
                                                      Float *out) = {
    linear_vector, overhauser_vector};
void (*_nj_cmotion_int_a_func_table_[1])(NJS_MKEY_A1 *keys, Uint32 index,
                                         Uint32 nb,
                                         Angle *out) = {linear_angle};

/* a channel reads the factor that _nj_cmotion_data_info_.factor indexes, and
 * adds 1 to factor. where the factor has keys, its value at the frame sets a
 * part of _nj_motion_camera_ */
static void position_channel(void)
{
    Uint32 factor = _nj_cmotion_data_info_.factor;
    NJS_MKEY_F *keys = _nj_cmotion_data_info_.p[factor];
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    /* @hack redundant: register on upper and middle */
    register Uint32 upper;
    register Uint32 middle;
    Float value[3];

    if (keys) {
        nb = _nj_cmotion_data_info_.nb[factor];
        lower = 0;
        upper = nb;
        frame = (Uint32)_nj_cmotion_data_info_.frame;
        FIND_KEY(keys, frame, lower, upper, middle);
        _nj_cmotion_int_f_func_(keys, lower, nb, value);
        _nj_motion_camera_.px = value[0];
        _nj_motion_camera_.py = value[1];
        _nj_motion_camera_.pz = value[2];
    }
    _nj_cmotion_data_info_.factor++;
}

static void vector_channel(void)
{
    Uint32 factor = _nj_cmotion_data_info_.factor;
    NJS_MKEY_F *keys = _nj_cmotion_data_info_.p[factor];
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;
    Float value[3];

    if (keys) {
        nb = _nj_cmotion_data_info_.nb[factor];
        lower = 0;
        upper = nb;
        /* @hack redundant: a test of upper that never runs */
        if (0) {
            if (upper) {
            }
        }
        frame = (Uint32)_nj_cmotion_data_info_.frame;
        FIND_KEY(keys, frame, lower, upper, middle);
        _nj_cmotion_int_l_func_(keys, lower, nb, value);
        value[0] += _nj_motion_camera_.px;
        value[1] += _nj_motion_camera_.py;
        value[2] += _nj_motion_camera_.pz;
        njPointCameraInterest(&_nj_motion_camera_, value[0], value[1],
                              value[2]);
    }
    _nj_cmotion_data_info_.factor++;
}

static void target_channel(void)
{
    Uint32 factor = _nj_cmotion_data_info_.factor;
    NJS_MKEY_F *keys = _nj_cmotion_data_info_.p[factor];
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;
    Float value[3];

    if (keys) {
        nb = _nj_cmotion_data_info_.nb[factor];
        lower = 0;
        upper = nb;
        frame = (Uint32)_nj_cmotion_data_info_.frame;
        FIND_KEY(keys, frame, lower, upper, middle);
        _nj_cmotion_int_l_func_(keys, lower, nb, value);
        njPointCameraInterest(&_nj_motion_camera_, value[0], value[1],
                              value[2]);
    }
    _nj_cmotion_data_info_.factor++;
}

static void roll_channel(void)
{
    Uint32 factor = _nj_cmotion_data_info_.factor;
    NJS_MKEY_A1 *keys = _nj_cmotion_data_info_.p[factor];
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;
    Angle value;

    if (keys) {
        nb = _nj_cmotion_data_info_.nb[factor];
        lower = 0;
        upper = nb;
        frame = (Uint32)_nj_cmotion_data_info_.frame;
        FIND_KEY(keys, frame, lower, upper, middle);
        _nj_cmotion_int_a_func_(keys, lower, nb, &value);
        _nj_motion_camera_.roll = 0;
        njRollCameraInterest(&_nj_motion_camera_, value);
    }
    _nj_cmotion_data_info_.factor++;
}

static void angle_channel(void)
{
    Uint32 factor = _nj_cmotion_data_info_.factor;
    NJS_MKEY_A1 *keys = _nj_cmotion_data_info_.p[factor];
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;
    Angle value;

    if (keys) {
        nb = _nj_cmotion_data_info_.nb[factor];
        lower = 0;
        upper = nb;
        frame = (Uint32)_nj_cmotion_data_info_.frame;
        FIND_KEY(keys, frame, lower, upper, middle);
        _nj_cmotion_int_a_func_(keys, lower, nb, &value);
        njSetCameraAngle(&_nj_motion_camera_, value);
    }
    _nj_cmotion_data_info_.factor++;
}

/* run the channel of each factor that type names, in the order of the type's
 * bits. roll is the roll's channel. the other factors' channels only step past
 * them */
#define RUN_CHANNELS(type, roll)                                               \
    {                                                                          \
        if ((type) & NJD_MTYPE_POS_0) {                                        \
            position_channel();                                                \
        }                                                                      \
        if ((type) & NJD_MTYPE_ANG_1) {                                        \
            _nj_cmotion_data_info_.factor++;                                   \
        }                                                                      \
        if ((type) & NJD_MTYPE_SCL_2) {                                        \
            _nj_cmotion_data_info_.factor++;                                   \
        }                                                                      \
        if ((type) & NJD_MTYPE_TARGET_3) {                                     \
            target_channel();                                                  \
        } else if ((type) & NJD_MTYPE_VEC_3) {                                 \
            vector_channel();                                                  \
        }                                                                      \
        if ((type) & NJD_MTYPE_ROLL_6) {                                       \
            roll();                                                            \
        }                                                                      \
        if ((type) & NJD_MTYPE_ANGLE_7) {                                      \
            angle_channel();                                                   \
        }                                                                      \
        if ((type) & NJD_MTYPE_RGB_8) {                                        \
            _nj_cmotion_data_info_.factor++;                                   \
        }                                                                      \
        if ((type) & NJD_MTYPE_INTENSITY_9) {                                  \
            _nj_cmotion_data_info_.factor++;                                   \
        }                                                                      \
        if ((type) & NJD_MTYPE_SPOT_10) {                                      \
            _nj_cmotion_data_info_.factor++;                                   \
        } else if ((type) & NJD_MTYPE_POINT_10) {                              \
            _nj_cmotion_data_info_.factor++;                                   \
        }                                                                      \
    }

void njCameraAction(NJS_CACTION *caction, Float frame)
{
    NJS_CAMERA *camera = caction->camera;
    NJS_MOTION *motion = caction->motion;
    Uint16 type;
    Uint16 factors;
    Uint16 spline;
    void *mdata;

    if (!motion) {
        return;
    }
    if (frame >= (Float)motion->nbFrame) {
        return;
    }
    START_CAMERA_MOTION(camera, motion, frame, type, factors, spline, mdata);
    switch (factors) {
        CAMERA_ENTRY_CASE(1, mdata);
        CAMERA_ENTRY_CASE(2, mdata);
        CAMERA_ENTRY_CASE(3, mdata);
        CAMERA_ENTRY_CASE(4, mdata);
        CAMERA_ENTRY_CASE(5, mdata);
    default:
        return;
    }
    RUN_CHANNELS(type, roll_channel);
    njSetCamera(&_nj_motion_camera_);
}

void njCameraMotion(NJS_CAMERA *camera, NJS_MOTION *motion, Float frame)
{
    Uint16 type;
    Uint16 factors;
    Uint16 spline;
    void *mdata;

    if (!motion) {
        return;
    }
    if (frame >= (Float)motion->nbFrame) {
        return;
    }
    START_CAMERA_MOTION(camera, motion, frame, type, factors, spline, mdata);
    switch (factors) {
        CAMERA_ENTRY_CASE(1, mdata);
        CAMERA_ENTRY_CASE(2, mdata);
        CAMERA_ENTRY_CASE(3, mdata);
        CAMERA_ENTRY_CASE(4, mdata);
    default:
        return;
    }
    RUN_CHANNELS(type, roll_channel);
    njSetCamera(&_nj_motion_camera_);
}

static void old_roll_channel(void)
{
    Uint32 factor = _nj_cmotion_data_info_.factor;
    NJS_MKEY_A1 *keys = _nj_cmotion_data_info_.p[factor];
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;
    Angle value;

    if (keys) {
        nb = _nj_cmotion_data_info_.nb[factor];
        lower = 0;
        upper = nb;
        frame = (Uint32)_nj_cmotion_data_info_.frame;
        FIND_KEY(keys, frame, lower, upper, middle);
        _nj_cmotion_int_a_func_(keys, lower, nb, &value);
        njRollCameraInterest(&_nj_motion_camera_, value);
    }
    _nj_cmotion_data_info_.factor++;
}

void njCameraActionOld(NJS_CACTION *caction, Float frame)
{
    NJS_CAMERA *camera = caction->camera;
    NJS_MOTION *motion = caction->motion;
    Uint16 type;
    Uint16 factors;
    Uint16 spline;
    void *mdata;

    if (!motion) {
        return;
    }
    if (frame >= (Float)motion->nbFrame) {
        return;
    }
    START_CAMERA_MOTION(camera, motion, frame, type, factors, spline, mdata);
    switch (factors) {
        CAMERA_ENTRY_CASE(1, mdata);
        CAMERA_ENTRY_CASE(2, mdata);
        CAMERA_ENTRY_CASE(3, mdata);
        CAMERA_ENTRY_CASE(4, mdata);
        CAMERA_ENTRY_CASE(5, mdata);
    default:
        return;
    }
    RUN_CHANNELS(type, old_roll_channel);
    njSetCamera(&_nj_motion_camera_);
}

void njCameraMotionOld(NJS_CAMERA *camera, NJS_MOTION *motion, Float frame)
{
    Uint16 type;
    Uint16 factors;
    Uint16 spline;
    void *mdata;

    if (!motion) {
        return;
    }
    if (frame >= (Float)motion->nbFrame) {
        return;
    }
    START_CAMERA_MOTION(camera, motion, frame, type, factors, spline, mdata);
    switch (factors) {
        CAMERA_ENTRY_CASE(1, mdata);
        CAMERA_ENTRY_CASE(2, mdata);
        CAMERA_ENTRY_CASE(3, mdata);
        CAMERA_ENTRY_CASE(4, mdata);
    default:
        return;
    }
    RUN_CHANNELS(type, old_roll_channel);
    njSetCamera(&_nj_motion_camera_);
}
