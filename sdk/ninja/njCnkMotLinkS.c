/* the simple chunk drawer's motion links. njCnkMotLinkE.c compiles this
 * text again for the easy chunk drawer, with the parameters below */
#include <nj_cnk.h>
#include <nj_cnk_motlink.h>
#include <nj_cnk_shape.h>
#include <nj_km.h>
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the drawer that draws each node's model, and the call that blends and
 * draws a node's shape link */
#ifndef LINK_DRAW_MODEL
#define LINK_DRAW_MODEL _njCnkSimpleDrawModel
#endif
#ifndef LINK_MAKE_SHAPE_LINK
#define LINK_MAKE_SHAPE_LINK njCnkMakeSimpleShapeLink
#endif
/* the interpolations each slot's steps and njCnkMakeMotionLink take, and the
 * shape link's point function. the simple drawer takes them from their tables
 * by the spline counts */
#ifndef LINK_MOTION_INT_F
#define LINK_MOTION_INT_F _nj_cnk_motlink_int_f_func_table_[_nj_mot_spline_cnt_]
#endif
#ifndef LINK_MOTION_INT_A
#define LINK_MOTION_INT_A _nj_cnk_motlink_int_a_func_table_[_nj_mot_spline_cnt_]
#endif
#ifndef LINK_INT_F
#define LINK_INT_F _nj_cnk_int_f_func_table_[_nj_mot_spline_cnt_]
#endif
#ifndef LINK_INT_A
#define LINK_INT_A _nj_cnk_int_a_func_table_[_nj_mot_spline_cnt_]
#endif
#ifndef LINK_SHAPE_POINT
#define LINK_SHAPE_POINT                                                       \
    _nj_cnk_s_shapelink_int_func_table[_nj_mot_shape_spline_cnt_]
#endif
/* the Old draws' angle interpolations, which the simple drawer's other draws
 * take too */
#ifndef LINK_MOTION_INT_A_OLD
#define LINK_MOTION_INT_A_OLD LINK_MOTION_INT_A
#endif
#ifndef LINK_INT_A_OLD
#define LINK_INT_A_OLD LINK_INT_A
#endif

/* take slot's motion from link: its count of factors, its type, and where it
 * stands. the motion must not be null */
#define TAKE_LINK_MOTION(link, slot)                                           \
    _nj_motlink_data_cnt_[slot] =                                              \
        (link)->motion[slot]->inp_fn & ~NJD_MTYPE_MASK;                        \
    /* @bug logic: stores the motion's spline count in                         \
     * _nj_mot_shape_spline_cnt_, not _nj_mot_spline_cnt_. so the              \
     * interpolations SET_LINK_STEPS picks, and njCnkMakeMotionLink's test of  \
     * one slot's keys, go by the count an earlier draw left in                \
     * _nj_mot_spline_cnt_. the eight takes store it at 0x8c10093e,            \
     * 0x8c1009b2, 0x8c100c34, 0x8c100cf6, 0x8c1010aa, 0x8c10111e, 0x8c1013d8  \
     * and 0x8c10149a, and MOV.W @R3,R5 at 0x8c100a8c reads the stale count */ \
    _nj_mot_shape_spline_cnt_ =                                                \
        ((link)->motion[slot]->inp_fn & NJD_MTYPE_MASK) > 0;                   \
    _nj_motlink_data_type_[slot] = (link)->motion[slot]->type;                 \
    _nj_motlink_data_info_[slot].mdata = (link)->motion[slot]->mdata;          \
    _nj_motlink_data_info_[slot].nbFrame = (link)->motion[slot]->nbFrame;      \
    _nj_motlink_data_info_[slot].frame = (link)->frame[slot];
/* take slot's shape motion from link, as TAKE_LINK_MOTION takes a motion */
#define TAKE_LINK_SHAPE(link, slot)                                            \
    _nj_motlink_shape_data_type_[slot] = (link)->motion[slot]->type;           \
    _nj_motlink_shape_data_cnt_[slot] =                                        \
        (link)->motion[slot]->inp_fn & ~NJD_MTYPE_MASK;                        \
    _nj_mot_shape_spline_cnt_ =                                                \
        ((link)->motion[slot]->inp_fn & NJD_MTYPE_MASK) > 0;                   \
    _nj_shapelink_data_info_[slot].mdata = (link)->motion[slot]->mdata;        \
    _nj_shapelink_data_info_[slot].nbFrame = (link)->motion[slot]->nbFrame;    \
    _nj_shapelink_data_info_[slot].frame = (link)->frame[slot];
