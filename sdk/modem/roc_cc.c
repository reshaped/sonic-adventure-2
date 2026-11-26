/* each of the pump's three tone detectors filters the line with a pair of
 * biquads. detector A's follow the country's profile, and B's and C's are
 * fixed */
#include <sg_xpt.h>
#include "ecdcutil.h"
#include "roc_cc.h"
#include "roc_k56.h"
#include "target.h"

struct biquad {
    Uint16 b0, b1, b2, a1, a2;
};
/* a profile of detector A: its two filters and its levels. on_ticks and
 * off_ticks are the tone and the gap of a cadence to keep, in 10 ms ticks.
 * an on_ticks of 0 keeps none */
struct tone_filter {
    struct biquad first, second;
    Uint16 scale, gain, threshold;
    Uint16 on_ticks, off_ticks;
};
/* the cadence detector A keeps. on_ticks and off_ticks are its tone's and
 * gap's lengths in 10 ms ticks, which start as the profile's. while measuring
 * is set, a tone or gap that ends replaces its length, SLACK_TICKS longer.
 * changed_at is the clock's reading at the tone's last change */
struct cadence_tracker {
    /* @hack type: char flags, where Uint8 would do */
    char tone, measuring;
    Uint32 on_ticks, off_ticks, changed_at;
};
#define SLACK_TICKS 3

/* the gains of 0 dB to 5 dB in steps of 1 dB. GAIN_ONE is a gain of 1 */
static const Uint16 gain_steps[] = {10000, 11220, 12589, 14125, 15848, 17782};
#define GAIN_ONE 10000
/* the filters dp_set_call_progress_tone_detector tunes to 245 Hz */
#define DEFAULT_FIRST {0x08c2, 0xee7c, 0x08c2, 0xc774, 0x74fe}
#define DEFAULT_SECOND {0x08c2, 0x1184, 0x08c2, 0xcd4f, 0x6495}
/* a profile's scale, then the gain and threshold every profile takes */
#define LEVEL(scale) scale, 31536, 1231
#define DEFAULT_PROFILE(scale)                                                 \
    {DEFAULT_FIRST, DEFAULT_SECOND, LEVEL(scale), 0, 0}

/* the dial filter and then the handshake filter of each pair select_filter
 * picks from */
