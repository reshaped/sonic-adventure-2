/* the chunk motions' steps and their interpolations between keys */
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* a third, to five decimal places */
#define THIRD 0.33333F

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set out[0] to out[2] to the point at t of a Catmull-Rom curve. keys, index,
 * nb and t are as nj_motion.h says for its interpolations. the curve runs from
 * key index's three floats to the next key's. its tangent at a key is half of
 * the key after it less the key before it. the key before key 0 is the last.
 *
 * njCnkOverhauserMotionA does the same for three angles, as floats, truncating
 * each toward 0 */
void njCnkOverhauserMotionF(NJS_MKEY_F *keys, Uint32 index, Uint32 nb,
                            Float *out);
void njCnkOverhauserMotionA(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                            Angle *out);
/* set out[0] to out[2] to the point at t of a cubic Bezier curve. keys, index,
 * nb and t are as nj_motion.h says for its interpolations. the curve runs from
 * key index's three floats to the next key's. the span is the frames between
 * the two keys. the first inner control point lies a third of the span along
 * key index's tangent. the second lies as far back along the next key's
 * tangent.
 *
 * a key's tangent is a weighted mean of the slopes of the spans either side of
 * it. a span's slope is its change in value over its frames. each slope's
 * weight is the other span's frames over both spans' frames. where index is
 * nb - 2, the next key's tangent is twice the span's slope less key index's.
 * where index is 0 and not nb - 2, key index's tangent is twice that slope less
 * the next key's. a span of no frames, or a negative one, counts as one frame.
 *
 * njCnkBezierMotionA does the same for three angles, as floats, truncating
 * each toward 0. njCnkBezierMotionAOld first takes a turn from each difference
 * of two keys' angles above HALF_TURN. it adds a turn to each at -HALF_TURN or
 * below */
void njCnkBezierMotionF(NJS_MKEY_F *keys, Uint32 index, Uint32 nb, Float *out);
void njCnkBezierMotionA(NJS_MKEY_A *keys, Uint32 index, Uint32 nb, Angle *out);
void njCnkBezierMotionAOld(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                           Angle *out);
/* the node steps of _nj_cnk_motion_data_func_table_, for 0 to 4 factors */
void njCnkMotionData0(NJS_CNK_OBJECT *object);
void njCnkMotionData1(NJS_CNK_OBJECT *object);
void njCnkMotionData2(NJS_CNK_OBJECT *object);
void njCnkMotionData3(NJS_CNK_OBJECT *object);
void njCnkMotionData4(NJS_CNK_OBJECT *object);
/* NOLINTEND(misc-use-internal-linkage) */

void (*_nj_cnk_motion_data_func_table_[NODE_STEPS])(NJS_CNK_OBJECT *object) = {
    njCnkMotionData0, njCnkMotionData1, njCnkMotionData2,
    njCnkMotionData3, njCnkMotionData4,
};
void (*_nj_cnk_motion_int_f_func_table_[INTERPOLATIONS])(NJS_MKEY_F *keys,
                                                         Uint32 index,
                                                         Uint32 nb,
                                                         Float *out) = {
    njCnkLinearMotionF,
    njCnkOverhauserMotionF,
};
void (*_nj_cnk_motion_int_a_func_table_[INTERPOLATIONS])(NJS_MKEY_A *keys,
                                                         Uint32 index,
                                                         Uint32 nb,
                                                         Angle *out) = {
    njCnkLinearMotionA,
    njCnkLinearMotionA,
};
void (*_nj_cnk_motion_int_a_func_table_old_[INTERPOLATIONS])(NJS_MKEY_A *keys,
                                                             Uint32 index,
                                                             Uint32 nb,
                                                             Angle *out) = {
    njCnkLinearMotionAOld,
    njCnkLinearMotionAOld,
};