/* set slot's shape link steps as nj_cnk_motlink.h says. the next point
 * function then finds the layout again */
#define SET_SHAPE_LINK_STEPS(slot)                                             \
    _nj_cnk_shape_init_flag_ = FALSE;                                          \
    _nj_shapelink_data_info_[slot].factor = 0;                                 \
    _nj_shapelink_data_info_[slot].m = NULL;                                   \
    _nj_cnk_shapelink_data_func_[slot] =                                       \
        _nj_cnk_shapelink_data_func_table_[_nj_motlink_shape_data_cnt_[slot]]; \
    _nj_cnk_shapelink_data_[slot] = LINK_SHAPE_POINT;
/* set slot's steps as nj_cnk_motlink.h says. then set the interpolations,
 * with motion_int_a and int_a for the angles */
#define SET_LINK_STEPS(object, slot, motion_int_a, int_a)                      \
    _nj_cnk_motlink_data_func_[slot] =                                         \
        _nj_cnk_motlink_data_func_table_[_nj_motlink_data_cnt_[slot]];         \
    if ((object)->evalflags & NJD_EVAL_ZXY_ANG) {                              \
        _nj_rotate_func_flag_ = 1;                                             \
    } else {                                                                   \
        _nj_rotate_func_flag_ = 0;                                             \
    }                                                                          \
    if (_nj_motlink_data_type_[slot] & NJD_MTYPE_POS_0) {                      \
        _nj_cnk_motlink_translate_data_[slot] = njCnkMotLinkIntTranslate;      \
    } else {                                                                   \
        _nj_cnk_motlink_translate_data_[slot] = njCnkMotLinkObjTranslate;      \
    }                                                                          \
    if (_nj_motlink_data_type_[slot] & NJD_MTYPE_ANG_1) {                      \
        _nj_cnk_motlink_rotate_data_[slot] = njCnkMotLinkIntRotate;            \
    } else if (_nj_motlink_data_type_[slot] & NJD_MTYPE_QUAT_1) {              \
        _nj_cnk_motlink_rotate_data_[slot] = njCnkMotLinkIntQuaternion;        \
    } else {                                                                   \
        _nj_cnk_motlink_rotate_data_[slot] = njCnkMotLinkObjRotate;            \
    }                                                                          \
    if (_nj_motlink_data_type_[slot] & NJD_MTYPE_SCL_2) {                      \
        _nj_cnk_motlink_scale_data_[slot] = njCnkMotLinkIntScale;              \
    } else {                                                                   \
        _nj_cnk_motlink_scale_data_[slot] = njCnkMotLinkObjScale;              \
    }                                                                          \
    _nj_motion_int_f_func_ = LINK_MOTION_INT_F;                                \
    _nj_motion_int_a_func_ = (motion_int_a);                                   \
    _nj_int_f_func_ = LINK_INT_F;                                              \
    _nj_int_a_func_ = (int_a);

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* pose and draw object, its children and its siblings. a node with
 * NJD_EVAL_SKIP takes its own pos, ang and scl. the two slots' node steps and
 * njCnkMakeMotionLink pose any other */
void njCnkSimplePushPopMotionLink(NJS_CNK_OBJECT *object);
/* pose and draw as njCnkSimplePushPopMotionLink does. a node without
 * NJD_EVAL_SHAPE_SKIP draws by the two shape slots' node steps and
 * LINK_MAKE_SHAPE_LINK instead */
void njCnkSimplePushPopShapeLink(NJS_CNK_OBJECT *object);
/* NOLINTEND(misc-use-internal-linkage) */

#ifdef NONMATCHING
/* this side's walk returns whether it drew the whole tree. where the stack is
 * full, it abandons the rest of the draw and unwinds the pushes that succeeded.
 * motion keys follow depth-first object order, so going on at a sibling would
 * give it the rejected node's keys */