/* clang-format off */
static const struct tone_filter profiles[] = {
    DEFAULT_PROFILE(1280),
    DEFAULT_PROFILE(512),
    {{0x14e2, 0xcfe8, 0x1c3d, 0xd0ab, 0x69f2},
     {0x1bad, 0xe51c, 0x1c3d, 0xd0ab, 0x6583}, LEVEL(3328), 0, 0},
    {{0x081c, 0xeeff, 0x08fc, 0xc505, 0x742f},
     {0x079b, 0xf904, 0x08fc, 0xc63e, 0x6e34}, LEVEL(512), 0, 0},
    {{0x0a52, 0xea5a, 0x0b70, 0xc467, 0x74c9},
     {0x0b35, 0xfcf7, 0x0b70, 0xc5a2, 0x6ddd}, LEVEL(2048), 0, 0},
    DEFAULT_PROFILE(1024),
    {{0x0da1, 0xe9c9, 0x0f6d, 0xc5a2, 0x70ba},
     {0x0e37, 0xe269, 0x0f6d, 0xc505, 0x7643}, LEVEL(512), 0, 0},
    {{0x0937, 0xefb8, 0x0997, 0xc289, 0x757e},
     {0x0937, 0xeda6, 0x0997, 0xc289, 0x7377}, LEVEL(512), 0, 0},
    {{0x095a, 0xed34, 0x09bd, 0xc289, 0x769a},
     {0x07e3, 0xf293, 0x09bd, 0xc289, 0x7308}, LEVEL(1792), 0, 0},
    {{0x1851, 0xcb39, 0x1eb2, 0xcd4f, 0x6bdf},
     {0x1851, 0xd625, 0x1eb2, 0xcd4f, 0x6845}, LEVEL(2304), 120, 540},
    {{0x1153, 0xdd2f, 0x1209, 0xc505, 0x742f},
     {0x109f, 0xe9bd, 0x1209, 0xc6ba, 0x6be3}, LEVEL(1536), 0, 0},
    {{0x0a60, 0xeaa7, 0x0b42, 0xc505, 0x745b},
     {0x0987, 0xf73f, 0x0b42, 0xc63e, 0x6bdb}, LEVEL(1280), 0, 0},
    DEFAULT_PROFILE(1280),
    DEFAULT_PROFILE(352),
    {{0x09ae, 0xebe5, 0x0a81, 0xc3c9, 0x7669},
     {0x0882, 0xf3d9, 0x0a81, 0xc3c9, 0x7086}, LEVEL(1672), 0, 0},
    {{0x14af, 0xd336, 0x186f, 0xcb01, 0x6f16},
     {0x143c, 0xddef, 0x186f, 0xcb01, 0x6990}, LEVEL(1573), 0, 0},
    DEFAULT_PROFILE(2304),
    DEFAULT_PROFILE(1536),
    {{0x1004, 0xe100, 0x10ae, 0xc289, 0x757e},
     {0x1004, 0xe223, 0x10ae, 0xc289, 0x744d}, LEVEL(384), 0, 0},
    {{0x0a7a, 0xeb42, 0x0ae8, 0xc63e, 0x71e5},
     {0x08d6, 0xf7b4, 0x0ae8, 0xc9d5, 0x6d2f}, LEVEL(368), 0, 0},
    {{0x0807, 0xeefe, 0x09e9, 0xc147, 0x7675},
     {0x0807, 0xf02e, 0x09e9, 0xc147, 0x74a5}, LEVEL(1536), 0, 0},
    {{0x0d49, 0xe32e, 0x1066, 0xc3c9, 0x74ab},
     {0x0d49, 0xe66f, 0x1066, 0xc3c9, 0x71dc}, LEVEL(768), 0, 0},
    {{0x3dbe, 0x87d4, 0x4049, 0xc289, 0x769a},
     {0x3dbe, 0x903c, 0x4049, 0xc289, 0x70be}, LEVEL(3328), 0, 0},
    {{0x0b94, 0xe6f0, 0x0dfc, 0xcb01, 0x6e21},
     {0x0b94, 0xf53f, 0x0dfc, 0xc9d5, 0x69d7}, LEVEL(364), 0, 0},
    {{0x090b, 0xec04, 0x0b2a, 0xcc29, 0x6ddd},
     {0x090b, 0x037d, 0x0b2a, 0xcc29, 0x662e}, LEVEL(896), 0, 0},
    {{0x090b, 0xec04, 0x0b2a, 0xcc29, 0x6ddd},
     {0x090b, 0x037d, 0x0b2a, 0xcc29, 0x662e}, LEVEL(1024), 0, 0},
    DEFAULT_PROFILE(1280),
    DEFAULT_PROFILE(512),
    {{0x04b5, 0xfe99, 0x04b5, 0xc8b9, 0x6b63},
     {0x2ab9, 0xaaaa, 0x2ab9, 0xc58e, 0x7637}, LEVEL(1536), 0, 0},
    {{0x026b, 0x021c, 0x026b, 0xcbcb, 0x67ab},
     {0x24c8, 0xb67e, 0x24c8, 0xc6c2, 0x75dd}, LEVEL(512), 0, 0},
    {{0x0a45, 0xea76, 0x0b61, 0xc467, 0x74c9},
     {0x930a, 0xf693, 0x0b61, 0xc5a2, 0x6dde}, LEVEL(2048), 0, 0},
    DEFAULT_PROFILE(1024),
    {{0x0c08, 0xe726, 0x0d0e, 0xc505, 0x7377},
     {0x0b89, 0xed33, 0x0d0e, 0xc63e, 0x6f4c}, LEVEL(2048), 0, 0},
    {{0x06f1, 0xf182, 0x07b1, 0xc505, 0x7377},
     {0x06a7, 0xf9f4, 0x07b1, 0xc63e, 0x6f15}, LEVEL(1024), 0, 0},
    {{0x095a, 0xed34, 0x09bd, 0xc289, 0x769a},
     {0x07e3, 0xf293, 0x09bd, 0xc289, 0x7308}, LEVEL(2304), 0, 0},
    {{0x0a7a, 0xeb42, 0x0ae8, 0xc63e, 0x71e5},
     {0x08d6, 0xf7b4, 0x0ae8, 0xc9d5, 0x6d2f}, LEVEL(2304), 0, 0},
    {{0x2dbe, 0xa0b5, 0x31a2, 0xc505, 0x7a69},
     {0x2dbe, 0xaf2f, 0x31a2, 0xc505, 0x6c7e}, LEVEL(2048), 0, 0},
    {{0x1811, 0xcab8, 0x1db7, 0xcc29, 0x6e2a},
     {0x1811, 0xd707, 0x1db7, 0xcc29, 0x662e}, LEVEL(1536), 0, 0},
    {{0x0fc9, 0xdf93, 0x10c7, 0xc505, 0x76b0},
     {0x0f25, 0xe794, 0x10c7, 0xc63e, 0x7057}, LEVEL(2048), 0, 0},
    {{0x0920, 0xed42, 0x09b3, 0xc505, 0x76b0},
     {0x0863, 0xf860, 0x09b3, 0xc63e, 0x7057}, LEVEL(832), 0, 0},
    {{0x0a45, 0xea76, 0x0b61, 0xc467, 0x74c9},
     {0x930a, 0xf693, 0x0b61, 0xc5a2, 0x6dde}, LEVEL(4096), 0, 0},
    {DEFAULT_FIRST,
     {0x0bc2, 0x1184, 0x08c2, 0xcd4f, 0x6495}, LEVEL(304), 0, 0}};
