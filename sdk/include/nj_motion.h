/* the state Ninja's motion calls share, and the chunk motion steps. a chunk
 * motion draw sets the state from its motion and frame. it then walks the
 * object's tree. at each node without NJD_EVAL_SKIP, a node step takes the
 * node's entry from the motion's mdata. it then runs a position, an angle and a
 * scale step on the node, in that order. each puts its transform before the
 * current matrix.
 *
 * nj_cnk_shape.h holds the shape motion steps. nj_cnk_motlink.h holds the state
 * and steps of a motion link */
#ifndef NJ_MOTION_H
#define NJ_MOTION_H
#include <nj_motion_types.h> /* IWYU pragma: export */
#include <ninja.h>
#include <sg_xpt.h>

/* _nj_motion_data_info_ is where the motion stands, and _nj_shape_data_info_
 * where the shape motion stands. a draw, or njSetCnkCurrentMotion, sets
 * _nj_mot_data_cnt_ to its motion's count of factors, inp_fn less its
 * NJD_MTYPE_MASK bits. it sets _nj_mot_data_type_ to the motion's type. it sets
 * _nj_mot_spline_cnt_ to the motion's spline count: 1 where inp_fn has an
 * NJD_MTYPE_MASK bit, else 0. given a null motion, each sets all three to 0. a
 * shape draw sets the three _nj_mot_shape_ counters from its shape motion the
 * same way. the basic models' shape steps, in njMotSrc.c, read
 * _nj_mot_shape_data_cnt_ otherwise: as nj_cnk_shape.h's SHAPE_POINTS,
 * SHAPE_NORMALS or their sum.
 *
 * a step interpolates a factor of floats through _nj_motion_int_f_func_. it
 * interpolates a factor of angles through _nj_motion_int_a_func_. a link draw
 * points both at interpolations that write the current slot's values. the link
 * draws store their motions' spline counts in _nj_mot_shape_spline_cnt_. they
 * leave _nj_mot_spline_cnt_ as it was, the defect the bug tag in
 * njCnkMotLinkS.c describes.
 *
 * _nj_shape_buf_ is the buffer njInitShape takes */
extern struct nj_motion_info _nj_motion_data_info_;
extern struct nj_motion_info _nj_shape_data_info_;
extern Uint16 _nj_mot_data_cnt_;
extern Uint16 _nj_mot_data_type_;
extern Uint16 _nj_mot_shape_data_cnt_;
extern Uint16 _nj_mot_shape_data_type_;
extern void (*_nj_motion_int_f_func_)(NJS_MKEY_F *keys, Uint32 index, Uint32 nb,
                                      Float *out);
extern void (*_nj_motion_int_a_func_)(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                                      Angle *out);
extern Uint16 _nj_mot_spline_cnt_;
extern Uint16 _nj_mot_shape_spline_cnt_;
extern Float *_nj_shape_buf_;
/* njMotSrc.c's node steps for basic models run _nj_motion_translate_,
 * _nj_motion_rotate_ and _nj_motion_scale_ on the node, in that order. a shape
 * node step that takes an entry runs _nj_motion_shape_. the shape steps draw
 * the node's model through _nj_draw_model_sub_. so does the shape node step
 * that takes none */
extern void (*_nj_motion_translate_)(NJS_OBJECT *object);
extern void (*_nj_motion_rotate_)(NJS_OBJECT *object);
extern void (*_nj_motion_scale_)(NJS_OBJECT *object);
extern void (*_nj_motion_shape_)(NJS_OBJECT *object);
extern void (*_nj_draw_model_sub_)(NJS_MODEL *model);

/* the camera motion calls copy their camera to _nj_motion_camera_, and move the
 * copy. _nj_cmotion_data_info_ is where the camera motion stands. the position
 * interpolates through _nj_cmotion_int_f_func_, which a call picks by the
 * motion's spline count. the target or direction interpolates through
 * _nj_cmotion_int_l_func_, and the roll and the angle of view through
 * _nj_cmotion_int_a_func_. both take the straight line.
 *
 * njGetCameraMotion and njGetCameraAction write each factor's value through the
 * _nj_cmot_ pointer of its kind. each write moves its pointer past the value */
extern NJS_CAMERA _nj_motion_camera_;
extern struct nj_motion_info _nj_cmotion_data_info_;
extern void (*_nj_cmotion_int_f_func_)(NJS_MKEY_F *keys, Uint32 index,
                                       Uint32 nb, Float *out);
