#include <float_bits.h>
#include <mathf.h>

float _in_pows(float x, float y)
{
    int x_kind = classify(x);
    int y_kind = classify(y);
    float half;

    /* @hack redundant: an else after a return, here and in the zero x's
     * test */
    if ((x_kind | y_kind) & CLASS_NAN) {
        return _IFNANS;
    } else if (x_kind & CLASS_ZERO) {
        if (y_kind & (CLASS_ZERO | CLASS_NEGATIVE)) {
            return _IFNANS;
        } else {
            return _IFZEROS;
        }
    }
    if (y_kind & CLASS_ZERO) {
        return _IFONES;
    }
    /* @hack redundant: an else after each return below */
    if (x_kind & CLASS_NEGATIVE) {
        if (y == truncated(y) && !(y_kind & CLASS_INFINITE)) {
            /* @hack spelling: x negated in a statement of its own, where
             * _in_logs(-x) would do */
            x = -x;
            x = _in_exps(_in_logs(x) * y);
            half = _in_ldexps(y, -1);
            if (half != truncated(half)) {
                x = -x;
            }
            return x;
        } else {
            return _IFNANS;
        }
    } else {
        return _in_exps(_in_logs(x) * y);
    }
}