static Bool draw_motion_link_tree(NJS_CNK_OBJECT *object)
#else
void njCnkSimplePushPopMotionLink(NJS_CNK_OBJECT *object)
#endif
{
    do {
#ifdef NONMATCHING
        /* this side abandons the draw where the stack is full */
        if (!njPushMatrixEx()) {
            return FALSE;
        }
#else
        njPushMatrixEx();
#endif
        if (!(object->evalflags & NJD_EVAL_SKIP)) {
            _nj_pos_int_flag_ = 0;
            _nj_ang_int_flag_ = 0;
            _nj_scl_int_flag_ = 0;
            _nj_mot_current_data_ = 0;
            _nj_cnk_motlink_data_func_[0](object);
            _nj_mot_current_data_ = 1;
            _nj_cnk_motlink_data_func_[1](object);
            njCnkMakeMotionLink();
        } else {
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, object->evalflags & NJD_EVAL_ZXY_ANG);
            njScaleEx((NJS_VECTOR *)object->scl);
        }
        if (_nj_cnk_motion_callback_ != NULL) {
            _nj_cnk_motion_callback_(object);
        }
        if (object->model != NULL && !(object->evalflags & NJD_EVAL_HIDE)) {
            LINK_DRAW_MODEL(object->model);
        }
        if (object->child != NULL) {
#ifdef NONMATCHING
            /* this side pops its own push, and abandons the draw where the
             * child's walk did */
            if (!draw_motion_link_tree(object->child)) {
                njPopMatrixEx();
                return FALSE;
            }
#else
            njCnkSimplePushPopMotionLink(object->child);
#endif
        }
        njPopMatrixEx();
        object = object->sibling;
    } while (object != NULL);
#ifdef NONMATCHING
    /* this side's walk drew every node */
    return TRUE;
#endif
}

#ifdef NONMATCHING
/* this side's export calls the walk, and drops its result */
void njCnkSimplePushPopMotionLink(NJS_CNK_OBJECT *object)
{
    draw_motion_link_tree(object);
}
#endif

#ifdef NONMATCHING
/* this side's walk returns whether it drew the whole tree, and abandons the
 * draw where the stack is full, as the motion walk above does */
static Bool draw_shape_link_tree(NJS_CNK_OBJECT *object)
#else
void njCnkSimplePushPopShapeLink(NJS_CNK_OBJECT *object)
#endif
{
    do {
#ifdef NONMATCHING
        /* this side abandons the draw where the stack is full */
        if (!njPushMatrixEx()) {
            return FALSE;
        }
#else
        njPushMatrixEx();
#endif
        if (!(object->evalflags & NJD_EVAL_SKIP)) {
            _nj_pos_int_flag_ = 0;
            _nj_ang_int_flag_ = 0;
            _nj_scl_int_flag_ = 0;
            _nj_mot_current_data_ = 0;
            _nj_cnk_motlink_data_func_[0](object);
            _nj_mot_current_data_ = 1;
            _nj_cnk_motlink_data_func_[1](object);
            njCnkMakeMotionLink();
        } else {
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, object->evalflags & NJD_EVAL_ZXY_ANG);
            njScaleEx((NJS_VECTOR *)object->scl);
        }
        if (_nj_cnk_motion_callback_ != NULL) {
            _nj_cnk_motion_callback_(object);
        }
        if (!(object->evalflags & NJD_EVAL_SHAPE_SKIP)) {
            _nj_mot_current_data_ = 0;
            _nj_cnk_shapelink_data_func_[0](object);
            _nj_mot_current_data_ = 1;
            _nj_cnk_shapelink_data_func_[1](object);
            LINK_MAKE_SHAPE_LINK(object);
        } else if (object->model != NULL &&
                   !(object->evalflags & NJD_EVAL_HIDE)) {
            LINK_DRAW_MODEL(object->model);
        }
        if (object->child != NULL) {
#ifdef NONMATCHING
            /* this side pops its own push, and abandons the draw where the
             * child's walk did */
            if (!draw_shape_link_tree(object->child)) {
                njPopMatrixEx();
                return FALSE;
            }
#else
            njCnkSimplePushPopShapeLink(object->child);
#endif
        }
        njPopMatrixEx();
        object = object->sibling;
    } while (object != NULL);
#ifdef NONMATCHING
    /* this side's walk drew every node */
    return TRUE;
#endif
}

#ifdef NONMATCHING
/* this side's export calls the walk, and drops its result */
void njCnkSimplePushPopShapeLink(NJS_CNK_OBJECT *object)
{
    draw_shape_link_tree(object);
}
#endif

