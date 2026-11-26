/* set adxb up for a PCM format, once a header reader has read its channels
 * and block sizes. it clears the prediction coefficients and the loop, and
 * points the decode step's output at adxb's PCM buffer */
/* @hack spelling: the assignments as a macro each header reader expands,
 * where a function they all call would do */
#define ADXB_SETUP_PCM(adxb, format_id)                                        \
    do {                                                                       \
        (adxb)->cof = 0;                                                       \
        (adxb)->k1 = 0;                                                        \
        (adxb)->k0 = 0;                                                        \
        (adxb)->lp_type = 0;                                                   \
        (adxb)->nloop = 0;                                                     \
        (adxb)->lp_end_ofst = 0;                                               \
        (adxb)->lp_end_pos = 0;                                                \
        (adxb)->lp_start_ofst = 0;                                             \
        (adxb)->lp_start_pos = 0;                                              \
        (adxb)->lp_ins_nsmpl = 0;                                              \
        (adxb)->dec.nch = (adxb)->nch;                                         \
        (adxb)->dec.blklen = (adxb)->blklen;                                   \
        (adxb)->dec.blksmpl = (adxb)->blksmpl;                                 \
        (adxb)->dec.obuf = (adxb)->pcmbuf;                                     \
        (adxb)->dec.obufsize = (adxb)->pcmbsize;                               \
        (adxb)->dec.obufdist = (adxb)->pcmbdist;                               \
        (adxb)->dec.coef[0] = (adxb)->k0;                                      \
        (adxb)->dec.coef[1] = (adxb)->k1;                                      \
        (adxb)->dec.dly = (adxb)->dly;                                         \
        (adxb)->wpos = 0;                                                      \
        (adxb)->nsmpl = 0;                                                     \
        (adxb)->format = (format_id);                                          \
    } while (0)
