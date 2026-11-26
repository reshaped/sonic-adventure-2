/* the simple chunk drawer's motion draws. njCnkMotionE.c compiles this text
 * again for the easy chunk drawer, with the parameters below */
#include <nj_cnk.h>
#include <nj_cnk_shape.h>
#include <nj_km.h>
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the interpolations and the shape point function a draw sets. the
 * interpolations follow its motion's spline count, and the point function its
 * shape motion's. the simple drawer looks each up in its table */
#ifndef MOTION_INT_F
#define MOTION_INT_F(spline) _nj_cnk_motion_int_f_func_table_[spline]
#endif
#ifndef MOTION_INT_A
#define MOTION_INT_A(spline) _nj_cnk_motion_int_a_func_table_[spline]
#endif
#ifndef MOTION_INT_A_OLD
#define MOTION_INT_A_OLD(spline) _nj_cnk_motion_int_a_func_table_old_[spline]
#endif
#ifndef SHAPE_INT
#define SHAPE_INT(spline) _nj_cnk_s_shape_int_func_table_[spline]
#endif

/* pose and draw object, its children and its siblings. a node with
 * NJD_EVAL_SKIP takes its own pos, ang and scl. _nj_cnk_motion_data_func_
 * poses any other */
#ifdef NONMATCHING
/* this side's walk returns whether it drew the whole tree. where the stack is
 * full, it abandons the rest of the draw and unwinds the pushes that succeeded.
 * motion keys follow depth-first object order, so going on at a sibling would
 * give it the rejected node's keys */
static Bool draw_motion_tree(NJS_CNK_OBJECT *object)
#else
static void draw_motion_tree(NJS_CNK_OBJECT *object)
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
            _nj_cnk_motion_data_func_(object);
        } else {
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, object->evalflags & NJD_EVAL_ZXY_ANG);
            njScaleEx((NJS_VECTOR *)object->scl);
        }
        if (_nj_cnk_motion_callback_) {
            _nj_cnk_motion_callback_(object);
        }
        if (object->model && !(object->evalflags & NJD_EVAL_HIDE)) {
            _njCnkSimpleDrawModel(object->model);
        }
        if (object->child) {
#ifdef NONMATCHING
            /* this side pops its own push, and abandons the draw where the
             * child's walk did */
            if (!draw_motion_tree(object->child)) {
                njPopMatrixEx();
                return FALSE;
            }
#else
            draw_motion_tree(object->child);
#endif
        }
        njPopMatrixEx();
        object = object->sibling;
    } while (object);
#ifdef NONMATCHING
    /* this side's walk drew every node */
    return TRUE;
#endif
}

/* pose and draw as draw_motion_tree does. a node without NJD_EVAL_SHAPE_SKIP
 * draws through _nj_cnk_shape_data_func_ instead */
#ifdef NONMATCHING
/* this side's walk returns whether it drew the whole tree, and abandons the
 * draw where the stack is full, as the motion walk above does */
static Bool draw_shape_tree(NJS_CNK_OBJECT *object)
#else
static void draw_shape_tree(NJS_CNK_OBJECT *object)
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
            _nj_cnk_motion_data_func_(object);
        } else {
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, object->evalflags & NJD_EVAL_ZXY_ANG);
            njScaleEx((NJS_VECTOR *)object->scl);
        }
        if (_nj_cnk_motion_callback_) {
            _nj_cnk_motion_callback_(object);
        }
        if (!(object->evalflags & NJD_EVAL_SHAPE_SKIP)) {
            _nj_cnk_shape_data_func_(object);
        } else if (object->model && !(object->evalflags & NJD_EVAL_HIDE)) {
            _njCnkSimpleDrawModel(object->model);
        }
        if (object->child) {
#ifdef NONMATCHING
            /* this side pops its own push, and abandons the draw where the
             * child's walk did */
            if (!draw_shape_tree(object->child)) {
                njPopMatrixEx();
                return FALSE;
            }
#else
            draw_shape_tree(object->child);
#endif
        }
        njPopMatrixEx();
        object = object->sibling;
    } while (object);
#ifdef NONMATCHING
    /* this side's walk drew every node */
    return TRUE;
#endif
}

void njCnkSimpleDrawMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                           Float frame)
{
    if (motion) {
        if (frame >= (Float)motion->nbFrame) {
            return;
        }
        TAKE_MOTION(motion)
    } else {
        _nj_mot_data_cnt_ = 0;
        _nj_mot_spline_cnt_ = 0;
        _nj_mot_data_type_ = 0;
    }
    _nj_motion_data_info_.frame = frame;
    _nj_motion_data_info_.factor = 0;
    _nj_motion_data_info_.m = NULL;
    SET_MOTION_STEPS(object)
    _nj_motion_int_f_func_ = MOTION_INT_F(_nj_mot_spline_cnt_);
    _nj_motion_int_a_func_ = MOTION_INT_A(_nj_mot_spline_cnt_);
    njGetkmPointer();
    draw_motion_tree(object);
    njSetkmPointer();
}

void njCnkSimpleDrawShapeMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                NJS_MOTION *shape, Float frame)
{
    if (motion) {
        if (frame >= (Float)motion->nbFrame) {
            return;
        }
        TAKE_MOTION(motion)
    } else {
        _nj_mot_data_cnt_ = 0;
        _nj_mot_spline_cnt_ = 0;
        _nj_mot_data_type_ = 0;
    }
    _nj_motion_data_info_.frame = frame;
    _nj_motion_data_info_.factor = 0;
    _nj_motion_data_info_.m = NULL;
    if (shape) {
        if (frame >= (Float)shape->nbFrame) {
            return;
        }
        TAKE_SHAPE(shape)
    } else {
        _nj_mot_shape_data_type_ = 0;
        _nj_mot_shape_spline_cnt_ = 0;
        _nj_mot_shape_data_cnt_ = 0;
    }
    _nj_shape_data_info_.frame = frame;
    _nj_shape_data_info_.factor = 0;
    _nj_shape_data_info_.m = NULL;
    _nj_cnk_shape_data_func_ =
        _nj_cnk_shape_data_func_table_[_nj_mot_shape_data_cnt_];
    _nj_cnk_shape_ = SHAPE_INT(_nj_mot_shape_spline_cnt_);
    SET_MOTION_STEPS(object)
    _nj_motion_int_f_func_ = MOTION_INT_F(_nj_mot_spline_cnt_);
    _nj_motion_int_a_func_ = MOTION_INT_A(_nj_mot_spline_cnt_);
    _nj_cnk_draw_model_ = _njCnkSimpleDrawModel;
    njGetkmPointer();
    draw_shape_tree(object);
    njSetkmPointer();
}

void njCnkSimpleDrawMotionOld(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                              Float frame)
{
    if (motion) {
        if (frame >= (Float)motion->nbFrame) {
            return;
        }
        TAKE_MOTION(motion)
    } else {
        _nj_mot_data_cnt_ = 0;
        _nj_mot_spline_cnt_ = 0;
        _nj_mot_data_type_ = 0;
    }
    _nj_motion_data_info_.frame = frame;
    _nj_motion_data_info_.factor = 0;
    _nj_motion_data_info_.m = NULL;
    SET_MOTION_STEPS(object)
    _nj_motion_int_f_func_ = MOTION_INT_F(_nj_mot_spline_cnt_);
    _nj_motion_int_a_func_ = MOTION_INT_A_OLD(_nj_mot_spline_cnt_);
    njGetkmPointer();
    draw_motion_tree(object);
    njSetkmPointer();
}

void njCnkSimpleDrawShapeMotionOld(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                   NJS_MOTION *shape, Float frame)
{
    if (motion) {
        if (frame >= (Float)motion->nbFrame) {
            return;
        }
        TAKE_MOTION(motion)
    } else {
        _nj_mot_data_cnt_ = 0;
        _nj_mot_spline_cnt_ = 0;
        _nj_mot_data_type_ = 0;
    }
    _nj_motion_data_info_.frame = frame;
    _nj_motion_data_info_.factor = 0;
    _nj_motion_data_info_.m = NULL;
    if (shape) {
        if (frame >= (Float)shape->nbFrame) {
            return;
        }
        TAKE_SHAPE(shape)
    } else {
        _nj_mot_shape_data_type_ = 0;
        _nj_mot_shape_spline_cnt_ = 0;
        _nj_mot_shape_data_cnt_ = 0;
    }
    _nj_shape_data_info_.frame = frame;
    _nj_shape_data_info_.factor = 0;
    _nj_shape_data_info_.m = NULL;
    _nj_cnk_shape_data_func_ =
        _nj_cnk_shape_data_func_table_[_nj_mot_shape_data_cnt_];
    _nj_cnk_shape_ = SHAPE_INT(_nj_mot_shape_spline_cnt_);
    SET_MOTION_STEPS(object)
    _nj_motion_int_f_func_ = MOTION_INT_F(_nj_mot_spline_cnt_);
    _nj_motion_int_a_func_ = MOTION_INT_A_OLD(_nj_mot_spline_cnt_);
    _nj_cnk_draw_model_ = _njCnkSimpleDrawModel;
    njGetkmPointer();
    draw_shape_tree(object);
    njSetkmPointer();
}