void (*_nj_cnk_motion_data_func_)(NJS_CNK_OBJECT *object);
void (*_nj_cnk_motion_translate_)(NJS_CNK_OBJECT *object);
void (*_nj_cnk_motion_rotate_)(NJS_CNK_OBJECT *object);
void (*_nj_cnk_motion_rotate_qua_)(NJS_CNK_OBJECT *object);
void (*_nj_cnk_motion_scale_)(NJS_CNK_OBJECT *object);
void (*_nj_cnk_motion_rotate_zxy_)(NJS_CNK_OBJECT *object);
void (*_nj_cnk_motion_callback_)(NJS_CNK_OBJECT *object);

void njCnkLinearMotionQ(NJS_MKEY_QUAT *keys, Uint32 index, Uint32 nb,
                        Float *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_QUAT *key;
    NJS_MKEY_QUAT *next;
    Float t;
    Float s;
    Float dot;
    Float length2;
    Float scale;

    key = &keys[index];
    next = &keys[(index + 1) % nb];
    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) /
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
        /* @hack spelling: t multiplied by -1, where its negation would
         * do */
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

void njCnkLinearMotionF(NJS_MKEY_F *keys, Uint32 index, Uint32 nb, Float *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_F *key;
    NJS_MKEY_F *next;
    Float t;
    Float s;

    key = &keys[index];
    next = &keys[(index + 1) % nb];
    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) /
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

void njCnkLinearMotionA(NJS_MKEY_A *keys, Uint32 index, Uint32 nb, Angle *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_A *key;
    NJS_MKEY_A *next;
    Float t;
    Float s;

    key = &keys[index];
    next = &keys[(index + 1) % nb];
    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) /
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

void njCnkOverhauserMotionF(NJS_MKEY_F *keys, Uint32 index, Uint32 nb,
                            Float *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_F *before;
    NJS_MKEY_F *key;
    NJS_MKEY_F *next;
    NJS_MKEY_F *after;
    Float t;
    Float t2;
    Float t3;
    /* the Catmull-Rom weights of before, key, next and after */
    Float wa;
    Float wb;
    Float wc;
    Float wd;

    before = &keys[(index + nb - 1) % nb];
    key = &keys[index];
    next = &keys[(index + 1) % nb];
    after = &keys[(index + 2) % nb];
    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) /
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
}

void njCnkOverhauserMotionA(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                            Angle *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_A *before;
    NJS_MKEY_A *key;
    NJS_MKEY_A *next;
    NJS_MKEY_A *after;
    Float t;
    Float t2;
    Float t3;
    /* the Catmull-Rom weights of before, key, next and after */
    Float wa;
    Float wb;
    Float wc;
    Float wd;

    before = &keys[(index + nb - 1) % nb];
    key = &keys[index];
    next = &keys[(index + 1) % nb];
    after = &keys[(index + 2) % nb];
    divisor[1] = (Float)(next->keyframe - key->keyframe);
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) /
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
}

void njCnkBezierMotionF(NJS_MKEY_F *keys, Uint32 index, Uint32 nb, Float *out)
{
    NJS_MKEY_F *before;
    NJS_MKEY_F *key;
    NJS_MKEY_F *next;
    NJS_MKEY_F *after;
    /* the frames from before to key, next to after, and key to next */
    Float span_before;
    Float span_after;
    Float span;
    /* the weights of the slopes before key and after next in their keys'
     * tangents */
    Float before_weight;
    Float after_weight;
    Float third;
    Float t;
    Float t2;
    Float b3;
    Float s;
    Float s2;
    Float b0;
    Float b1;
    Float b2;
    Float d;
    Float d_before;
    Float d_after;
    Float tangent;
    Float next_tangent;
    Float p1;
    Float p2;
    Uint32 i;

    before = &keys[(index + nb - 1) % nb];
    key = &keys[index];
    next = &keys[(index + 1) % nb];
    after = &keys[(index + 2) % nb];
    span = (Float)next->keyframe - (Float)key->keyframe;
    if (span <= 0.0F) {
        span = 1.0F;
    }
    span_after = (Float)after->keyframe - (Float)next->keyframe;
    if (span_after <= 0.0F) {
        span_after = 1.0F;
    }
    span_before = (Float)key->keyframe - (Float)before->keyframe;
    if (span_before <= 0.0F) {
        span_before = 1.0F;
    }
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) / span;
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
            p1 = __builtin_fmaf(tangent, third, key->key[i]);
