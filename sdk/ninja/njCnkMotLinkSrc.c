/* the chunk motion links' steps, their interpolations between keys and
 * between the two slots, and njCnkMakeMotionLink */
#include <nj_cnk_motlink.h>
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* slot's bit in an int flag such as _nj_pos_int_flag_, and both slots' */
#define SLOT_BIT(slot) ((slot) + 1)
#define BOTH_SLOTS 3
/* a third, to five decimal places */
#define THIRD 0.33333F

/* the member exports these functions and data, though no header of the SDK
 * declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set q to the quaternion of a rotation, in NJS_QUATERNION's order. the
 * rotation is by ang[2] about z, then by ang[1] about y, then by ang[0] about
 * x. where zxy isn't 0, ang[1] and ang[2] trade places.
 *
 * q's real part is half the square root of 1 plus the trace of the rotation's
 * matrix. each other part is a difference of two of the matrix's elements,
 * over 4 times the real part. so q loses precision for a rotation of nearly
 * half a turn. at half a turn the real part is 0, and the other parts divide
 * by it */
void njAng2Quat(const Angle *ang, Float *q, Sint32 zxy);
/* set out[0] to out[2] to the point at t of a Catmull-Rom curve. keys, index,
 * nb and t are as nj_motion.h says for its interpolations, at the current
 * slot's frame. the curve runs from key index's three floats to the next
 * key's. its tangent at a key is half of the key after it less the key before
 * it. the key before key 0 is the last. the three floats LINK_TANGENTS past out
 * get the curve's tangents at the two keys, blended by t.
 *
 * njCnkOverhauserMotLinkA does the same for three angles, as floats,
 * truncating each toward 0 */
void njCnkOverhauserMotLinkF(NJS_MKEY_F *keys, Uint32 index, Uint32 nb,
                             Float *out);
void njCnkOverhauserMotLinkA(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                             Angle *out);
/* set out[0] to out[2] to the point at t of a cubic Bezier curve. keys, index,
 * nb and t are as nj_motion.h says for its interpolations, at the current
 * slot's frame. the curve runs from key index's three floats to the next key's.
 * the span is the frames between the two keys. the first inner control point
 * lies a third of the span along key index's tangent. the second lies as far
 * back along the next key's tangent.
 *
 * a key's tangent is a weighted mean of the slopes of the spans either side of
 * it. a span's slope is its change in value over its frames. each slope's
 * weight is the other span's frames over both spans' frames. where index is
 * nb - 2, the next key's tangent is twice the span's slope less key index's.
 * where index is 0 and not nb - 2, key index's tangent is twice that slope less
 * the next key's. a span of no frames, or a negative one, counts as one frame.
 *
 * njCnkBezierMotLinkA does the same for three angles, as floats, truncating
 * each toward 0. njCnkBezierMotLinkAOld first takes a turn from each difference
 * of two keys' angles above HALF_TURN. it adds a turn to each at -HALF_TURN or
 * below */
void njCnkBezierMotLinkF(NJS_MKEY_F *keys, Uint32 index, Uint32 nb, Float *out);
void njCnkBezierMotLinkA(NJS_MKEY_A *keys, Uint32 index, Uint32 nb, Angle *out);
void njCnkBezierMotLinkAOld(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                            Angle *out);
/* set out as njCnkLinearMotionQ does, at the current slot's frame */
void njCnkLinearMotLinkQ(NJS_MKEY_QUAT *keys, Uint32 index, Uint32 nb,
                         Float *out);
/* set out to the two slots' quaternions blended by rate, as
 * njCnkLinearMotLinkQ blends two keys' by t. each slot's lies at LINK_QUA(slot)
 * in values */
void njCnkLinearIntQ(const Float *values, Float *out, Float rate);
/* set out to the point at rate of a cubic Hermite curve between the two
 * slots. values holds each slot's three floats and tangent as nj_cnk_motlink.h
 * lays out _nj_motion_link_pos_. njCnkHermiteIntA does the same for angles, as
 * floats, truncating each toward 0 */
void njCnkHermiteIntF(const Float *values, Float *out, Float rate);
void njCnkHermiteIntA(const Angle *values, Angle *out, Float rate);
/* the node steps of _nj_cnk_motlink_data_func_table_, for 0 to 4 factors */
void njCnkMotLinkData0(NJS_CNK_OBJECT *object);
void njCnkMotLinkData1(NJS_CNK_OBJECT *object);
void njCnkMotLinkData2(NJS_CNK_OBJECT *object);
void njCnkMotLinkData3(NJS_CNK_OBJECT *object);
void njCnkMotLinkData4(NJS_CNK_OBJECT *object);

void (*_nj_cnk_motlink_data_func_table_[NODE_STEPS])(NJS_CNK_OBJECT *object) = {
    njCnkMotLinkData0, njCnkMotLinkData1, njCnkMotLinkData2,
    njCnkMotLinkData3, njCnkMotLinkData4,
};
void (*_nj_cnk_motlink_int_f_func_table_[INTERPOLATIONS])(NJS_MKEY_F *keys,
                                                          Uint32 index,
                                                          Uint32 nb,
                                                          Float *out) = {
    njCnkLinearMotLinkF,
    njCnkOverhauserMotLinkF,
};
void (*_nj_cnk_motlink_int_a_func_table_[INTERPOLATIONS])(NJS_MKEY_A *keys,
                                                          Uint32 index,
                                                          Uint32 nb,
                                                          Angle *out) = {
    njCnkLinearMotLinkA,
    njCnkLinearMotLinkA,
};
void (*_nj_cnk_int_f_func_table_[INTERPOLATIONS])(const Float *values,
                                                  Float *out, Float rate) = {
    njCnkLinearIntF,
    njCnkHermiteIntF,
};
void (*_nj_cnk_int_a_func_table_[INTERPOLATIONS])(const Angle *values,
                                                  Angle *out, Float rate) = {
    njCnkLinearIntA,
    njCnkLinearIntA,
};
/* the angle tables of the Old interpolations. each holds the straight line's
 * njCnkLinearMotLinkAOld or njCnkLinearIntAOld in the spline's place too */
void (*_nj_cnk_motlink_int_a_func_table_old_[INTERPOLATIONS])(NJS_MKEY_A *keys,
                                                              Uint32 index,
                                                              Uint32 nb,
                                                              Angle *out) = {
    njCnkLinearMotLinkAOld,
    njCnkLinearMotLinkAOld,
};
void (*_nj_cnk_int_a_func_table_old_[INTERPOLATIONS])(const Angle *values,
                                                      Angle *out,
                                                      Float rate) = {
    njCnkLinearIntAOld,
    njCnkLinearIntAOld,
};
/* NOLINTEND(misc-use-internal-linkage) */

void (*_nj_cnk_motlink_data_func_[2])(NJS_CNK_OBJECT *object);
void (*_nj_cnk_motlink_translate_data_[2])(NJS_CNK_OBJECT *object);
void (*_nj_cnk_motlink_rotate_data_[2])(NJS_CNK_OBJECT *object);
void (*_nj_cnk_motlink_scale_data_[2])(NJS_CNK_OBJECT *object);

void njCnkLinearMotLinkF(NJS_MKEY_F *keys, Uint32 index, Uint32 nb, Float *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_F *key = &keys[index];
    NJS_MKEY_F *next = &keys[(index + 1) % nb];
    Float t;
    Float s;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motlink_data_info_[_nj_mot_current_data_].frame -
         (Float)key->keyframe) /
        divisor[(Sint32)(next->keyframe - key->keyframe) > 0];
    s = 1.0F - t;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses each product by s
     * into its sum by an FMAC */
    out[0] = __builtin_fmaf(key->key[0], s, next->key[0] * t);
    out[1] = __builtin_fmaf(key->key[1], s, next->key[1] * t);
    out[2] = __builtin_fmaf(key->key[2], s, next->key[2] * t);
#else
    out[0] = (key->key[0] * s) + (next->key[0] * t);
    out[1] = (key->key[1] * s) + (next->key[1] * t);
    out[2] = (key->key[2] * s) + (next->key[2] * t);
#endif
}

void njCnkLinearMotLinkA(NJS_MKEY_A *keys, Uint32 index, Uint32 nb, Angle *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_A *key = &keys[index];
    NJS_MKEY_A *next = &keys[(index + 1) % nb];
    Float t;
    Float s;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motlink_data_info_[_nj_mot_current_data_].frame -
         (Float)key->keyframe) /
        divisor[(Sint32)(next->keyframe - key->keyframe) > 0];
    s = 1.0F - t;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses each product by s
     * into its sum by an FMAC */
    out[0] =
        (Angle)__builtin_fmaf((Float)key->key[0], s, (Float)next->key[0] * t);
    out[1] =
        (Angle)__builtin_fmaf((Float)key->key[1], s, (Float)next->key[1] * t);
    out[2] =
        (Angle)__builtin_fmaf((Float)key->key[2], s, (Float)next->key[2] * t);
