/* kernels that stand in for a window. a window pushes the unit matrix, and may
 * move it by njTranslateV or njTranslateEx. it turns the matrix by one or more
 * of njRotateX, njRotateY and njRotateZ, each with a null matrix. it then
 * transforms one vector by njCalcVector, or one point by njCalcPoint, and pops.
 * a kernel does the same in registers, and leaves XMTRX and the matrix stack
 * alone. Each caller supplies three contiguous floats per vector, and out may
 * alias any input. PR and SZ must be clear. The kernels preserve PR, SZ and
 * FR and clobber FR0 to FR11 and FPUL. For finite input they give the
 * window's bits.
 *
 * a kernel gives those bits by running the window's FIPRs. each float that a
 * turn makes comes from the turn's FTRV. that float is a FIPR of the turn's
 * factors with a column of the matrix being turned. the multiplier recodes the
 * column's floats. a kernel runs the FIPR of the same factors with the same
 * column. a FIPR forms each product alone, aligns the products to the largest
 * and rounds their sum once. so which of the four lanes a product takes doesn't
 * change the bits. 0 times a finite float adds nothing. a product whose recoded
 * factor is 1 is the other factor. so a first turn's rows on the unit matrix
 * are FSCA's floats.
 *
 * where in has an infinite component, a zam_vec3_turn_ kernel can differ from
 * its window in one float alone. that float is the one of its last turn's axis.
 * it can be the NaN H'7FBFFFFF where the window's is an infinity. an infinite
 * component of pos makes each float of the window that NaN. zam_vec3_place_y
 * and zam_vec3_place_xyz add it to the float of its own axis alone.
 *
 * a window of a kernel's shape, with no statement but its calls and no branch,
 * takes one guard. its #ifdef NONMATCHING side is one call of the kernel. its
 * #else side is the window, from its push to its pop. the call spells the
 * window's own arguments, in the kernel's order. it passes each vector by its
 * first float's address, and the angles in the order x, y, z.
 * tools/windows.py --check holds each guarded window to this. a source includes
 * this header as "engine/dc/turn.h", under #ifdef NONMATCHING.
 *
 * an angle is a Ninja Angle, 65536 to a turn, and FSCA reads its low 16 bits.
 * njCalcVector here doesn't scale its result to unit length, since src/main.c
 * passes njInitMatrix a flag of 0. */
#ifndef DC_TURN_H
#define DC_TURN_H

#include <stdint.h>

/* in turned about one axis by ang: njRotateX, njRotateY or njRotateZ, then
 * njCalcVector */
void zam_vec3_turn_x(int32_t ang, const float *in, float *out);
void zam_vec3_turn_y(int32_t ang, const float *in, float *out);
void zam_vec3_turn_z(int32_t ang, const float *in, float *out);

/* in turned about y by y, then about x by x, then about z by z. the window
 * turns by njRotateZ, njRotateX and njRotateY in that order */
void zam_vec3_turn_yxz(int32_t x, int32_t y, int32_t z, const float *in,
                       float *out);
/* in turned about z by z, then about x by x, then about y by y. the window
 * turns by njRotateY, njRotateX and njRotateZ in that order */
void zam_vec3_turn_zxy(int32_t x, int32_t y, int32_t z, const float *in,
                       float *out);

/* pos plus offset turned about y by ang. the window calls njTranslateEx and
 * njRotateY on the unit matrix, then njCalcPoint */
void zam_vec3_place_y(const float *pos, int32_t ang, const float *offset,
                      float *out);

/* pos plus offset turned about x by x, then about y by y, then about z by z.
 * the window moves the unit matrix by pos. it turns it by njRotateZ, njRotateY
 * and njRotateX in that order, then reads by njCalcPoint. two infinite
 * components of offset that meet in out[2] can make it the other infinity than
 * the window's */
void zam_vec3_place_xyz(const float *pos, int32_t x, int32_t y, int32_t z,
                        const float *offset, float *out);

#endif