#else
            p1 = key->key[i] + (tangent * third);
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
            p1 = __builtin_fmaf(tangent, third, key->key[i]);
#else
            p1 = key->key[i] + (tangent * third);
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
            p1 = __builtin_fmaf(tangent, third, key->key[i]);
#else
            p1 = key->key[i] + (tangent * third);
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

void njCnkBezierMotionA(NJS_MKEY_A *keys, Uint32 index, Uint32 nb, Angle *out)
{
    NJS_MKEY_A *before;
    NJS_MKEY_A *key;
    NJS_MKEY_A *next;
    NJS_MKEY_A *after;
    /* the frames from before to key, next to after, and key to next */
    Float span_before;
    Float span_after;
    Float span;
    /* the weights of the slopes before key and after next in their keys'
     * tangents */
    Float before_weight;
    Float after_weight;
    Float third;
    Float t;
    Float t2;
    Float b3;
    Float s;
    Float s2;
    Float b0;
    Float b1;
    Float b2;
    Float d;
    Float d_before;
    Float d_after;
    Float tangent;
    Float next_tangent;
    Float p1;
    Float p2;
    Uint32 i;

    before = &keys[(index + nb - 1) % nb];
    key = &keys[index];
    next = &keys[(index + 1) % nb];
    after = &keys[(index + 2) % nb];
    span = (Float)next->keyframe - (Float)key->keyframe;
    if (span <= 0.0F) {
        span = 1.0F;
    }
    span_after = (Float)after->keyframe - (Float)next->keyframe;
    if (span_after <= 0.0F) {
        span_after = 1.0F;
    }
    span_before = (Float)key->keyframe - (Float)before->keyframe;
    if (span_before <= 0.0F) {
        span_before = 1.0F;
    }
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) / span;
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
            p1 = __builtin_fmaf(tangent, third, (Float)key->key[i]);
#else
            p1 = (Float)key->key[i] + (tangent * third);
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
            p1 = __builtin_fmaf(tangent, third, (Float)key->key[i]);
#else
            p1 = (Float)key->key[i] + (tangent * third);
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
            p1 = __builtin_fmaf(tangent, third, (Float)key->key[i]);
#else
            p1 = (Float)key->key[i] + (tangent * third);
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

void njCnkMotionIntTranslate(NJS_CNK_OBJECT *object)
{
    NJS_MKEY_F *keys;
    Uint32 nb;
    Uint32 low;
    Uint32 high;
    Uint32 middle;
    Uint32 frame;
    Float value[3];

    keys = _nj_motion_data_info_.p[_nj_motion_data_info_.factor];
    if (keys) {
        nb = _nj_motion_data_info_.nb[_nj_motion_data_info_.factor];
        low = 0;
        high = nb;
        frame = (Uint32)_nj_motion_data_info_.frame;
        while (high - low > 1) {
            middle = (low + high) >> 1;
            if (frame >= keys[middle].keyframe) {
                low = middle;
            } else {
                high = middle;
            }
        }
        _nj_motion_int_f_func_(keys, low, nb, value);
        njTranslateEx((NJS_VECTOR *)value);
    } else {
        njTranslateEx((NJS_VECTOR *)object->pos);
    }
    _nj_motion_data_info_.factor++;
}

void njCnkMotionIntScale(NJS_CNK_OBJECT *object)
{
    NJS_MKEY_F *keys;
    Uint32 nb;
    Uint32 low;
    Uint32 high;
    Uint32 middle;
    Uint32 frame;
    Float value[3];

    keys = _nj_motion_data_info_.p[_nj_motion_data_info_.factor];
    if (keys) {
        nb = _nj_motion_data_info_.nb[_nj_motion_data_info_.factor];
        low = 0;
        high = nb;
        frame = (Uint32)_nj_motion_data_info_.frame;
        while (high - low > 1) {
            middle = (low + high) >> 1;
            if (frame >= keys[middle].keyframe) {
                low = middle;
            } else {
                high = middle;
            }
        }
        _nj_motion_int_f_func_(keys, low, nb, value);
        njScaleEx((NJS_VECTOR *)value);
    } else {
        njScaleEx((NJS_VECTOR *)object->scl);
    }
    _nj_motion_data_info_.factor++;
}

