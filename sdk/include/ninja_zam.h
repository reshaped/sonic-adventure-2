/* the non-matching code's inline helpers for Ninja's matrix stack, its
 * transforms, and its vector and scalar math. ninja.h includes this header
 * under NONMATCHING and DREAMCAST, after its prototypes.
 *
 * a function's helper is named zam_ and the function's name. the function's own
 * source defines the function from it. where a function-like macro of the name
 * follows the helper, each C call runs the helper inline. the macro expands
 * only where a ( follows the name. so the function's address, and a call from
 * assembly, still reach the definition. the helper keeps the function's
 * contract and its type. ZAM_ASSERT_COMPATIBLE checks the type alone.
 *
 * the shipped body of a function is its code in the image. every helper keeps
 * the console's bits, running the shipped body's arithmetic on the same
 * operands. where the operands' order shows in the bits, as FIPR's does, it
 * keeps that order too. GCC may swap an FADD's or an FMUL's operands, which
 * shows in no bit. the sum or product doesn't depend on their order. a NaN
 * result is the SH-4's one NaN, 0x7fbfffff, whichever operand was a NaN.
 * zam_fipr_self, zam_fipr_columns, zam_store_slot, zam_push_xmtrx and
 * zam_njInnerProduct are the helpers' own asms. each other instruction the
 * helpers run that GCC doesn't compile from C is SH4ZAM's.
 *
 * SH4ZAM's matrices are column-major, and Ninja's take a point as a row on
 * their left. so SH4ZAM's column i of XMTRX is row i of a Ninja matrix */
#ifndef NINJA_ZAM_H
#define NINJA_ZAM_H
#include <zam.h>
#include <assert.h>
#include <stdint.h>
#include <sh4zam/shz_matrix.h>
#include <sh4zam/shz_scalar.h>
#include <sh4zam/shz_trig.h>
#include <sh4zam/shz_vector.h>
#include <sh4zam/shz_xmtrx.h>

/* the matrix stack's room, in matrices: njInitMatrix's n */
extern Sint32 _nj_max_stack_size_;
/* njInitMatrix's flag. while it isn't 0, njCalcVector scales each vector it
 * writes to unit length */
extern Int _nj_matrix_flag_;

/* the helpers cast an NJS_MATRIX pointer to shz_mat4x4_t *, and a pointer into
 * a matrix to shz_vec4_t *. so each of those SH4ZAM types must have the size of
 * what it stands for. it must also align no more strictly. ninja.h says what
 * keeps a caller's matrix on 8. an shz_vec3_t becomes an NJS_POINT3 by value,
 * through SH4ZAM's shz_vec3_to, which holds the two types to one size itself */
_Static_assert(sizeof(shz_mat4x4_t) == sizeof(NJS_MATRIX),
               "an shz_mat4x4_t isn't an NJS_MATRIX's size");
_Static_assert(_Alignof(shz_mat4x4_t) <= _Alignof(NJS_MATRIX),
               "an shz_mat4x4_t aligns past an NJS_MATRIX");
_Static_assert(sizeof(shz_vec4_t) == 4 * sizeof(Float),
               "an shz_vec4_t isn't four Floats' size");
_Static_assert(_Alignof(shz_vec4_t) <= _Alignof(Float),
               "an shz_vec4_t aligns past a Float");

/* p's x, y and z as an SH4ZAM vector. the helper reads them as p's members,
 * where SH4ZAM's shz_vec3_from(*p) reads p whole. at -Os, GCC then copies some
 * callers' vectors to the stack before it reads them, and
 * src/object/mover.c's code grows by a fifth. read as members, a warning that
 * one is unset also names the caller's variable */
ZAM_INLINE shz_vec3_t zam_load_vec3(const NJS_POINT3 *p)
{
    return shz_vec3_init(p->x, p->y, p->z);
}

