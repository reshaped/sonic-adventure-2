/* the state of Ninja's motion walkers, for models, cameras and lights, and
 * some texture state after it */
#include <nj_motion_types.h>
#include <ninja.h>
#include <sg_xpt.h>

/* a step applies the current node's motion or shape keys to object */
typedef void nj_motion_step(NJS_OBJECT *object);
typedef void nj_float_interpolation(NJS_MKEY_F *keys, Uint32 index,
                                    Uint32 count, Float *result);
typedef void nj_angle_interpolation(NJS_MKEY_A *keys, Uint32 index,
                                    Uint32 count, Angle *result);
typedef void nj_camera_angle_interpolation(NJS_MKEY_A1 *keys, Uint32 index,
                                           Uint32 count, Angle *result);

/* the library exports each of these */
/* NOLINTBEGIN(misc-use-internal-linkage) */
nj_motion_step *_nj_motion_data_func_;
nj_motion_step *_nj_shape_data_func_;
nj_motion_step *_nj_motion_translate_;
nj_motion_step *_nj_motion_rotate_;
nj_motion_step *_nj_motion_scale_;
nj_motion_step *_nj_motion_rotate_zxy_;
nj_motion_step *_nj_motion_shape_;
struct nj_motion_info _nj_motion_data_info_;
struct nj_motion_info _nj_shape_data_info_;
Uint16 _nj_mot_data_cnt_;
Uint16 _nj_mot_data_type_;
Uint16 _nj_mot_shape_data_cnt_;
Uint16 _nj_mot_shape_data_type_;
struct nj_motion_info _nj_cmotion_data_info_;
struct nj_motion_info _nj_lmotion_data_info_;
NJS_LIGHT _nj_motion_light;

/* the arrays of two hold an entry for each of the two motions a motion link
 * blends */
nj_motion_step *_nj_motlink_data_func_[2];
nj_motion_step *_nj_shapelink_data_func_[2];
void (*_nj_int_f_func_)(const Float *values, Float *out, Float rate);
void (*_nj_int_a_func_)(const Angle *values, Angle *out, Float rate);
nj_motion_step *_nj_motion_translate_data_[2];
nj_motion_step *_nj_motion_rotate_data_[2];
nj_motion_step *_nj_motion_scale_data_[2];
nj_motion_step *_nj_motion_shape_data_[2];
struct nj_motion_info _nj_motlink_data_info_[2];
struct nj_motion_info _nj_shapelink_data_info_[2];
Uint16 _nj_motlink_data_cnt_[2];
Uint16 _nj_motlink_data_type_[2];
Uint16 _nj_motlink_shape_data_cnt_[2];
Uint16 _nj_motlink_shape_data_type_[2];
Uint16 _nj_mot_current_data_;
Float _nj_mot_current_frame_;
Uint16 _nj_pos_int_flag_;
Uint16 _nj_ang_int_flag_;
Uint16 _nj_scl_int_flag_;
Uint16 _nj_pnt_int_flag_;
Uint16 _nj_nml_int_flag_;
Float _nj_motion_link_pos_[LINK_PLACES];
Float _nj_motion_link_scl_[LINK_PLACES];
Angle _nj_motion_link_ang_[LINK_PLACES];
Float _nj_motion_link_qua_[LINK_QUA_PLACES];
NJS_POINT3 *_nj_shape_link_point_[4];
NJS_VECTOR *_nj_shape_link_normal_[4];
nj_float_interpolation *_nj_motion_int_f_func_;
nj_angle_interpolation *_nj_motion_int_a_func_;
void (*_nj_draw_model_sub_)(NJS_MODEL *model);
Uint16 _nj_mot_spline_cnt_;
Uint16 _nj_mot_shape_spline_cnt_;
Float *_nj_shape_buf_;
nj_float_interpolation *_nj_cmotion_int_f_func_;
nj_float_interpolation *_nj_cmotion_int_l_func_;
nj_camera_angle_interpolation *_nj_cmotion_int_a_func_;
nj_float_interpolation *_nj_lmotion_int_f_func_;

/* the pointers through which the model, camera and light motion calls write
 * each kind of value */
Float *_nj_mot_pos_buf_ptr_;
Float *_nj_mot_scl_buf_ptr_;
Angle *_nj_mot_ang_buf_ptr_;
Float *_nj_cmot_pos_buf_ptr_;
Float *_nj_cmot_vect_buf_ptr_;
Angle *_nj_cmot_roll_buf_ptr_;
Angle *_nj_cmot_ang_buf_ptr_;
Float *_nj_lmot_pos_buf_ptr_;
Float *_nj_lmot_vect_buf_ptr_;
Float *_nj_lmot_rgb_buf_ptr_;
Float *_nj_lmot_inten_buf_ptr_;
Float *_nj_lmot_rang_buf_ptr_;
Angle *_nj_lmot_ang_buf_ptr_;

NJS_TEXSURFACE _nj_tex_err_surface;
Sint32 _nj_tex_ext_;
Sint32 _nj_palette_bpp_;
Uint32 _nj_palette_mode_;
Uint32 _nj_current_texture_back_;
Uint32 _nj_current_texture_bank_back_;
Int _nj_matrix_flag_;
/* NOLINTEND(misc-use-internal-linkage) */