#else
    out[0] = (Angle)(((Float)key->key[0] * s) + ((Float)next->key[0] * t));
    out[1] = (Angle)(((Float)key->key[1] * s) + ((Float)next->key[1] * t));
    out[2] = (Angle)(((Float)key->key[2] * s) + ((Float)next->key[2] * t));
#endif
}

void njAng2Quat(const Angle *ang, Float *q, Sint32 zxy)
{
    Angle about_x;
    Angle about_z;
    Angle about_y;
    /* the cosines and sines of the rotations about z, y and x, in that order */
    Float c[3];
    Float s[3];
    Float re;
    /* 1 plus the trace of the rotation's matrix */
    Float trace;
    /* each of q[1] to q[3] in turn, times 4 times the real part */
    Float numerator;

    /* @bug logic: rotates in another order than njRotateEx does for the same
     * angles. njRotateEx rotates by ang[0] about x first, or by ang[2] about
     * z first given zxy as its order. so where a link blends a slot of angles
     * with a slot of quaternions, a rate that picks the angles' slot poses
     * the node otherwise than that slot's motion does (FSUB FR1,FR3 at
     * 0x8c11f73c subtracts the sines' product in the trace, and MOV.L @R5,R12
     * at 0x8c11f6d2 takes ang[1] for the rotation about z under zxy) */
    about_x = ang[0];
    if (zxy) {
        about_z = ang[1];
        about_y = ang[2];
    } else {
        /* @hack spelling: about_y set before about_z, where the other arm's
         * order would do */
        about_y = ang[1];
        about_z = ang[2];
    }
    c[0] = njCos(about_z);
    s[0] = njSin(about_z);
    c[1] = njCos(about_y);
    s[1] = njSin(about_y);
    c[2] = njCos(about_x);
    s[2] = njSin(about_x);
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses the two products by
     * c[2] into the sum by FMACs */
    trace =
        __builtin_fmaf(
            c[2], c[0],
            __builtin_fmaf(c[2], c[1], (c[0] * c[1]) - (s[0] * s[1] * s[2]))) +
        1.0F;
#else
    trace = (c[0] * c[1]) - (s[0] * s[1] * s[2]) + (c[2] * c[1]) +
            (c[2] * c[0]) + 1.0F;
#endif
    re = _builtin_sqrtf(trace) * 0.5F;
    q[0] = re;
    /* @hack spelling: re reused for 0.25F / re, where a local of its own
     * would do */
    re = 0.25F / re;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses the product by s[2]
     * into the sum by an FMAC */
    numerator = __builtin_fmaf(s[2], c[0] + c[1], s[0] * s[1] * c[2]);
#else
    numerator = (s[2] * (c[0] + c[1])) + (s[0] * s[1] * c[2]);
#endif
    q[1] = re * numerator;
    numerator = (c[0] * s[1] * c[2]) - (s[0] * s[2]) + s[1];
    q[2] = re * numerator;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses the product by s[0]
     * into the sum by an FMAC */
    numerator = __builtin_fmaf(s[0], c[1] + c[2], c[0] * s[1] * s[2]);
#else
    numerator = (s[0] * (c[1] + c[2])) + (c[0] * s[1] * s[2]);
#endif
    q[3] = re * numerator;
}

void njCnkOverhauserMotLinkF(NJS_MKEY_F *keys, Uint32 index, Uint32 nb,
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
    Float wa;
    Float wb;
    Float wc;
    Float wd;
    Float s;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motlink_data_info_[_nj_mot_current_data_].frame -
         (Float)key->keyframe) /
        divisor[(Sint32)(next->keyframe - key->keyframe) > 0];
    t2 = t * t;
    t3 = t2 * t;
    wa = ((2.0F * t2) - t - t3) * 0.5F;
    wb = (1.5F * t3) - (2.5F * t2) + 1.0F;
    wc = (0.5F * t) - (1.5F * t3) + (2.0F * t2);
    wd = (t3 - t2) * 0.5F;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses the products by wa,
     * wc and wd into each sum by FMACs */
    out[0] = __builtin_fmaf(
        wd, after->key[0],
        __builtin_fmaf(wc, next->key[0],
                       __builtin_fmaf(wa, before->key[0], wb * key->key[0])));
    out[1] = __builtin_fmaf(
        wd, after->key[1],
        __builtin_fmaf(wc, next->key[1],
                       __builtin_fmaf(wa, before->key[1], wb * key->key[1])));
    out[2] = __builtin_fmaf(
        wd, after->key[2],
        __builtin_fmaf(wc, next->key[2],
                       __builtin_fmaf(wa, before->key[2], wb * key->key[2])));
#else
    out[0] = (wa * before->key[0]) + (wb * key->key[0]) + (wc * next->key[0]) +
             (wd * after->key[0]);
    out[1] = (wa * before->key[1]) + (wb * key->key[1]) + (wc * next->key[1]) +
             (wd * after->key[1]);
    out[2] = (wa * before->key[2]) + (wb * key->key[2]) + (wc * next->key[2]) +
             (wd * after->key[2]);
#endif
    t *= 0.5F;
    s = 0.5F - t;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses each product by s
     * into its sum by an FMAC */
    out[LINK_TANGENTS] = __builtin_fmaf(next->key[0] - before->key[0], s,
                                        (after->key[0] - key->key[0]) * t);
    out[LINK_TANGENTS + 1] = __builtin_fmaf(next->key[1] - before->key[1], s,
                                            (after->key[1] - key->key[1]) * t);
    out[LINK_TANGENTS + 2] = __builtin_fmaf(next->key[2] - before->key[2], s,
                                            (after->key[2] - key->key[2]) * t);
#else
    out[LINK_TANGENTS] = ((next->key[0] - before->key[0]) * s) +
                         ((after->key[0] - key->key[0]) * t);
    out[LINK_TANGENTS + 1] = ((next->key[1] - before->key[1]) * s) +
                             ((after->key[1] - key->key[1]) * t);
    out[LINK_TANGENTS + 2] = ((next->key[2] - before->key[2]) * s) +
                             ((after->key[2] - key->key[2]) * t);
#endif
}

void njCnkOverhauserMotLinkA(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
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
    Float wa;
    Float wb;
    Float wc;
    Float wd;
    Float s;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motlink_data_info_[_nj_mot_current_data_].frame -
         (Float)key->keyframe) /
        divisor[(Sint32)(next->keyframe - key->keyframe) > 0];
    t2 = t * t;
    t3 = t2 * t;
    wa = ((2.0F * t2) - t - t3) * 0.5F;
    wb = (1.5F * t3) - (2.5F * t2) + 1.0F;
    wc = (0.5F * t) - (1.5F * t3) + (2.0F * t2);
    wd = (t3 - t2) * 0.5F;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses the products by wa,
     * wc and wd into each sum by FMACs */
    out[0] = (Angle)__builtin_fmaf(
        wd, (Float)after->key[0],
        __builtin_fmaf(wc, (Float)next->key[0],
                       __builtin_fmaf(wa, (Float)before->key[0],
                                      wb * (Float)key->key[0])));
    out[1] = (Angle)__builtin_fmaf(
        wd, (Float)after->key[1],
        __builtin_fmaf(wc, (Float)next->key[1],
                       __builtin_fmaf(wa, (Float)before->key[1],
                                      wb * (Float)key->key[1])));
    out[2] = (Angle)__builtin_fmaf(
        wd, (Float)after->key[2],
        __builtin_fmaf(wc, (Float)next->key[2],
                       __builtin_fmaf(wa, (Float)before->key[2],
                                      wb * (Float)key->key[2])));
#else
    out[0] = (Angle)((wa * (Float)before->key[0]) + (wb * (Float)key->key[0]) +
                     (wc * (Float)next->key[0]) + (wd * (Float)after->key[0]));
    out[1] = (Angle)((wa * (Float)before->key[1]) + (wb * (Float)key->key[1]) +
                     (wc * (Float)next->key[1]) + (wd * (Float)after->key[1]));
    out[2] = (Angle)((wa * (Float)before->key[2]) + (wb * (Float)key->key[2]) +
                     (wc * (Float)next->key[2]) + (wd * (Float)after->key[2]));
