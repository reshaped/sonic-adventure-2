/* the easy multi and the simple multi chunk drawers' motion draws. the draws
 * interpolate in a straight line, whatever the spline counts */
#include <nj_cnk.h>
#include <nj_cnk_shape.h>
#include <nj_km.h>
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the member exports this datum, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the vertex setup of the drawer the last draw took, njCnkInitEasyMultiVertex
 * or njCnkInitSimpleMultiVertex. the draw runs it after njGetkmPointer */
void (*_nj_init_vertex_func_)(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* pose and draw object, its children and its siblings, as njCnkMotionS.c's
 * draw_motion_tree does. each model draws through _nj_cnk_draw_model_ */
#ifdef NONMATCHING
/* this side's walk returns whether it drew the whole tree. where the stack is
 * full, it abandons the rest of the draw and unwinds the pushes that succeeded.
 * motion keys follow depth-first object order, so going on at a sibling would
 * give it the rejected node's keys */
static Bool draw_multi_motion_tree(NJS_CNK_OBJECT *object)
#else
static void draw_multi_motion_tree(NJS_CNK_OBJECT *object)
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
            _nj_cnk_draw_model_(object->model);
        }
        if (object->child) {
#ifdef NONMATCHING
            /* this side pops its own push, and abandons the draw where the
             * child's walk did */
            if (!draw_multi_motion_tree(object->child)) {
                njPopMatrixEx();
                return FALSE;
            }
#else
            draw_multi_motion_tree(object->child);
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

/* pose and draw as draw_multi_motion_tree does. a node without
 * NJD_EVAL_SHAPE_SKIP draws through _nj_cnk_shape_data_func_ instead */
#ifdef NONMATCHING
/* this side's walk returns whether it drew the whole tree, and abandons the
 * draw where the stack is full, as the motion walk above does */
static Bool draw_multi_shape_tree(NJS_CNK_OBJECT *object)
#else
static void draw_multi_shape_tree(NJS_CNK_OBJECT *object)
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
            _nj_cnk_draw_model_(object->model);
        }
        if (object->child) {
#ifdef NONMATCHING
            /* this side pops its own push, and abandons the draw where the
             * child's walk did */
            if (!draw_multi_shape_tree(object->child)) {
                njPopMatrixEx();
                return FALSE;
            }
#else
            draw_multi_shape_tree(object->child);
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

void njCnkEasyMultiDrawMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                              Float frame)
{
    _nj_init_vertex_func_ = njCnkInitEasyMultiVertex;
    _nj_cnk_draw_model_ = _njCnkEasyMultiDrawModel;
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
    _nj_motion_int_f_func_ = njCnkLinearMotionF;
    _nj_motion_int_a_func_ = njCnkLinearMotionA;
    njGetkmPointer();
    _nj_init_vertex_func_();
    draw_multi_motion_tree(object);
    njSetkmPointer();
}

void njCnkEasyMultiDrawShapeMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                   NJS_MOTION *shape, Float frame)
{
    _nj_init_vertex_func_ = njCnkInitEasyMultiVertex;
    _nj_cnk_draw_model_ = _njCnkEasyMultiDrawModel;
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
    _nj_cnk_shape_ = njCnkEasyMultiShapeLinearPoint;
    SET_MOTION_STEPS(object)
    _nj_motion_int_f_func_ = njCnkLinearMotionF;
    _nj_motion_int_a_func_ = njCnkLinearMotionA;
    njGetkmPointer();
    _nj_init_vertex_func_();
    draw_multi_shape_tree(object);
    njSetkmPointer();
}

void njCnkSimpleMultiDrawMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                Float frame)
{
    _nj_init_vertex_func_ = njCnkInitSimpleMultiVertex;
    _nj_cnk_draw_model_ = _njCnkSimpleMultiDrawModel;
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
    _nj_motion_int_f_func_ = njCnkLinearMotionF;
    _nj_motion_int_a_func_ = njCnkLinearMotionA;
    njGetkmPointer();
    _nj_init_vertex_func_();
    draw_multi_motion_tree(object);
    njSetkmPointer();
}

void njCnkSimpleMultiDrawShapeMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                     NJS_MOTION *shape, Float frame)
{
    _nj_init_vertex_func_ = njCnkInitSimpleMultiVertex;
    _nj_cnk_draw_model_ = _njCnkSimpleMultiDrawModel;
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
    _nj_cnk_shape_ = njCnkEasyMultiShapeLinearPoint;
    SET_MOTION_STEPS(object)
    _nj_motion_int_f_func_ = njCnkLinearMotionF;
    _nj_motion_int_a_func_ = njCnkLinearMotionA;
    njGetkmPointer();
    _nj_init_vertex_func_();
    draw_multi_shape_tree(object);
    njSetkmPointer();
}

void njCnkEasyMultiDrawMotionOld(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                 Float frame)
{
    _nj_init_vertex_func_ = njCnkInitEasyMultiVertex;
    _nj_cnk_draw_model_ = _njCnkEasyMultiDrawModel;
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
    _nj_motion_int_f_func_ = njCnkLinearMotionF;
    _nj_motion_int_a_func_ = njCnkLinearMotionAOld;
    njGetkmPointer();
    _nj_init_vertex_func_();
    draw_multi_motion_tree(object);
    njSetkmPointer();
}

void njCnkEasyMultiDrawShapeMotionOld(NJS_CNK_OBJECT *object,
                                      NJS_MOTION *motion, NJS_MOTION *shape,
                                      Float frame)
{
    _nj_init_vertex_func_ = njCnkInitEasyMultiVertex;
    _nj_cnk_draw_model_ = _njCnkEasyMultiDrawModel;
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
    _nj_cnk_shape_ = njCnkEasyMultiShapeLinearPoint;
    SET_MOTION_STEPS(object)
    _nj_motion_int_f_func_ = njCnkLinearMotionF;
    _nj_motion_int_a_func_ = njCnkLinearMotionAOld;
    njGetkmPointer();
    _nj_init_vertex_func_();
    draw_multi_shape_tree(object);
    njSetkmPointer();
}

void njCnkSimpleMultiDrawMotionOld(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                   Float frame)
{
    _nj_init_vertex_func_ = njCnkInitSimpleMultiVertex;
    _nj_cnk_draw_model_ = _njCnkSimpleMultiDrawModel;
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
    _nj_motion_int_f_func_ = njCnkLinearMotionF;
    _nj_motion_int_a_func_ = njCnkLinearMotionAOld;
    njGetkmPointer();
    _nj_init_vertex_func_();
    draw_multi_motion_tree(object);
    njSetkmPointer();
}

void njCnkSimpleMultiDrawShapeMotionOld(NJS_CNK_OBJECT *object,
                                        NJS_MOTION *motion, NJS_MOTION *shape,
                                        Float frame)
{
    _nj_init_vertex_func_ = njCnkInitSimpleMultiVertex;
    _nj_cnk_draw_model_ = _njCnkSimpleMultiDrawModel;
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
    _nj_cnk_shape_ = njCnkEasyMultiShapeLinearPoint;
    SET_MOTION_STEPS(object)
    _nj_motion_int_f_func_ = njCnkLinearMotionF;
    _nj_motion_int_a_func_ = njCnkLinearMotionAOld;
    njGetkmPointer();
    _nj_init_vertex_func_();
    draw_multi_shape_tree(object);
    njSetkmPointer();
}
