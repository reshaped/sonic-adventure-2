/* the shape motions' key readers and interpolations, and the shape draws'
 * node steps */
#include <machine.h>
#include <nj_cnk_shape.h>
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* where a packed color's bytes lie, as shifts */
#define ALPHA_SHIFT 24
#define RED_SHIFT 16
#define GREEN_SHIFT 8
#define BLUE_SHIFT 0

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* blend c1 and c2 into out a byte at a time. each byte of out is c1's times
 * (1 - rate) plus c2's times rate, truncated */
void njCnkInterpolatesColor(NJS_COLOR *c1, NJS_COLOR *c2, NJS_COLOR *out,
                            Float rate);
/* the node steps of _nj_cnk_shape_data_func_table_, for 0 to 3 factors */
void njCnkShapeData0(NJS_CNK_OBJECT *object);
void njCnkShapeData1(NJS_CNK_OBJECT *object);
void njCnkShapeData2(NJS_CNK_OBJECT *object);
void njCnkShapeData3(NJS_CNK_OBJECT *object);
/* the same as njCnkGetLinearKFdata, line for line */
Float njCnkGetLinearKFdataU(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                            void **out);
/* NOLINTEND(misc-use-internal-linkage) */

Bool _nj_cnk_shape_init_flag_ = FALSE;
void (*_nj_cnk_shape_data_func_table_[SHAPE_STEPS])(NJS_CNK_OBJECT *object) = {
    njCnkShapeData0,
    njCnkShapeData1,
    njCnkShapeData2,
    njCnkShapeData3,
};
Sint32 (*_nj_cnk_draw_model_)(NJS_CNK_MODEL *model);
void (*_nj_cnk_shape_data_func_)(NJS_CNK_OBJECT *object);
void (*_nj_cnk_shape_)(NJS_CNK_OBJECT *object);
Sint32 _nj_cnk_shape_nextstep_;
Sint32 _nj_cnk_shape_v_ini_;
Sint32 _nj_cnk_shape_n_ini_;
Sint32 _nj_cnk_shape_c_ini_;

Float njCnkGetSplineKFdata(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                           void **out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_P *before = &key[(index + nbkeys - 1) % nbkeys];
    NJS_MKEY_P *at = &key[index];
    NJS_MKEY_P *next = &key[(index + 1) % nbkeys];
    NJS_MKEY_P *after = &key[(index + 2) % nbkeys];
    Float t;

    divisor[1] = (Float)(next->keyframe - at->keyframe);
    t = (_nj_shape_data_info_.frame - (Float)at->keyframe) /
        divisor[(Sint32)(next->keyframe - at->keyframe) > 0];
    out[0] = before->key;
    out[1] = at->key;
    out[2] = next->key;
    out[3] = after->key;
    return t;
}

Float njCnkGetLinearKFdata(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                           void **out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_P *at = &key[index];
    NJS_MKEY_P *next = &key[(index + 1) % nbkeys];
    Float t;

    divisor[1] = (Float)(next->keyframe - at->keyframe);
    t = (_nj_shape_data_info_.frame - (Float)at->keyframe) /
        divisor[(Sint32)(next->keyframe - at->keyframe) > 0];
    out[0] = at->key;
    out[1] = next->key;
    return t;
}

Float njCnkGetLinearKFdataU(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                            void **out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_P *at = &key[index];
    NJS_MKEY_P *next = &key[(index + 1) % nbkeys];
    Float t;

    divisor[1] = (Float)(next->keyframe - at->keyframe);
    t = (_nj_shape_data_info_.frame - (Float)at->keyframe) /
        divisor[(Sint32)(next->keyframe - at->keyframe) > 0];
    out[0] = at->key;
    out[1] = next->key;
    return t;
}

void njCnkInterpolatesColor(NJS_COLOR *c1, NJS_COLOR *c2, NJS_COLOR *out,
                            Float rate)
{
    Uint32 a;
    Uint32 r;
    Uint32 g;
    Uint32 b;

    a = (Uint32)(((Float)((c1->color >> ALPHA_SHIFT) & 0xff) * (1.0F - rate)) +
                 ((Float)((c2->color >> ALPHA_SHIFT) & 0xff) * rate));
    r = (Uint32)(((Float)((c1->color >> RED_SHIFT) & 0xff) * (1.0F - rate)) +
                 ((Float)((c2->color >> RED_SHIFT) & 0xff) * rate));
    g = (Uint32)(((Float)((c1->color >> GREEN_SHIFT) & 0xff) * (1.0F - rate)) +
                 ((Float)((c2->color >> GREEN_SHIFT) & 0xff) * rate));
    b = (Uint32)(((Float)((c1->color >> BLUE_SHIFT) & 0xff) * (1.0F - rate)) +
                 ((Float)((c2->color >> BLUE_SHIFT) & 0xff) * rate));
    out->color = ((a & 0xff) << ALPHA_SHIFT) | ((r & 0xff) << RED_SHIFT) |
                 ((g & 0xff) << GREEN_SHIFT) | ((b & 0xff) << BLUE_SHIFT);
}

