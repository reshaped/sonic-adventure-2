/* the chunk motion links. a link poses a model tree between two motions:
 * slots 0 and 1, each at its own frame. a link draw walks the tree once. at
 * each node it runs slot 0's steps, then slot 1's, with _nj_mot_current_data_
 * the slot. a slot's steps leave the node's position, angles and scale for
 * that slot. njCnkMakeMotionLink blends the two slots' by the link's rate,
 * _nj_mot_current_frame_. it puts the blended transform before the current
 * matrix. a shape link moves the nodes' vertices the same way, between two
 * shape motions.
 *
 * this header holds the motion link members' names, and njGlobal's state of a
 * link. those members are njCnkMotLinkSrc, njCnkShapeLinkSrc and the four that
 * hold the link draws. nj_motion.h holds njGlobal's state of a motion, the
 * interpolation pointers and spline counts among it. nj_cnk_shape.h holds the
 * shape members' names, which a shape link shares */
#ifndef NJ_CNK_MOTLINK_H
#define NJ_CNK_MOTLINK_H
#include <nj_cnk_shape.h>
#include <nj_motion.h>
#include <nj_motion_types.h> /* IWYU pragma: export */
#include <ninja.h>
#include <sg_xpt.h>

/* _nj_mot_current_data_ is the slot whose steps run, 0 or 1.
 * _nj_mot_current_frame_ is the link's rate: 0 gives slot 0's pose, and 1
 * slot 1's */
extern Uint16 _nj_mot_current_data_;
extern Float _nj_mot_current_frame_;
/* where each slot's motion and shape motion stand. _nj_motlink_data_cnt_ is
 * each slot's motion's count of factors, and _nj_motlink_data_type_ its
 * NJD_MTYPE_ bits. _nj_motlink_shape_data_cnt_ and
 * _nj_motlink_shape_data_type_ are its shape motion's. a draw sets a null
 * motion's count and type to 0. it sets a null shape motion's type alone */
extern struct nj_motion_info _nj_motlink_data_info_[2];
extern struct nj_motion_info _nj_shapelink_data_info_[2];
extern Uint16 _nj_motlink_data_cnt_[2];
extern Uint16 _nj_motlink_data_type_[2];
extern Uint16 _nj_motlink_shape_data_cnt_[2];
extern Uint16 _nj_motlink_shape_data_type_[2];
/* 1 where the link draw's object has NJD_EVAL_ZXY_ANG, else 0. each node
 * without NJD_EVAL_SKIP turns in the order it gives, as njRotateEx's lv */
extern Sint32 _nj_rotate_func_flag_;
/* the node's values that each slot's steps leave. slot 0's three lie in
 * [0] to [2], and slot 1's in [3] to [5]. a spline interpolation also leaves
 * the slots' tangents. slot 0's lies in [6] to [8], and slot 1's in [9] to
 * [11]. so each array holds LINK_PLACES. the quaternion path stores four
 * components per slot and uses linear interpolation */
/* slot's value starts at LINK_VALUE(slot) in the arrays above, and its tangent
 * LINK_TANGENTS places on. its quaternion starts at LINK_QUA(slot) */
#define LINK_VALUE(slot) ((slot) * 3)
#define LINK_TANGENTS 6
#define LINK_QUA(slot) ((slot) * 4)
extern Float _nj_motion_link_pos_[LINK_PLACES];
extern Float _nj_motion_link_scl_[LINK_PLACES];
extern Angle _nj_motion_link_ang_[LINK_PLACES];
extern Float _nj_motion_link_qua_[LINK_QUA_PLACES];
/* the slots whose keys gave the node's position, angles and scale. bit 0
 * stands for slot 0, and bit 1 for slot 1. a quaternion slot's keys set the
 * angle bit */
extern Uint16 _nj_pos_int_flag_;
extern Uint16 _nj_ang_int_flag_;
extern Uint16 _nj_scl_int_flag_;
/* _nj_int_f_func_ and _nj_int_a_func_ blend the two slots' values by rate.
 *
 * a link draw also points nj_motion.h's _nj_motion_int_f_func_ and
 * _nj_motion_int_a_func_ at its key interpolations. _nj_motion_int_f_func_
 * writes the current slot's position or scale. _nj_motion_int_a_func_ writes
 * its angles. each runs from key index of the nb at keys toward the next. the
 * key after the last is the first. out is the slot's place in the arrays above.
 * the simple drawer's link draws pick all four by _nj_mot_spline_cnt_. the easy
 * drawer's take the straight line */
extern void (*_nj_int_f_func_)(const Float *values, Float *out, Float rate);
extern void (*_nj_int_a_func_)(const Angle *values, Angle *out, Float rate);