/* store v in p's x, y and z */
ZAM_INLINE void zam_store_vec3(NJS_POINT3 *p, shz_vec3_t v)
{
    *p = shz_vec3_to(NJS_POINT3, v);
}

/* FIPR FV4,FV4: the sum of the squares of x, y, z and w. it writes FR7
 * alone, so x, y and z stay in FR4 to FR6 past it. SH4ZAM's single dots are
 * C multiply-adds, whose bits needn't match FIPR's */
ZAM_INLINE Float zam_fipr_self(Float x, Float y, Float z, Float w)
{
    register Float fr4 __asm__("fr4") = x;
    register Float fr5 __asm__("fr5") = y;
    register Float fr6 __asm__("fr6") = z;
    register Float square __asm__("fr7") = w;

    __asm__("fipr fv4,fv4" : "+f"(square) : "f"(fr4), "f"(fr5), "f"(fr6));
    return square;
}

/* the square of v's length: the FIPR of (v, 0) with itself */
ZAM_INLINE Float zam_squared_length(shz_vec3_t v)
{
    return zam_fipr_self(v.x, v.y, v.z, 0.0F);
}

/* v turned by m, a matrix in memory: the shipped njCalcVector's instructions
 * from its first load of m to its third FIPR, inline. each FIPR takes v and a
 * 0 as FVm, and a column of m as FVn, with a 0 in place of the column's last
 * float. the loads share R0, as the shipped body's do. they are single
 * floats, so m need lie only on 4. the "m" operand tells GCC which memory the
 * asm reads. SH4ZAM's shz_mat4x4_transform_vec3 leaves the first FIPR's
 * result in the third's FVn, against FVm's 0. so an infinite or NaN x makes
 * its z the NaN, where the shipped body's z reads no x */
ZAM_INLINE shz_vec3_t zam_fipr_columns(NJS_MATRIX *m, shz_vec3_t v)
{
    register Float vx __asm__("fr0") = v.x;
    register Float vy __asm__("fr1") = v.y;
    register Float vz __asm__("fr2") = v.z;
    register Float vw __asm__("fr3") = 0.0F;
    register Float x __asm__("fr10");
    register Float y __asm__("fr11");
    register Float z __asm__("fr7");

    __asm__("mov #16,r0\n\t"
            "fmov.s @%3,fr4\n\t"
            "fmov.s @(r0,%3),fr5\n\t"
            "mov #32,r0\n\t"
            "fmov.s @(r0,%3),fr6\n\t"
            "fldi0 fr7\n\t"
            "fipr fv0,fv4\n\t"
            "mov #4,r0\n\t"
            "fmov.s @(r0,%3),fr8\n\t"
            "mov #20,r0\n\t"
            "fmov.s @(r0,%3),fr9\n\t"
            "mov #36,r0\n\t"
            "fmov.s @(r0,%3),fr10\n\t"
            "fldi0 fr11\n\t"
            "fipr fv0,fv8\n\t"
            "fmov fr7,fr10\n\t"
            "mov #8,r0\n\t"
            "fmov.s @(r0,%3),fr4\n\t"
            "mov #24,r0\n\t"
            "fmov.s @(r0,%3),fr5\n\t"
            "mov #40,r0\n\t"
            "fmov.s @(r0,%3),fr6\n\t"
            "fldi0 fr7\n\t"
            "fipr fv0,fv4"
            : "=f"(x), "=f"(y), "=f"(z)
            : "r"(m), "m"(*m), "f"(vx), "f"(vy), "f"(vz), "f"(vw)
            : "r0", "fr4", "fr5", "fr6", "fr8", "fr9");
    return shz_vec3_init(x, y, z);
}

/* FSCA's sine and cosine of ang, by SH4ZAM's SH-4 back end. the public
 * shz_sincosu16 takes a constant Angle to radians and back. that can land a
 * 65536th of a turn short. FSCA reads FPUL's low 16 bits alone, so the
 * conversion to uint16_t changes no bit it reads */