void njCnkMotionIntRotateXYZ(NJS_CNK_OBJECT *object)
{
    NJS_MKEY_A *keys;
    Uint32 nb;
    Uint32 low;
    Uint32 high;
    Uint32 middle;
    Uint32 frame;
    Angle value[3];

    keys = _nj_motion_data_info_.p[_nj_motion_data_info_.factor];
    if (keys) {
        nb = _nj_motion_data_info_.nb[_nj_motion_data_info_.factor];
        low = 0;
        high = nb;
        frame = (Uint32)_nj_motion_data_info_.frame;
        while (high - low > 1) {
            middle = (low + high) >> 1;
            if (frame >= keys[middle].keyframe) {
                low = middle;
            } else {
                high = middle;
            }
        }
        _nj_motion_int_a_func_(keys, low, nb, value);
        njRotateEx(value, 0);
    } else {
        njRotateEx(object->ang, 0);
    }
    _nj_motion_data_info_.factor++;
}

void njCnkMotionIntRotateZXY(NJS_CNK_OBJECT *object)
{
    NJS_MKEY_A *keys;
    Uint32 nb;
    Uint32 low;
    Uint32 high;
    Uint32 middle;
    Uint32 frame;
    Angle value[3];

    keys = _nj_motion_data_info_.p[_nj_motion_data_info_.factor];
    if (keys) {
        nb = _nj_motion_data_info_.nb[_nj_motion_data_info_.factor];
        low = 0;
        high = nb;
        frame = (Uint32)_nj_motion_data_info_.frame;
        while (high - low > 1) {
            middle = (low + high) >> 1;
            if (frame >= keys[middle].keyframe) {
                low = middle;
            } else {
                high = middle;
            }
        }
        _nj_motion_int_a_func_(keys, low, nb, value);
        njRotateEx(value, 1);
    } else {
        njRotateEx(object->ang, 1);
    }
    _nj_motion_data_info_.factor++;
}

void njCnkMotionIntQuaternion(NJS_CNK_OBJECT *object)
{
    NJS_MKEY_QUAT *keys;
    Uint32 nb;
    Uint32 low;
    Uint32 high;
    Uint32 middle;
    Uint32 frame;
    NJS_QUATERNION value;

    keys = _nj_motion_data_info_.p[_nj_motion_data_info_.factor];
    if (keys) {
        nb = _nj_motion_data_info_.nb[_nj_motion_data_info_.factor];
        low = 0;
        high = nb;
        frame = (Uint32)_nj_motion_data_info_.frame;
        while (high - low > 1) {
            middle = (low + high) >> 1;
            if (frame >= keys[middle].keyframe) {
                low = middle;
            } else {
                high = middle;
            }
        }
        njCnkLinearMotionQ(keys, low, nb, (Float *)&value);
        njQuaternionEx(&value);
    } else {
        njRotateEx(object->ang, object->evalflags & NJD_EVAL_ZXY_ANG);
    }
    _nj_motion_data_info_.factor++;
}

void njCnkMotionObjTranslate(NJS_CNK_OBJECT *object)
{
    njTranslateEx((NJS_VECTOR *)object->pos);
}

void njCnkMotionObjRotateXYZ(NJS_CNK_OBJECT *object)
{
    njRotateEx(object->ang, 0);
}

void njCnkMotionObjRotateZXY(NJS_CNK_OBJECT *object)
{
    njRotateEx(object->ang, 1);
}

void njCnkMotionObjRotate(NJS_CNK_OBJECT *object)
{
    njRotateEx(object->ang, object->evalflags & NJD_EVAL_ZXY_ANG);
}

void njCnkMotionObjScale(NJS_CNK_OBJECT *object)
{
    njScaleEx((NJS_VECTOR *)object->scl);
}

