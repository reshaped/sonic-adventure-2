#include <float_bits.h>
#include <mathf.h>

float _in_modfs(float value, float *iptr)
{
    float fraction;
    int kind;

    *iptr = truncated(value);
    if (*iptr == value) {
        fraction = _IFZEROS;
        /* @hack spelling: classify's result stored in kind and tested there,
         * where a test of classify(value) itself would do */
        kind = classify(value);
        if (kind & CLASS_NEGATIVE) {
            fraction = -fraction;
        }
    } else {
        fraction = value - *iptr;
    }
    return fraction;
}