extern void (*_nj_cmotion_int_l_func_)(NJS_MKEY_F *keys, Uint32 index,
                                       Uint32 nb, Float *out);
extern void (*_nj_cmotion_int_a_func_)(NJS_MKEY_A1 *keys, Uint32 index,
                                       Uint32 nb, Angle *out);
extern Float *_nj_cmot_pos_buf_ptr_;
extern Float *_nj_cmot_vect_buf_ptr_;
extern Angle *_nj_cmot_roll_buf_ptr_;
extern Angle *_nj_cmot_ang_buf_ptr_;
/* the spline count that picks the straight line from each interpolation table
 * below */
#define STRAIGHT_LINE 0
/* the interpolations of a table indexed by spline count: the straight line,
 * then a spline */
#define INTERPOLATIONS 2
/* the camera calls' interpolations, indexed by spline count. the float table
 * holds the straight line, then njOverhauserSpline's curve. the angle table
 * holds the straight line alone */
extern void (*_nj_cmotion_int_f_func_table_[INTERPOLATIONS])(NJS_MKEY_F *keys,
                                                             Uint32 index,
                                                             Uint32 nb,
                                                             Float *out);
extern void (*_nj_cmotion_int_a_func_table_[1])(NJS_MKEY_A1 *keys, Uint32 index,
                                                Uint32 nb, Angle *out);
/* set lower to the index of the last key at keys whose keyframe is at most
 * frame. lower is 0 where no key after key 0 is that early. lower must start at
 * 0, and upper at the count of keys, which must lie in keyframe order */
#define FIND_KEY(keys, frame, lower, upper, middle)                            \
    while ((upper) - (lower) > 1) {                                            \
        (middle) = ((lower) + (upper)) >> 1;                                   \
        if ((frame) >= (keys)[middle].keyframe) {                              \
            (lower) = (middle);                                                \
        } else {                                                               \
            (upper) = (middle);                                                \
        }                                                                      \
    }
/* start reading motion, a camera motion, at start_frame: set
 * _nj_cmotion_data_info_ from them, and _nj_motion_camera_ to *camera. kind
 * takes the motion's type, and factors its count of factors. spline takes its
 * spline count, and entry its entry */
#define START_CAMERA_MOTION(camera, motion, start_frame, kind, factors,        \
                            spline, entry)                                     \
    {                                                                          \
        (kind) = (motion)->type;                                               \
        (factors) = (motion)->inp_fn & ~NJD_MTYPE_MASK;                        \
        (spline) = ((motion)->inp_fn & NJD_MTYPE_MASK) > 0;                    \
        _nj_cmotion_data_info_.mdata = (motion)->mdata;                        \
        _nj_cmotion_data_info_.nbFrame = (motion)->nbFrame;                    \
        _nj_cmotion_data_info_.frame = (start_frame);                          \
        _nj_cmotion_data_info_.factor = 0;                                     \
        /* @hack type: m set to a plain 0, where NULL would do */              \
        _nj_cmotion_data_info_.m = 0;                                          \
        _nj_motion_camera_ = *(camera);                                        \
        _nj_cmotion_int_f_func_ = _nj_cmotion_int_f_func_table_[spline];       \
        _nj_cmotion_int_l_func_ =                                              \
            _nj_cmotion_int_f_func_table_[STRAIGHT_LINE];                      \
        _nj_cmotion_int_a_func_ =                                              \
            _nj_cmotion_int_a_func_table_[STRAIGHT_LINE];                      \
        (entry) = _nj_cmotion_data_info_.mdata;                                \
    }
/* a switch's case for a camera motion's entry of n factors. it points
 * _nj_cmotion_data_info_'s p and nb at mdata's */
#define CAMERA_ENTRY_CASE(n, mdata)                                            \
    case n: {                                                                  \
        NJS_MDATA##n *entry = (mdata);                                         \
                                                                               \
        _nj_cmotion_data_info_.p = entry->p;                                   \
        _nj_cmotion_data_info_.nb = entry->nb;                                 \
    } break

/* the node step, and the position, angle and scale steps it runs. a draw takes
 * the node step from _nj_cnk_motion_data_func_table_ by _nj_mot_data_cnt_. a
 * draw or njSetCnkCurrentMotion picks each other step by the motion's type. it
 * takes an Int step for a factor the motion moves, and an Obj step for any
 * other. a draw picks the angle step's order by the NJD_EVAL_ZXY_ANG of the
 * object it was given */
