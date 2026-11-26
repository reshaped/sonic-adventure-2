/* the AHX decoder's MPEG layer II synthesis coefficients */
#include <mpeg_synth_tables.h>

/* Ckk is cos(kk * pi / 64) to nine decimal places. a cosine in another
 * quadrant is one of these, or its negation */
#define C00 1.000000000F
#define C01 0.998795456F
#define C02 0.995184727F
#define C03 0.989176510F
#define C04 0.980785280F
#define C05 0.970031253F
#define C06 0.956940336F
#define C07 0.941544065F
#define C08 0.923879533F
#define C09 0.903989293F
#define C10 0.881921264F
#define C11 0.857728610F
#define C12 0.831469612F
#define C13 0.803207531F
#define C14 0.773010453F
#define C15 0.740951125F
#define C16 0.707106781F
#define C17 0.671558955F
#define C18 0.634393284F
#define C19 0.595699304F
#define C20 0.555570233F
#define C21 0.514102744F
#define C22 0.471396737F
#define C23 0.427555093F
#define C24 0.382683432F
#define C25 0.336889853F
#define C26 0.290284677F
#define C27 0.242980180F
#define C28 0.195090322F
#define C29 0.146730474F
#define C30 0.098017140F
#define C31 0.049067674F

/* row n of the matrix is at frequency f = n + 16. its entry for band k is
 * cos(f * (2 * k + 1) * pi / 64). MATRIX_BANDS writes bands 0 to 15 as a
 * sign s times the cosines a to p. bands 16 to 31 are those cosines
 * backward, times a sign r. r is s at an even f, and -s at an odd one */

#define MATRIX_BANDS(s, r, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)     \
    {(s) * (a), (s) * (b), (s) * (c), (s) * (d), (s) * (e), (s) * (f),         \
     (s) * (g), (s) * (h), (s) * (i), (s) * (j), (s) * (k), (s) * (l),         \
     (s) * (m), (s) * (n), (s) * (o), (s) * (p), (r) * (p), (r) * (o),         \
     (r) * (n), (r) * (m), (r) * (l), (r) * (k), (r) * (j), (r) * (i),         \
     (r) * (h), (r) * (g), (r) * (f), (r) * (e), (r) * (d), (r) * (c),         \
     (r) * (b), (r) * (a)}

/* rows 16 and 48, where f is 32 and 64, are all 0 and all -1 */
struct mpeg_matrix_table mpeg_matrix_store = {
#include "sofdec/mpeg_synth_tables/mpeg_matrix_store.inc"
};

/* row n's entry j is the synthesis window's tap n + 32 * j, to nine decimal
 * places. for n from 1 to 16, row 32 - n is row n backward and negated */

/* WEIGHT scales a tap to the window kernel's full scale of 2^31 */
#define WEIGHT(coefficient) ((coefficient) * 2147483648.0F)
#define WINDOW_TAPS(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)            \
    {WEIGHT(a), WEIGHT(b), WEIGHT(c), WEIGHT(d), WEIGHT(e), WEIGHT(f),         \
     WEIGHT(g), WEIGHT(h), WEIGHT(i), WEIGHT(j), WEIGHT(k), WEIGHT(l),         \
     WEIGHT(m), WEIGHT(n), WEIGHT(o), WEIGHT(p)}
#define REVERSE_TAPS(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)           \
    WINDOW_TAPS(-(p), -(o), -(n), -(m), -(l), -(k), -(j), -(i), -(h), -(g),    \
                -(f), -(e), -(d), -(c), -(b), -(a))

struct mpeg_window_table mpeg_window_store = {
#include "sofdec/mpeg_synth_tables/mpeg_window_store.inc"
};
