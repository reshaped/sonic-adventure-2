/* the chunk shape motions. a shape motion moves the vertices of a node's
 * model: its points, its normals, or both. a node's entry gives each moved
 * factor keys of NJS_MKEY_P. a key points to three floats for each vertex. a
 * shape point function copies the model's first vertex chunk to _nj_shape_buf_,
 * and moves the copy's vertices. the model then draws from the copy. its own
 * vertex list is put back after the draw.
 *
 * this header holds the names of the shape members: njCnkShapeSrc,
 * njCnkShapeS, njCnkShapeE and njCnkShapeEM. nj_motion.h holds where a shape
 * motion stands, and _nj_shape_buf_ */
#ifndef NJ_CNK_SHAPE_H
#define NJ_CNK_SHAPE_H
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* a bit of a shape motion's type: NJD_MTYPE_NORM_5's counterpart for a
 * vertex's color. a shape setup keeps each of the two only where the
 * chunk's vertices carry what it names. no shape point function moves
 * colors */
#define SHAPE_MTYPE_COLOR 0x4000

/* the drawer that draws each moved model, such as _njCnkSimpleDrawModel. the
 * shape draws set it before they walk the tree */
extern Sint32 (*_nj_cnk_draw_model_)(NJS_CNK_MODEL *model);
/* the shape draws' node steps. a draw sets _nj_cnk_shape_data_func_ from
 * _nj_cnk_shape_data_func_table_ by _nj_mot_shape_data_cnt_. it runs the
 * step at each node without NJD_EVAL_SHAPE_SKIP. the step for no factor draws
 * the node's model as it is. a null model, or a node under NJD_EVAL_HIDE, draws
 * nothing. each other step takes the node's entry from _nj_shape_data_info_ and
 * runs _nj_cnk_shape_ on the node. it moves mdata to the next entry */
extern void (*_nj_cnk_shape_data_func_)(NJS_CNK_OBJECT *object);
/* the shape steps, one for each count of factors from 0 to SHAPE_STEPS less
 * 1 */
#define SHAPE_STEPS 4
extern void (*_nj_cnk_shape_data_func_table_[SHAPE_STEPS])(
    NJS_CNK_OBJECT *object);
extern void (*_nj_cnk_shape_)(NJS_CNK_OBJECT *object);

/* the shape point functions' layout of the vertex chunk they move, in words
 * from the chunk's first. _nj_cnk_shape_nextstep_ is a vertex's size.
 * _nj_cnk_shape_v_ini_ is where the first vertex's point lies,
 * _nj_cnk_shape_n_ini_ its normal and _nj_cnk_shape_c_ini_ its color. each
 * is SHAPE_NONE where the chunk's vertices lack it.
 *
 * a point function finds the layout from its chunk's type only while
 * _nj_cnk_shape_init_flag_ is clear. it then sets the flag. a shape draw
 * clears the flag as it sets up its shape motion, before it walks the tree.
 * so the first model the draw moves gives the layout for every model after
 * it */
extern Sint32 _nj_cnk_shape_nextstep_;
extern Sint32 _nj_cnk_shape_v_ini_;
extern Sint32 _nj_cnk_shape_n_ini_;
extern Sint32 _nj_cnk_shape_c_ini_;
extern Bool _nj_cnk_shape_init_flag_;
/* take shape, which must not be null, as nj_motion.h's TAKE_MOTION takes a
 * motion: into the _nj_mot_shape_ counters and _nj_shape_data_info_. the next
 * point function then finds the layout again */
#define TAKE_SHAPE(shape)                                                      \
    _nj_mot_shape_data_type_ = (shape)->type;                                  \
    _nj_mot_shape_data_cnt_ = (shape)->inp_fn & ~NJD_MTYPE_MASK;               \
    _nj_mot_shape_spline_cnt_ = ((shape)->inp_fn & NJD_MTYPE_MASK) > 0;        \
    _nj_shape_data_info_.mdata = (shape)->mdata;                               \
    _nj_shape_data_info_.nbFrame = (shape)->nbFrame;                           \
    _nj_cnk_shape_init_flag_ = FALSE;
/* the value of a layout offset that the chunk's vertices lack */
#define SHAPE_NONE (-1)
/* a vertex chunk's head: the two words before its first vertex. the first
 * holds the chunk's type and flags. the second holds its index offset, and
 * its count of vertices in the high halfword */
#define SHAPE_HEAD 2
#define SHAPE_COUNT(chunk) ((Uint16)((chunk)[1] >> 16))
/* the words a vertex chunk of count vertices takes in the layout, its head
 * included */
#define SHAPE_WORDS(count) ((_nj_cnk_shape_nextstep_ * (count)) + SHAPE_HEAD)
/* the words a vertex's point or normal takes, and its packed color */
#define SHAPE_VECTOR 3
#define SHAPE_COLOR 1
/* what a shape point function found keys for: the points, the normals, or
 * the two summed */
#define SHAPE_POINTS 1
#define SHAPE_NORMALS 2

/* njCnkGetLinearKFdata takes key index of the nbkeys at key, and the key
 * after it. the key after the last is the first. out[0] and out[1] get their
 * values. the call returns where _nj_shape_data_info_'s frame lies between
 * their keyframes, 0 at index's and 1 at the next's. where the next keyframe
 * isn't later than index's, it returns the frame less index's keyframe.
 *
 * njCnkGetSplineKFdata does the same for the key before index, index, the
 * next and the one after. out[0] to out[3] get their values, in that order */
Float njCnkGetLinearKFdata(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                           void **out);
Float njCnkGetSplineKFdata(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                           void **out);
/* the squared length a blended normal must pass to be scaled to length 1 */
#define MIN_LENGTH2 0.00001F
/* njCnkInterpolatesF3 sets out to (1 - rate) * p1 + rate * p2.
 *
 * njCnkInterpolatesNormal sets out, three floats, the same way from v1 and
 * v2. it then scales out to length 1 by FSRRA's 1 over the square root of
 * out's length squared. out becomes (0, 0, 1) where that square is
 * MIN_LENGTH2 or less */
void njCnkInterpolatesF3(NJS_POINT3 *p1, NJS_POINT3 *p2, NJS_POINT3 *out,
                         Float rate);
void njCnkInterpolatesNormal(NJS_VECTOR *v1, NJS_VECTOR *v2, Float *out,
                             Float rate);
/* set out to the point at t on njOverhauserSpline's curve from p1 to p2. p0
 * lies before p1, and p3 after p2. t runs from 0 at p1 to 1 at p2 */
void njCnkInterpolatesSF3(NJS_POINT3 *p0, NJS_POINT3 *p1, NJS_POINT3 *p2,
                          NJS_POINT3 *p3, Float t, NJS_POINT3 *out);

/* the simple drawer's shape point functions by the shape motion's
 * _nj_mot_shape_spline_cnt_: njCnkSimpleShapeLinearPoint, then
 * njCnkSimpleShapeSplinePoint */
extern void (*_nj_cnk_s_shape_int_func_table_[INTERPOLATIONS])(
    NJS_CNK_OBJECT *object);
/* the easy drawer's shape point function: njCnkSimpleShapeLinearPoint for
 * the chunk types the easy drawer moves. they are NJD_CV, NJD_CV_D8 and
 * NJD_CV_VN */
void njCnkEasyShapeLinearPoint(NJS_CNK_OBJECT *object);
/* the multi drawers' shape point function: njCnkEasyShapeLinearPoint for an
 * NJD_CV_VN chunk alone */
void njCnkEasyMultiShapeLinearPoint(NJS_CNK_OBJECT *object);

#endif