ZAM_INLINE shz_sincos_t zam_fsca(Angle ang)
{
    return shz_sincosu16_sh4((uint16_t)ang);
}

/* FSRRA's 1 over the square root of n, by SH4ZAM's SH-4 back end. the public
 * shz_inv_sqrtf_fsrra takes a constant's root by FSQRT and FDIV instead */
ZAM_INLINE Float zam_fsrra(Float n)
{
    return shz_inv_sqrtf_fsrra_sh4(n);
}

/* copy the four words at from to to, loading all four before storing any. GCC
 * can't rule out that the two overlap. so it wouldn't move a load ahead of a
 * store itself */
ZAM_INLINE void zam_copy_four(Uint32 *to, const Uint32 *from)
{
    Uint32 a = from[0];
    Uint32 b = from[1];
    Uint32 c = from[2];
    Uint32 d = from[3];

    to[0] = a;
    to[1] = b;
    to[2] = c;
    to[3] = d;
}

/* copy ms's 16 words to md, four at a time, in registers a call needn't
 * keep. SH4ZAM's shz_memcpy4 copies eight words at a time. that takes
 * registers a call must keep, which a copy through it would push and pop */
ZAM_INLINE void zam_copy_matrix(Float *md, const Float *ms)
{
    zam_copy_four((Uint32 *)md, (const Uint32 *)ms);
    zam_copy_four((Uint32 *)md + 4, (const Uint32 *)ms + 4);
    zam_copy_four((Uint32 *)md + 8, (const Uint32 *)ms + 8);
    zam_copy_four((Uint32 *)md + 12, (const Uint32 *)ms + 12);
}

/* the matrix stack's helpers. XMTRX holds the current matrix. a push stores it
 * in the slot at _nj_current_matrix_ptr_, and moves the pointer up a slot. a
 * pop moves the pointer down, and loads XMTRX from the slot there by
 * shz_xmtrx_load_4x4. njInitMatrix's m must lie on 32 bytes, and a slot is 64.
 * so each slot is two whole lines of the operand cache */

/* zam_store_slot on the current slot, moving _nj_current_matrix_ptr_ up a
 * slot. it returns the slot the pointer moves to. the asm writes the new
 * pointer between the claims and the pair stores, as the shipped pushes do.
 * its register holds that address once the claims are made, which C would
 * compute again */
ZAM_INLINE NJS_MATRIX *zam_push_xmtrx(void)
{
    NJS_MATRIX **pointer = &_nj_current_matrix_ptr_;
    NJS_MATRIX *slot = *pointer;
    Float *at = *slot;

    __asm__ volatile("movca.l r0,@%0\n\t"
                     "add #32,%0\n\t"
                     "movca.l r0,@%0\n\t"
                     "add #32,%0\n\t"
                     "mov.l %0,@%3\n\t"
                     "fschg\n\t"
                     "fmov xd14,@-%0\n\t"
                     "fmov xd12,@-%0\n\t"
                     "fmov xd10,@-%0\n\t"
                     "fmov xd8,@-%0\n\t"
                     "fmov xd6,@-%0\n\t"
                     "fmov xd4,@-%0\n\t"
                     "fmov xd2,@-%0\n\t"
                     "fmov xd0,@-%0\n\t"
                     "fschg"
                     : "+r"(at), "=m"(*slot), "=m"(*pointer)
                     : "r"(pointer));
    return slot + 1;
}

/* the push fails where the stack already holds _nj_max_stack_size_ matrices.
 * the test is signed, as the shipped body's CMP/GE is */
ZAM_INLINE Bool zam_njPushMatrixEx(void)
{
    if (_nj_stack_size_ >= _nj_max_stack_size_) {
        return FALSE;
    }
    _nj_stack_size_++;
    zam_push_xmtrx();
    return TRUE;
}
ZAM_ASSERT_COMPATIBLE(njPushMatrixEx);
#define njPushMatrixEx() zam_njPushMatrixEx()