#endif
    t *= 0.5F;
    s = 0.5F - t;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses each product by s
     * into its sum by an FMAC */
    out[LINK_TANGENTS] =
        (Angle)__builtin_fmaf((Float)(next->key[0] - before->key[0]), s,
                              (Float)(after->key[0] - key->key[0]) * t);
    out[LINK_TANGENTS + 1] =
        (Angle)__builtin_fmaf((Float)(next->key[1] - before->key[1]), s,
                              (Float)(after->key[1] - key->key[1]) * t);
    out[LINK_TANGENTS + 2] =
        (Angle)__builtin_fmaf((Float)(next->key[2] - before->key[2]), s,
                              (Float)(after->key[2] - key->key[2]) * t);
#else
    out[LINK_TANGENTS] = (Angle)(((Float)(next->key[0] - before->key[0]) * s) +
                                 ((Float)(after->key[0] - key->key[0]) * t));
    out[LINK_TANGENTS + 1] =
        (Angle)(((Float)(next->key[1] - before->key[1]) * s) +
                ((Float)(after->key[1] - key->key[1]) * t));
    out[LINK_TANGENTS + 2] =
        (Angle)(((Float)(next->key[2] - before->key[2]) * s) +
                ((Float)(after->key[2] - key->key[2]) * t));
#endif
}

void njCnkBezierMotLinkF(NJS_MKEY_F *keys, Uint32 index, Uint32 nb, Float *out)
{
    NJS_MKEY_F *before = &keys[(index + nb - 1) % nb];
    NJS_MKEY_F *key = &keys[index];
    NJS_MKEY_F *next = &keys[(index + 1) % nb];
    NJS_MKEY_F *after = &keys[(index + 2) % nb];
    /* the frames from next to after, before to key, and key to next */
    Float span_after;
    Float span_before;
    Float span;
    Float t;
    Float t2;
    Float s;
    Float s2;
    Float third;
    /* the weights of the slopes before key and after next in their keys'
     * tangents */
    Float before_weight;
    Float after_weight;
    Float b0;
    Float b1;
    Float b2;
    Float b3;
    Float d_before;
    Float d;
    Float d_after;
    Float tangent;
    Float next_tangent;
    Float p1;
    Float p2;
    Uint32 i;

    span = (Float)next->keyframe - (Float)key->keyframe;
    if (!(span > 0.0F)) {
        span = 1.0F;
    }
    span_after = (Float)after->keyframe - (Float)next->keyframe;
    if (!(span_after > 0.0F)) {
        span_after = 1.0F;
    }
    span_before = (Float)key->keyframe - (Float)before->keyframe;
    if (!(span_before > 0.0F)) {
        span_before = 1.0F;
    }
    t = (_nj_motlink_data_info_[_nj_mot_current_data_].frame -
         (Float)key->keyframe) /
        span;
    third = span * THIRD;
    after_weight = span / (span_after + span);
    before_weight = span / (span_before + span);
    s = 1.0F - t;
    t2 = t * t;
    s2 = s * s;
    /* b0 to b3 are the Bernstein weights of key, the two control points and
     * next */
    b3 = t * t2;
    b0 = s * s2;
    b1 = t * s2 * 3.0F;
    b2 = t2 * s * 3.0F;
    if (index == nb - 2) {
        for (i = 0; i < 3; i++) {
            d = next->key[i] - key->key[i];
            d_before = key->key[i] - before->key[i];
            tangent = ((1.0F - before_weight) * d / span) +
                      (before_weight * d_before / span_before);
            next_tangent = ((before_weight + 1.0F) * d / span) -
                           (before_weight * d_before / span_before);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the product
             * into the sum by an FMAC */
            p1 = __builtin_fmaf(third, tangent, key->key[i]);
#else
            p1 = key->key[i] + (third * tangent);
#endif
            p2 = next->key[i] - (third * next_tangent);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the products
             * by b0, b2 and b3 into the sum by FMACs */
            out[i] = __builtin_fmaf(
                b3, next->key[i],
                __builtin_fmaf(b2, p2,
                               __builtin_fmaf(b0, key->key[i], b1 * p1)));
#else
            out[i] = (b0 * key->key[i]) + (b1 * p1) + (b2 * p2) +
                     (b3 * next->key[i]);
#endif
        }
    } else if (index == 0) {
        for (i = 0; i < 3; i++) {
            d = next->key[i] - key->key[i];
            d_after = after->key[i] - next->key[i];
            tangent = ((after_weight + 1.0F) * d / span) -
                      (after_weight * d_after / span_after);
            next_tangent = ((1.0F - after_weight) * d / span) +
                           (after_weight * d_after / span_after);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the product
             * into the sum by an FMAC */
            p1 = __builtin_fmaf(third, tangent, key->key[i]);
#else
            p1 = key->key[i] + (third * tangent);
#endif
            p2 = next->key[i] - (third * next_tangent);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the products
             * by b0, b2 and b3 into the sum by FMACs */
            out[i] = __builtin_fmaf(
                b3, next->key[i],
                __builtin_fmaf(b2, p2,
                               __builtin_fmaf(b0, key->key[i], b1 * p1)));
#else
            out[i] = (b0 * key->key[i]) + (b1 * p1) + (b2 * p2) +
                     (b3 * next->key[i]);
#endif
        }
    } else {
        for (i = 0; i < 3; i++) {
            d = next->key[i] - key->key[i];
            d_before = key->key[i] - before->key[i];
            d_after = after->key[i] - next->key[i];
            tangent = ((1.0F - before_weight) * d / span) +
                      (before_weight * d_before / span_before);
            next_tangent = ((1.0F - after_weight) * d / span) +
                           (after_weight * d_after / span_after);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the product
             * into the sum by an FMAC */
            p1 = __builtin_fmaf(third, tangent, key->key[i]);
#else
            p1 = key->key[i] + (third * tangent);
#endif
            p2 = next->key[i] - (third * next_tangent);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the products
             * by b0, b2 and b3 into the sum by FMACs */
            out[i] = __builtin_fmaf(
                b3, next->key[i],
                __builtin_fmaf(b2, p2,
                               __builtin_fmaf(b0, key->key[i], b1 * p1)));
#else
            out[i] = (b0 * key->key[i]) + (b1 * p1) + (b2 * p2) +
                     (b3 * next->key[i]);
#endif
        }
    }
}

