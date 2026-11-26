#include <float_bits.h>
#include <mathf.h>

/* cos's series, by its coefficients of x^12, x^10 and on down to 1. each is
 * the float nearest the term beside it, or as far off as its row says */
static const float coefficients[7] = {
    2.087561e-09F, /* 1/12!, 516 ulps below */
    -2.7557314e-07F, /* -1/10!, 2 ulps nearer 0 */
    2.4801588e-05F, /* 1/8! */
    -0.001388889F, /* -1/6!, an ulp further from 0 */
    0.04166667F, /* 1/4!, an ulp above */
    -0.5F, /* -1/2! */
    1.0F,
};

float _app_coss(float x)
{
    return polynomial(x * x, coefficients, 6);
}
