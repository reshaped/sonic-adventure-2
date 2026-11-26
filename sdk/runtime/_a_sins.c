#include <float_bits.h>
#include <mathf.h>

/* sin's series, by its coefficients of x^13, x^11 and on down to x. each is
 * the float nearest the term beside it, or as far off as its row says */
static const float coefficients[7] = {
    1.6058363e-10F, /* 1/13!, 491 ulps below */
    -2.5052104e-08F, /* -1/11!, 2 ulps nearer 0 */
    2.755732e-06F, /* 1/9!, an ulp above */
    -0.00019841272F, /* -1/7!, an ulp further from 0 */
    0.008333334F, /* 1/5! */
    -0.16666669F, /* -1/3!, an ulp further from 0 */
    1.0F,
};

float _app_sins(float x)
{
    return x * polynomial(x * x, coefficients, 6);
}
