#include <float_bits.h>
#include <mathf.h>

float _in_atan2s(float y, float x)
{
    float angle;

    angle = _in_atans(y / x);
    /* @hack type: x's and y's sign bits read through their addresses, where
     * union copies taken after the call would do */
    if (*(unsigned long *)&x & FLOAT_SIGN) {
        if (*(unsigned long *)&y & FLOAT_SIGN) {
            angle = angle + _NPAIS;
        } else {
            angle = angle + _PAIS;
        }
    }
    return angle;
}