/* the pop fails where it would leave the stack no matrix. the test is signed,
 * as the shipped body's CMP/PL is */
ZAM_INLINE_FLAT Bool zam_njPopMatrixEx(void)
{
    Sint32 depth = _nj_stack_size_ - 1;

    if (depth <= 0) {
        return FALSE;
    }
    _nj_stack_size_ = depth;
    _nj_current_matrix_ptr_--;
    shz_xmtrx_load_4x4((const shz_mat4x4_t *)_nj_current_matrix_ptr_);
    return TRUE;
}
ZAM_ASSERT_COMPATIBLE(njPopMatrixEx);
#define njPopMatrixEx() zam_njPopMatrixEx()

/* the push fails as zam_njPushMatrixEx's does. it stores XMTRX in the current
 * slot before it loads m, as the shipped body does. m is loaded by pairs where
 * it lies on 8, and by single floats where it doesn't. the shipped body's pair
 * loads take an address error on an m off 8. the macro runs the helper inline
 * only where a source compiles for speed. at -Os, inlining its two ways to
 * load m would add about 150 bytes to each call */
ZAM_INLINE Bool zam_njPushMatrix(NJS_MATRIX *m)
{
    NJS_MATRIX *next;

    if (_nj_stack_size_ >= _nj_max_stack_size_) {
        return FALSE;
    }
    _nj_stack_size_++;
    next = zam_push_xmtrx();
    if (m == NULL) {
        zam_store_slot(*next);
    } else {
        shz_xmtrx_load_unaligned_4x4(*m);
    }
    return TRUE;
}
ZAM_ASSERT_COMPATIBLE(njPushMatrix);
#ifndef __OPTIMIZE_SIZE__
#define njPushMatrix(m) zam_njPushMatrix(m)
#endif

/* A zero pop keeps the live XMTRX, including changes made since the slot
 * was saved. A positive pop must leave the base matrix on the stack. */
ZAM_INLINE_FLAT Bool zam_njPopMatrix(Uint32 n)
{
    if (_nj_stack_size_ <= 0 || n >= (Uint32)_nj_stack_size_) {
        return FALSE;
    }
    if (n == 0) {
        return TRUE;
    }
    _nj_stack_size_ -= (Sint32)n;
    _nj_current_matrix_ptr_ -= n;
    shz_xmtrx_load_4x4((const shz_mat4x4_t *)_nj_current_matrix_ptr_);
    return TRUE;
}
ZAM_ASSERT_COMPATIBLE(njPopMatrix);
#define njPopMatrix(n) zam_njPopMatrix(n)

/* A matrix in memory must be 8-byte aligned. SH4ZAM's four FIPRs take
 * the point first and a column of m second, in the shipped order. */
ZAM_INLINE void zam_njTranslate(NJS_MATRIX *m, Float x, Float y, Float z)
{
    shz_vec4_t moved;

    if (m == NULL) {
        shz_xmtrx_translate(x, y, z);
        return;
    }
    assert((uintptr_t)m % 8 == 0);
    moved = shz_mat4x4_transform_vec4((const shz_mat4x4_t *)m,
                                      shz_vec4_init(x, y, z, 1.0F));
    shz_vec4_deref(&(*m)[12]) = moved;
}
ZAM_ASSERT_COMPATIBLE(njTranslate);
#define njTranslate(m, x, y, z) zam_njTranslate(m, x, y, z)

/* zam_njTranslate by v's x, y and z */
ZAM_INLINE void zam_njTranslateV(NJS_MATRIX *m, NJS_VECTOR *v)
{
    shz_vec3_t by = zam_load_vec3(v);

    zam_njTranslate(m, by.x, by.y, by.z);
}
ZAM_ASSERT_COMPATIBLE(njTranslateV);
#define njTranslateV(m, v) zam_njTranslateV(m, v)