/* put the node's transform before the current matrix, as njTranslateEx,
 * njRotateEx and njScaleEx put theirs. the position goes first, then the turn,
 * then the scale. each is the two slots' values blended by _nj_int_f_func_ or
 * _nj_int_a_func_, where either slot's keys gave the factor. slot 0's value
 * stands alone where neither did.
 *
 * under a spline count of 1, a factor may come from one slot's keys alone.
 * the other slot's tangent is then half of slot 1's value less slot 0's. the
 * factor's int flag, _nj_pos_int_flag_ or _nj_ang_int_flag_, is left holding
 * that other slot's index.
 *
 * where either slot's motion has NJD_MTYPE_QUAT_1, the turn blends the two
 * slots' quaternions by njCnkLinearIntQ instead. a slot without the bit
 * gives the quaternion of its angles by njAng2Quat */
void njCnkMakeMotionLink(void);
/* the slots' node steps. a draw sets _nj_cnk_motlink_data_func_[slot] from
 * _nj_cnk_motlink_data_func_table_ by the slot's count of factors. the step
 * for no factor runs the slot's position, angle and scale steps on the node.
 * each other step first takes the node's entry from the slot's
 * _nj_motlink_data_info_. it moves mdata to the next entry after */
extern void (*_nj_cnk_motlink_data_func_[2])(NJS_CNK_OBJECT *object);
extern void (*_nj_cnk_motlink_data_func_table_[NODE_STEPS])(
    NJS_CNK_OBJECT *object);
/* each slot's position, angle and scale steps. a draw sets an Int step where
 * the slot's motion has the factor's NJD_MTYPE_ bit. it sets an Obj step
 * otherwise. the angle step is njCnkMotLinkIntQuaternion under
 * NJD_MTYPE_QUAT_1 without NJD_MTYPE_ANG_1 */
extern void (*_nj_cnk_motlink_translate_data_[2])(NJS_CNK_OBJECT *object);
extern void (*_nj_cnk_motlink_rotate_data_[2])(NJS_CNK_OBJECT *object);
extern void (*_nj_cnk_motlink_scale_data_[2])(NJS_CNK_OBJECT *object);
/* an Int step reads the node's keys of the slot's next factor. it finds the
 * last key whose keyframe is at most the slot's frame, truncated to an
 * integer. it takes the first key where no later one is. the keys must lie
 * in keyframe order. the slot's interpolation writes the node's value from that
 * key. the step then sets the slot's bit in the factor's int flag. a node
 * without keys of the factor takes its own pos, ang or scl instead. either way
 * the slot's factor moves on by one. njCnkMotLinkIntQuaternion interpolates by
 * njCnkLinearMotLinkQ, or takes njAng2Quat of the node's ang.
 *
 * an Obj step takes the node's own pos, ang or scl, and reads no factor */
void njCnkMotLinkIntTranslate(NJS_CNK_OBJECT *object);
void njCnkMotLinkIntScale(NJS_CNK_OBJECT *object);
void njCnkMotLinkIntRotate(NJS_CNK_OBJECT *object);
void njCnkMotLinkIntQuaternion(NJS_CNK_OBJECT *object);
void njCnkMotLinkObjTranslate(NJS_CNK_OBJECT *object);
void njCnkMotLinkObjRotate(NJS_CNK_OBJECT *object);
void njCnkMotLinkObjScale(NJS_CNK_OBJECT *object);
/* the tables the four interpolations above are chosen from, by spline
 * count. each angle table holds its linear interpolation twice */
extern void (*_nj_cnk_motlink_int_f_func_table_[INTERPOLATIONS])(
    NJS_MKEY_F *keys, Uint32 index, Uint32 nb, Float *out);
extern void (*_nj_cnk_motlink_int_a_func_table_[INTERPOLATIONS])(
    NJS_MKEY_A *keys, Uint32 index, Uint32 nb, Angle *out);
extern void (*_nj_cnk_int_f_func_table_[INTERPOLATIONS])(const Float *values,
                                                         Float *out,
                                                         Float rate);
extern void (*_nj_cnk_int_a_func_table_[INTERPOLATIONS])(const Angle *values,
                                                         Angle *out,
                                                         Float rate);
/* the linear interpolations. njCnkLinearMotLinkF sets out to
 * (1 - t) * a + t * b. a is key index's three floats, and b the next key's.
 * t is the slot's frame less index's keyframe. it is divided by the
 * keyframes' difference where the next keyframe is later.
 * njCnkLinearMotLinkA does the same for three angles, through floats.
 * njCnkLinearMotLinkAOld first moves the next key's angle a turn toward
 * index's. it does so where the two lie over half a turn apart. so keys less
 * than a turn and a half apart blend the shorter way.
 *
 * njCnkLinearIntF and njCnkLinearIntA blend the two slots the same way, by
 * rate. njCnkLinearIntAOld moves slot 1's angles as njCnkLinearMotLinkAOld
 * moves the next key's */
