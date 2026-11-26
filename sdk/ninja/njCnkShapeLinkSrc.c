/* the shape links' key readers and node steps, and a Bezier interpolation */
#include <nj_cnk_motlink.h>
#include <nj_cnk_shape.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the same as njCnkGetLinkLinearKFdata, line for line */
Float njCnkGetLinkLinearKFdataU(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                                void **out);
/* set out to the point at t on the cubic Bezier curve from p0 to p1. p2 is the
 * control point after p0, and p3 the one before p1. t runs from 0 at p0 to 1
 * at p1 */
void njCnkInterpolatesBF3(NJS_POINT3 *p0, NJS_POINT3 *p1, NJS_POINT3 *p2,
                          NJS_POINT3 *p3, Float t, NJS_POINT3 *out);
/* the node steps of _nj_cnk_shapelink_data_func_table_, for 0 to 3 factors */
void njCnkShapeLinkData0(NJS_CNK_OBJECT *object);
void njCnkShapeLinkData1(NJS_CNK_OBJECT *object);
void njCnkShapeLinkData2(NJS_CNK_OBJECT *object);
void njCnkShapeLinkData3(NJS_CNK_OBJECT *object);
/* NOLINTEND(misc-use-internal-linkage) */

void (*_nj_cnk_shapelink_data_func_table_[SHAPE_STEPS])(
    NJS_CNK_OBJECT *object) = {
    njCnkShapeLinkData0,
    njCnkShapeLinkData1,
    njCnkShapeLinkData2,
    njCnkShapeLinkData3,
};
Sint32 *_nj_cnk_shapelink_vlist_[2];
void (*_nj_cnk_shapelink_data_func_[2])(NJS_CNK_OBJECT *object);
void (*_nj_cnk_shapelink_data_[2])(NJS_CNK_OBJECT *object);

Float njCnkGetLinkSplineKFdata(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
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
    t = (_nj_shapelink_data_info_[_nj_mot_current_data_].frame -
         (Float)at->keyframe) /
        divisor[(Sint32)(next->keyframe - at->keyframe) > 0];
    out[0] = before->key;
    out[1] = at->key;
    out[2] = next->key;
    out[3] = after->key;
    return t;
}

Float njCnkGetLinkLinearKFdata(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                               void **out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_P *at = &key[index];
    NJS_MKEY_P *next = &key[(index + 1) % nbkeys];
    Float t;

    divisor[1] = (Float)(next->keyframe - at->keyframe);
    t = (_nj_shapelink_data_info_[_nj_mot_current_data_].frame -
         (Float)at->keyframe) /
        divisor[(Sint32)(next->keyframe - at->keyframe) > 0];
    out[0] = at->key;
    out[1] = next->key;
    return t;
}

Float njCnkGetLinkLinearKFdataU(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                                void **out)
{
    /* @hack spelling: the divisor picked from an array of 1 and the span,
     * where a conditional would do */
    Float divisor[2] = {1.0F, 1.0F};
    NJS_MKEY_P *at = &key[index];
    NJS_MKEY_P *next = &key[(index + 1) % nbkeys];
    Float t;

    divisor[1] = (Float)(next->keyframe - at->keyframe);
    t = (_nj_shapelink_data_info_[_nj_mot_current_data_].frame -
         (Float)at->keyframe) /
        divisor[(Sint32)(next->keyframe - at->keyframe) > 0];
    out[0] = at->key;
    out[1] = next->key;
    return t;
}

void njCnkInterpolatesBF3(NJS_POINT3 *p0, NJS_POINT3 *p1, NJS_POINT3 *p2,
                          NJS_POINT3 *p3, Float t, NJS_POINT3 *out)
{
    Float s = 1.0F - t;
    Float w0 = s * s * s;
    Float w2 = 3.0F * t * s * s;
    Float w3 = 3.0F * t * t * s;
    Float w1 = t * t * t;

    out->x = (p0->x * w0) + (p1->x * w1) + (p2->x * w2) + (p3->x * w3);
    out->y = (p0->y * w0) + (p1->y * w1) + (p2->y * w2) + (p3->y * w3);
    out->z = (p0->z * w0) + (p1->z * w1) + (p2->z * w2) + (p3->z * w3);
}

void njCnkShapeLinkData0(NJS_CNK_OBJECT *object)
{
    if (object->model != NULL && !(object->evalflags & NJD_EVAL_HIDE)) {
        _nj_cnk_shapelink_vlist_[_nj_mot_current_data_] = NULL;
    }
}

void njCnkShapeLinkData1(NJS_CNK_OBJECT *object)
{
    NJS_MDATA1 *entry = _nj_shapelink_data_info_[_nj_mot_current_data_].mdata;

    _nj_shapelink_data_info_[_nj_mot_current_data_].p = entry->p;
    _nj_shapelink_data_info_[_nj_mot_current_data_].nb = entry->nb;
    _nj_shapelink_data_info_[_nj_mot_current_data_].factor = 0;
    _nj_cnk_shapelink_data_[_nj_mot_current_data_](object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_shapelink_data_info_[_nj_mot_current_data_].mdata = entry;
}

void njCnkShapeLinkData2(NJS_CNK_OBJECT *object)
{
    NJS_MDATA2 *entry = _nj_shapelink_data_info_[_nj_mot_current_data_].mdata;

    _nj_shapelink_data_info_[_nj_mot_current_data_].p = entry->p;
    _nj_shapelink_data_info_[_nj_mot_current_data_].nb = entry->nb;
    _nj_shapelink_data_info_[_nj_mot_current_data_].factor = 0;
    _nj_cnk_shapelink_data_[_nj_mot_current_data_](object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_shapelink_data_info_[_nj_mot_current_data_].mdata = entry;
}

void njCnkShapeLinkData3(NJS_CNK_OBJECT *object)
{
    NJS_MDATA3 *entry = _nj_shapelink_data_info_[_nj_mot_current_data_].mdata;

    _nj_shapelink_data_info_[_nj_mot_current_data_].p = entry->p;
    _nj_shapelink_data_info_[_nj_mot_current_data_].nb = entry->nb;
    _nj_shapelink_data_info_[_nj_mot_current_data_].factor = 0;
    _nj_cnk_shapelink_data_[_nj_mot_current_data_](object);
    /* @hack spelling: entry stepped on, then stored, where a store of
     * entry + 1 would do */
    entry++;
    _nj_shapelink_data_info_[_nj_mot_current_data_].mdata = entry;
}