/* SH4ZAM's translation runs the shipped body's FTRV of v and 1. the product
 * becomes XMTRX's row 3 */
ZAM_INLINE void zam_njTranslateEx(NJS_VECTOR *v)
{
    shz_xmtrx_translate(v->x, v->y, v->z);
}
ZAM_ASSERT_COMPATIBLE(njTranslateEx);
#define njTranslateEx(v) zam_njTranslateEx(v)

/* each element of m's rows 0 to 2 takes one FMUL by its row's factor. m
 * need lie only on 4, since an shz_vec4_t aligns to 4 */
ZAM_INLINE void zam_njScale(NJS_MATRIX *m, Float sx, Float sy, Float sz)
{
    if (m == NULL) {
        shz_xmtrx_scale(sx, sy, sz);
        return;
    }
    shz_vec4_deref(&(*m)[0]) = shz_vec4_scale(shz_vec4_deref(&(*m)[0]), sx);
    shz_vec4_deref(&(*m)[4]) = shz_vec4_scale(shz_vec4_deref(&(*m)[4]), sy);
    shz_vec4_deref(&(*m)[8]) = shz_vec4_scale(shz_vec4_deref(&(*m)[8]), sz);
}

/* zam_njScale by v's x, y and z */
ZAM_INLINE void zam_njScaleV(NJS_MATRIX *m, NJS_VECTOR *v)
{
    shz_vec3_t by = zam_load_vec3(v);

    zam_njScale(m, by.x, by.y, by.z);
}

/* SH4ZAM's scale takes one FMUL for each element of XMTRX's rows 0 to 2. the
 * element is FMUL's Rn, and its row's factor is Rm, as in the shipped body */
ZAM_INLINE void zam_njScaleEx(NJS_VECTOR *v)
{
    shz_xmtrx_scale(v->x, v->y, v->z);
}
ZAM_ASSERT_COMPATIBLE(njScaleEx);
#define njScaleEx(v) zam_njScaleEx(v)

/* the axes of the turns below */
enum zam_axis { ZAM_X, ZAM_Y, ZAM_Z };

/* XMTRX = R * XMTRX, where R turns by ang about axis. SH4ZAM runs FSCA for
 * ang's sine and cosine. it then runs an FTRV of each of R's rows that aren't
 * the unit matrix's. the products replace the same rows of XMTRX. each row
 * holds what the shipped body's holds, lane for lane */
ZAM_INLINE_FLAT void zam_turn(enum zam_axis axis, Angle ang)
{
    switch (axis) {
    case ZAM_X:
        shz_xmtrx_rotate_x_u16((uint16_t)ang);
        break;
    case ZAM_Y:
        shz_xmtrx_rotate_y_u16((uint16_t)ang);
        break;
    case ZAM_Z:
        shz_xmtrx_rotate_z_u16((uint16_t)ang);
        break;
    }
}

/* zam_turn, skipping an ang of 0 as the shipped bodies do. a turn by 0 would
 * still run its FTRVs, which can change XMTRX. an infinity they multiply by 0
 * makes NaNs of the rows they write. the test reads all 32 bits of ang, as the
 * shipped TST does */
ZAM_INLINE void zam_turn_nonzero(enum zam_axis axis, Angle ang)
{
    if (ang != 0) {
        zam_turn(axis, ang);
    }
}

/* m = R * m, as the shipped bodies turn a matrix in memory. the two rows of m
 * that the turn changes stand in for XMTRX's rows of the same numbers. the
 * helper puts XMTRX's own rows back after. the turn's FTRVs multiply XMTRX's
 * other two rows by zeros. so a NaN or an infinity there still reaches m, as
 * in the shipped bodies */
