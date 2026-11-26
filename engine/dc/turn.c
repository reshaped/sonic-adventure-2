/* turn.h's kernels: on the Dreamcast each is one asm, and on a host its C
 * model. the model spells each FIPR the asm runs as FIPR of its two vectors.
 * the second vector is the column that turn.h's window reads by FTRV for that
 * FIPR's float. a 0 in either vector marks a product that adds nothing.
 *
 * the asm can give such a product a lane of its own. there one vector holds 0,
 * and the other an FSCA float or an earlier FIPR's result. both are finite for
 * a finite vector, so the product adds nothing. an infinite component of the
 * vector makes an earlier result an infinity or a NaN. 0 times that is the NaN
 * turn.h names.
 *
 * below, sx and cx are the sine and cosine of x, and so for y and z */
#include "turn.h"

#include <stdint.h>

#ifdef DREAMCAST

typedef char turn_operand_words[(sizeof(float[3]) == 12 && sizeof(int32_t) == 4)
                                    ? 1
                                    : -1];
#else
#include "fpu.h"

/* FIPR of (a, b, c, d) and (e, f, g, h) */
#define FIPR(a, b, c, d, e, f, g, h)                                           \
    dc_fipr((const float[4]){a, b, c, d}, (const float[4]){e, f, g, h})
#endif

void zam_vec3_turn_x(int32_t ang, const float *in, float *out)
{
#ifdef DREAMCAST
    __asm__ volatile("lds %[ang],fpul\n\t"
                     "fmov.s @%[in]+,fr0\n\t"
                     "fmov.s @%[in]+,fr1\n\t"
                     "fsca fpul,dr6\n\t"
                     "fmov.s @%[in],fr2\n\t"
                     "fldi0 fr3\n\t"
                     "fldi0 fr8\n\t"
                     "fmov fr6,fr9\n\t"
                     "fmov fr7,fr10\n\t"
                     "fldi0 fr11\n\t"
                     "fldi0 fr4\n\t"
                     "fmov fr7,fr5\n\t"
                     "fneg fr6\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fldi1 fr4\n\t"
                     "fldi0 fr5\n\t"
                     "fldi0 fr6\n\t"
                     "fmov fr7,fr8\n\t"
                     "fipr fv0,fv4\n\t"
                     "add #12,%[out]\n\t"
                     "fmov.s fr11,@-%[out]\n\t"
                     "fmov.s fr8,@-%[out]\n\t"
                     "fmov.s fr7,@-%[out]"
                     : [in] "+&r"(in), [out] "+r"(out), "=m"(*(float (*)[3])out)
                     : [ang] "r"(ang), "m"(*(const float (*)[3])in)
                     : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7",
                       "fr8", "fr9", "fr10", "fr11", "fpul");
#else
    float x = in[0];
    float y = in[1];
    float z = in[2];
    dc_sincos_t turn = dc_fsca((uint32_t)ang);

    out[0] = FIPR(x, y, z, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F);
    out[1] = FIPR(x, y, z, 0.0F, 0.0F, turn.cos, -turn.sin, 0.0F);
    out[2] = FIPR(x, y, z, 0.0F, 0.0F, turn.sin, turn.cos, 0.0F);
#endif
}

