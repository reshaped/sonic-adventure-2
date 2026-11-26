#include <float_bits.h>
#include <mathf.h>

float _in_coss(float x)
{
    float result;
    unsigned int octant;
    int kind = classify(x);

    if (kind & (CLASS_NAN | CLASS_INFINITE)) {
        return _IFNANS;
    }
    if (kind & CLASS_NEGATIVE) {
        x = -x;
    }
    x = _in_fmods(x, _QUOPAIS, &octant);
    /* @bug logic: returns 1 for a remainder above pi/4, whatever the octant.
     * for x = 3365.4312 (H'455256E6), rounding toward zero gives octant
     * 4284 and remainder H'3F491000, just above pi/4. rounding to nearest
     * gives octant 4285 and zero remainder (FCMP/GT FR3,FR4 at 0x8c0cc376) */
    if (x > _QUOPAIS) {
        return _IFONES;
    }
    octant &= 7;
    if (octant & 1) {
        x = _NQUOPAIS + x;
    }
    switch (octant) {
    case 0:
    case 7:
        result = _app_coss(x);
        break;
    case 1:
    case 2:
        /* @hack spelling: the series' value stored in x and negated into
         * result, here and for octants 3 and 4, where result = -_app_sins(x)
         * would do */
        x = _app_sins(x);
        result = -x;
        break;
    case 3:
    case 4:
        x = _app_coss(x);
        result = -x;
        break;
    case 5:
    case 6:
        result = _app_sins(x);
        break;
    }
    return result;
}
