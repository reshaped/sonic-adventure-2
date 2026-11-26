#include <machine.h>
#include <ninja.h>
#include <nj_light_state.h>
#include <sg_xpt.h>

void njSetCamera(NJS_CAMERA *c)
{
    Float *m;
    Float sin_z;
    Float cos_z;
    Float xz_length2;
    Float xz_length;
    Float inv_length;
    Float cos_x;
    Float sin_x;
    Float inv_xz_length;
    Float cos_y;
    Float sin_y;
    Float *base;

    /* @hack spelling: m, a pointer to _nj_camera_matrix_ taken here, where
     * the matrix written in place would do */
    m = _nj_camera_matrix_;
    fsca(-c->roll, &sin_z, &cos_z);
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code adds x's square to z's
     * by an FMAC */
    xz_length2 = __builtin_fmaf(c->vx, c->vx, c->vz * c->vz);
#else
    xz_length2 = (c->vx * c->vx) + (c->vz * c->vz);
#endif
    /* @hack spelling: xz_length, a local for the root, where the call in
     * cos_x's product would do */
    xz_length = _builtin_sqrtf(xz_length2);
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code adds y's square to
     * xz_length2 by an FMAC */
    inv_length = fsrra(__builtin_fmaf(c->vy, c->vy, xz_length2));
#else
    inv_length = fsrra(xz_length2 + (c->vy * c->vy));
#endif
    cos_x = xz_length * inv_length;
    sin_x = c->vy * inv_length;
    if (xz_length2 > NO_YAW_LENGTH2_MAX) {
        inv_xz_length = fsrra(xz_length2);
        cos_y = c->vz * inv_xz_length;
        sin_y = -c->vx * inv_xz_length;
    } else {
        cos_y = 1.0F;
        sin_y = 0.0F;
    }
    /* a point takes the turn about y first, then x, then z */
    m[0] = (cos_z * cos_y) - (sin_z * sin_x * sin_y);
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses the second
     * product's last multiply and the sum into one FMAC */
    m[1] = __builtin_fmaf(cos_z * sin_x, sin_y, sin_z * cos_y);
#else
    m[1] = (sin_z * cos_y) + (cos_z * sin_x * sin_y);
#endif
    m[2] = -cos_x * sin_y;
    m[3] = 0.0F;
    m[4] = -sin_z * cos_x;
    m[5] = cos_z * cos_x;
    m[6] = sin_x;
    m[7] = 0.0F;
#ifdef NONMATCHING
    /* this side keeps the console's bits. SHC's code fuses this sum as it
     * fuses m[1]'s */
    m[8] = __builtin_fmaf(sin_z * sin_x, cos_y, cos_z * sin_y);
#else
    m[8] = (cos_z * sin_y) + (sin_z * sin_x * cos_y);
#endif
    m[9] = (sin_z * sin_y) - (cos_z * sin_x * cos_y);
    m[10] = cos_x * cos_y;
    m[11] = 0.0F;
    /* before the turns, the camera's place moves to the origin */
    m[12] = (-c->px * m[0]) - (c->py * m[4]) - (c->pz * m[8]);
    m[13] = (-c->px * m[1]) - (c->py * m[5]) - (c->pz * m[9]);
    m[14] = (-c->px * m[2]) - (c->py * m[6]) - (c->pz * m[10]);
    m[15] = 1.0F;
    _nj_current_matrix_ptr_ = _nj_base_matrix_ptr_;
    _nj_stack_size_ = 1;
    /* @hack spelling: base, a pointer to the stack's matrix taken here, where
     * *_nj_base_matrix_ptr_ written in place would do */
    base = *_nj_base_matrix_ptr_;
    /* the stack's one matrix is m followed by a scale of -1 in x and y */
    base[0] = -m[0];
    base[1] = -m[1];
    base[2] = m[2];
    base[3] = 0.0F;
    base[4] = -m[4];
    base[5] = -m[5];
    base[6] = m[6];
    base[7] = 0.0F;
    base[8] = -m[8];
    base[9] = -m[9];
    base[10] = m[10];
    base[11] = 0.0F;
    base[12] = -m[12];
    base[13] = -m[13];
    base[14] = m[14];
    base[15] = 1.0F;
    /* @hack spelling: _nj_base_matrix_ptr_ read again, here and for ld_ext
     * below, where base would do */
    njMemCopy4(_nj_view_matrix_, *_nj_base_matrix_ptr_,
               sizeof(NJS_MATRIX) / sizeof(Sint32));
#ifdef NONMATCHING
    /* this side leaves the x aspect out, which is 1 but under the port's
     * 16:9. there the aspect widens the view, and the angle still spans the
     * screen's 4:3 middle (engine/dc/port.h) */
    _nj_screen_.dist = _nj_screen_.w / (njTan(c->ang / 2) * 2.0F);
#else
    _nj_screen_.dist =
        _nj_screen_.w / _nj_scr_aspect_x_ / (njTan(c->ang / 2) * 2.0F);
#endif
    _nj_near_clip_ = -c->n_clip;
    _nj_far_clip_ = -c->f_clip;
#ifdef NONMATCHING
    /* njInitMatrix requires a 32-byte-aligned stack. */
    ld_ext(__builtin_assume_aligned(*_nj_base_matrix_ptr_, 32));
#else
    ld_ext((Float(*)[4]) * _nj_base_matrix_ptr_);
#endif
}