void njCnkBezierMotLinkA(NJS_MKEY_A *keys, Uint32 index, Uint32 nb, Angle *out)
{
    NJS_MKEY_A *before = &keys[(index + nb - 1) % nb];
    NJS_MKEY_A *key = &keys[index];
    NJS_MKEY_A *next = &keys[(index + 1) % nb];
    NJS_MKEY_A *after = &keys[(index + 2) % nb];
    /* the frames from next to after, before to key, and key to next */
    Float span_after;
    Float span_before;
    Float span;
    Float t;
    Float t2;
    Float s;
    Float s2;
    Float third;
    /* the weights of the slopes before key and after next in their keys'
     * tangents */
    Float before_weight;
    Float after_weight;
    Float b0;
    Float b1;
    Float b2;
    Float b3;
    Float d_before;
    Float d;
    Float d_after;
    Float tangent;
    Float next_tangent;
    Float p1;
    Float p2;
    Uint32 i;

    span = (Float)next->keyframe - (Float)key->keyframe;
    if (!(span > 0.0F)) {
        span = 1.0F;
    }
    span_after = (Float)after->keyframe - (Float)next->keyframe;
    if (!(span_after > 0.0F)) {
        span_after = 1.0F;
    }
    span_before = (Float)key->keyframe - (Float)before->keyframe;
    if (!(span_before > 0.0F)) {
        span_before = 1.0F;
    }
    t = (_nj_motlink_data_info_[_nj_mot_current_data_].frame -
         (Float)key->keyframe) /
        span;
    third = span * THIRD;
    after_weight = span / (span_after + span);
    before_weight = span / (span_before + span);
    s = 1.0F - t;
    t2 = t * t;
    s2 = s * s;
    /* b0 to b3 are the Bernstein weights of key, the two control points and
     * next */
    b3 = t * t2;
    b0 = s * s2;
    b1 = t * s2 * 3.0F;
    b2 = t2 * s * 3.0F;
    if (index == nb - 2) {
        for (i = 0; i < 3; i++) {
            d = (Float)(next->key[i] - key->key[i]);
            d_before = (Float)(key->key[i] - before->key[i]);
            tangent = ((1.0F - before_weight) * d / span) +
                      (before_weight * d_before / span_before);
            next_tangent = ((before_weight + 1.0F) * d / span) -
                           (before_weight * d_before / span_before);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the product
             * into the sum by an FMAC */
            p1 = __builtin_fmaf(third, tangent, (Float)key->key[i]);
#else
            p1 = (Float)key->key[i] + (third * tangent);
#endif
            p2 = (Float)next->key[i] - (third * next_tangent);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the products
             * by b0, b2 and b3 into the sum by FMACs */
            out[i] = (Angle)__builtin_fmaf(
                b3, (Float)next->key[i],
                __builtin_fmaf(
                    b2, p2, __builtin_fmaf(b0, (Float)key->key[i], b1 * p1)));
#else
            out[i] = (Angle)((b0 * (Float)key->key[i]) + (b1 * p1) + (b2 * p2) +
                             (b3 * (Float)next->key[i]));
#endif
        }
    } else if (index == 0) {
        for (i = 0; i < 3; i++) {
            d = (Float)(next->key[i] - key->key[i]);
            d_after = (Float)(after->key[i] - next->key[i]);
            tangent = ((after_weight + 1.0F) * d / span) -
                      (after_weight * d_after / span_after);
            next_tangent = ((1.0F - after_weight) * d / span) +
                           (after_weight * d_after / span_after);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the product
             * into the sum by an FMAC */
            p1 = __builtin_fmaf(third, tangent, (Float)key->key[i]);
#else
            p1 = (Float)key->key[i] + (third * tangent);
#endif
            p2 = (Float)next->key[i] - (third * next_tangent);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the products
             * by b0, b2 and b3 into the sum by FMACs */
            out[i] = (Angle)__builtin_fmaf(
                b3, (Float)next->key[i],
                __builtin_fmaf(
                    b2, p2, __builtin_fmaf(b0, (Float)key->key[i], b1 * p1)));
#else
            out[i] = (Angle)((b0 * (Float)key->key[i]) + (b1 * p1) + (b2 * p2) +
                             (b3 * (Float)next->key[i]));
#endif
        }
    } else {
        for (i = 0; i < 3; i++) {
            d = (Float)(next->key[i] - key->key[i]);
            d_before = (Float)(key->key[i] - before->key[i]);
            d_after = (Float)(after->key[i] - next->key[i]);
            tangent = ((1.0F - before_weight) * d / span) +
                      (before_weight * d_before / span_before);
            next_tangent = ((1.0F - after_weight) * d / span) +
                           (after_weight * d_after / span_after);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the product
             * into the sum by an FMAC */
            p1 = __builtin_fmaf(third, tangent, (Float)key->key[i]);
#else
            p1 = (Float)key->key[i] + (third * tangent);
#endif
            p2 = (Float)next->key[i] - (third * next_tangent);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the products
             * by b0, b2 and b3 into the sum by FMACs */
            out[i] = (Angle)__builtin_fmaf(
                b3, (Float)next->key[i],
                __builtin_fmaf(
                    b2, p2, __builtin_fmaf(b0, (Float)key->key[i], b1 * p1)));
#else
            out[i] = (Angle)((b0 * (Float)key->key[i]) + (b1 * p1) + (b2 * p2) +
                             (b3 * (Float)next->key[i]));
#endif
        }
    }
}

void njCnkLinearMotLinkQ(NJS_MKEY_QUAT *keys, Uint32 index, Uint32 nb,
                         Float *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_QUAT *key = &keys[index];
    NJS_MKEY_QUAT *next = &keys[(index + 1) % nb];
    Float t;
    Float s;
    Float dot;
    Float length2;
    Float scale;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motlink_data_info_[_nj_mot_current_data_].frame -
         (Float)key->keyframe) /
        divisor[(Sint32)(next->keyframe - key->keyframe) > 0];
    s = 1.0F - t;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses the products of
     * key->key[0], [2] and [3] with next's into the sum by FMACs */
    dot = __builtin_fmaf(
        key->key[3], next->key[3],
        __builtin_fmaf(key->key[2], next->key[2],
                       __builtin_fmaf(key->key[0], next->key[0],
                                      key->key[1] * next->key[1])));
#else
    dot = (key->key[0] * next->key[0]) + (key->key[1] * next->key[1]) +
          (key->key[2] * next->key[2]) + (key->key[3] * next->key[3]);
#endif
    if (dot < 0.0F) {
        /* @hack spelling: t multiplied by -1, where its negation would do */
        t *= -1.0F;
    }
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses each blend's product
     * by s into its sum by an FMAC. it fuses the squares of out[1], out[3] and
     * out[0] into their sum by FMACs */
    out[0] = __builtin_fmaf(key->key[0], s, next->key[0] * t);
    out[1] = __builtin_fmaf(key->key[1], s, next->key[1] * t);
    out[2] = __builtin_fmaf(key->key[2], s, next->key[2] * t);
    out[3] = __builtin_fmaf(key->key[3], s, next->key[3] * t);
    length2 = __builtin_fmaf(
        out[0], out[0],
        __builtin_fmaf(out[3], out[3],
                       __builtin_fmaf(out[1], out[1], out[2] * out[2])));
#else
    out[0] = (key->key[0] * s) + (next->key[0] * t);
    out[1] = (key->key[1] * s) + (next->key[1] * t);
    out[2] = (key->key[2] * s) + (next->key[2] * t);
    out[3] = (key->key[3] * s) + (next->key[3] * t);
    /* @hack spelling: length2, the sum of the squares, set by a statement of
     * its own, where one expression would give scale */
    length2 = (out[0] * out[0]) + (out[1] * out[1]) + (out[2] * out[2]) +
              (out[3] * out[3]);
#endif
    scale = 1.0F / _builtin_sqrtf(length2);
    out[0] *= scale;
    out[1] *= scale;
    out[2] *= scale;
    out[3] *= scale;
}

void njCnkLinearIntF(const Float *values, Float *out, Float rate)
{
    /* @hack spelling: t, a copy of rate, where rate itself would do */
    Float t = rate;
    Float s = 1.0F - rate;

#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses out[0]'s product by
     * t into its sum by an FMAC. it fuses the other two sums' products by s */
    out[0] = __builtin_fmaf(values[LINK_VALUE(1)], t, values[0] * s);
    out[1] = __builtin_fmaf(values[1], s, values[LINK_VALUE(1) + 1] * t);
    out[2] = __builtin_fmaf(values[2], s, values[LINK_VALUE(1) + 2] * t);
#else
    out[0] = (values[0] * s) + (values[LINK_VALUE(1)] * t);
    out[1] = (values[1] * s) + (values[LINK_VALUE(1) + 1] * t);
    out[2] = (values[2] * s) + (values[LINK_VALUE(1) + 2] * t);
#endif
}

void njCnkLinearIntQ(const Float *values, Float *out, Float rate)
{
    /* @hack spelling: t, a copy of rate, where rate itself would do */
    Float t = rate;
    Float s = 1.0F - rate;
    Float dot;
    Float length2;
    Float scale;

#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses the products of
     * values[1], [3] and [0] with slot 1's into the sum by FMACs */
    dot = __builtin_fmaf(
        values[0], values[LINK_QUA(1)],
        __builtin_fmaf(values[3], values[LINK_QUA(1) + 3],
                       __builtin_fmaf(values[1], values[LINK_QUA(1) + 1],
                                      values[2] * values[LINK_QUA(1) + 2])));
#else
    dot = (values[0] * values[LINK_QUA(1)]) +
          (values[1] * values[LINK_QUA(1) + 1]) +
          (values[2] * values[LINK_QUA(1) + 2]) +
          (values[3] * values[LINK_QUA(1) + 3]);
#endif
    if (dot < 0.0F) {
        /* @hack spelling: t multiplied by -1, where its negation would do */
        t *= -1.0F;
    }
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses out[0]'s product by
     * t into its sum by an FMAC. it fuses the other blends' products by s. it
     * fuses the squares of out[1], out[3] and out[0] into their sum */
    out[0] = __builtin_fmaf(values[LINK_QUA(1)], t, values[0] * s);
    out[1] = __builtin_fmaf(values[1], s, values[LINK_QUA(1) + 1] * t);
    out[2] = __builtin_fmaf(values[2], s, values[LINK_QUA(1) + 2] * t);
    out[3] = __builtin_fmaf(values[3], s, values[LINK_QUA(1) + 3] * t);
    length2 = __builtin_fmaf(
        out[0], out[0],
        __builtin_fmaf(out[3], out[3],
                       __builtin_fmaf(out[1], out[1], out[2] * out[2])));
#else
    out[0] = (values[0] * s) + (values[LINK_QUA(1)] * t);
    out[1] = (values[1] * s) + (values[LINK_QUA(1) + 1] * t);
    out[2] = (values[2] * s) + (values[LINK_QUA(1) + 2] * t);
    out[3] = (values[3] * s) + (values[LINK_QUA(1) + 3] * t);
    /* @hack spelling: length2, the sum of the squares, set by a statement of
     * its own, where one expression would give scale */
    length2 = (out[0] * out[0]) + (out[1] * out[1]) + (out[2] * out[2]) +
              (out[3] * out[3]);
#endif
    scale = 1.0F / _builtin_sqrtf(length2);
    out[0] *= scale;
    out[1] *= scale;
    out[2] *= scale;
    out[3] *= scale;
}

