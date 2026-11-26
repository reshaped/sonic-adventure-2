#include <float_bits.h>
#include <mathf.h>

/* asin x is 2 atan t, where t is the tangent of half of asin x. for |x| below
 * a half, t is x / (1 + sqrt(1 - x * x)). otherwise t takes the equal form
 * (1 - sqrt(1 - x * x)) / x */
float _in_asins(float x)
{
    /* @hack type: kind as unsigned int, where int would do */
    unsigned int kind = classify(x);
    float magnitude;
    float result;

    if (kind & CLASS_NAN) {
        /* @hack spelling: _IFNANS stored in result and result returned, where
         * return _IFNANS would do */
        result = _IFNANS;
        return result;
    }
    magnitude = x;
    /* @hack type: magnitude's sign bit read through its address, where
     * magnitude as a union with an int would do */
    if (*(int *)&magnitude & FLOAT_SIGN) {
        magnitude = -magnitude;
    }
    if (magnitude > _IFONES) {
        result = _IFNANS;
    } else if (magnitude == _IFONES) {
        result = _HLFPAIS;
        if (kind & CLASS_NEGATIVE) {
            result = -result;
        }
    } else if (kind & CLASS_ZERO) {
        result = x;
    } else {
        if (_HALFS > magnitude) {
            result = x / (_IFONES + sqrtf(_IFONES - (x * x)));
        } else {
            result = (_IFONES - sqrtf(_IFONES - (x * x))) / x;
        }
        result = _in_ldexps(_in_atans(result), 1);
    }
    return result;
}