extern void (*_nj_cnk_motion_data_func_)(NJS_CNK_OBJECT *object);
extern void (*_nj_cnk_motion_translate_)(NJS_CNK_OBJECT *object);
extern void (*_nj_cnk_motion_rotate_)(NJS_CNK_OBJECT *object);
/* the angle step of njCnkMotionTransformQuaternion and njCnkMotionQuaternion,
 * which njSetCnkCurrentMotion picks */
extern void (*_nj_cnk_motion_rotate_qua_)(NJS_CNK_OBJECT *object);
extern void (*_nj_cnk_motion_scale_)(NJS_CNK_OBJECT *object);
/* the angle step of njCnkMotionTransformZXY and njCnkMotionRotateZXY, which
 * njSetCnkCurrentMotion picks */
extern void (*_nj_cnk_motion_rotate_zxy_)(NJS_CNK_OBJECT *object);
/* the function njCnkSetMotionCallback set, or null. a draw calls it with each
 * node once the node is posed, before the node's model draws */
extern void (*_nj_cnk_motion_callback_)(NJS_CNK_OBJECT *object);

/* the node steps, indexed by the count of factors in each entry, from 0 to
 * NODE_STEPS less 1. the step at 0 takes no entry */
#define NODE_STEPS 5
extern void (*_nj_cnk_motion_data_func_table_[NODE_STEPS])(
    NJS_CNK_OBJECT *object);
/* the interpolations, indexed by spline count. the simple drawer's motion draws
 * index them by _nj_mot_spline_cnt_. the calls that take a spline_flag index
 * them by it. the easy and multi drawers' draws take the straight line whatever
 * the count. the float table's spline is njCnkOverhauserMotionF. each angle
 * table holds its straight line at both places, so angles never take a spline.
 * njCnkSimpleDrawMotionOld and njCnkSimpleDrawShapeMotionOld take the old angle
 * table. quaternions take njCnkLinearMotionQ, which no table holds */
extern void (*_nj_cnk_motion_int_f_func_table_[INTERPOLATIONS])(
    NJS_MKEY_F *keys, Uint32 index, Uint32 nb, Float *out);
extern void (*_nj_cnk_motion_int_a_func_table_[INTERPOLATIONS])(
    NJS_MKEY_A *keys, Uint32 index, Uint32 nb, Angle *out);
extern void (*_nj_cnk_motion_int_a_func_table_old_[INTERPOLATIONS])(
    NJS_MKEY_A *keys, Uint32 index, Uint32 nb, Angle *out);

/* the position, angle and scale steps. an Int step reads the factor that
 * _nj_motion_data_info_.factor indexes, then adds 1 to factor. it finds the
 * last key whose keyframe is at most the frame, truncated to an integer. where
 * no key after key 0 is that early, it finds key 0. the keys must lie in
 * keyframe order. the step's interpolation gives the factor's value from the
 * key found. a factor without keys takes object's own pos, ang or scl instead.
 * an Obj step takes object's own, and reads no factor.
 *
 * the XYZ steps turn about x, then y, then z, as njRotateEx does with lv 0. the
 * ZXY steps turn about z, then x, then y. njCnkMotionObjRotate turns in the
 * order object's NJD_EVAL_ZXY_ANG gives. njCnkMotionIntQuaternion interpolates
 * NJS_MKEY_QUAT keys by njCnkLinearMotionQ. it turns by the result through
 * njQuaternionEx. without keys it turns as njCnkMotionObjRotate does */
void njCnkMotionIntTranslate(NJS_CNK_OBJECT *object);
void njCnkMotionIntScale(NJS_CNK_OBJECT *object);
void njCnkMotionIntRotateXYZ(NJS_CNK_OBJECT *object);
void njCnkMotionIntRotateZXY(NJS_CNK_OBJECT *object);
void njCnkMotionIntQuaternion(NJS_CNK_OBJECT *object);
void njCnkMotionObjTranslate(NJS_CNK_OBJECT *object);
void njCnkMotionObjRotateXYZ(NJS_CNK_OBJECT *object);
void njCnkMotionObjRotateZXY(NJS_CNK_OBJECT *object);
void njCnkMotionObjRotate(NJS_CNK_OBJECT *object);
void njCnkMotionObjScale(NJS_CNK_OBJECT *object);
/* take motion, which must not be null: set _nj_mot_data_cnt_,
 * _nj_mot_spline_cnt_ and _nj_mot_data_type_ from it, and point
 * _nj_motion_data_info_ at its first entry and its count of frames */