void njCnkLinearIntA(const Angle *values, Angle *out, Float rate)
{
    /* @hack spelling: t, a copy of rate, where rate itself would do */
    Float t = rate;
    Float s = 1.0F - rate;

#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses out[0]'s product by
     * t into its sum by an FMAC. it fuses the other two sums' products by s */
    out[0] = (Angle)__builtin_fmaf((Float)values[LINK_VALUE(1)], t,
                                   (Float)values[0] * s);
    out[1] = (Angle)__builtin_fmaf((Float)values[1], s,
                                   (Float)values[LINK_VALUE(1) + 1] * t);
    out[2] = (Angle)__builtin_fmaf((Float)values[2], s,
                                   (Float)values[LINK_VALUE(1) + 2] * t);
#else
    out[0] =
        (Angle)(((Float)values[0] * s) + ((Float)values[LINK_VALUE(1)] * t));
    out[1] = (Angle)(((Float)values[1] * s) +
                     ((Float)values[LINK_VALUE(1) + 1] * t));
    out[2] = (Angle)(((Float)values[2] * s) +
                     ((Float)values[LINK_VALUE(1) + 2] * t));
#endif
}

void njCnkHermiteIntF(const Float *values, Float *out, Float rate)
{
    Float t2;
    Float t3;
    /* the Hermite weights of slot 0's value, slot 1's, and their tangents */
    Float h0;
    Float h1;
    Float h2;
    Float h3;

    t2 = rate * rate;
    t3 = t2 * rate;
    h0 = (2.0F * t3) - (3.0F * t2) + 1.0F;
    h1 = (3.0F * t2) - (2.0F * t3);
    h2 = rate - (2.0F * t2) + t3;
    h3 = t3 - t2;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses out[0]'s products by
     * h1, h3 and h0 into its sum by FMACs. it fuses the other two sums'
     * products by h0, h2 and h3 */
    out[0] = __builtin_fmaf(
        h0, values[0],
        __builtin_fmaf(h3, values[LINK_VALUE(1) + LINK_TANGENTS],
                       __builtin_fmaf(h1, values[LINK_VALUE(1)],
                                      h2 * values[LINK_TANGENTS])));
    out[1] = __builtin_fmaf(
        h3, values[LINK_VALUE(1) + (LINK_TANGENTS + 1)],
        __builtin_fmaf(
            h2, values[LINK_TANGENTS + 1],
            __builtin_fmaf(h0, values[1], h1 * values[LINK_VALUE(1) + 1])));
    out[2] = __builtin_fmaf(
        h3, values[LINK_VALUE(1) + (LINK_TANGENTS + 2)],
        __builtin_fmaf(
            h2, values[LINK_TANGENTS + 2],
            __builtin_fmaf(h0, values[2], h1 * values[LINK_VALUE(1) + 2])));
#else
    out[0] = (h0 * values[0]) + (h1 * values[LINK_VALUE(1)]) +
             (h2 * values[LINK_TANGENTS]) +
             (h3 * values[LINK_VALUE(1) + LINK_TANGENTS]);
    out[1] = (h0 * values[1]) + (h1 * values[LINK_VALUE(1) + 1]) +
             (h2 * values[LINK_TANGENTS + 1]) +
             (h3 * values[LINK_VALUE(1) + (LINK_TANGENTS + 1)]);
    out[2] = (h0 * values[2]) + (h1 * values[LINK_VALUE(1) + 2]) +
             (h2 * values[LINK_TANGENTS + 2]) +
             (h3 * values[LINK_VALUE(1) + (LINK_TANGENTS + 2)]);
#endif
}

void njCnkHermiteIntA(const Angle *values, Angle *out, Float rate)
{
    Float t2;
    Float t3;
    /* the Hermite weights of slot 0's value, slot 1's, and their tangents */
    Float h0;
    Float h1;
    Float h2;
    Float h3;

    t2 = rate * rate;
    t3 = t2 * rate;
    h0 = (2.0F * t3) - (3.0F * t2) + 1.0F;
    h1 = (3.0F * t2) - (2.0F * t3);
    h2 = rate - (2.0F * t2) + t3;
    h3 = t3 - t2;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses out[0]'s products by
     * h1, h3 and h0 into its sum by FMACs. it fuses the other two sums'
     * products by h0, h2 and h3 */
    out[0] = (Angle)__builtin_fmaf(
        h0, (Float)values[0],
        __builtin_fmaf(h3, (Float)values[LINK_VALUE(1) + LINK_TANGENTS],
                       __builtin_fmaf(h1, (Float)values[LINK_VALUE(1)],
                                      h2 * (Float)values[LINK_TANGENTS])));
    out[1] = (Angle)__builtin_fmaf(
        h3, (Float)values[LINK_VALUE(1) + (LINK_TANGENTS + 1)],
        __builtin_fmaf(h2, (Float)values[LINK_TANGENTS + 1],
                       __builtin_fmaf(h0, (Float)values[1],
                                      h1 * (Float)values[LINK_VALUE(1) + 1])));
    out[2] = (Angle)__builtin_fmaf(
        h3, (Float)values[LINK_VALUE(1) + (LINK_TANGENTS + 2)],
        __builtin_fmaf(h2, (Float)values[LINK_TANGENTS + 2],
                       __builtin_fmaf(h0, (Float)values[2],
                                      h1 * (Float)values[LINK_VALUE(1) + 2])));
#else
    out[0] =
        (Angle)((h0 * (Float)values[0]) + (h1 * (Float)values[LINK_VALUE(1)]) +
                (h2 * (Float)values[LINK_TANGENTS]) +
                (h3 * (Float)values[LINK_VALUE(1) + LINK_TANGENTS]));
    out[1] = (Angle)((h0 * (Float)values[1]) +
                     (h1 * (Float)values[LINK_VALUE(1) + 1]) +
                     (h2 * (Float)values[LINK_TANGENTS + 1]) +
                     (h3 * (Float)values[LINK_VALUE(1) + (LINK_TANGENTS + 1)]));
    out[2] = (Angle)((h0 * (Float)values[2]) +
                     (h1 * (Float)values[LINK_VALUE(1) + 2]) +
                     (h2 * (Float)values[LINK_TANGENTS + 2]) +
                     (h3 * (Float)values[LINK_VALUE(1) + (LINK_TANGENTS + 2)]));
#endif
}

void njCnkMotLinkIntTranslate(NJS_CNK_OBJECT *object)
{
    NJS_MKEY_F *keys;
    Uint32 nb;
    Uint32 frame;
    Uint32 low;
    Uint32 high;
    Uint32 middle;

    keys = _nj_motlink_data_info_[_nj_mot_current_data_]
               .p[_nj_motlink_data_info_[_nj_mot_current_data_].factor];
    if (keys != NULL) {
        nb = _nj_motlink_data_info_[_nj_mot_current_data_]
                 .nb[_nj_motlink_data_info_[_nj_mot_current_data_].factor];
        low = 0;
        high = nb;
        frame = (Uint32)_nj_motlink_data_info_[_nj_mot_current_data_].frame;
        while (high - low > 1) {
            middle = (low + high) >> 1;
            if (frame >= keys[middle].keyframe) {
                low = middle;
            } else {
                high = middle;
            }
        }
        _nj_motion_int_f_func_(
            keys, low, nb,
            &_nj_motion_link_pos_[LINK_VALUE(_nj_mot_current_data_)]);
        _nj_pos_int_flag_ |= SLOT_BIT(_nj_mot_current_data_);
    } else {
        _nj_motion_link_pos_[LINK_VALUE(_nj_mot_current_data_)] =
            object->pos[0];
        _nj_motion_link_pos_[LINK_VALUE(_nj_mot_current_data_) + 1] =
            object->pos[1];
        _nj_motion_link_pos_[LINK_VALUE(_nj_mot_current_data_) + 2] =
            object->pos[2];
    }
    _nj_motlink_data_info_[_nj_mot_current_data_].factor++;
}

