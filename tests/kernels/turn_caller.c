#include "turn.h"

#include <stdint.h>

/* Keep the four observed words live through the turn. The compiler can also
 * prove that the first angle and the writable pointer enter with equal bits. */
__attribute__((flatten)) void turn_with_live_inputs(const float *in, float *out,
                                                    volatile uint32_t *words,
                                                    int zxy)
{
    uint32_t a = words[0];
    uint32_t b = words[1];
    uint32_t c = words[2];
    uint32_t d = words[3];
    int32_t x = (int32_t)(uintptr_t)out;
    if (zxy) {
        zam_vec3_turn_zxy(x, (int32_t)a, (int32_t)b, in, out);
    } else {
        zam_vec3_turn_yxz(x, (int32_t)a, (int32_t)b, in, out);
    }
    words[0] = d;
    words[1] = a;
    words[2] = b;
    words[3] = c;
}
