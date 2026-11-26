/* the reads of a camera motion. each writes the motion's factors at a frame
 * to data's buffers, and sets no camera */
#include <nj_motion.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the floats of a position, target or direction */
#define VECTOR_FLOATS (sizeof(NJS_VECTOR) / sizeof(Float))

/* the member exports these functions, though no header of the SDK declares
 * them. each is the channel of one kind of factor. it reads the motion's
 * factor numbered _nj_cmotion_data_info_.factor, and adds 1 to that number.
 * where the factor has keys, the channel writes its value at the frame. it
 * writes through the _nj_cmot_ pointer of its kind. njGetCMotionIntPosition
 * also copies the position to _nj_motion_camera_. njGetCMotionIntVector
 * writes its direction plus that position, as a target */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void njGetCMotionIntPosition(void);
void njGetCMotionIntTarget(void);
void njGetCMotionIntVector(void);
void njGetCMotionIntRoll(void);
/* NOLINTEND(misc-use-internal-linkage) */

void njGetCMotionIntPosition(void)
{
    /* @hack spelling: _nj_cmotion_data_info_.factor read at each index, where
     * a local factor would do */
    NJS_MKEY_F *keys = _nj_cmotion_data_info_.p[_nj_cmotion_data_info_.factor];
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;

    if (keys) {
        nb = _nj_cmotion_data_info_.nb[_nj_cmotion_data_info_.factor];
        lower = 0;
        upper = nb;
        frame = (Uint32)_nj_cmotion_data_info_.frame;
        FIND_KEY(keys, frame, lower, upper, middle);
        _nj_cmotion_int_f_func_(keys, lower, nb, _nj_cmot_pos_buf_ptr_);
        _nj_motion_camera_.px = _nj_cmot_pos_buf_ptr_[0];
        _nj_motion_camera_.py = _nj_cmot_pos_buf_ptr_[1];
        _nj_motion_camera_.pz = _nj_cmot_pos_buf_ptr_[2];
        _nj_cmot_pos_buf_ptr_ += VECTOR_FLOATS;
    }
    _nj_cmotion_data_info_.factor++;
}

void njGetCMotionIntTarget(void)
{
    Uint32 factor = _nj_cmotion_data_info_.factor;
    NJS_MKEY_F *keys = _nj_cmotion_data_info_.p[factor];
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;

    if (keys) {
        nb = _nj_cmotion_data_info_.nb[factor];
        lower = 0;
        upper = nb;
        frame = (Uint32)_nj_cmotion_data_info_.frame;
        FIND_KEY(keys, frame, lower, upper, middle);
        _nj_cmotion_int_l_func_(keys, lower, nb, _nj_cmot_vect_buf_ptr_);
        _nj_cmot_vect_buf_ptr_ += VECTOR_FLOATS;
    }
    _nj_cmotion_data_info_.factor++;
}

void njGetCMotionIntVector(void)
{
    /* @hack spelling: _nj_cmotion_data_info_.factor read at each index, where
     * a local factor would do */
    NJS_MKEY_F *keys = _nj_cmotion_data_info_.p[_nj_cmotion_data_info_.factor];
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;

    if (keys) {
        nb = _nj_cmotion_data_info_.nb[_nj_cmotion_data_info_.factor];
        lower = 0;
        upper = nb;
        frame = (Uint32)_nj_cmotion_data_info_.frame;
        FIND_KEY(keys, frame, lower, upper, middle);
        _nj_cmotion_int_l_func_(keys, lower, nb, _nj_cmot_vect_buf_ptr_);
        _nj_cmot_vect_buf_ptr_[0] += _nj_motion_camera_.px;
        _nj_cmot_vect_buf_ptr_[1] += _nj_motion_camera_.py;
        _nj_cmot_vect_buf_ptr_[2] += _nj_motion_camera_.pz;
        _nj_cmot_vect_buf_ptr_ += VECTOR_FLOATS;
    }
    _nj_cmotion_data_info_.factor++;
}

void njGetCMotionIntRoll(void)
{
    Uint32 factor = _nj_cmotion_data_info_.factor;
    NJS_MKEY_A1 *keys = _nj_cmotion_data_info_.p[factor];
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;

    if (keys) {
        nb = _nj_cmotion_data_info_.nb[factor];
        lower = 0;
        upper = nb;
        frame = (Uint32)_nj_cmotion_data_info_.frame;
        FIND_KEY(keys, frame, lower, upper, middle);
        _nj_cmotion_int_a_func_(keys, lower, nb, _nj_cmot_roll_buf_ptr_);
        _nj_cmot_roll_buf_ptr_++;
    }
    _nj_cmotion_data_info_.factor++;
}