void njCnkSimpleDrawMotionLink(NJS_CNK_OBJECT *object,
                               NJS_MOTION_LINK *motion_link, Float rate)
{
    if (rate > 1.0F) {
        return;
    }
    _nj_mot_current_data_ = 0;
    _nj_mot_current_frame_ = rate;
    if (motion_link->motion[0] != NULL) {
        if (!(motion_link->frame[0] < motion_link->motion[0]->nbFrame)) {
            return;
        }
        TAKE_LINK_MOTION(motion_link, 0)
    } else {
        _nj_motlink_data_cnt_[0] = 0;
        _nj_motlink_data_type_[0] = 0;
    }
    _nj_motlink_data_info_[0].factor = 0;
    _nj_motlink_data_info_[0].m = NULL;
    if (motion_link->motion[1] != NULL) {
        if (!(motion_link->frame[1] < motion_link->motion[1]->nbFrame)) {
            return;
        }
        TAKE_LINK_MOTION(motion_link, 1)
    } else {
        _nj_motlink_data_cnt_[1] = 0;
        _nj_motlink_data_type_[1] = 0;
    }
    _nj_motlink_data_info_[1].factor = 0;
    _nj_motlink_data_info_[1].m = NULL;
    SET_LINK_STEPS(object, 0, LINK_MOTION_INT_A, LINK_INT_A)
    SET_LINK_STEPS(object, 1, LINK_MOTION_INT_A, LINK_INT_A)
    njGetkmPointer();
    njCnkSimplePushPopMotionLink(object);
    njSetkmPointer();
}

void njCnkSimpleDrawShapeMotionLink(NJS_CNK_OBJECT *object,
                                    NJS_MOTION_LINK *motion_link,
                                    NJS_MOTION_LINK *shape_link, Float rate)
{
    if (rate > 1.0F) {
        return;
    }
    _nj_mot_current_data_ = 0;
    _nj_mot_current_frame_ = rate;
    if (motion_link->motion[0] != NULL) {
        if (!(motion_link->frame[0] < motion_link->motion[0]->nbFrame)) {
            return;
        }
        TAKE_LINK_MOTION(motion_link, 0)
    } else {
        _nj_motlink_data_cnt_[0] = 0;
        _nj_motlink_data_type_[0] = 0;
    }
    _nj_motlink_data_info_[0].factor = 0;
    _nj_motlink_data_info_[0].m = NULL;
    if (motion_link->motion[1] != NULL) {
        if (!(motion_link->frame[1] < motion_link->motion[1]->nbFrame)) {
            return;
        }
        TAKE_LINK_MOTION(motion_link, 1)
    } else {
        _nj_motlink_data_cnt_[1] = 0;
        _nj_motlink_data_type_[1] = 0;
    }
    _nj_motlink_data_info_[1].factor = 0;
    _nj_motlink_data_info_[1].m = NULL;
    if (shape_link->motion[0] != NULL) {
        if (!(shape_link->frame[0] < shape_link->motion[0]->nbFrame)) {
            return;
        }
        TAKE_LINK_SHAPE(shape_link, 0)
    } else {
        /* @bug logic: leaves the slot's count of shape factors as an earlier
         * draw set it, though a null motion's count is cleared above. a stale
         * count above 0 runs slot 0's point function, which then takes the
         * draw's one layout setup from slot 1. slot 1's step can then write
         * through an unset pointer, as nj_cnk_motlink.h says (MOV.W R4,@R8 and
         * MOV.W R4,@R14 at 0x8c100d80 and 0x8c100d82 clear the type and the
         * spline count alone, and MOV.W @R5,R1 at 0x8c100d98 reads the
         * count) */
        _nj_motlink_shape_data_type_[0] = 0;
        _nj_mot_shape_spline_cnt_ = 0;
    }
    SET_SHAPE_LINK_STEPS(0)
    if (shape_link->motion[1] != NULL) {
        if (!(shape_link->frame[1] < shape_link->motion[1]->nbFrame)) {
            return;
        }
        TAKE_LINK_SHAPE(shape_link, 1)
    } else {
        /* @bug logic: leaves slot 1's count of shape factors stale, as slot
         * 0's branch does (MOV.W R4,@R8 and MOV.W R4,@R14 at 0x8c100e0c and
         * 0x8c100e0e, and MOV.W @(2,R5),R0 at 0x8c100e22 reads the count) */
        _nj_motlink_shape_data_type_[1] = 0;
        _nj_mot_shape_spline_cnt_ = 0;
    }
    SET_SHAPE_LINK_STEPS(1)
    SET_LINK_STEPS(object, 0, LINK_MOTION_INT_A, LINK_INT_A)
    SET_LINK_STEPS(object, 1, LINK_MOTION_INT_A, LINK_INT_A)
    _nj_cnk_draw_model_ = LINK_DRAW_MODEL;
    njGetkmPointer();
    njCnkSimplePushPopShapeLink(object);
    njSetkmPointer();
}