void zam_vec3_turn_y(int32_t ang, const float *in, float *out)
{
#ifdef DREAMCAST
    __asm__ volatile("lds %[ang],fpul\n\t"
                     "fmov.s @%[in]+,fr0\n\t"
                     "fmov.s @%[in]+,fr1\n\t"
                     "fsca fpul,dr6\n\t"
                     "fmov.s @%[in],fr2\n\t"
                     "fldi0 fr3\n\t"
                     "fmov fr7,fr4\n\t"
                     "fldi0 fr5\n\t"
                     "fmov fr6,fr8\n\t"
                     "fneg fr8\n\t"
                     "fldi0 fr9\n\t"
                     "fmov fr7,fr10\n\t"
                     "fldi0 fr11\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fldi0 fr4\n\t"
                     "fldi1 fr5\n\t"
                     "fldi0 fr6\n\t"
                     "fmov fr7,fr8\n\t"
                     "fipr fv0,fv4\n\t"
                     "add #12,%[out]\n\t"
                     "fmov.s fr11,@-%[out]\n\t"
                     "fmov.s fr7,@-%[out]\n\t"
                     "fmov.s fr8,@-%[out]"
                     : [in] "+&r"(in), [out] "+r"(out), "=m"(*(float (*)[3])out)
                     : [ang] "r"(ang), "m"(*(const float (*)[3])in)
                     : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7",
                       "fr8", "fr9", "fr10", "fr11", "fpul");
#else
    float x = in[0];
    float y = in[1];
    float z = in[2];
    dc_sincos_t turn = dc_fsca((uint32_t)ang);

    out[0] = FIPR(x, y, z, 0.0F, turn.cos, 0.0F, turn.sin, 0.0F);
    out[1] = FIPR(x, y, z, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
    out[2] = FIPR(x, y, z, 0.0F, -turn.sin, 0.0F, turn.cos, 0.0F);
#endif
}

void zam_vec3_turn_z(int32_t ang, const float *in, float *out)
{
#ifdef DREAMCAST
    __asm__ volatile("lds %[ang],fpul\n\t"
                     "fmov.s @%[in]+,fr0\n\t"
                     "fmov.s @%[in]+,fr1\n\t"
                     "fsca fpul,dr8\n\t"
                     "fmov.s @%[in],fr2\n\t"
                     "fldi0 fr3\n\t"
                     "fmov fr9,fr4\n\t"
                     "fmov fr8,fr5\n\t"
                     "fneg fr5\n\t"
                     "fldi0 fr6\n\t"
                     "fldi0 fr7\n\t"
                     "fldi0 fr10\n\t"
                     "fldi0 fr11\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fldi0 fr4\n\t"
                     "fldi0 fr5\n\t"
                     "fldi1 fr6\n\t"
                     "fmov fr7,fr8\n\t"
                     "fipr fv0,fv4\n\t"
                     "add #12,%[out]\n\t"
                     "fmov.s fr7,@-%[out]\n\t"
                     "fmov.s fr11,@-%[out]\n\t"
                     "fmov.s fr8,@-%[out]"
                     : [in] "+&r"(in), [out] "+r"(out), "=m"(*(float (*)[3])out)
                     : [ang] "r"(ang), "m"(*(const float (*)[3])in)
                     : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7",
                       "fr8", "fr9", "fr10", "fr11", "fpul");
#else
    float x = in[0];
    float y = in[1];
    float z = in[2];
    dc_sincos_t turn = dc_fsca((uint32_t)ang);

    out[0] = FIPR(x, y, z, 0.0F, turn.cos, -turn.sin, 0.0F, 0.0F);
    out[1] = FIPR(x, y, z, 0.0F, turn.sin, turn.cos, 0.0F, 0.0F);
    out[2] = FIPR(x, y, z, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F);
#endif
}

/* on the unit matrix, the turn about z makes rows 0 and 1 (cz, sz, 0) and
 * (-sz, cz, 0). the turn about x then makes rows 1 and 2 (r1_0, r1_1, sx) and
 * (r2_0, r2_1, cx), from its factors (0, cx, sx) and (0, -sx, cx). the turn
 * about y then makes rows 0 and 2 (e0, e1, e2) and (f0, f1, f2), from its
 * factors (cy, 0, -sy) and (sy, 0, cy).
 *
 * the asm holds columns 0 and 1 in FV4 and FV8, and FV0 takes each FIPR's first
 * vector. a FIPR writes its result over its second vector's fourth float. so
 * each result moves into its column before the next FIPR. e0 waits in FPUL, and
 * e1 in kept, a general register, while the f FIPRs read the columns. column 2
 * comes last, from FSCAs run again */
void zam_vec3_turn_yxz(int32_t x, int32_t y, int32_t z, const float *in,
                       float *out)
{
#ifdef DREAMCAST
    int32_t kept;

    __asm__ volatile("lds %[z],fpul\n\t"
                     "fsca fpul,dr8\n\t"
                     "lds %[x],fpul\n\t"
                     "fsca fpul,dr6\n\t"
                     "fmov fr9,fr4\n\t"
                     "fmov fr8,fr5\n\t"
                     "fneg fr5\n\t"
                     "fldi0 fr10\n\t"
                     "fldi0 fr11\n\t"
                     "fmov fr6,fr1\n\t"
                     "fneg fr1\n\t"
                     "fldi0 fr0\n\t"
                     "fldi0 fr2\n\t"
                     "fldi0 fr3\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fsca fpul,dr0\n\t"
                     "fldi0 fr0\n\t"
                     "fmov fr7,fr6\n\t"
                     "fmov fr11,fr10\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "lds %[y],fpul\n\t"
                     "fsca fpul,dr2\n\t"
                     "fmov fr3,fr0\n\t"
                     "fneg fr2\n\t"
                     "fldi0 fr1\n\t"
                     "fldi0 fr3\n\t"
                     "fmov fr7,fr5\n\t"
                     "fmov fr11,fr9\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fsca fpul,dr2\n\t"
                     "fmov fr2,fr0\n\t"
                     "fmov fr3,fr2\n\t"
                     "fldi0 fr3\n\t"
                     "flds fr11,fpul\n\t"
                     "sts fpul,%[kept]\n\t"
                     "flds fr7,fpul\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fsts fpul,fr4\n\t"
                     "lds %[kept],fpul\n\t"
                     "fsts fpul,fr8\n\t"
                     "fmov.s @%[in]+,fr0\n\t"
                     "fmov.s @%[in]+,fr1\n\t"
                     "fmov.s @%[in],fr3\n\t"
                     "fldi0 fr2\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fmov.s fr7,@%[out]\n\t"
                     "add #4,%[out]\n\t"
                     "fmov.s fr11,@%[out]\n\t"
                     "add #4,%[out]\n\t"
                     "lds %[x],fpul\n\t"
                     "fsca fpul,dr8\n\t"
                     "fmov fr9,fr4\n\t"
                     "fmov fr8,fr5\n\t"
                     "fmov fr9,fr6\n\t"
                     "lds %[y],fpul\n\t"
                     "fsca fpul,dr10\n\t"
                     "flds fr11,fpul\n\t"
                     "fmov fr10,fr8\n\t"
                     "fneg fr8\n\t"
                     "fldi0 fr9\n\t"
                     "fldi0 fr10\n\t"
                     "fldi0 fr11\n\t"
                     "fipr fv8,fv4\n\t"
                     "fmov fr7,fr4\n\t"
                     "fldi0 fr8\n\t"
                     "fsts fpul,fr10\n\t"
                     "fipr fv8,fv4\n\t"
                     "fipr fv0,fv4\n\t"
                     "fmov.s fr7,@%[out]"
                     : [in] "+&r"(in), [out] "+&r"(out), [kept] "=&r"(kept),
                       "=m"(*(float (*)[3])out)
                     : [x] "r"(x), [y] "r"(y), [z] "r"(z),
                       "m"(*(const float (*)[3])in)
                     : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7",
                       "fr8", "fr9", "fr10", "fr11", "fpul");
#else
    float vx = in[0];
    float vy = in[1];
    float vz = in[2];
    dc_sincos_t rz = dc_fsca((uint32_t)z);
    dc_sincos_t rx = dc_fsca((uint32_t)x);
    dc_sincos_t ry = dc_fsca((uint32_t)y);
    float r1_0 = FIPR(0.0F, rx.cos, 0.0F, 0.0F, rz.cos, -rz.sin, 0.0F, 0.0F);
    float r1_1 = FIPR(0.0F, rx.cos, 0.0F, 0.0F, rz.sin, rz.cos, 0.0F, 0.0F);
    float r2_0 = FIPR(0.0F, -rx.sin, 0.0F, 0.0F, rz.cos, -rz.sin, 0.0F, 0.0F);
    float r2_1 = FIPR(0.0F, -rx.sin, 0.0F, 0.0F, rz.sin, rz.cos, 0.0F, 0.0F);
    float e0 = FIPR(ry.cos, 0.0F, -ry.sin, 0.0F, rz.cos, r1_0, r2_0, 0.0F);
    float e1 = FIPR(ry.cos, 0.0F, -ry.sin, 0.0F, rz.sin, r1_1, r2_1, 0.0F);
    float e2 = FIPR(ry.cos, 0.0F, -ry.sin, 0.0F, 0.0F, rx.sin, rx.cos, 0.0F);
    float f0 = FIPR(ry.sin, 0.0F, ry.cos, 0.0F, rz.cos, r1_0, r2_0, 0.0F);
    float f1 = FIPR(ry.sin, 0.0F, ry.cos, 0.0F, rz.sin, r1_1, r2_1, 0.0F);
    float f2 = FIPR(ry.sin, 0.0F, ry.cos, 0.0F, 0.0F, rx.sin, rx.cos, 0.0F);

    out[0] = FIPR(vx, vy, vz, 0.0F, e0, r1_0, f0, 0.0F);
    out[1] = FIPR(vx, vy, vz, 0.0F, e1, r1_1, f1, 0.0F);
    out[2] = FIPR(vx, vy, vz, 0.0F, e2, rx.sin, f2, 0.0F);
#endif
}

/* on the unit matrix, the turn about y makes rows 0 and 2 (cy, 0, -sy) and
 * (sy, 0, cy). the turn about x then makes rows 1 and 2 (r1_0, cx, r1_2) and
 * (r2_0, -sx, r2_2), from its factors (0, cx, sx) and (0, -sx, cx). the turn
 * about z then makes rows 0 and 1 (p0, p1, p2) and (q0, q1, q2), from its
 * factors (cz, sz, 0) and (-sz, cz, 0).
 *
 * the asm works as zam_vec3_turn_yxz's does. it holds columns 0 and 2 in FV4
 * and FV8, and comes to column 1 last */
void zam_vec3_turn_zxy(int32_t x, int32_t y, int32_t z, const float *in,
                       float *out)
{
#ifdef DREAMCAST
    int32_t kept;

    __asm__ volatile("lds %[y],fpul\n\t"
                     "fsca fpul,dr4\n\t"
                     "fmov fr4,fr6\n\t"
                     "fmov fr5,fr4\n\t"
                     "fldi0 fr7\n\t"
                     "fmov fr6,fr8\n\t"
                     "fneg fr8\n\t"
                     "fmov fr4,fr10\n\t"
                     "fldi0 fr9\n\t"
                     "fldi0 fr11\n\t"
                     "lds %[x],fpul\n\t"
                     "fsca fpul,dr2\n\t"
                     "fldi0 fr0\n\t"
                     "fldi0 fr1\n\t"
                     "fldi0 fr3\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fsca fpul,dr2\n\t"
                     "fmov fr3,fr2\n\t"
                     "fldi0 fr3\n\t"
                     "fmov fr7,fr5\n\t"
                     "fmov fr11,fr9\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "lds %[z],fpul\n\t"
                     "fsca fpul,dr0\n\t"
                     "fmov fr0,fr2\n\t"
                     "fmov fr1,fr0\n\t"
                     "fmov fr2,fr1\n\t"
                     "fldi0 fr2\n\t"
                     "fldi0 fr3\n\t"
                     "fmov fr7,fr6\n\t"
                     "fmov fr11,fr10\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fmov fr0,fr2\n\t"
                     "fmov fr1,fr0\n\t"
                     "fneg fr0\n\t"
                     "fmov fr2,fr1\n\t"
                     "fldi0 fr2\n\t"
                     "flds fr11,fpul\n\t"
                     "sts fpul,%[kept]\n\t"
                     "flds fr7,fpul\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fsts fpul,fr4\n\t"
                     "lds %[kept],fpul\n\t"
                     "fsts fpul,fr8\n\t"
                     "fmov.s @%[in]+,fr0\n\t"
                     "fmov.s @%[in]+,fr3\n\t"
                     "fmov.s @%[in],fr2\n\t"
                     "fldi0 fr1\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fmov.s fr7,@%[out]\n\t"
                     "add #8,%[out]\n\t"
                     "fmov.s fr11,@%[out]\n\t"
                     "add #-4,%[out]\n\t"
                     "lds %[x],fpul\n\t"
                     "fsca fpul,dr8\n\t"
                     "fmov fr9,fr5\n\t"
                     "fmov fr8,fr6\n\t"
                     "fneg fr6\n\t"
                     "lds %[z],fpul\n\t"
                     "fsca fpul,dr10\n\t"
                     "flds fr11,fpul\n\t"
                     "fmov fr10,fr9\n\t"
                     "fldi0 fr8\n\t"
                     "fldi0 fr10\n\t"
                     "fldi0 fr11\n\t"
                     "fipr fv8,fv4\n\t"
                     "fmov fr7,fr4\n\t"
                     "fsts fpul,fr9\n\t"
                     "fipr fv8,fv4\n\t"
                     "fipr fv0,fv4\n\t"
                     "fmov.s fr7,@%[out]"
                     : [in] "+&r"(in), [out] "+&r"(out), [kept] "=&r"(kept),
                       "=m"(*(float (*)[3])out)
                     : [x] "r"(x), [y] "r"(y), [z] "r"(z),
                       "m"(*(const float (*)[3])in)
                     : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7",
                       "fr8", "fr9", "fr10", "fr11", "fpul");
#else
    float vx = in[0];
    float vy = in[1];
    float vz = in[2];
    dc_sincos_t ry = dc_fsca((uint32_t)y);
    dc_sincos_t rx = dc_fsca((uint32_t)x);
    dc_sincos_t rz = dc_fsca((uint32_t)z);
    float r1_0 = FIPR(0.0F, rx.cos, rx.sin, 0.0F, ry.cos, 0.0F, ry.sin, 0.0F);
    float r1_2 = FIPR(0.0F, rx.cos, rx.sin, 0.0F, -ry.sin, 0.0F, ry.cos, 0.0F);
    float r2_0 = FIPR(0.0F, -rx.sin, rx.cos, 0.0F, ry.cos, 0.0F, ry.sin, 0.0F);
    float r2_2 = FIPR(0.0F, -rx.sin, rx.cos, 0.0F, -ry.sin, 0.0F, ry.cos, 0.0F);
    float p0 = FIPR(rz.cos, rz.sin, 0.0F, 0.0F, ry.cos, r1_0, r2_0, 0.0F);
    float p1 = FIPR(rz.cos, rz.sin, 0.0F, 0.0F, 0.0F, rx.cos, -rx.sin, 0.0F);
    float p2 = FIPR(rz.cos, rz.sin, 0.0F, 0.0F, -ry.sin, r1_2, r2_2, 0.0F);
    float q0 = FIPR(-rz.sin, rz.cos, 0.0F, 0.0F, ry.cos, r1_0, r2_0, 0.0F);
    float q1 = FIPR(-rz.sin, rz.cos, 0.0F, 0.0F, 0.0F, rx.cos, -rx.sin, 0.0F);
    float q2 = FIPR(-rz.sin, rz.cos, 0.0F, 0.0F, -ry.sin, r1_2, r2_2, 0.0F);

    out[0] = FIPR(vx, vy, vz, 0.0F, p0, q0, r2_0, 0.0F);
    out[1] = FIPR(vx, vy, vz, 0.0F, p1, q1, -rx.sin, 0.0F);
    out[2] = FIPR(vx, vy, vz, 0.0F, p2, q2, r2_2, 0.0F);
#endif
}

void zam_vec3_place_y(const float *pos, int32_t ang, const float *offset,
                      float *out)
{
#ifdef DREAMCAST
    __asm__ volatile(
        "lds %[ang],fpul\n\t"
        "fmov.s @%[offset]+,fr0\n\t"
        "fmov.s @%[offset]+,fr1\n\t"
        "fsca fpul,dr8\n\t"
        "fmov.s @%[offset],fr2\n\t"
        "fldi1 fr3\n\t"
        "fmov.s @%[pos]+,fr7\n\t"
        "fmov fr9,fr4\n\t"
        "fldi0 fr5\n\t"
        "fmov fr8,fr6\n\t"
        "add #4,%[pos]\n\t"
        "fmov.s @%[pos],fr11\n\t"
        "fmov fr9,fr10\n\t"
        "fneg fr8\n\t"
        "fldi0 fr9\n\t"
        "fipr fv0,fv4\n\t"
        "fipr fv0,fv8\n\t"
        "add #-4,%[pos]\n\t"
        "fldi0 fr4\n\t"
        "fldi1 fr5\n\t"
        "fldi0 fr6\n\t"
        "fmov fr7,fr8\n\t"
        "fmov.s @%[pos],fr7\n\t"
        "fipr fv0,fv4\n\t"
        "add #12,%[out]\n\t"
        "fmov.s fr11,@-%[out]\n\t"
        "fmov.s fr7,@-%[out]\n\t"
        "fmov.s fr8,@-%[out]"
        : [pos] "+&r"(pos), [offset] "+&r"(offset), [out] "+r"(out),
          "=m"(*(float (*)[3])out)
        : [ang] "r"(ang), "m"(*(const float (*)[3])pos),
          "m"(*(const float (*)[3])offset)
        : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8", "fr9",
          "fr10", "fr11", "fpul");
#else
    float x = offset[0];
    float y = offset[1];
    float z = offset[2];
    dc_sincos_t turn = dc_fsca((uint32_t)ang);

    out[0] = FIPR(x, y, z, 1.0F, turn.cos, 0.0F, turn.sin, pos[0]);
    out[1] = FIPR(x, y, z, 1.0F, 0.0F, 1.0F, 0.0F, pos[1]);
    out[2] = FIPR(x, y, z, 1.0F, -turn.sin, 0.0F, turn.cos, pos[2]);
#endif
}

/* the move puts pos in row 3. each turn's factor vectors hold 0 as their fourth
 * float, so no turn changes row 3. on the unit matrix, the turn about z makes
 * columns 0 and 1 (cz, -sz, 0) and (sz, cz, 0). the turn about y then makes
 * rows 0 and 2 (a0, a1, -sy) and (b0, b1, cy). the turn about x then makes rows
 * 1 and 2 (c0, c1, c2) and (d0, d1, d2).
 *
 * the asm works out[2] first, from column 2, and keeps it in kept, a general
 * register. FV4 and FV8 then hold columns 0 and 1, and FV0 each turn's factors.
 * a FIPR writes its result over its second vector's fourth float. so each
 * result waits in FR3 or FPUL while the turn's other FIPRs read the column */
void zam_vec3_place_xyz(const float *pos, int32_t x, int32_t y, int32_t z,
                        const float *offset, float *out)
{
#ifdef DREAMCAST
    int32_t kept;

    __asm__ volatile("lds %[y],fpul\n\t"
                     "fsca fpul,dr4\n\t"
                     "lds %[x],fpul\n\t"
                     "fsca fpul,dr2\n\t"
                     "fmov fr3,fr1\n\t"
                     "fldi0 fr0\n\t"
                     "fldi0 fr3\n\t"
                     "fmov fr5,fr6\n\t"
                     "fneg fr4\n\t"
                     "fldi0 fr5\n\t"
                     "fldi0 fr7\n\t"
                     "fipr fv0,fv4\n\t"
                     "fmov fr2,fr9\n\t"
                     "fneg fr9\n\t"
                     "fmov fr1,fr10\n\t"
                     "fldi0 fr8\n\t"
                     "fldi0 fr11\n\t"
                     "fmov fr7,fr3\n\t"
                     "fipr fv8,fv4\n\t"
                     "fmov fr3,fr5\n\t"
                     "fmov fr7,fr6\n\t"
                     "add #8,%[pos]\n\t"
                     "fmov.s @%[pos],fr7\n\t"
                     "add #-8,%[pos]\n\t"
                     "fmov.s @%[offset]+,fr0\n\t"
                     "fmov.s @%[offset]+,fr1\n\t"
                     "fmov.s @%[offset],fr2\n\t"
                     "fldi1 fr3\n\t"
                     "fipr fv0,fv4\n\t"
                     "flds fr7,fpul\n\t"
                     "sts fpul,%[kept]\n\t"
                     "lds %[z],fpul\n\t"
                     "fsca fpul,dr8\n\t"
                     "fmov fr9,fr4\n\t"
                     "fmov fr8,fr5\n\t"
                     "fneg fr5\n\t"
                     "fldi0 fr6\n\t"
                     "fldi0 fr7\n\t"
                     "fldi0 fr10\n\t"
                     "fldi0 fr11\n\t"
                     "lds %[y],fpul\n\t"
                     "fsca fpul,dr0\n\t"
                     "fmov fr0,fr2\n\t"
                     "fneg fr2\n\t"
                     "fmov fr1,fr0\n\t"
                     "fldi0 fr1\n\t"
                     "fldi0 fr3\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fmov fr0,fr3\n\t"
                     "fmov fr2,fr0\n\t"
                     "fneg fr0\n\t"
                     "fmov fr3,fr2\n\t"
                     "fmov fr7,fr3\n\t"
                     "fldi0 fr7\n\t"
                     "flds fr11,fpul\n\t"
                     "fldi0 fr11\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fmov fr3,fr4\n\t"
                     "fmov fr7,fr6\n\t"
                     "fsts fpul,fr8\n\t"
                     "fmov fr11,fr10\n\t"
                     "lds %[x],fpul\n\t"
                     "fsca fpul,dr2\n\t"
                     "fmov fr3,fr1\n\t"
                     "fldi0 fr0\n\t"
                     "fldi0 fr3\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fmov fr1,fr3\n\t"
                     "fmov fr2,fr1\n\t"
                     "fneg fr1\n\t"
                     "fmov fr3,fr2\n\t"
                     "fmov fr7,fr3\n\t"
                     "fldi0 fr7\n\t"
                     "flds fr11,fpul\n\t"
                     "fldi0 fr11\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fmov fr3,fr5\n\t"
                     "fmov fr7,fr6\n\t"
                     "fsts fpul,fr9\n\t"
                     "fmov fr11,fr10\n\t"
                     "fmov.s @%[pos]+,fr7\n\t"
                     "fmov.s @%[pos],fr11\n\t"
                     "add #-8,%[offset]\n\t"
                     "fmov.s @%[offset]+,fr0\n\t"
                     "fmov.s @%[offset]+,fr1\n\t"
                     "fmov.s @%[offset],fr2\n\t"
                     "fldi1 fr3\n\t"
                     "fipr fv0,fv4\n\t"
                     "fipr fv0,fv8\n\t"
                     "fmov.s fr7,@%[out]\n\t"
                     "add #4,%[out]\n\t"
                     "fmov.s fr11,@%[out]\n\t"
                     "add #4,%[out]\n\t"
                     "mov.l %[kept],@%[out]"
                     : [pos] "+&r"(pos), [offset] "+&r"(offset),
                       [out] "+r"(out), [kept] "=&r"(kept)
                     : [x] "r"(x), [y] "r"(y), [z] "r"(z)
                     : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7",
                       "fr8", "fr9", "fr10", "fr11", "fpul", "memory");
#else
    float vx = offset[0];
    float vy = offset[1];
    float vz = offset[2];
    dc_sincos_t rz = dc_fsca((uint32_t)z);
    dc_sincos_t ry = dc_fsca((uint32_t)y);
    dc_sincos_t rx = dc_fsca((uint32_t)x);
    float a0 = FIPR(ry.cos, 0.0F, -ry.sin, 0.0F, rz.cos, -rz.sin, 0.0F, 0.0F);
    float a1 = FIPR(ry.cos, 0.0F, -ry.sin, 0.0F, rz.sin, rz.cos, 0.0F, 0.0F);
    float b0 = FIPR(ry.sin, 0.0F, ry.cos, 0.0F, rz.cos, -rz.sin, 0.0F, 0.0F);
    float b1 = FIPR(ry.sin, 0.0F, ry.cos, 0.0F, rz.sin, rz.cos, 0.0F, 0.0F);
    float c0 = FIPR(0.0F, rx.cos, rx.sin, 0.0F, a0, -rz.sin, b0, 0.0F);
    float c1 = FIPR(0.0F, rx.cos, rx.sin, 0.0F, a1, rz.cos, b1, 0.0F);
    float c2 = FIPR(0.0F, rx.cos, rx.sin, 0.0F, -ry.sin, 0.0F, ry.cos, 0.0F);
    float d0 = FIPR(0.0F, -rx.sin, rx.cos, 0.0F, a0, -rz.sin, b0, 0.0F);
    float d1 = FIPR(0.0F, -rx.sin, rx.cos, 0.0F, a1, rz.cos, b1, 0.0F);
    float d2 = FIPR(0.0F, -rx.sin, rx.cos, 0.0F, -ry.sin, 0.0F, ry.cos, 0.0F);
    float px = pos[0];
    float py = pos[1];
    float pz = pos[2];

    out[0] = FIPR(vx, vy, vz, 1.0F, a0, c0, d0, px);
    out[1] = FIPR(vx, vy, vz, 1.0F, a1, c1, d1, py);
    out[2] = FIPR(vx, vy, vz, 1.0F, -ry.sin, c2, d2, pz);
#endif
}