/* the channel of the angle of view, as the exported ones above are theirs */
static void get_view_angle(void)
{
    Uint32 factor = _nj_cmotion_data_info_.factor;
    NJS_MKEY_A1 *keys = _nj_cmotion_data_info_.p[factor];
    Uint32 nb;
    Uint32 frame;
    Uint32 lower;
    Uint32 upper;
    Uint32 middle;

    if (keys) {
        nb = _nj_cmotion_data_info_.nb[factor];
        lower = 0;
        upper = nb;
        frame = (Uint32)_nj_cmotion_data_info_.frame;
        FIND_KEY(keys, frame, lower, upper, middle);
        _nj_cmotion_int_a_func_(keys, lower, nb, _nj_cmot_ang_buf_ptr_);
        _nj_cmot_ang_buf_ptr_++;
    }
    _nj_cmotion_data_info_.factor++;
}

/* run the channel of each factor that type names, in the order of the type's
 * bits. a factor with no channel here is only stepped past */
#define RUN_CHANNELS(type)                                                     \
    {                                                                          \
        if ((type) & NJD_MTYPE_POS_0) {                                        \
            njGetCMotionIntPosition();                                         \
        }                                                                      \
        if ((type) & NJD_MTYPE_ANG_1) {                                        \
            _nj_cmotion_data_info_.factor++;                                   \
        }                                                                      \
        if ((type) & NJD_MTYPE_SCL_2) {                                        \
            _nj_cmotion_data_info_.factor++;                                   \
        }                                                                      \
        if ((type) & NJD_MTYPE_TARGET_3) {                                     \
            njGetCMotionIntTarget();                                           \
        } else if ((type) & NJD_MTYPE_VEC_3) {                                 \
            njGetCMotionIntVector();                                           \
        }                                                                      \
        if ((type) & NJD_MTYPE_ROLL_6) {                                       \
            njGetCMotionIntRoll();                                             \
        }                                                                      \
        if ((type) & NJD_MTYPE_ANGLE_7) {                                      \
            get_view_angle();                                                  \
        }                                                                      \
        if ((type) & NJD_MTYPE_RGB_8) {                                        \
            _nj_cmotion_data_info_.factor++;                                   \
        }                                                                      \
        if ((type) & NJD_MTYPE_INTENSITY_9) {                                  \
            _nj_cmotion_data_info_.factor++;                                   \
        }                                                                      \
        if ((type) & NJD_MTYPE_SPOT_10) {                                      \
            _nj_cmotion_data_info_.factor++;                                   \
        } else if ((type) & NJD_MTYPE_POINT_10) {                              \
            _nj_cmotion_data_info_.factor++;                                   \
        }                                                                      \
    }

void njGetCameraAction(NJS_CACTION *caction, NJS_CMOTION_DATA *data,
                       Float frame)
{
    NJS_CAMERA *camera = caction->camera;
    NJS_MOTION *motion = caction->motion;
    Uint16 type;
    Uint16 factors;
    Uint16 spline;
    void *mdata;

    _nj_cmot_pos_buf_ptr_ = data->pos;
    _nj_cmot_vect_buf_ptr_ = data->vect;
    _nj_cmot_roll_buf_ptr_ = data->roll;
    _nj_cmot_ang_buf_ptr_ = data->ang;
    if (motion && (Float)motion->nbFrame > frame) {
        START_CAMERA_MOTION(camera, motion, frame, type, factors, spline,
                            mdata);
        switch (factors) {
            CAMERA_ENTRY_CASE(1, mdata);
            CAMERA_ENTRY_CASE(2, mdata);
            CAMERA_ENTRY_CASE(3, mdata);
            CAMERA_ENTRY_CASE(4, mdata);
        default:
            return;
        }
        RUN_CHANNELS(type);
    }
}

void njGetCameraMotion(NJS_CAMERA *camera, NJS_MOTION *motion,
                       NJS_CMOTION_DATA *data, Float frame)
{
    Uint16 type;
    Uint16 factors;
    Uint16 spline;
    void *mdata;

    _nj_cmot_pos_buf_ptr_ = data->pos;
    _nj_cmot_vect_buf_ptr_ = data->vect;
    _nj_cmot_roll_buf_ptr_ = data->roll;
    _nj_cmot_ang_buf_ptr_ = data->ang;
    if (motion && (Float)motion->nbFrame > frame) {
        START_CAMERA_MOTION(camera, motion, frame, type, factors, spline,
                            mdata);
        switch (factors) {
            CAMERA_ENTRY_CASE(1, mdata);
            CAMERA_ENTRY_CASE(2, mdata);
            CAMERA_ENTRY_CASE(3, mdata);
            CAMERA_ENTRY_CASE(4, mdata);
        default:
            return;
        }
        RUN_CHANNELS(type);
    }
}
