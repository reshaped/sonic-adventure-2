/* C runtime double math, as far as the sources use it. these functions, and
 * the sources that call them, build with -fpu=single, under which a double is
 * a float */
#ifndef MATH_H
#define MATH_H

/* the C library's frexp. value must not be a denormal. a zero gives a *exp
 * of 0. an infinity sets errno to ERANGE, and a NaN sets it to EDOM. the call
 * returns either as it is, and leaves *exp as it was */
double frexp(double value, int *exp);
/* the C library's double functions. each returns the result of the _in_
 * routine of its name: cos returns _in_coss's. a NaN result sets errno to
 * EDOM, and an infinite one sets it to ERANGE */
double atan2(double y, double x);
double cos(double x);
double log10(double x);
#ifdef NONMATCHING
/* C99's INFINITY and isinf, by GCC's builtins: SH4ZAM's shz_complex.inl.h
 * reads both */
#define INFINITY __builtin_inff()
#define isinf(x) __builtin_isinf(x)
#endif

#endif