void njCnkMotLinkIntScale(NJS_CNK_OBJECT *object)
{
    NJS_MKEY_F *keys;
    Uint32 nb;
    Uint32 frame;
    Uint32 low;
    Uint32 high;
    Uint32 middle;

    keys = _nj_motlink_data_info_[_nj_mot_current_data_]
               .p[_nj_motlink_data_info_[_nj_mot_current_data_].factor];
    if (keys != NULL) {
        nb = _nj_motlink_data_info_[_nj_mot_current_data_]
                 .nb[_nj_motlink_data_info_[_nj_mot_current_data_].factor];
        low = 0;
        high = nb;
        frame = (Uint32)_nj_motlink_data_info_[_nj_mot_current_data_].frame;
        while (high - low > 1) {
            middle = (low + high) >> 1;
            if (frame >= keys[middle].keyframe) {
                low = middle;
            } else {
                high = middle;
            }
        }
        _nj_motion_int_f_func_(
            keys, low, nb,
            &_nj_motion_link_scl_[LINK_VALUE(_nj_mot_current_data_)]);
        _nj_scl_int_flag_ |= SLOT_BIT(_nj_mot_current_data_);
    } else {
        _nj_motion_link_scl_[LINK_VALUE(_nj_mot_current_data_)] =
            object->scl[0];
        _nj_motion_link_scl_[LINK_VALUE(_nj_mot_current_data_) + 1] =
            object->scl[1];
        _nj_motion_link_scl_[LINK_VALUE(_nj_mot_current_data_) + 2] =
            object->scl[2];
    }
    _nj_motlink_data_info_[_nj_mot_current_data_].factor++;
}

void njCnkMotLinkIntRotate(NJS_CNK_OBJECT *object)
{
    NJS_MKEY_A *keys;
    Uint32 nb;
    Uint32 frame;
    Uint32 low;
    Uint32 high;
    Uint32 middle;

    keys = _nj_motlink_data_info_[_nj_mot_current_data_]
               .p[_nj_motlink_data_info_[_nj_mot_current_data_].factor];
    if (keys != NULL) {
        nb = _nj_motlink_data_info_[_nj_mot_current_data_]
                 .nb[_nj_motlink_data_info_[_nj_mot_current_data_].factor];
        low = 0;
        high = nb;
        frame = (Uint32)_nj_motlink_data_info_[_nj_mot_current_data_].frame;
        while (high - low > 1) {
            middle = (low + high) >> 1;
            if (frame >= keys[middle].keyframe) {
                low = middle;
            } else {
                high = middle;
            }
        }
        _nj_motion_int_a_func_(
            keys, low, nb,
            &_nj_motion_link_ang_[LINK_VALUE(_nj_mot_current_data_)]);
        _nj_ang_int_flag_ |= SLOT_BIT(_nj_mot_current_data_);
    } else {
        _nj_motion_link_ang_[LINK_VALUE(_nj_mot_current_data_)] =
            object->ang[0];
        _nj_motion_link_ang_[LINK_VALUE(_nj_mot_current_data_) + 1] =
            object->ang[1];
        _nj_motion_link_ang_[LINK_VALUE(_nj_mot_current_data_) + 2] =
            object->ang[2];
    }
    _nj_motlink_data_info_[_nj_mot_current_data_].factor++;
}

void njCnkMotLinkIntQuaternion(NJS_CNK_OBJECT *object)
{
    NJS_MKEY_QUAT *keys;
    Uint32 nb;
    Uint32 frame;
    Uint32 low;
    Uint32 high;
    Uint32 middle;
    /* @hack redundant: four floats of the frame, which nothing reads */
    Float unused[4];

    keys = _nj_motlink_data_info_[_nj_mot_current_data_]
               .p[_nj_motlink_data_info_[_nj_mot_current_data_].factor];
    if (keys != NULL) {
        nb = _nj_motlink_data_info_[_nj_mot_current_data_]
                 .nb[_nj_motlink_data_info_[_nj_mot_current_data_].factor];
        low = 0;
        high = nb;
        frame = (Uint32)_nj_motlink_data_info_[_nj_mot_current_data_].frame;
        while (high - low > 1) {
            middle = (low + high) >> 1;
            if (frame >= keys[middle].keyframe) {
                low = middle;
            } else {
                high = middle;
            }
        }
        njCnkLinearMotLinkQ(
            keys, low, nb,
            &_nj_motion_link_qua_[LINK_QUA(_nj_mot_current_data_)]);
        _nj_ang_int_flag_ |= SLOT_BIT(_nj_mot_current_data_);
    } else {
        njAng2Quat(object->ang,
                   &_nj_motion_link_qua_[LINK_QUA(_nj_mot_current_data_)],
                   _nj_rotate_func_flag_);
    }
    _nj_motlink_data_info_[_nj_mot_current_data_].factor++;
}

void njCnkMotLinkObjTranslate(NJS_CNK_OBJECT *object)
{
    _nj_motion_link_pos_[LINK_VALUE(_nj_mot_current_data_)] = object->pos[0];
    _nj_motion_link_pos_[LINK_VALUE(_nj_mot_current_data_) + 1] =
        object->pos[1];
    _nj_motion_link_pos_[LINK_VALUE(_nj_mot_current_data_) + 2] =
        object->pos[2];
}

void njCnkMotLinkObjRotate(NJS_CNK_OBJECT *object)
{
    _nj_motion_link_ang_[LINK_VALUE(_nj_mot_current_data_)] = object->ang[0];
    _nj_motion_link_ang_[LINK_VALUE(_nj_mot_current_data_) + 1] =
        object->ang[1];
    _nj_motion_link_ang_[LINK_VALUE(_nj_mot_current_data_) + 2] =
        object->ang[2];
}

void njCnkMotLinkObjScale(NJS_CNK_OBJECT *object)
{
    _nj_motion_link_scl_[LINK_VALUE(_nj_mot_current_data_)] = object->scl[0];
    _nj_motion_link_scl_[LINK_VALUE(_nj_mot_current_data_) + 1] =
        object->scl[1];
    _nj_motion_link_scl_[LINK_VALUE(_nj_mot_current_data_) + 2] =
        object->scl[2];
}

void njCnkMotLinkData0(NJS_CNK_OBJECT *object)
{
    _nj_cnk_motlink_translate_data_[_nj_mot_current_data_](object);
    _nj_cnk_motlink_rotate_data_[_nj_mot_current_data_](object);
    _nj_cnk_motlink_scale_data_[_nj_mot_current_data_](object);
}

void njCnkMotLinkData1(NJS_CNK_OBJECT *object)
{
    NJS_MDATA1 *entry = _nj_motlink_data_info_[_nj_mot_current_data_].mdata;

    _nj_motlink_data_info_[_nj_mot_current_data_].p = entry->p;
    _nj_motlink_data_info_[_nj_mot_current_data_].nb = entry->nb;
    _nj_motlink_data_info_[_nj_mot_current_data_].factor = 0;
    _nj_cnk_motlink_translate_data_[_nj_mot_current_data_](object);
    _nj_cnk_motlink_rotate_data_[_nj_mot_current_data_](object);
    _nj_cnk_motlink_scale_data_[_nj_mot_current_data_](object);
    _nj_motlink_data_info_[_nj_mot_current_data_].mdata = entry + 1;
}

void njCnkMotLinkData2(NJS_CNK_OBJECT *object)
{
    NJS_MDATA2 *entry = _nj_motlink_data_info_[_nj_mot_current_data_].mdata;

    _nj_motlink_data_info_[_nj_mot_current_data_].p = entry->p;
    _nj_motlink_data_info_[_nj_mot_current_data_].nb = entry->nb;
    _nj_motlink_data_info_[_nj_mot_current_data_].factor = 0;
    _nj_cnk_motlink_translate_data_[_nj_mot_current_data_](object);
    _nj_cnk_motlink_rotate_data_[_nj_mot_current_data_](object);
    _nj_cnk_motlink_scale_data_[_nj_mot_current_data_](object);
    _nj_motlink_data_info_[_nj_mot_current_data_].mdata = entry + 1;
}

void njCnkMotLinkData3(NJS_CNK_OBJECT *object)
{
    NJS_MDATA3 *entry = _nj_motlink_data_info_[_nj_mot_current_data_].mdata;

    _nj_motlink_data_info_[_nj_mot_current_data_].p = entry->p;
    _nj_motlink_data_info_[_nj_mot_current_data_].nb = entry->nb;
    _nj_motlink_data_info_[_nj_mot_current_data_].factor = 0;
    _nj_cnk_motlink_translate_data_[_nj_mot_current_data_](object);
    _nj_cnk_motlink_rotate_data_[_nj_mot_current_data_](object);
    _nj_cnk_motlink_scale_data_[_nj_mot_current_data_](object);
    _nj_motlink_data_info_[_nj_mot_current_data_].mdata = entry + 1;
}