void njCnkInterpolatesF3(NJS_POINT3 *p1, NJS_POINT3 *p2, NJS_POINT3 *out,
                         Float rate)
{
    Float s = 1.0F - rate;

#ifdef NONMATCHING
    /* SHC rounds the product by rate and fuses the product by s into it. */
    out->x = __builtin_fmaf(p1->x, s, p2->x * rate);
    out->y = __builtin_fmaf(p1->y, s, p2->y * rate);
    out->z = __builtin_fmaf(p1->z, s, p2->z * rate);
#else
    /* @hack spelling: s first in x's product, where p1->x * s as in y's and
     * z's would do */
    out->x = (s * p1->x) + (p2->x * rate);
    out->y = (p1->y * s) + (p2->y * rate);
    out->z = (p1->z * s) + (p2->z * rate);
#endif
}

/* @hack type: out a Float *, where an NJS_VECTOR * would do */
void njCnkInterpolatesNormal(NJS_VECTOR *v1, NJS_VECTOR *v2, Float *out,
                             Float rate)
{
    Float s = 1.0F - rate;
    Float length2;
    Float scale;

    /* @hack spelling: s first in x's product, where v1->x * s as in y's and
     * z's would do */
    out[0] = (s * v1->x) + (v2->x * rate);
    out[1] = (v1->y * s) + (v2->y * rate);
    out[2] = (v1->z * s) + (v2->z * rate);
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

void njCnkInterpolatesSF3(NJS_POINT3 *p0, NJS_POINT3 *p1, NJS_POINT3 *p2,
                          NJS_POINT3 *p3, Float t, NJS_POINT3 *out)
{
    Float t2 = t * t;
    Float t3 = t * t2;
    /* the Catmull-Rom weights of p0, p1, p2 and p3 */
    Float wa = ((2.0F * t2) - t - t3) * 0.5F;
    Float wb = (1.5F * t3) - (2.5F * t2) + 1.0F;
    Float wc = (0.5F * t) - (1.5F * t3) + (2.0F * t2);
    Float wd = (t3 - t2) * 0.5F;

    out->x = (p0->x * wa) + (p1->x * wb) + (p2->x * wc) + (p3->x * wd);
    out->y = (p0->y * wa) + (p1->y * wb) + (p2->y * wc) + (p3->y * wd);
    out->z = (p0->z * wa) + (p1->z * wb) + (p2->z * wc) + (p3->z * wd);
}

void njCnkShapeData0(NJS_CNK_OBJECT *object)
{
    if (object->model != NULL && !(object->evalflags & NJD_EVAL_HIDE)) {
        _nj_cnk_draw_model_(object->model);
    }
}

void njCnkShapeData1(NJS_CNK_OBJECT *object)
{
    NJS_MDATA1 *entry = _nj_shape_data_info_.mdata;

    _nj_shape_data_info_.p = entry->p;
    _nj_shape_data_info_.nb = entry->nb;
    _nj_shape_data_info_.factor = 0;
    _nj_cnk_shape_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_shape_data_info_.mdata = entry;
}

void njCnkShapeData2(NJS_CNK_OBJECT *object)
{
    NJS_MDATA2 *entry = _nj_shape_data_info_.mdata;

    _nj_shape_data_info_.p = entry->p;
    _nj_shape_data_info_.nb = entry->nb;
    _nj_shape_data_info_.factor = 0;
    _nj_cnk_shape_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_shape_data_info_.mdata = entry;
}

void njCnkShapeData3(NJS_CNK_OBJECT *object)
{
    NJS_MDATA3 *entry = _nj_shape_data_info_.mdata;

    _nj_shape_data_info_.p = entry->p;
    _nj_shape_data_info_.nb = entry->nb;
    _nj_shape_data_info_.factor = 0;
    _nj_cnk_shape_(object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_shape_data_info_.mdata = entry;
}