ZAM_INLINE void zam_turn_rows(NJS_MATRIX *m, enum zam_axis axis, Angle ang)
{
    unsigned first = axis == ZAM_X ? 1 : 0;
    unsigned second = axis == ZAM_Z ? 1 : 2;
    shz_vec4_t *row_first = &shz_vec4_deref(&(*m)[4 * first]);
    shz_vec4_t *row_second = &shz_vec4_deref(&(*m)[4 * second]);
    shz_vec4_t keep_first = shz_xmtrx_read_col(first);
    shz_vec4_t keep_second = shz_xmtrx_read_col(second);

    shz_xmtrx_write_col(first, *row_first);
    shz_xmtrx_write_col(second, *row_second);
    zam_turn(axis, ang);
    *row_first = shz_xmtrx_read_col(first);
    *row_second = shz_xmtrx_read_col(second);
    shz_xmtrx_write_col(first, keep_first);
    shz_xmtrx_write_col(second, keep_second);
}

/* the helper turns XMTRX in the shipped body's order. it turns about z, y and
 * x, or about y, x and z where lv is above 0. so a point takes the turns in the
 * reverse order, as ninja.h says. each turn is the shipped body's, lane for
 * lane, and skips an Angle of 0 as it does. the macro runs the helper inline
 * only where a source compiles for speed. at -Os, inlining its three turns
 * would add about 120 bytes to each call */
ZAM_INLINE_FLAT void zam_njRotateEx(Angle *ang, Sint32 lv)
{
    Angle x = ang[0];
    Angle y = ang[1];
    Angle z = ang[2];

    if (lv > 0) {
        zam_turn_nonzero(ZAM_Y, y);
        zam_turn_nonzero(ZAM_X, x);
        zam_turn_nonzero(ZAM_Z, z);
    } else {
        zam_turn_nonzero(ZAM_Z, z);
        zam_turn_nonzero(ZAM_Y, y);
        zam_turn_nonzero(ZAM_X, x);
    }
}
ZAM_ASSERT_COMPATIBLE(njRotateEx);
#ifndef __OPTIMIZE_SIZE__
#define njRotateEx(ang, lv) zam_njRotateEx(ang, lv)
#endif

/* A null matrix selects XMTRX. A matrix in memory has the alignment and
 * FIPR operand order described at zam_njTranslate. */
ZAM_INLINE void zam_njCalcPoint(NJS_MATRIX *m, NJS_POINT3 *ps, NJS_POINT3 *pd)
{
    shz_vec3_t point = zam_load_vec3(ps);

    if (m == NULL) {
        point = shz_xmtrx_transform_point3(point);
    } else {
        assert((uintptr_t)m % 8 == 0);
        point = shz_mat4x4_transform_point3((const shz_mat4x4_t *)m, point);
    }
    zam_store_vec3(pd, point);
}
ZAM_ASSERT_COMPATIBLE(njCalcPoint);
#define njCalcPoint(m, ps, pd) zam_njCalcPoint(m, ps, pd)

/* the helper reads m[12] to m[14] as Floats. at -Os, GCC would copy them as
 * one shz_vec3_t through the stack, by two calls of libgcc's copy */
ZAM_INLINE void zam_njGetTranslation(NJS_MATRIX *m, NJS_POINT3 *p)
{
    if (m == NULL) {
        zam_store_vec3(p, shz_xmtrx_get_translation());
    } else {
        zam_store_vec3(p, shz_vec3_init((*m)[12], (*m)[13], (*m)[14]));
    }
}
ZAM_ASSERT_COMPATIBLE(njGetTranslation);
#define njGetTranslation(m, p) zam_njGetTranslation(m, p)

/* the shipped body, but for its return, inline at the call. FIPR FV4,FV0 takes
 * v2 as FVm and v1 as FVn, with each vector's fourth part 0. FIPR's sum can
 * change with the operands' order. the loads share R0, as the shipped body's
 * do. GCC's own loads would take a register and an ADD for each part. the "m"
 * operands tell GCC which memory the asm reads */
