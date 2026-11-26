/* The simple shape link point functions move no normals for a slot whose
 * shape motion moves normals the layout lacks. slot 0's setup found the
 * layout of an NJD_CV chunk, which has none, and cleared the normals from
 * slot 0's shape type alone. slot 1's type still names them. */
#include <assert.h>
#include <nj_cnk.h>
#include <nj_cnk_motlink.h>
#include <nj_cnk_shape.h>
#include <nj_motion.h>
#include <ninja.h>
#include <string.h>

#define VERTICES 2
/* the chunk's words: its head, three floats a vertex, and NJD_CE */
#define CHUNK_WORDS (SHAPE_HEAD + (SHAPE_VECTOR * VERTICES) + 1)
#define SLOT 1

void njCnkSimpleShapeLinkLinearPoint(NJS_CNK_OBJECT *object);
void njCnkSimpleShapeLinkSplinePoint(NJS_CNK_OBJECT *object);

Sint32 (*_nj_cnk_draw_model_)(NJS_CNK_MODEL *model);
Sint32 _nj_cnk_shape_nextstep_;
Sint32 _nj_cnk_shape_v_ini_;
Sint32 _nj_cnk_shape_n_ini_;
Sint32 _nj_cnk_shape_c_ini_;
Bool _nj_cnk_shape_init_flag_;
Sint32 *_nj_cnk_shapelink_vlist_[2];
Uint16 _nj_mot_current_data_;
Float _nj_mot_current_frame_;
Uint16 _nj_motlink_shape_data_cnt_[2];
Uint16 _nj_motlink_shape_data_type_[2];
Float *_nj_shape_buf_;
struct nj_motion_info _nj_shapelink_data_info_[2];

/* the key fetchers give index's key, then the next key. each point blend
 * takes the next key, so the points a call moves take that key's values */
Float njCnkGetLinkLinearKFdata(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                               void **out)
{
    out[0] = key[index].key;
    out[1] = key[(index + 1) % nbkeys].key;
    return 0.5F;
}
Float njCnkGetLinkSplineKFdata(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                               void **out)
{
    out[0] = key[index].key;
    out[1] = key[index].key;
    out[2] = key[(index + 1) % nbkeys].key;
    out[3] = key[(index + 1) % nbkeys].key;
    return 0.5F;
}
void njCnkInterpolatesF3(NJS_POINT3 *p1, NJS_POINT3 *p2, NJS_POINT3 *out,
                         Float rate)
{
    *out = *p2;
}
void njCnkInterpolatesSF3(NJS_POINT3 *p0, NJS_POINT3 *p1, NJS_POINT3 *p2,
                          NJS_POINT3 *p3, Float t, NJS_POINT3 *out)
{
    *out = *p2;
}
void njCnkInterpolatesNormal(NJS_VECTOR *v1, NJS_VECTOR *v2, Float *out,
                             Float rate)
{
    out[0] = v1->x;
    out[1] = v1->y;
    out[2] = v1->z;
}

static Sint32 chunk[CHUNK_WORDS];
static NJS_CNK_MODEL model;
static NJS_CNK_OBJECT node;
static NJS_POINT3 first[VERTICES] = {{1.0F, 2.0F, 3.0F}, {4.0F, 5.0F, 6.0F}};
static NJS_POINT3 second[VERTICES] = {{7.0F, 8.0F, 9.0F},
                                      {10.0F, 11.0F, 12.0F}};
static NJS_MKEY_P point_keys[2] = {{0, first}, {1, second}};
static NJS_MKEY_P normal_keys[2] = {{0, first}, {1, second}};
static void *factors[2] = {point_keys, normal_keys};
static Uint32 counts[2] = {2, 2};
static Float shape_buf[2 * CHUNK_WORDS];

static void check(void (*point)(NJS_CNK_OBJECT *object))
{
    Sint32 *copy;
    int i;

    memset(shape_buf, 0, sizeof(shape_buf));
    _nj_cnk_shape_init_flag_ = TRUE;
    _nj_cnk_shape_nextstep_ = SHAPE_VECTOR;
    copy = (Sint32 *)&shape_buf[SHAPE_WORDS(VERTICES) * SLOT];
    _nj_cnk_shape_v_ini_ = SHAPE_HEAD;
    _nj_cnk_shape_n_ini_ = SHAPE_NONE;
    _nj_cnk_shape_c_ini_ = SHAPE_NONE;
    _nj_cnk_shapelink_vlist_[SLOT] = NULL;
    _nj_mot_current_data_ = SLOT;
    _nj_motlink_shape_data_cnt_[SLOT] = 2;
    _nj_motlink_shape_data_type_[SLOT] = NJD_MTYPE_VERT_4 | NJD_MTYPE_NORM_5;
    _nj_shapelink_data_info_[SLOT].p = factors;
    _nj_shapelink_data_info_[SLOT].nb = counts;
    _nj_shapelink_data_info_[SLOT].factor = 0;
    _nj_shapelink_data_info_[SLOT].frame = 0.0F;
    point(&node);
    assert(_nj_motlink_shape_data_type_[SLOT] == NJD_MTYPE_VERT_4);
    assert(_nj_shapelink_data_info_[SLOT].factor == 1);
    assert(_nj_cnk_shapelink_vlist_[SLOT] == copy);
    assert(copy[0] == chunk[0] && copy[1] == chunk[1]);
    assert(memcmp(&copy[SHAPE_HEAD], second, sizeof(second)) == 0);
    assert(copy[SHAPE_WORDS(VERTICES)] == NJD_CE);
    /* slot 0's copy stays as it was */
    for (i = 0; i < SHAPE_WORDS(VERTICES); i++) {
        assert(((Sint32 *)shape_buf)[i] == 0);
    }
}

int main(void)
{
    chunk[0] = NJD_CV | (((SHAPE_VECTOR * VERTICES) + 1) << 16);
    chunk[1] = VERTICES << 16;
    memcpy(&chunk[SHAPE_HEAD], first, sizeof(first));
    chunk[CHUNK_WORDS - 1] = NJD_CE;
    model.vlist = chunk;
    node.model = &model;
    _nj_shape_buf_ = shape_buf;
    check(njCnkSimpleShapeLinkLinearPoint);
    check(njCnkSimpleShapeLinkSplinePoint);
    return 0;
}
