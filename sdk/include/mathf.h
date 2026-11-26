/* C runtime float math, as far as the sources use it */
#ifndef MATHF_H
#define MATHF_H

#include <float_constants.h> /* IWYU pragma: export */

/* the C library's float functions. each returns the result of the _in_
 * routine of its name: acosf returns _in_acoss's. a NaN result sets errno to
 * EDOM, and an infinite one sets it to ERANGE */
float acosf(float x);
float asinf(float x);
float atan2f(float y, float x);
float atanf(float x);
float ceilf(float x);
float floorf(float x);
float logf(float x);
float modff(float value, float *iptr);
float powf(float x, float y);
/* sqrtf is SHC's float square root intrinsic, an FSQRT */
#ifdef DREAMCAST
#define sqrtf _builtin_sqrtf
#else
float sqrtf(float x);
#endif
#ifdef DREAMCAST
#ifdef NONMATCHING
#define _builtin_sqrtf __builtin_sqrtf
#include <zam.h>
#include <sh4zam/shz_scalar.h>

/* the least magnitude from which no float holds a fraction, 2^23 */
#define ZAM_FLOAT_WHOLE 0x1p23F
/* on this side, a C call of floorf or ceilf runs zam_floorf or zam_ceilf
 * inline. neither sets errno, where the runtime's routines set it for a NaN
 * or an infinite result. each returns the runtime's result for every x but a
 * NaN. for a NaN, zam_floorf and zam_ceilf return x. the runtime's floorf
 * adds -1 to a NaN whose sign is set. its ceilf adds 1 to one whose sign is
 * clear. each such sum is a NaN, which may hold other bits than x.
 *
 * a float of magnitude ZAM_FLOAT_WHOLE or more is its own floor and ceiling.
 * so is an infinity or a NaN. shz_floorf and shz_ceilf round a smaller x by
 * FTRC and FLOAT, as the runtime's routines do. FLOAT's zero is +0, but the
 * runtime's zero has x's sign: floorf(-0.0F) and ceilf(-0.5F) give -0. so a
 * zero result here is x * 0.0F. with FPSCR.DN set, the FPU takes a denormal
 * as a zero of its sign. so on both sides, a negative denormal floors to -0,
 * and a positive one ceils to +0. for a constant x, GCC folds the call to the
 * exact floor or ceiling */
ZAM_INLINE float zam_floorf(float x)
{
    float result;

    if (!(__builtin_fabsf(x) < ZAM_FLOAT_WHOLE)) {
        return x;
    }
    result = shz_floorf(x);
    return result == 0.0F ? x * 0.0F : result;
}
ZAM_ASSERT_COMPATIBLE(floorf);
#define floorf(x) zam_floorf(x)
ZAM_INLINE float zam_ceilf(float x)
{
    float result;

    if (!(__builtin_fabsf(x) < ZAM_FLOAT_WHOLE)) {
        return x;
    }
    result = shz_ceilf(x);
    return result == 0.0F ? x * 0.0F : result;
}
ZAM_ASSERT_COMPATIBLE(ceilf);
#define ceilf(x) zam_ceilf(x)
#endif
#endif

/* the _in_ routines, which set no errno */

/* the arc cosine of x: _HLFPAIS less _in_asins(x) */
float _in_acoss(float x);
/* the arc sine of x. an x above 1 in magnitude, or a NaN, gives a NaN. 1
 * and -1 give _HLFPAIS with x's sign, and a zero gives x */
float _in_asins(float x);
/* the arc tangent of x. a NaN gives a NaN, and a zero gives x. 1 and -1 give
 * _QUOPAIS with x's sign. an x of magnitude 2^32 or more, infinity among them,
 * gives _HLFPAIS with x's sign */
float _in_atans(float x);
/* _in_atans(y / x), plus _PAIS where x alone is negative, or plus _NPAIS
 * where both are. a -0 counts as negative */
float _in_atan2s(float y, float x);
/* the ceiling of x. a negative x above -1 gives -0. a NaN gives a NaN, and
 * an infinity comes back as it is */
float _in_ceils(float x);
/* the cosine of x. a NaN or an infinity gives a NaN. a reduced argument
 * above pi/4 returns 1. rounding toward zero takes this path for 3365.4312
 * (0x455256e6), whose cosine is about -0.7071 */
float _in_coss(float x);
/* e to the x. a NaN gives a NaN, and a zero gives 1. an x of magnitude 2^15
 * or more, infinity among them, gives infinity, or 0 where x is negative. a
 * result past the floats' range gives infinity. one below the smallest normal
 * float gives 0 */
float _in_exps(float x);
/* the floor of x. a NaN gives a NaN, and an infinity comes back as it is */
float _in_floors(float x);
/* the natural logarithm of x. a NaN or a negative x gives a NaN, and a zero
 * gives minus infinity. infinity gives infinity, and 1 gives 0 */
float _in_logs(float x);
/* the logarithm to base 10: _in_logs(x) times the float nearest 1 over the
 * natural logarithm of 10 */
float _in_log10s(float x);
/* value's fractional part. *iptr gets value truncated toward zero, with
 * value's sign, and the call returns value less *iptr. a whole value or an
 * infinity returns a zero of value's sign. a NaN stores and returns a NaN */
float _in_modfs(float value, float *iptr);
/* x to the y, as e to the y log x. a negative x gives a NaN unless y is whole
 * and finite. an odd y then negates the result */
float _in_pows(float x, float y);
/* x times 2^n, by adding n to x's exponent. a zero or a denormal x gives a
 * zero. an exponent pushed out of range gives an infinity, or a zero
 * for a negative n. these results keep x's sign. an infinity comes back as it
 * is, and a NaN gives _IFNANS */
float _in_ldexps(float x, int n);
/* the remainder of x by y: |x| less q times |y|, with x's sign bit ORed in.
 * q is the integer part of the floating-point division |x| / |y|. the
 * division and product both round in the current FPSCR mode.
 *
 * *quotient gets q modulo 2^32. FTRC overflows for a q of 2^31 exactly, and
 * for one of 2^32 + 2^31 or more. *quotient then holds other bits.
 *
 * an infinite or NaN x, or a zero or NaN y, returns a NaN. an infinite y
 * returns x. both store a quotient of 0 */
float _in_fmods(float x, float y, unsigned int *quotient);
/* _app_atans, _app_sins and _app_coss sum the series of atan, sin and cos to a
 * fixed term. they reduce nothing. _in_atans passes _app_atans an x below 1/8
 * in magnitude. _in_coss passes the other two its reduced x */
float _app_atans(float x);
float _app_sins(float x);
float _app_coss(float x);

/* The runtime reads these three bit patterns as floats. Their addresses
 * are the addresses of the library's exported constants. */
#define _IFNANS (_IFNANS.value)
#define _IFINFS (_IFINFS.value)
#define _NIFINFS (_NIFINFS.value)

#endif
