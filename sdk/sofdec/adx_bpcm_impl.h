/* a PCM format's decode step, which a source defines by including this after
 * the macros it reads. PCM_FUNCTION names the step, and PCM_INPUT is an input
 * sample's type. PCM_LEFT, PCM_RIGHT and PCM_MONO give a channel's sample i
 * as a 16-bit output sample. PCM_BYTES is an input sample's size. where
 * PCM_BIG_ENDIAN is defined, the step swaps each output sample's bytes */
#include <adx_xpnd.h>

#ifdef PCM_BIG_ENDIAN
#define PCM_STORE(destination, value)                                          \
    {                                                                          \
        sample = (value);                                                      \
        (destination) = (sample << 8) | (sample >> 8);                         \
    }
#else
#define PCM_STORE(destination, value) ((destination) = (value))
#endif

PCM_FUNCTION(ADXB adxb)
{
    ADXB_DEC *step = &adxb->dec;
    PCM_INPUT *input = (PCM_INPUT *)step->ibuf;
    Sint16 *buffer;
    Sint16 *left;
    Sint16 *right;
    Sint32 samples;
    Sint32 i;
#ifdef PCM_BIG_ENDIAN
    Uint16 sample;
#endif

    if (adxb->stat == ADXB_STAT_PREP) {
        if (ADXPD_GetStat(adxb->xpd) == ADXPD_STAT_STOP) {
            adxb->get_wr(adxb->get_obj, &step->wpos, &step->nroom,
                         &step->nleft);
            samples = step->obufsize - step->wpos;
            if (samples > step->nroom) {
                samples = step->nroom;
            }
            if (samples > step->nblk) {
                samples = step->nblk;
            }
            buffer = step->obuf;
            left = buffer + step->wpos;
            if (adxb->nch == 2) {
                right = buffer + (step->obufdist + step->wpos);
                for (i = 0; i < samples; i++) {
                    PCM_STORE(left[i], PCM_LEFT(input, i));
                    PCM_STORE(right[i], PCM_RIGHT(input, i));
                }
            } else {
                for (i = 0; i < samples; i++) {
                    PCM_STORE(left[i], PCM_MONO(input, i));
                }
            }
            adxb->dec_nsmpl = samples;
            adxb->dec_dtlen = samples * adxb->nch * PCM_BYTES;
            adxb->stat = ADXB_STAT_EXEC;
        }
    }
    if (adxb->stat == ADXB_STAT_EXEC) {
        adxb->add_wr(adxb->add_obj, adxb->dec_dtlen, adxb->dec_nsmpl);
        adxb->stat = ADXB_STAT_END;
    }
}
#undef PCM_STORE
#undef PCM_BIG_ENDIAN
#undef PCM_FUNCTION
#undef PCM_INPUT
#undef PCM_BYTES
#undef PCM_LEFT
#undef PCM_RIGHT
#undef PCM_MONO