void njCnkMotLinkData4(NJS_CNK_OBJECT *object)
{
    NJS_MDATA4 *entry = _nj_motlink_data_info_[_nj_mot_current_data_].mdata;

    _nj_motlink_data_info_[_nj_mot_current_data_].p = entry->p;
    _nj_motlink_data_info_[_nj_mot_current_data_].nb = entry->nb;
    _nj_motlink_data_info_[_nj_mot_current_data_].factor = 0;
    _nj_cnk_motlink_translate_data_[_nj_mot_current_data_](object);
    _nj_cnk_motlink_rotate_data_[_nj_mot_current_data_](object);
    _nj_cnk_motlink_scale_data_[_nj_mot_current_data_](object);
    _nj_motlink_data_info_[_nj_mot_current_data_].mdata = entry + 1;
}

void njCnkMakeMotionLink(void)
{
    /* BOTH_SLOTS under a spline count of 1, else 0. an int flag above 0 and
     * below it holds one slot's bit alone. none does without a spline */
    Sint32 spline_both;
    Bool quaternion;
    /* @hack type: pos, scl and qua Float arrays, cast at their use, where an
     * NJS_VECTOR each and an NJS_QUATERNION would do */
    Float pos[3];
    Float scl[3];
    Float qua[4];
    Angle ang[3];

    quaternion = FALSE;
    spline_both = _nj_mot_spline_cnt_ * BOTH_SLOTS;
    if ((_nj_motlink_data_type_[0] | _nj_motlink_data_type_[1]) &
        NJD_MTYPE_QUAT_1) {
        quaternion = TRUE;
    }
    if (_nj_pos_int_flag_ > 0) {
        /* one slot alone has keys. _nj_pos_int_flag_ becomes the index of the
         * slot without keys, whose tangent is set here */
        if (_nj_pos_int_flag_ < spline_both) {
            _nj_pos_int_flag_--;
            _nj_pos_int_flag_ ^= 1;
            _nj_motion_link_pos_[LINK_VALUE(_nj_pos_int_flag_) +
                                 LINK_TANGENTS] =
                (_nj_motion_link_pos_[LINK_VALUE(1)] -
                 _nj_motion_link_pos_[0]) *
                0.5F;
            /* @hack spelling: the tangent's second and third places written
             * (_nj_pos_int_flag_ * 3) + 7 and + 8, where
             * LINK_VALUE(_nj_pos_int_flag_) + LINK_TANGENTS + 1 and + 2 would
             * do */
            _nj_motion_link_pos_[(_nj_pos_int_flag_ * 3) + 7] =
                (_nj_motion_link_pos_[LINK_VALUE(1) + 1] -
                 _nj_motion_link_pos_[1]) *
                0.5F;
            _nj_motion_link_pos_[(_nj_pos_int_flag_ * 3) + 8] =
                (_nj_motion_link_pos_[LINK_VALUE(1) + 2] -
                 _nj_motion_link_pos_[2]) *
                0.5F;
        }
        _nj_int_f_func_(_nj_motion_link_pos_, pos, _nj_mot_current_frame_);
    } else {
        pos[0] = _nj_motion_link_pos_[0];
        pos[1] = _nj_motion_link_pos_[1];
        pos[2] = _nj_motion_link_pos_[2];
    }
    if (quaternion) {
        if (!(_nj_motlink_data_type_[0] & NJD_MTYPE_QUAT_1)) {
            njAng2Quat(_nj_motion_link_ang_, _nj_motion_link_qua_,
                       _nj_rotate_func_flag_);
        }
        if (!(_nj_motlink_data_type_[1] & NJD_MTYPE_QUAT_1)) {
            njAng2Quat(&_nj_motion_link_ang_[LINK_VALUE(1)],
                       &_nj_motion_link_qua_[LINK_QUA(1)],
                       _nj_rotate_func_flag_);
        }
        if (_nj_ang_int_flag_ > 0) {
            njCnkLinearIntQ(_nj_motion_link_qua_, qua, _nj_mot_current_frame_);
        } else {
            qua[0] = _nj_motion_link_qua_[0];
            qua[1] = _nj_motion_link_qua_[1];
            qua[2] = _nj_motion_link_qua_[2];
            qua[3] = _nj_motion_link_qua_[3];
        }
    } else if (_nj_ang_int_flag_ > 0) {
        if (_nj_ang_int_flag_ < spline_both) {
            _nj_ang_int_flag_--;
            _nj_ang_int_flag_ ^= 1;
            _nj_motion_link_ang_[LINK_VALUE(_nj_ang_int_flag_) +
                                 LINK_TANGENTS] =
                (Angle)((Float)(_nj_motion_link_ang_[LINK_VALUE(1)] -
                                _nj_motion_link_ang_[0]) *
                        0.5F);
            /* @hack spelling: the tangent's second and third places written
             * (_nj_ang_int_flag_ * 3) + 7 and + 8, where
             * LINK_VALUE(_nj_ang_int_flag_) + LINK_TANGENTS + 1 and + 2 would
             * do */
            _nj_motion_link_ang_[(_nj_ang_int_flag_ * 3) + 7] =
                (Angle)((Float)(_nj_motion_link_ang_[LINK_VALUE(1) + 1] -
                                _nj_motion_link_ang_[1]) *
                        0.5F);
            _nj_motion_link_ang_[(_nj_ang_int_flag_ * 3) + 8] =
                (Angle)((Float)(_nj_motion_link_ang_[LINK_VALUE(1) + 2] -
                                _nj_motion_link_ang_[2]) *
                        0.5F);
        }
        _nj_int_a_func_(_nj_motion_link_ang_, ang, _nj_mot_current_frame_);
    } else {
        ang[0] = _nj_motion_link_ang_[0];
        ang[1] = _nj_motion_link_ang_[1];
        ang[2] = _nj_motion_link_ang_[2];
    }
    if (_nj_scl_int_flag_ > 0) {
        if (_nj_scl_int_flag_ < spline_both) {
            _nj_scl_int_flag_--;
            _nj_scl_int_flag_ ^= 1;
            _nj_motion_link_scl_[LINK_VALUE(_nj_scl_int_flag_) +
                                 LINK_TANGENTS] =
                (_nj_motion_link_scl_[LINK_VALUE(1)] -
                 _nj_motion_link_scl_[0]) *
                0.5F;
            /* @hack spelling: the tangent's second and third places written
             * (_nj_scl_int_flag_ * 3) + 7 and + 8, where
             * LINK_VALUE(_nj_scl_int_flag_) + LINK_TANGENTS + 1 and + 2 would
             * do */
            _nj_motion_link_scl_[(_nj_scl_int_flag_ * 3) + 7] =
                (_nj_motion_link_scl_[LINK_VALUE(1) + 1] -
                 _nj_motion_link_scl_[1]) *
                0.5F;
            _nj_motion_link_scl_[(_nj_scl_int_flag_ * 3) + 8] =
                (_nj_motion_link_scl_[LINK_VALUE(1) + 2] -
                 _nj_motion_link_scl_[2]) *
                0.5F;
        }
        _nj_int_f_func_(_nj_motion_link_scl_, scl, _nj_mot_current_frame_);
    } else {
        scl[0] = _nj_motion_link_scl_[0];
        scl[1] = _nj_motion_link_scl_[1];
        scl[2] = _nj_motion_link_scl_[2];
    }
    njTranslateEx((NJS_VECTOR *)pos);
    if (quaternion) {
        njQuaternionEx((NJS_QUATERNION *)qua);
    } else {
        njRotateEx(ang, _nj_rotate_func_flag_);
    }
    njScaleEx((NJS_VECTOR *)scl);
}

void njCnkLinearMotLinkAOld(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
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
     * lie over half a turn apart */
    Angle x;
    Angle y;
    Angle z;

    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motlink_data_info_[_nj_mot_current_data_].frame -
         (Float)key->keyframe) /
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
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses each product by s
     * into its sum by an FMAC */
    out[0] = (Angle)__builtin_fmaf((Float)key->key[0], s, (Float)x * t);
    out[1] = (Angle)__builtin_fmaf((Float)key->key[1], s, (Float)y * t);
    out[2] = (Angle)__builtin_fmaf((Float)key->key[2], s, (Float)z * t);