ZAM_INLINE Float zam_njInnerProduct(NJS_VECTOR *v1, NJS_VECTOR *v2)
{
    register Float dot __asm__("fr3");

    __asm__("mov #4,r0\n\t"
            "fmov.s @%1,fr0\n\t"
            "fmov.s @%2,fr4\n\t"
            "fmov.s @(r0,%1),fr1\n\t"
            "fmov.s @(r0,%2),fr5\n\t"
            "mov #8,r0\n\t"
            "fldi0 fr7\n\t"
            "fmov.s @(r0,%1),fr2\n\t"
            "fmov.s @(r0,%2),fr6\n\t"
            "fldi0 fr3\n\t"
            "fipr fv4,fv0"
            : "=f"(dot)
            : "r"(v1), "r"(v2), "m"(*v1), "m"(*v2)
            : "r0", "fr0", "fr1", "fr2", "fr4", "fr5", "fr6", "fr7");
    return dot;
}
ZAM_ASSERT_COMPATIBLE(njInnerProduct);
#define njInnerProduct(v1, v2) zam_njInnerProduct(v1, v2)

/* FSQRT of FIPR FV4,FV4 of v and 0, as the shipped body runs */
ZAM_INLINE Float zam_njScalor(NJS_VECTOR *v)
{
    return __builtin_sqrtf(zam_squared_length(zam_load_vec3(v)));
}
ZAM_ASSERT_COMPATIBLE(njScalor);
#define njScalor(v) zam_njScalor(v)

/* FIPR FV4,FV4 of v and 0, as the shipped body runs */
ZAM_INLINE Float zam_njScalor2(NJS_VECTOR *v)
{
    return zam_squared_length(zam_load_vec3(v));
}
ZAM_ASSERT_COMPATIBLE(njScalor2);
#define njScalor2(v) zam_njScalor2(v)

/* the length it returns is the squared length times FSRRA's 1 over its
 * root. the shipped body computes it so */
ZAM_INLINE Float zam_njUnitVector(NJS_VECTOR *v)
{
    shz_vec3_t unscaled = zam_load_vec3(v);
    Float squared = zam_squared_length(unscaled);
    Float scale = zam_fsrra(squared);

    zam_store_vec3(v, shz_vec3_scale(unscaled, scale));
    return squared * scale;
}
ZAM_ASSERT_COMPATIBLE(njUnitVector);
#define njUnitVector(v) zam_njUnitVector(v)

/* for an n below 0, njSqrt and njInvertSqrt take n's magnitude, and negate
 * the result. FSQRT and FSRRA make the results, as in the shipped bodies */
ZAM_INLINE Float zam_njSqrt(Float n)
{
    return n < 0.0F ? -__builtin_sqrtf(-n) : __builtin_sqrtf(n);
}
ZAM_ASSERT_COMPATIBLE(njSqrt);
#define njSqrt(n) zam_njSqrt(n)
ZAM_INLINE Float zam_njInvertSqrt(Float n)
{
    return n < 0.0F ? -zam_fsrra(-n) : zam_fsrra(n);
}
ZAM_ASSERT_COMPATIBLE(njInvertSqrt);
#define njInvertSqrt(n) zam_njInvertSqrt(n)

/* njSin and njCos return FSCA's sine and cosine, and njTan the sine over
 * the cosine by FDIV. the shipped bodies return the same */
ZAM_INLINE Float zam_njSin(Angle n)
{
    return zam_fsca(n).sin;
}
ZAM_ASSERT_COMPATIBLE(njSin);
#define njSin(n) zam_njSin(n)
ZAM_INLINE Float zam_njCos(Angle n)
{
    return zam_fsca(n).cos;
}
ZAM_ASSERT_COMPATIBLE(njCos);
#define njCos(n) zam_njCos(n)
ZAM_INLINE Float zam_njTan(Angle n)
{
    shz_sincos_t pair = zam_fsca(n);

    return pair.sin / pair.cos;
}
ZAM_ASSERT_COMPATIBLE(njTan);
#define njTan(n) zam_njTan(n)

#endif