void njCnkMotionData0(NJS_CNK_OBJECT *object)
{
    _nj_cnk_motion_translate_(object);
    _nj_cnk_motion_rotate_(object);
    _nj_cnk_motion_scale_(object);
}

void njCnkMotionData1(NJS_CNK_OBJECT *object)
{
    NJS_MDATA1 *entry = _nj_motion_data_info_.mdata;

    _nj_motion_data_info_.p = entry->p;
    _nj_motion_data_info_.nb = entry->nb;
    _nj_motion_data_info_.factor = 0;
    _nj_cnk_motion_translate_(object);
    _nj_cnk_motion_rotate_(object);
    _nj_cnk_motion_scale_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_motion_data_info_.mdata = entry;
}

void njCnkMotionData2(NJS_CNK_OBJECT *object)
{
    NJS_MDATA2 *entry = _nj_motion_data_info_.mdata;

    _nj_motion_data_info_.p = entry->p;
    _nj_motion_data_info_.nb = entry->nb;
    _nj_motion_data_info_.factor = 0;
    _nj_cnk_motion_translate_(object);
    _nj_cnk_motion_rotate_(object);
    _nj_cnk_motion_scale_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_motion_data_info_.mdata = entry;
}

void njCnkMotionData3(NJS_CNK_OBJECT *object)
{
    NJS_MDATA3 *entry = _nj_motion_data_info_.mdata;

    _nj_motion_data_info_.p = entry->p;
    _nj_motion_data_info_.nb = entry->nb;
    _nj_motion_data_info_.factor = 0;
    _nj_cnk_motion_translate_(object);
    _nj_cnk_motion_rotate_(object);
    _nj_cnk_motion_scale_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_motion_data_info_.mdata = entry;
}

void njCnkMotionData4(NJS_CNK_OBJECT *object)
{
    NJS_MDATA4 *entry = _nj_motion_data_info_.mdata;

    _nj_motion_data_info_.p = entry->p;
    _nj_motion_data_info_.nb = entry->nb;
    _nj_motion_data_info_.factor = 0;
    _nj_cnk_motion_translate_(object);
    _nj_cnk_motion_rotate_(object);
    _nj_cnk_motion_scale_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_motion_data_info_.mdata = entry;
}

void njCnkSetMotionCallback(void (*func)(NJS_CNK_OBJECT *object))
{
    _nj_cnk_motion_callback_ = func;
}

void njCnkLinearMotionAOld(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                           Angle *out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_A *key;
    NJS_MKEY_A *next;
    Float t;
    Float s;
    /* the next key's angles. each is moved a turn toward key's where the two
     * lie over half a turn apart */
    Angle x;
    Angle y;
    Angle z;

    key = &keys[index];
    next = &keys[(index + 1) % nb];
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

void njCnkBezierMotionAOld(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                           Angle *out)
{
    NJS_MKEY_A *before;
    NJS_MKEY_A *key;
    NJS_MKEY_A *next;
    NJS_MKEY_A *after;
    /* the frames from before to key, next to after, and key to next */
    Float span_before;
    Float span_after;
    Float span;
    /* the weights of the slopes before key and after next in their keys'
     * tangents */
    Float before_weight;
    Float after_weight;
    Float third;
    Float t;
    Float t2;
    Float b3;
    Float s;
    Float s2;
    Float b0;
    Float b1;
    Float b2;
    Float d;
    Float d_before;
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

    before = &keys[(index + nb - 1) % nb];
    key = &keys[index];
    next = &keys[(index + 1) % nb];
    after = &keys[(index + 2) % nb];
    span = (Float)next->keyframe - (Float)key->keyframe;
    if (span <= 0.0F) {
        span = 1.0F;
    }
    span_after = (Float)after->keyframe - (Float)next->keyframe;
    if (span_after <= 0.0F) {
        span_after = 1.0F;
    }
    span_before = (Float)key->keyframe - (Float)before->keyframe;
    if (span_before <= 0.0F) {
        span_before = 1.0F;
    }
    t = (_nj_motion_data_info_.frame - (Float)key->keyframe) / span;
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
