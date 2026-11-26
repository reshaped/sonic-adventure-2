#include <float_bits.h>
#include <mathf.h>

float _in_floors(float x)
{
    float rounded;

    rounded = truncated(x);
    /* @hack spelling: two nested tests, where one test with && would do */
    /* @hack type: x's sign bit read through its address, where a union copy
     * would do */
    if (*(int *)&x & FLOAT_SIGN) {
        if (x != rounded) {
            rounded = _NIFONES + rounded;
        }
    }
    return rounded;
}