void njCnkLinearMotLinkF(NJS_MKEY_F *keys, Uint32 index, Uint32 nb, Float *out);
void njCnkLinearMotLinkA(NJS_MKEY_A *keys, Uint32 index, Uint32 nb, Angle *out);
void njCnkLinearMotLinkAOld(NJS_MKEY_A *keys, Uint32 index, Uint32 nb,
                            Angle *out);
void njCnkLinearIntF(const Float *values, Float *out, Float rate);
void njCnkLinearIntA(const Angle *values, Angle *out, Float rate);
void njCnkLinearIntAOld(const Angle *values, Angle *out, Float rate);

/* each slot's moved copy of the node's vertex chunk, in _nj_shape_buf_. it
 * is null where the slot moves none of the node's vertices */
extern Sint32 *_nj_cnk_shapelink_vlist_[2];
/* the shape link's steps, as the motion link's are. a draw sets each slot's
 * _nj_cnk_shapelink_data_func_ from _nj_cnk_shapelink_data_func_table_ by
 * the count of factors in _nj_motlink_shape_data_cnt_. the step for no factor
 * clears the slot's _nj_cnk_shapelink_vlist_. it does so only where the node
 * has a model and isn't under NJD_EVAL_HIDE. each other step takes the node's
 * entry and runs the slot's _nj_cnk_shapelink_data_ on the node. it moves mdata
 * to the next entry.
 *
 * _nj_cnk_shapelink_data_ finds the chunk's layout as nj_cnk_shape.h's point
 * functions do, once a draw. its setup clears the shape type bits the chunk
 * can't serve, but for the current slot alone. so the other slot's
 * _nj_cnk_shapelink_data_ writes through an unset pointer where its motion
 * moves normals the chunk lacks. it does so too for the points of a chunk
 * type the setup doesn't know */
extern void (*_nj_cnk_shapelink_data_func_[2])(NJS_CNK_OBJECT *object);
extern void (*_nj_cnk_shapelink_data_func_table_[SHAPE_STEPS])(
    NJS_CNK_OBJECT *object);
extern void (*_nj_cnk_shapelink_data_[2])(NJS_CNK_OBJECT *object);
/* njCnkGetLinearKFdata and njCnkGetSplineKFdata at the frame of the current
 * slot's shape motion */
Float njCnkGetLinkLinearKFdata(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                               void **out);
Float njCnkGetLinkSplineKFdata(NJS_MKEY_P *key, Uint32 index, Uint32 nbkeys,
                               void **out);
/* the simple drawer's shape link point functions, which its link draws pick by
 * _nj_mot_shape_spline_cnt_ */
extern void (*_nj_cnk_s_shapelink_int_func_table[INTERPOLATIONS])(
    NJS_CNK_OBJECT *object);
/* the easy drawer's shape link point function. it moves the current slot's
 * points and normals in a straight line between the keys around its frame.
 * njCnkSimpleShapeLinkLinearPoint differs on a chunk of points alone. it blends
 * one along a spline, as the bug tag in njCnkShapeLinkS.c says */
void njCnkEasyShapeLinkLinearPoint(NJS_CNK_OBJECT *object);
/* draw the node's model with _nj_cnk_draw_model_, its points and normals
 * blended between the two slots' by _nj_mot_current_frame_. each vertex
 * takes slot 0's color. a slot whose vertex chunk is null gives the model's
 * own. the blend goes to _nj_shape_buf_, past both slots' chunks, and the
 * model's vertex list ends after it. the model's own list is put back after
 * the draw.
 *
 * njCnkMakeSimpleShapeLink blends NJD_CV, NJD_CV_D8, NJD_CV_VN and
 * NJD_CV_VN_D8 chunks, and njCnkMakeEasyShapeLink the first three. a chunk of
 * another type draws as it is. so does a node whose slots are both null. a null
 * model, and a node under NJD_EVAL_HIDE, draws nothing.
 *
 * an NJD_CV_D8 or NJD_CV_VN_D8 chunk holds a color at each vertex. the blend
 * steps past slot 0's but not slot 1's. so slot 1 falls a word further behind
 * at each vertex. from the second vertex on, it blends from the wrong words */
void njCnkMakeSimpleShapeLink(NJS_CNK_OBJECT *object);
void njCnkMakeEasyShapeLink(NJS_CNK_OBJECT *object);

#endif