#else
    out[0] = (Angle)(((Float)key->key[0] * s) + ((Float)x * t));
    out[1] = (Angle)(((Float)key->key[1] * s) + ((Float)y * t));
    out[2] = (Angle)(((Float)key->key[2] * s) + ((Float)z * t));
#endif
}

void njCnkLinearIntAOld(const Angle *values, Angle *out, Float rate)
{
    /* @hack spelling: t, a copy of rate, where rate itself would do */
    Float t = rate;
    Float s = 1.0F - rate;
    /* slot 1's angles. each is moved a turn toward slot 0's where the two lie
     * over half a turn apart */
    Angle x;
    Angle y;
    Angle z;

    x = values[LINK_VALUE(1)] - values[0];
    if (x > HALF_TURN) {
        x -= TURN;
    } else if (x < -HALF_TURN) {
        x += TURN;
    }
    x += values[0];
    y = values[LINK_VALUE(1) + 1] - values[1];
    if (y > HALF_TURN) {
        y -= TURN;
    } else if (y < -HALF_TURN) {
        y += TURN;
    }
    y += values[1];
    z = values[LINK_VALUE(1) + 2] - values[2];
    if (z > HALF_TURN) {
        z -= TURN;
    } else if (z < -HALF_TURN) {
        z += TURN;
    }
    z += values[2];
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses each product by s
     * into its sum by an FMAC */
    out[0] = (Angle)__builtin_fmaf((Float)values[0], s, (Float)x * t);
    out[1] = (Angle)__builtin_fmaf((Float)values[1], s, (Float)y * t);
    out[2] = (Angle)__builtin_fmaf((Float)values[2], s, (Float)z * t);
#else
    out[0] = (Angle)(((Float)values[0] * s) + ((Float)x * t));
    out[1] = (Angle)(((Float)values[1] * s) + ((Float)y * t));
    out[2] = (Angle)(((Float)values[2] * s) + ((Float)z * t));
#endif
}

void njCnkBezierMotLinkAOld(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                            Angle *out)
{
    NJS_MKEY_A *before = &keys[(index + nb - 1) % nb];
    NJS_MKEY_A *key = &keys[index];
    NJS_MKEY_A *next = &keys[(index + 1) % nb];
    NJS_MKEY_A *after = &keys[(index + 2) % nb];
    /* the frames from next to after, before to key, and key to next */
    Float span_after;
    Float span_before;
    Float span;
    Float t;
    Float t2;
    Float s;
    Float s2;
    Float third;
    /* the weights of the slopes before key and after next in their keys'
     * tangents */
    Float before_weight;
    Float after_weight;
    Float b0;
    Float b1;
    Float b2;
    Float b3;
    Float d_before;
    Float d;
    Float d_after;
    Float tangent;
    Float next_tangent;
    Float p1;
    Float p2;
    /* key's angle, and the next key's. the next key's gains a turn where it
     * lies half a turn or more below key's. it loses one where it lies over
     * half a turn above */
    Float key_ang;
    Float next_ang;
    Uint32 i;

    span = (Float)next->keyframe - (Float)key->keyframe;
    if (!(span > 0.0F)) {
        span = 1.0F;
    }
    span_after = (Float)after->keyframe - (Float)next->keyframe;
    if (!(span_after > 0.0F)) {
        span_after = 1.0F;
    }
    span_before = (Float)key->keyframe - (Float)before->keyframe;
    if (!(span_before > 0.0F)) {
        span_before = 1.0F;
    }
    t = (_nj_motlink_data_info_[_nj_mot_current_data_].frame -
         (Float)key->keyframe) /
        span;
    third = span * THIRD;
    after_weight = span / (span_after + span);
    before_weight = span / (span_before + span);
    s = 1.0F - t;
    t2 = t * t;
    s2 = s * s;
    /* b0 to b3 are the Bernstein weights of key, the two control points and
     * next */
    b3 = t * t2;
    b0 = s * s2;
    b1 = t * s2 * 3.0F;
    b2 = t2 * s * 3.0F;
    if (index == nb - 2) {
        for (i = 0; i < 3; i++) {
            key_ang = (Float)key->key[i];
            next_ang = (Float)next->key[i];
            d = next_ang - key_ang;
            d_before = (Float)(key->key[i] - before->key[i]);
            if (d > HALF_TURN_F) {
                d -= TURN_F;
            } else if (d <= -HALF_TURN_F) {
                d += TURN_F;
            }
            next_ang = d + key_ang;
            if (d_before > HALF_TURN_F) {
                d_before -= TURN_F;
            } else if (d_before <= -HALF_TURN_F) {
                d_before += TURN_F;
            }
            tangent = ((1.0F - before_weight) * d / span) +
                      (before_weight * d_before / span_before);
            next_tangent = ((before_weight + 1.0F) * d / span) -
                           (before_weight * d_before / span_before);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the product
             * into the sum by an FMAC */
            p1 = __builtin_fmaf(third, tangent, key_ang);
#else
            p1 = key_ang + (third * tangent);
#endif
            p2 = next_ang - (third * next_tangent);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the products
             * by b0, b2 and b3 into the sum by FMACs */
            out[i] = (Angle)__builtin_fmaf(
                b3, next_ang,
                __builtin_fmaf(b2, p2, __builtin_fmaf(b0, key_ang, b1 * p1)));
#else
            out[i] = (Angle)((b0 * key_ang) + (b1 * p1) + (b2 * p2) +
                             (b3 * next_ang));
#endif
        }
    } else if (index == 0) {
        for (i = 0; i < 3; i++) {
            key_ang = (Float)key->key[i];
            next_ang = (Float)next->key[i];
            d = next_ang - key_ang;
            d_after = (Float)(after->key[i] - next->key[i]);
            if (d > HALF_TURN_F) {
                d -= TURN_F;
            } else if (d <= -HALF_TURN_F) {
                d += TURN_F;
            }
            next_ang = d + key_ang;
            if (d_after > HALF_TURN_F) {
                d_after -= TURN_F;
            } else if (d_after <= -HALF_TURN_F) {
                d_after += TURN_F;
            }
            tangent = ((after_weight + 1.0F) * d / span) -
                      (after_weight * d_after / span_after);
            next_tangent = ((1.0F - after_weight) * d / span) +
                           (after_weight * d_after / span_after);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the product
             * into the sum by an FMAC */
            p1 = __builtin_fmaf(third, tangent, key_ang);
#else
            p1 = key_ang + (third * tangent);
#endif
            p2 = next_ang - (third * next_tangent);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the products
             * by b0, b2 and b3 into the sum by FMACs */
            out[i] = (Angle)__builtin_fmaf(
                b3, next_ang,
                __builtin_fmaf(b2, p2, __builtin_fmaf(b0, key_ang, b1 * p1)));
#else
            out[i] = (Angle)((b0 * key_ang) + (b1 * p1) + (b2 * p2) +
                             (b3 * next_ang));
#endif
        }
    } else {
        for (i = 0; i < 3; i++) {
            key_ang = (Float)key->key[i];
            next_ang = (Float)next->key[i];
            d = next_ang - key_ang;
            d_before = (Float)(key->key[i] - before->key[i]);
            d_after = (Float)(after->key[i] - next->key[i]);
            if (d > HALF_TURN_F) {
                d -= TURN_F;
            } else if (d <= -HALF_TURN_F) {
                d += TURN_F;
            }
            next_ang = d + key_ang;
            if (d_before > HALF_TURN_F) {
                d_before -= TURN_F;
            } else if (d_before <= -HALF_TURN_F) {
                d_before += TURN_F;
            }
            if (d_after > HALF_TURN_F) {
                d_after -= TURN_F;
            } else if (d_after <= -HALF_TURN_F) {
                d_after += TURN_F;
            }
            tangent = ((1.0F - before_weight) * d / span) +
                      (before_weight * d_before / span_before);
            next_tangent = ((1.0F - after_weight) * d / span) +
                           (after_weight * d_after / span_after);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the product
             * into the sum by an FMAC */
            p1 = __builtin_fmaf(third, tangent, key_ang);
#else
            p1 = key_ang + (third * tangent);
#endif
            p2 = next_ang - (third * next_tangent);
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code fuses the products
             * by b0, b2 and b3 into the sum by FMACs */
            out[i] = (Angle)__builtin_fmaf(
                b3, next_ang,
                __builtin_fmaf(b2, p2, __builtin_fmaf(b0, key_ang, b1 * p1)));
#else
            out[i] = (Angle)((b0 * key_ang) + (b1 * p1) + (b2 * p2) +
                             (b3 * next_ang));
#endif
        }
    }
}