/* clang-format on */

static struct cadence_tracker cadence;
static const struct tone_filter *filter;
static void reset_cadence(void);
static char track_cadence(char tone, char timed);
static const struct tone_filter *select_filter(Uint16 profile, Uint8 handshake);

#define PROGRAM_BIQUAD(base, coeff)                                            \
    init_biquad_cc((base), (coeff).b0, (coeff).b1, (coeff).b2, (coeff).a1,     \
                   (coeff).a2)

void dp_set_handshake_tone_detector(Uint16 profile, Uint8 handshake)
{
    filter = select_filter(profile, handshake);
    target_dpNeedsHookControl(0);
    PROGRAM_BIQUAD(TONE_A_FILTER, filter->first);
    PROGRAM_BIQUAD(TONE_A_FILTER + SECOND_FILTER, filter->second);
    init_biquad_parameter_cc(TONE_DETECTOR_A, filter->gain, filter->threshold,
                             filter->scale * gain_steps[0] / GAIN_ONE,
                             filter->scale * gain_steps[0] / GAIN_ONE);
    /* B hears 2225 Hz, and C 1300 Hz, as dp_set_tone_detector tunes them */
    init_biquad_cc(TONE_B_FILTER, 0x0205, 0xfbf9, 0x0206, 0xc147, 0xd22d);
    init_biquad_cc(TONE_B_FILTER + SECOND_FILTER, 0x0205, 0x0380, 0x0206,
                   0xc147, 0xd1f8);
    init_biquad_parameter_cc(TONE_DETECTOR_B, 0x7e67, 0x02df, 0x2a00, 0x1c00);
#ifdef NONMATCHING
    /* this side sets by a mask the bit the matching side sets by name: tone_c
     * is bit 6 in both of roc_k56.h's layouts */
    rcdpi->r02.byte |= 0x40;
#else
    rcdpi->r02.bit.tone_c = 1;
#endif
    init_biquad_cc(TONE_C_PASS_FILTER, 0, 0, 0x7fff, 0, 0);
    init_biquad_cc(TONE_C_PASS_FILTER + SECOND_FILTER, 0, 0, 0x7fff, 0, 0);
    init_biquad_cc(TONE_C_FILTER, 0x0244, 0xfb7b, 0x0245, 0xc147, 0x35a7);
    init_biquad_cc(TONE_C_FILTER + SECOND_FILTER, 0x0244, 0x00ca, 0x0245,
                   0xc147, 0x3574);
    init_biquad_parameter_cc(TONE_DETECTOR_C, 0x7e67, 0x02df, 0x2a00, 0x1c00);
    reset_cadence();
    init_tonec_cc();
}

