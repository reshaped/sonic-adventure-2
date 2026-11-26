#include <float_bits.h>
#include <mathf.h>

/* atan's series, by its coefficients of x^11, x^9 and on down to x. each is
 * the float nearest the term beside it, or as far off as its row says */
static const float coefficients[6] = {
    -0.09090788F, /* -1/11, 163 ulps nearer 0 */
    0.11111111F, /* 1/9 */
    -0.14285715F, /* -1/7 */
    0.2F, /* 1/5 */
    -0.33333334F, /* -1/3 */
    1.0F,
};

float _app_atans(float x)
{
    return x * polynomial(x * x, coefficients, 5);
}