void njCnkSimpleDrawMotionLinkOld(NJS_CNK_OBJECT *object,
                                  NJS_MOTION_LINK *motion_link, Float rate)
{
    if (rate > 1.0F) {
        return;
    }
    _nj_mot_current_data_ = 0;
    _nj_mot_current_frame_ = rate;
    if (motion_link->motion[0] != NULL) {
        if (!(motion_link->frame[0] < motion_link->motion[0]->nbFrame)) {
            return;
        }
        TAKE_LINK_MOTION(motion_link, 0)
    } else {
        _nj_motlink_data_cnt_[0] = 0;
        _nj_motlink_data_type_[0] = 0;
    }
    _nj_motlink_data_info_[0].factor = 0;
    _nj_motlink_data_info_[0].m = NULL;
    if (motion_link->motion[1] != NULL) {
        if (!(motion_link->frame[1] < motion_link->motion[1]->nbFrame)) {
            return;
        }
        TAKE_LINK_MOTION(motion_link, 1)
    } else {
        _nj_motlink_data_cnt_[1] = 0;
        _nj_motlink_data_type_[1] = 0;
    }
    _nj_motlink_data_info_[1].factor = 0;
    _nj_motlink_data_info_[1].m = NULL;
    SET_LINK_STEPS(object, 0, LINK_MOTION_INT_A_OLD, LINK_INT_A_OLD)
    SET_LINK_STEPS(object, 1, LINK_MOTION_INT_A_OLD, LINK_INT_A_OLD)
    njGetkmPointer();
    njCnkSimplePushPopMotionLink(object);
    njSetkmPointer();
}

void njCnkSimpleDrawShapeMotionLinkOld(NJS_CNK_OBJECT *object,
                                       NJS_MOTION_LINK *motion_link,
                                       NJS_MOTION_LINK *shape_link, Float rate)
{
    if (rate > 1.0F) {
        return;
    }
    _nj_mot_current_data_ = 0;
    _nj_mot_current_frame_ = rate;
    if (motion_link->motion[0] != NULL) {
        if (!(motion_link->frame[0] < motion_link->motion[0]->nbFrame)) {
            return;
        }
        TAKE_LINK_MOTION(motion_link, 0)
    } else {
        _nj_motlink_data_cnt_[0] = 0;
        _nj_motlink_data_type_[0] = 0;
    }
    _nj_motlink_data_info_[0].factor = 0;
    _nj_motlink_data_info_[0].m = NULL;
    if (motion_link->motion[1] != NULL) {
        if (!(motion_link->frame[1] < motion_link->motion[1]->nbFrame)) {
            return;
        }
        TAKE_LINK_MOTION(motion_link, 1)
    } else {
        _nj_motlink_data_cnt_[1] = 0;
        _nj_motlink_data_type_[1] = 0;
    }
    _nj_motlink_data_info_[1].factor = 0;
    _nj_motlink_data_info_[1].m = NULL;
    if (shape_link->motion[0] != NULL) {
        if (!(shape_link->frame[0] < shape_link->motion[0]->nbFrame)) {
            return;
        }
        TAKE_LINK_SHAPE(shape_link, 0)
    } else {
        /* @bug logic: leaves slot 0's count of shape factors stale, as
         * njCnkSimpleDrawShapeMotionLink does (MOV.W R4,@R8 and MOV.W R4,@R14
         * at 0x8c101524 and 0x8c101526, and MOV.W @R5,R1 at 0x8c10153c reads
         * the count) */
        _nj_motlink_shape_data_type_[0] = 0;
        _nj_mot_shape_spline_cnt_ = 0;
    }
    SET_SHAPE_LINK_STEPS(0)
    if (shape_link->motion[1] != NULL) {
        if (!(shape_link->frame[1] < shape_link->motion[1]->nbFrame)) {
            return;
        }
        TAKE_LINK_SHAPE(shape_link, 1)
    } else {
        /* @bug logic: leaves slot 1's count of shape factors stale, as
         * njCnkSimpleDrawShapeMotionLink does (MOV.W R4,@R8 and MOV.W R4,@R14
         * at 0x8c1015b0 and 0x8c1015b2, and MOV.W @(2,R5),R0 at 0x8c1015c6
         * reads the count) */
        _nj_motlink_shape_data_type_[1] = 0;
        _nj_mot_shape_spline_cnt_ = 0;
    }
    SET_SHAPE_LINK_STEPS(1)
    SET_LINK_STEPS(object, 0, LINK_MOTION_INT_A_OLD, LINK_INT_A_OLD)
    SET_LINK_STEPS(object, 1, LINK_MOTION_INT_A_OLD, LINK_INT_A_OLD)
    _nj_cnk_draw_model_ = LINK_DRAW_MODEL;
    njGetkmPointer();
    njCnkSimplePushPopShapeLink(object);
    njSetkmPointer();
}