Uint8 dp_filter_test(void)
{
#ifdef NONMATCHING
    /* this side reads by a shift the bit the matching side reads by name:
     * tone_a is bit 7 in both of roc_k56.h's layouts */
    return track_cadence(rcdpi->tones.byte >> 7, rcdpi->conf.byte == CONF_TONE);
#else
    return track_cadence(rcdpi->tones.bit.tone_a,
                         rcdpi->conf.byte == CONF_TONE);
#endif
}

static void reset_cadence(void)
{
    cadence.tone = 0;
    cadence.measuring = 1;
    cadence.changed_at = 0;
    cadence.on_ticks = filter->on_ticks;
    cadence.off_ticks = filter->off_ticks;
}

static char track_cadence(char tone, char timed)
{
    if (filter->on_ticks == 0) {
        return tone;
    }
    if (!timed) {
        if (tone != cadence.tone) {
            if (cadence.measuring) {
                if (cadence.tone) {
                    setup_timer(&cadence.on_ticks, 0);
                    cadence.on_ticks =
                        cadence.on_ticks - cadence.changed_at + SLACK_TICKS;
                } else {
                    setup_timer(&cadence.off_ticks, 0);
                    cadence.off_ticks =
                        cadence.off_ticks - cadence.changed_at + SLACK_TICKS;
                }
            }
            setup_timer(&cadence.changed_at, 0);
            cadence.measuring = 1;
            cadence.tone = tone;
        }
        return tone;
    } else {
        Uint32 start = cadence.changed_at;
        if (cadence.tone) {
            if (timer_expired(cadence.on_ticks + start)) {
                cadence.tone = 0;
                setup_timer(&cadence.changed_at, 0);
            }
        } else {
            if (timer_expired(cadence.off_ticks + start)) {
                cadence.tone = 1;
                setup_timer(&cadence.changed_at, 0);
            }
        }
        cadence.measuring = 0;
        return cadence.tone;
    }
}

/* profile is a country's handshake_tones, 0 to 41. a profile past 41 takes
 * pair 0's dial filter, even for the handshake */
static const struct tone_filter *select_filter(Uint16 profile, Uint8 handshake)
{
#define PAIR(n)                                                                \
    if (!handshake) {                                                          \
        return &profiles[2 * (n)];                                             \
    } else {                                                                   \
        return &profiles[(2 * (n)) + 1];                                       \
    }
    switch (profile) {
    case 0:
        PAIR(0);
    case 1:
        PAIR(1);
    case 2:
        PAIR(2);
    case 3:
        PAIR(3);
    case 4:
        PAIR(4);
    case 5:
        PAIR(5);
    case 6:
        PAIR(6);
    case 7:
        PAIR(11);
    case 8:
        PAIR(10);
    case 9:
        PAIR(12);
    case 10:
        PAIR(9);
    case 11:
        PAIR(8);
    case 12:
        PAIR(7);
    case 13:
        PAIR(15);
    case 14:
        PAIR(0);
    case 15:
        PAIR(13);
    case 16:
        PAIR(14);
    case 17:
        PAIR(0);
    case 18:
        PAIR(0);
    case 19:
        PAIR(0);
    case 20:
        PAIR(0);
    case 21:
        PAIR(16);
    case 22:
        PAIR(0);
    case 23:
        PAIR(0);
    case 24:
        PAIR(0);
    case 25:
        PAIR(0);
    case 26:
        PAIR(17);
    case 27:
        PAIR(0);
    case 28:
        PAIR(0);
    case 29:
        PAIR(0);
    case 30:
        PAIR(18);
    case 31:
        PAIR(0);
    case 32:
        PAIR(0);
    case 33:
        PAIR(0);
    case 34:
        PAIR(19);
    case 35:
        PAIR(0);
    case 36:
        PAIR(20);
    case 37:
        PAIR(0);
    case 38:
        PAIR(0);
    case 39:
        PAIR(0);
    case 40:
        PAIR(0);
    case 41:
        PAIR(0);
    default:
        return &profiles[0];
    }
#undef PAIR
}