#define TAKE_MOTION(motion)                                                    \
    _nj_mot_data_cnt_ = (motion)->inp_fn & ~NJD_MTYPE_MASK;                    \
    _nj_mot_spline_cnt_ = ((motion)->inp_fn & NJD_MTYPE_MASK) > 0;             \
    _nj_mot_data_type_ = (motion)->type;                                       \
    _nj_motion_data_info_.mdata = (motion)->mdata;                             \
    _nj_motion_data_info_.nbFrame = (motion)->nbFrame;
/* set the steps a draw of object runs, as the comment on
 * _nj_cnk_motion_data_func_ says */
#define SET_MOTION_STEPS(object)                                               \
    _nj_cnk_motion_data_func_ =                                                \
        _nj_cnk_motion_data_func_table_[_nj_mot_data_cnt_];                    \
    if (_nj_mot_data_type_ & NJD_MTYPE_POS_0) {                                \
        _nj_cnk_motion_translate_ = njCnkMotionIntTranslate;                   \
    } else {                                                                   \
        _nj_cnk_motion_translate_ = njCnkMotionObjTranslate;                   \
    }                                                                          \
    if (_nj_mot_data_type_ & NJD_MTYPE_ANG_1) {                                \
        if ((object)->evalflags & NJD_EVAL_ZXY_ANG) {                          \
            _nj_cnk_motion_rotate_ = njCnkMotionIntRotateZXY;                  \
        } else {                                                               \
            _nj_cnk_motion_rotate_ = njCnkMotionIntRotateXYZ;                  \
        }                                                                      \
    } else if (_nj_mot_data_type_ & NJD_MTYPE_QUAT_1) {                        \
        _nj_cnk_motion_rotate_ = njCnkMotionIntQuaternion;                     \
    } else if ((object)->evalflags & NJD_EVAL_ZXY_ANG) {                       \
        _nj_cnk_motion_rotate_ = njCnkMotionObjRotateZXY;                      \
    } else {                                                                   \
        _nj_cnk_motion_rotate_ = njCnkMotionObjRotateXYZ;                      \
    }                                                                          \
    if (_nj_mot_data_type_ & NJD_MTYPE_SCL_2) {                                \
        _nj_cnk_motion_scale_ = njCnkMotionIntScale;                           \
    } else {                                                                   \
        _nj_cnk_motion_scale_ = njCnkMotionObjScale;                           \
    }

/* half a turn and a whole turn, as Angles count them, and the same as Floats */
#define HALF_TURN 0x8000
#define TURN 0x10000
#define HALF_TURN_F ((Float)HALF_TURN)
#define TURN_F ((Float)TURN)

/* the interpolations. each writes to out the value of a factor of nb keys at
 * _nj_motion_data_info_.frame. index is the key a step found. the key after the
 * last is key 0. t runs from 0 at key index to 1 at the next key. it is the
 * frame less index's keyframe, over the frames between the two keys. a span of
 * no frames, or a negative one, counts as one frame. the span from the last key
 * to key 0 is negative. so from the last key on, t is the frames past it, and
 * can pass 1.
 *
 * njCnkLinearMotionF and njCnkLinearMotionA weight key index by 1 - t, and the
 * next key by t. njCnkLinearMotionA blends the angles as floats. it truncates
 * each toward 0. njCnkLinearMotionAOld first moves the next key's angle a turn
 * toward index's. it does so where the two lie over half a turn apart. so keys
 * less than a turn and a half apart blend the shorter way.
 *
 * njCnkLinearMotionQ blends key index's quaternion with the next's the same
 * way. it then scales the blend to unit length. where the two keys' dot product
 * is negative, it blends toward the next key's negation. that is the same turn,
 * taken the shorter way. out takes four floats, in NJS_QUATERNION's order */
void njCnkLinearMotionQ(NJS_MKEY_QUAT *keys, Uint32 index, Uint32 nb,
                        Float *out);
void njCnkLinearMotionF(NJS_MKEY_F *keys, Uint32 index, Uint32 nb, Float *out);
void njCnkLinearMotionA(NJS_MKEY_A *keys, Uint32 index, Uint32 nb, Angle *out);
void njCnkLinearMotionAOld(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                           Angle *out);

#endif
