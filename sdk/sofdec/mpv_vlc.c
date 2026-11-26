/* the MPEG video decoder's variable-length code tables, and their builders */
#include <sg_xpt.h>
#include <sofdec.h>

#define MPVVLC_MB_INVALID                                                      \
    (MPVVLC_MB_QUANT | MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD |                \
     MPVVLC_MB_PATTERN | MPVVLC_MB_INTRA)
/* the run and level of run_level_8's escape entries. the block decoder knows
 * the escape by this run, which no other code's run reaches */
#define MPVVLC_RUN_LEVEL_ESCAPE 64

/* the macros build the tables' entries, in the formats sofdec.h gives. bits
 * is the count of stream bits an entry takes. where an MBAI entry takes
 * macroblock_type's code too, bits is the increment code's length plus the
 * type code's */
#define MBAI_INCREMENT(increment) ((increment) << MPVVLC_MBAI_INCREMENT_SHIFT)
#define MBAI(increment, bits, flags)                                           \
    ((Sint16)(MBAI_INCREMENT(increment) |                                      \
              (((flags) << MPVVLC_MBAI_FLAGS_SHIFT) | (bits))))
#define MBTYPE(flags, bits)                                                    \
    ((Sint16)(((flags) << MPVVLC_MBTYPE_FLAGS_SHIFT) | (bits)))
#define MOTION(code, bits)                                                     \
    ((Sint16)((Uint8)(code) | ((bits) << MPVVLC_MOTION_LENGTH_SHIFT)))
#define CBP(pattern, bits)                                                     \
    ((Sint16)((((((pattern) << MPV_BLOCKS) | (pattern)) & 0xff)                \
               << MPVVLC_CBP_PATTERN_SHIFT) |                                  \
              (bits)))
#define DCSIZ(size, bits)                                                      \
    ((Sint8)(((size) << MPVVLC_DCSIZ_SIZE_SHIFT) | (bits)))
#define RUN_LEVEL(run, level)                                                  \
    (((level) << MPVVLC_RUN_LEVEL_MAGNITUDE_SHIFT) | (run))
#define RUN_LEVEL_BITS(run, level, bits)                                       \
    (((bits) << MPVVLC_RUN_LEVEL_LENGTH_SHIFT) | RUN_LEVEL(run, level))
/* an MBAI entry that takes macroblock_type's code too, which MPVVLC_MB_TYPED
 * marks */
#define MBAI_TYPED(increment, bits, flags)                                     \
    MBAI(increment, bits, MPVVLC_MB_TYPED | (flags))

/* the member exports the mpvvlt_ tables and the mpvvlc_ functions, though
 * no header of the SDK declares them. the decoder's other members read the
 * tables only through the mpvvlc_ pointers */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the tables of the run and level codes with 6 or more leading zeros, which
 * MPVVLC_Init doesn't build */
const Uint16 mpvvlt_run_level_0c[16] = {
    RUN_LEVEL(1, 18), RUN_LEVEL(1, 17), RUN_LEVEL(1, 16), RUN_LEVEL(1, 15),
    RUN_LEVEL(6, 3),  RUN_LEVEL(16, 2), RUN_LEVEL(15, 2), RUN_LEVEL(14, 2),
    RUN_LEVEL(13, 2), RUN_LEVEL(12, 2), RUN_LEVEL(11, 2), RUN_LEVEL(31, 1),
    RUN_LEVEL(30, 1), RUN_LEVEL(29, 1), RUN_LEVEL(28, 1), RUN_LEVEL(27, 1),
};
const Uint16 mpvvlt_run_level_0b[16] = {
    RUN_LEVEL(0, 40), RUN_LEVEL(0, 39), RUN_LEVEL(0, 38), RUN_LEVEL(0, 37),
    RUN_LEVEL(0, 36), RUN_LEVEL(0, 35), RUN_LEVEL(0, 34), RUN_LEVEL(0, 33),
    RUN_LEVEL(0, 32), RUN_LEVEL(1, 14), RUN_LEVEL(1, 13), RUN_LEVEL(1, 12),
    RUN_LEVEL(1, 11), RUN_LEVEL(1, 10), RUN_LEVEL(1, 9),  RUN_LEVEL(1, 8),
};
const Uint16 mpvvlt_run_level_0a[16] = {
    RUN_LEVEL(0, 31), RUN_LEVEL(0, 30), RUN_LEVEL(0, 29), RUN_LEVEL(0, 28),
    RUN_LEVEL(0, 27), RUN_LEVEL(0, 26), RUN_LEVEL(0, 25), RUN_LEVEL(0, 24),
    RUN_LEVEL(0, 23), RUN_LEVEL(0, 22), RUN_LEVEL(0, 21), RUN_LEVEL(0, 20),
    RUN_LEVEL(0, 19), RUN_LEVEL(0, 18), RUN_LEVEL(0, 17), RUN_LEVEL(0, 16),
};
const Uint16 mpvvlt_run_level_1[16] = {
    RUN_LEVEL(10, 2), RUN_LEVEL(9, 2),  RUN_LEVEL(5, 3),  RUN_LEVEL(3, 4),
    RUN_LEVEL(2, 5),  RUN_LEVEL(1, 7),  RUN_LEVEL(1, 6),  RUN_LEVEL(0, 15),
    RUN_LEVEL(0, 14), RUN_LEVEL(0, 13), RUN_LEVEL(0, 12), RUN_LEVEL(26, 1),
    RUN_LEVEL(25, 1), RUN_LEVEL(24, 1), RUN_LEVEL(23, 1), RUN_LEVEL(22, 1),
};
const Uint16 mpvvlt_run_level_2[16] = {
    RUN_LEVEL(0, 11), RUN_LEVEL(8, 2),  RUN_LEVEL(4, 3),  RUN_LEVEL(0, 10),
    RUN_LEVEL(2, 4),  RUN_LEVEL(7, 2),  RUN_LEVEL(21, 1), RUN_LEVEL(20, 1),
    RUN_LEVEL(0, 9),  RUN_LEVEL(19, 1), RUN_LEVEL(18, 1), RUN_LEVEL(1, 5),
    RUN_LEVEL(3, 3),  RUN_LEVEL(0, 8),  RUN_LEVEL(6, 2),  RUN_LEVEL(17, 1),
};
const Uint16 mpvvlt_run_level_4[8] = {
    RUN_LEVEL(16, 1), RUN_LEVEL(5, 2),  RUN_LEVEL(0, 7),  RUN_LEVEL(2, 3),
    RUN_LEVEL(1, 4),  RUN_LEVEL(15, 1), RUN_LEVEL(14, 1), RUN_LEVEL(4, 2),
};

const Sint16 *mpvvlc_mbai_i_0;
const Sint16 *mpvvlc_mbai_i_1;
const Sint16 *mpvvlc_mbai_p_0;
const Sint16 *mpvvlc_mbai_p_1;
const Sint16 *mpvvlc_mbai_b_0;
const Sint16 *mpvvlc_mbai_b_1;
const Sint16 *mpvvlc_p_mbtype;
const Sint16 *mpvvlc_b_mbtype;
const Sint16 *mpvvlc_motion_0;
const Sint16 *mpvvlc_motion_1;
const Sint16 *mpvvlc_cbp;
const Uint8 *mpvvlc_y_dcsiz;
const Uint8 *mpvvlc_c_dcsiz;
const Uint16 *mpvvlc_run_level_0c;
const Uint16 *mpvvlc_run_level_0b;
const Uint16 *mpvvlc_run_level_0a;
const Uint16 *mpvvlc_run_level_1;
const Uint16 *mpvvlc_run_level_2;
const Uint16 *mpvvlc_run_level_4;
const Uint32 *mpvvlc_run_level_8;

/* the tables MPVVLC_Init builds, in the formats of the mpvvlc_ pointers of
 * the same suffixes */
Sint16 mpvvlt_mbai_i_0[256];
Sint16 mpvvlt_mbai_i_1[64];
Sint16 mpvvlt_mbai_p_0[128];
Sint16 mpvvlt_mbai_p_1[32];
Sint16 mpvvlt_mbai_b_0[128];
Sint16 mpvvlt_mbai_b_1[32];
Sint16 mpvvlt_p_mbtype[32];
Sint16 mpvvlt_b_mbtype[64];
Sint16 mpvvlt_motion_0[128];
Sint16 mpvvlt_motion_1[32];
Sint16 mpvvlt_cbp[512];
/* @hack type: signed entries, where Uint8 would do for them and for the out
 * pointer that fills them */
Sint8 mpvvlt_y_dcsiz[128];
/* @hack type: signed entries, where Uint8 would do for them and for the out
 * pointer that fills them */
Sint8 mpvvlt_c_dcsiz[128];
Uint32 mpvvlt_run_level_8[128];

/* each of these four builds one family of tables by calling its builders */
void mpvvlc_InitMbai(void);
void mpvvlc_InitMbType(void);
void mpvvlc_InitDcSiz(void);
void mpvvlc_InitRunLevel(void);

void mpvvlc_InitMbaiIpic(void);
void mpvvlc_InitMbaiPpic(void);
void mpvvlc_InitMbaiBpic(void);
void mpvvlc_InitMbTypePpic(void);
void mpvvlc_InitMbTypeBpic(void);
void mpvvlc_InitMotion(void);
void mpvvlc_InitCbp(void);
Sint16 *mpvvlc_InitCbpSub1(Sint16 *out);
Sint16 *mpvvlc_InitCbpSub2(Sint16 *out);
void mpvvlc_InitDcSizY(void);
void mpvvlc_InitDcSizC(void);
void mpvvlc_InitIntRunLevel(void);
void mpvvlc_SetDflPtr(void);
void mpvvlc_SetupVlc(Uint8 *work);
Uint8 *mpvvlc_SetVlcRunLevel(Uint8 *end);
Uint8 *mpvvlc_SetVlcDcSiz(Uint8 *end);
Uint8 *mpvvlc_SetVlcMotion(Uint8 *end);
Uint8 *mpvvlc_SetVlcMbType(Uint8 *end);
/* NOLINTEND(misc-use-internal-linkage) */

Bool MPVVLC_IsVlcSizErr(void)
{
    return FALSE;
}

void MPVVLC_Init(Uint8 *work)
{
    mpvvlc_InitMbaiIpic();
    mpvvlc_InitMbaiPpic();
    mpvvlc_InitMbaiBpic();
    mpvvlc_InitMbTypePpic();
    mpvvlc_InitMbTypeBpic();
    mpvvlc_InitMotion();
    mpvvlc_InitCbp();
    mpvvlc_InitDcSizY();
    mpvvlc_InitDcSizC();
    mpvvlc_InitIntRunLevel();
    mpvvlc_SetDflPtr();
    /* @hack redundant: a goto to the next statement */
    goto setup;
setup:
    if (work != NULL) {
        mpvvlc_SetupVlc(work);
    }
}

void mpvvlc_InitMbai(void)
{
    mpvvlc_InitMbaiIpic();
    mpvvlc_InitMbaiPpic();
    mpvvlc_InitMbaiBpic();
}

void mpvvlc_InitMbaiIpic(void)
{
    Sint16 *out = mpvvlt_mbai_i_0;
    /* @hack spelling: entries stored through value, not directly */
    Sint16 value;
    Sint32 i;
    Sint32 j;

    value = MBAI(MPVVLC_MBAI_INVALID, 0, 0);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
    value = MBAI(MPVVLC_MBAI_ESCAPE, 11, 0);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = MBAI(MPVVLC_MBAI_INVALID, 0, 0);
    for (i = 12; i != 0; i--) {
        *out++ = value;
    }
    value = MBAI(MPVVLC_MBAI_STUFFING, 11, 0);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = MBAI(MPVVLC_MBAI_INVALID, 0, 0);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
    for (i = 33; i >= 22; i--) {
        value = MBAI(i, 11 + 2, MPVVLC_MB_QUANT | MPVVLC_MB_INTRA);
        /* @hack redundant: here and below, loops of one pass around the
         * stores of a code that fills one entry, where plain stores would do */
        /* @hack spelling: here and below, the one-pass loops test j > 0,
         * where the other loops test != 0 */
        /* @hack spelling: here and below, the one-pass loops' count stepped
         * after the store, in a comma expression, not in the for statement */
        for (j = 1; j > 0;) {
            *out++ = value, j--;
        }
        value = MBAI(i, 11 + 1, MPVVLC_MB_INTRA);
        for (j = 1; j > 0;) {
            *out++ = value, j--;
        }
    }
    for (i = 21; i >= 16; i--) {
        value = MBAI(i, 10 + 2, MPVVLC_MB_QUANT | MPVVLC_MB_INTRA);
        for (j = 2; j != 0; j--) {
            *out++ = value;
        }
        value = MBAI(i, 10 + 1, MPVVLC_MB_INTRA);
        for (j = 2; j != 0; j--) {
            *out++ = value;
        }
    }
    for (i = 15; i >= 10; i--) {
        value = MBAI(i, 8 + 2, MPVVLC_MB_QUANT | MPVVLC_MB_INTRA);
        for (j = 8; j != 0; j--) {
            *out++ = value;
        }
        value = MBAI(i, 8 + 1, MPVVLC_MB_INTRA);
        for (j = 8; j != 0; j--) {
            *out++ = value;
        }
    }
    for (i = 9; i >= 8; i--) {
        value = MBAI(i, 7 + 2, MPVVLC_MB_QUANT | MPVVLC_MB_INTRA);
        for (j = 16; j != 0; j--) {
            *out++ = value;
        }
        value = MBAI(i, 7 + 1, MPVVLC_MB_INTRA);
        for (j = 16; j != 0; j--) {
            *out++ = value;
        }
    }
    out = mpvvlt_mbai_i_1;
    value = MBAI(MPVVLC_MBAI_INVALID, 0, 0);
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    for (i = 7; i >= 6; i--) {
        value = MBAI(i, 5 + 2, MPVVLC_MB_QUANT | MPVVLC_MB_INTRA);
        for (j = 1; j > 0;) {
            *out++ = value, j--;
        }
        value = MBAI(i, 5 + 1, MPVVLC_MB_INTRA);
        for (j = 1; j > 0;) {
            *out++ = value, j--;
        }
    }
    for (i = 5; i >= 4; i--) {
        value = MBAI(i, 4 + 2, MPVVLC_MB_QUANT | MPVVLC_MB_INTRA);
        for (j = 2; j != 0; j--) {
            *out++ = value;
        }
        value = MBAI(i, 4 + 1, MPVVLC_MB_INTRA);
        for (j = 2; j != 0; j--) {
            *out++ = value;
        }
    }
    for (i = 3; i >= 2; i--) {
        value = MBAI(i, 3 + 2, MPVVLC_MB_QUANT | MPVVLC_MB_INTRA);
        for (j = 4; j != 0; j--) {
            *out++ = value;
        }
        value = MBAI(i, 3 + 1, MPVVLC_MB_INTRA);
        for (j = 4; j != 0; j--) {
            *out++ = value;
        }
    }
    value = MBAI(1, 1 + 2, MPVVLC_MB_QUANT | MPVVLC_MB_INTRA);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
    value = MBAI(1, 1 + 1, MPVVLC_MB_INTRA);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
}

void mpvvlc_InitMbaiPpic(void)
{
    Sint16 *out = mpvvlt_mbai_p_0;
    Sint32 i;
    /* @hack spelling: entries stored through value, not directly */
    Sint16 value;

    value = MBAI(MPVVLC_MBAI_INVALID, 0, 0);
    for (i = 8; i != 0; i--) {
        *out++ = value;
    }
    value = MBAI(MPVVLC_MBAI_ESCAPE, 11, 0);
    *out++ = value;
    value = MBAI(MPVVLC_MBAI_INVALID, 0, 0);
    for (i = 6; i != 0; i--) {
        *out++ = value;
    }
    value = MBAI(MPVVLC_MBAI_STUFFING, 11, 0);
    *out++ = value;
    value = MBAI(MPVVLC_MBAI_INVALID, 0, 0);
    for (i = 8; i != 0; i--) {
        *out++ = value;
    }
    /* @hack spelling: here and in the do loops below, i is the increment
     * shifted into its field, where for loops over the increment would do */
    i = MBAI_INCREMENT(33);
    do {
        *out++ = i | MBAI(0, 11, 0);
        i -= MBAI_INCREMENT(1);
    } while (i >= MBAI_INCREMENT(22));
    i = MBAI_INCREMENT(21);
    do {
        *out++ = i | MBAI(0, 10, 0);
        *out++ =
            i | MBAI_TYPED(0, 10 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        i -= MBAI_INCREMENT(1);
    } while (i >= MBAI_INCREMENT(16));
    i = MBAI_INCREMENT(15);
    do {
        *out++ = i | MBAI(0, 8, 0);
        *out++ = i | MBAI_TYPED(0, 8 + 3, MPVVLC_MB_FORWARD);
        value = i | MBAI_TYPED(0, 8 + 2, MPVVLC_MB_PATTERN);
        *out++ = value;
        *out++ = value;
        value = i | MBAI_TYPED(0, 8 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        *out++ = value;
        *out++ = value;
        *out++ = value;
        *out++ = value;
        i -= MBAI_INCREMENT(1);
    } while (i >= MBAI_INCREMENT(10));
    i = MBAI_INCREMENT(9);
    do {
        *out++ = i | MBAI(0, 7, 0);
        *out++ = i | MBAI(0, 7, 0);
        *out++ = i | MBAI_TYPED(0, 7 + 3, MPVVLC_MB_FORWARD);
        *out++ = i | MBAI_TYPED(0, 7 + 3, MPVVLC_MB_FORWARD);
        *out++ = i | MBAI_TYPED(0, 7 + 2, MPVVLC_MB_PATTERN);
        *out++ = i | MBAI_TYPED(0, 7 + 2, MPVVLC_MB_PATTERN);
        *out++ = i | MBAI_TYPED(0, 7 + 2, MPVVLC_MB_PATTERN);
        *out++ = i | MBAI_TYPED(0, 7 + 2, MPVVLC_MB_PATTERN);
        *out++ =
            i | MBAI_TYPED(0, 7 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        *out++ =
            i | MBAI_TYPED(0, 7 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        *out++ =
            i | MBAI_TYPED(0, 7 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        *out++ =
            i | MBAI_TYPED(0, 7 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        *out++ =
            i | MBAI_TYPED(0, 7 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        *out++ =
            i | MBAI_TYPED(0, 7 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        *out++ =
            i | MBAI_TYPED(0, 7 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        *out++ =
            i | MBAI_TYPED(0, 7 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        /* @hack spelling: the step spelled out, where i -= MBAI_INCREMENT(1)
         * would do */
        i = i - MBAI_INCREMENT(1);
    } while (i >= MBAI_INCREMENT(8));
    /* @hack spelling: value set before out moves to the other table, not
     * after */
    value = MBAI(MPVVLC_MBAI_INVALID, 0, 0);
    out = mpvvlt_mbai_p_1;
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    i = MBAI_INCREMENT(7);
    do {
        *out++ = i | MBAI(0, 5, 0);
        i -= MBAI_INCREMENT(1);
    } while (i >= MBAI_INCREMENT(6));
    i = MBAI_INCREMENT(5);
    do {
        *out++ = i | MBAI(0, 4, 0);
        *out++ =
            i | MBAI_TYPED(0, 4 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        i -= MBAI_INCREMENT(1);
    } while (i >= MBAI_INCREMENT(4));
    for (i = 3; i >= 2; i--) {
        *out++ = MBAI(i, 3, 0);
        *out++ = MBAI_TYPED(i, 3 + 2, MPVVLC_MB_PATTERN);
        *out++ = MBAI_TYPED(i, 3 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        *out++ = MBAI_TYPED(i, 3 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
    }
    value = MBAI(1, 1, 0);
    *out++ = value;
    *out++ = value;
    value = MBAI_TYPED(1, 1 + 3, MPVVLC_MB_FORWARD);
    *out++ = value;
    *out++ = value;
    value = MBAI_TYPED(1, 1 + 2, MPVVLC_MB_PATTERN);
    *out++ = value;
    *out++ = value;
    *out++ = value;
    *out++ = value;
    value = MBAI_TYPED(1, 1 + 1, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
    *out++ = value;
    *out++ = value;
    *out++ = value;
    *out++ = value;
    *out++ = value;
    *out++ = value;
    *out++ = value;
    *out++ = value;
}

void mpvvlc_InitMbaiBpic(void)
{
    Sint16 *out = mpvvlt_mbai_b_0;
    Sint32 i;
    Sint32 j;
    /* @hack spelling: entries stored through value, not directly */
    Sint16 value;

    value = MBAI(MPVVLC_MBAI_INVALID, 0, 0);
    for (i = 8; i != 0; i--) {
        *out++ = value;
    }
    value = MBAI(MPVVLC_MBAI_ESCAPE, 11, 0);
    *out++ = value;
    value = MBAI(MPVVLC_MBAI_INVALID, 0, 0);
    for (i = 6; i != 0; i--) {
        *out++ = value;
    }
    value = MBAI(MPVVLC_MBAI_STUFFING, 11, 0);
    *out++ = value;
    value = MBAI(MPVVLC_MBAI_INVALID, 0, 0);
    for (i = 8; i != 0; i--) {
        *out++ = value;
    }
    for (i = 33; i >= 22; i--) {
        *out++ = MBAI(i, 11, 0);
    }
    for (i = 21; i >= 16; i--) {
        /* @hack redundant: here and below, loops of one pass around the
         * stores of a code that fills one entry, where plain stores would do */
        /* @hack spelling: here and below, the one-pass loops test j > 0,
         * where the other loops test != 0 */
        for (j = 1; j > 0; j--) {
            *out++ = MBAI(i, 10, 0);
        }
        for (j = 1; j > 0; j--) {
            *out++ = MBAI(i, 10, 0);
        }
    }
    for (i = 15; i >= 10; i--) {
        *out++ = MBAI(i, 8, 0);
        *out++ = MBAI(i, 8, 0);
        *out++ = MBAI_TYPED(i, 8 + 3, MPVVLC_MB_BACKWARD);
        *out++ = MBAI_TYPED(i, 8 + 3, MPVVLC_MB_BACKWARD | MPVVLC_MB_PATTERN);
        *out++ = MBAI_TYPED(i, 8 + 2, MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD);
        *out++ = MBAI_TYPED(i, 8 + 2, MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD);
        *out++ = MBAI_TYPED(i, 8 + 2,
                            MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD |
                                MPVVLC_MB_PATTERN);
        *out++ = MBAI_TYPED(i, 8 + 2,
                            MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD |
                                MPVVLC_MB_PATTERN);
    }
    for (i = 9; i >= 8; i--) {
        *out++ = MBAI(i, 7, 0);
        *out++ = MBAI(i, 7, 0);
        *out++ = MBAI_TYPED(i, 7 + 4, MPVVLC_MB_FORWARD);
        *out++ = MBAI_TYPED(i, 7 + 4, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
        *out++ = MBAI_TYPED(i, 7 + 3, MPVVLC_MB_BACKWARD);
        *out++ = MBAI_TYPED(i, 7 + 3, MPVVLC_MB_BACKWARD);
        *out++ = MBAI_TYPED(i, 7 + 3, MPVVLC_MB_BACKWARD | MPVVLC_MB_PATTERN);
        *out++ = MBAI_TYPED(i, 7 + 3, MPVVLC_MB_BACKWARD | MPVVLC_MB_PATTERN);
        *out++ = MBAI_TYPED(i, 7 + 2, MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD);
        *out++ = MBAI_TYPED(i, 7 + 2, MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD);
        *out++ = MBAI_TYPED(i, 7 + 2, MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD);
        *out++ = MBAI_TYPED(i, 7 + 2, MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD);
        *out++ = MBAI_TYPED(i, 7 + 2,
                            MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD |
                                MPVVLC_MB_PATTERN);
        *out++ = MBAI_TYPED(i, 7 + 2,
                            MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD |
                                MPVVLC_MB_PATTERN);
        *out++ = MBAI_TYPED(i, 7 + 2,
                            MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD |
                                MPVVLC_MB_PATTERN);
        *out++ = MBAI_TYPED(i, 7 + 2,
                            MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD |
                                MPVVLC_MB_PATTERN);
    }
    out = mpvvlt_mbai_b_1;
    value = MBAI(MPVVLC_MBAI_INVALID, 0, 0);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    for (i = 7; i >= 6; i--) {
        *out++ = MBAI(i, 5, 0);
    }
    for (i = 5; i >= 4; i--) {
        for (j = 1; j > 0; j--) {
            *out++ = MBAI(i, 4, 0);
        }
        for (j = 1; j > 0; j--) {
            *out++ = MBAI(i, 4, 0);
        }
    }
    for (i = 3; i >= 2; i--) {
        *out++ = MBAI(i, 3, 0);
        *out++ = MBAI(i, 3, 0);
        *out++ = MBAI_TYPED(i, 3 + 2, MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD);
        *out++ = MBAI_TYPED(i, 3 + 2,
                            MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD |
                                MPVVLC_MB_PATTERN);
    }
    value = MBAI(1, 1, 0);
    *out++ = value;
    *out++ = value;
    *out++ = MBAI_TYPED(1, 1 + 4, MPVVLC_MB_FORWARD);
    *out++ = MBAI_TYPED(1, 1 + 4, MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN);
    value = MBAI_TYPED(1, 1 + 3, MPVVLC_MB_BACKWARD);
    *out++ = value;
    *out++ = value;
    value = MBAI_TYPED(1, 1 + 3, MPVVLC_MB_BACKWARD | MPVVLC_MB_PATTERN);
    *out++ = value;
    *out++ = value;
    value = MBAI_TYPED(1, 1 + 2, MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD);
    *out++ = value;
    *out++ = value;
    *out++ = value;
    *out++ = value;
    value = MBAI_TYPED(
        1, 1 + 2, MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD | MPVVLC_MB_PATTERN);
    *out++ = value;
    *out++ = value;
    *out++ = value;
    *out++ = value;
}

void mpvvlc_InitMbType(void)
{
    mpvvlc_InitMbTypePpic();
    mpvvlc_InitMbTypeBpic();
}

void mpvvlc_InitMbTypePpic(void)
{
    Sint16 *out = mpvvlt_p_mbtype;
    Sint32 i;
    /* @hack spelling: entries stored through value, not directly */
    Sint16 value;

    value = MBTYPE(MPVVLC_MB_QUANT | MPVVLC_MB_INTRA, 6);
    *out++ = value;
    value = MBTYPE(MPVVLC_MB_QUANT | MPVVLC_MB_PATTERN, 5);
    *out++ = value;
    value = MBTYPE(MPVVLC_MB_QUANT | MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN, 5);
    *out++ = value;
    value = MBTYPE(MPVVLC_MB_INTRA, 5);
    *out++ = value;
    value = MBTYPE(MPVVLC_MB_FORWARD, 3);
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    value = MBTYPE(MPVVLC_MB_PATTERN, 2);
    for (i = 8; i != 0; i--) {
        *out++ = value;
    }
    value = MBTYPE(MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN, 1);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
}

void mpvvlc_InitMbTypeBpic(void)
{
    Sint16 *out = mpvvlt_b_mbtype;
    Sint32 i;
    /* @hack spelling: entries stored through value, not directly */
    Sint16 value;

    value = MBTYPE(MPVVLC_MB_INVALID, 0);
    *out++ = value;
    value = MBTYPE(MPVVLC_MB_QUANT | MPVVLC_MB_INTRA, 6);
    *out++ = value;
    value = MBTYPE(MPVVLC_MB_QUANT | MPVVLC_MB_BACKWARD | MPVVLC_MB_PATTERN, 6);
    *out++ = value;
    value = MBTYPE(MPVVLC_MB_QUANT | MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN, 6);
    *out++ = value;
    value = MBTYPE(MPVVLC_MB_QUANT | MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD |
                       MPVVLC_MB_PATTERN,
                   5);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = MBTYPE(MPVVLC_MB_INTRA, 5);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = MBTYPE(MPVVLC_MB_FORWARD, 4);
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    value = MBTYPE(MPVVLC_MB_FORWARD | MPVVLC_MB_PATTERN, 4);
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    value = MBTYPE(MPVVLC_MB_BACKWARD, 3);
    for (i = 8; i != 0; i--) {
        *out++ = value;
    }
    value = MBTYPE(MPVVLC_MB_BACKWARD | MPVVLC_MB_PATTERN, 3);
    for (i = 8; i != 0; i--) {
        *out++ = value;
    }
    value = MBTYPE(MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD, 2);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
    value =
        MBTYPE(MPVVLC_MB_FORWARD | MPVVLC_MB_BACKWARD | MPVVLC_MB_PATTERN, 2);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
}

void mpvvlc_InitMotion(void)
{
    Sint16 *out = mpvvlt_motion_0;
    Sint32 i;
    Sint32 j;
    /* @hack spelling: entries stored through value, not directly */
    Sint16 value;
    /* @hack redundant: mark, a local nothing reads, and the two stores to
     * it */
    Sint16 *mark; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    value = MOTION(MPVVLC_MOTION_INVALID, 0);
    for (i = 24; i != 0; i--) {
        *out++ = value;
    }
    /* @hack spelling: the loops over the codes as do loops, where for loops
     * would do */
    i = 16;
    do {
        value = MOTION(i, 11);
        *out++ = value;
        value = MOTION(-i, 11);
        *out++ = value;
    } while (--i >= 11);
    i = 10;
    do {
        value = MOTION(i, 10);
        mark = out;
        for (j = 2; j != 0; j--) {
            *out++ = value;
        }
        value = MOTION(-i, 10);
        mark = out;
        for (j = 2; j != 0; j--) {
            *out++ = value;
        }
    } while (--i >= 8);
    i = 7;
    do {
        value = MOTION(i, 8);
        for (j = 8; j != 0; j--) {
            *out++ = value;
        }
        value = MOTION(-i, 8);
        for (j = 8; j != 0; j--) {
            *out++ = value;
        }
    } while (--i >= 5);
    value = MOTION(4, 7);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
    value = MOTION(-4, 7);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
    out = mpvvlt_motion_1;
    value = MOTION(MPVVLC_MOTION_INVALID, 0);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = MOTION(3, 5);
    *out++ = value;
    value = MOTION(-3, 5);
    *out++ = value;
    value = MOTION(2, 4);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = MOTION(-2, 4);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = MOTION(1, 3);
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    value = MOTION(-1, 3);
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    value = MOTION(0, 1);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
}

void mpvvlc_InitCbp(void)
{
    Sint16 *out = mpvvlc_InitCbpSub1(mpvvlt_cbp);

    /* @hack redundant: a store of the result that nothing reads */
    out = mpvvlc_InitCbpSub2(out);
}

/* fill the coded_block_pattern entries of no valid code, then of the 9- and
 * 8-bit codes, from out. it returns the entry after the last it fills */
Sint16 *mpvvlc_InitCbpSub1(Sint16 *out)
{
    Sint32 i;
    /* @hack spelling: entries stored through value, not directly */
    Sint16 value;

    value = CBP(0, 0);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(39, 9);
    *out++ = value;
    value = CBP(27, 9);
    *out++ = value;
    value = CBP(59, 9);
    *out++ = value;
    value = CBP(55, 9);
    *out++ = value;
    value = CBP(47, 9);
    *out++ = value;
    value = CBP(31, 9);
    *out++ = value;
    value = CBP(58, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(54, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(46, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(30, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(57, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(53, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(45, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(29, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(38, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(26, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(37, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(25, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(43, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(23, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(51, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(15, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(42, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(22, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(50, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(14, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(41, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(21, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(49, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(13, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(35, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(19, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(11, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = CBP(7, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    return out;
}

/* fill the coded_block_pattern entries of the 7- to 3-bit codes, from out.
 * it returns the entry after the last it fills */
Sint16 *mpvvlc_InitCbpSub2(Sint16 *out)
{
    Sint32 i;

    /* @hack spelling: the counts run up from 0, not down to 0 as in the
     * other functions */
    for (i = 0; i < 4; i++) {
        *out++ = CBP(34, 7);
    }
    for (i = 0; i < 4; i++) {
        *out++ = CBP(18, 7);
    }
    for (i = 0; i < 4; i++) {
        *out++ = CBP(10, 7);
    }
    for (i = 0; i < 4; i++) {
        *out++ = CBP(6, 7);
    }
    for (i = 0; i < 4; i++) {
        *out++ = CBP(33, 7);
    }
    for (i = 0; i < 4; i++) {
        *out++ = CBP(17, 7);
    }
    for (i = 0; i < 4; i++) {
        *out++ = CBP(9, 7);
    }
    for (i = 0; i < 4; i++) {
        *out++ = CBP(5, 7);
    }
    for (i = 0; i < 8; i++) {
        *out++ = CBP(63, 6);
    }
    for (i = 0; i < 8; i++) {
        *out++ = CBP(3, 6);
    }
    for (i = 0; i < 8; i++) {
        *out++ = CBP(36, 6);
    }
    for (i = 0; i < 8; i++) {
        *out++ = CBP(24, 6);
    }
    for (i = 0; i < 16; i++) {
        *out++ = CBP(62, 5);
    }
    for (i = 0; i < 16; i++) {
        *out++ = CBP(2, 5);
    }
    for (i = 0; i < 16; i++) {
        *out++ = CBP(61, 5);
    }
    for (i = 0; i < 16; i++) {
        *out++ = CBP(1, 5);
    }
    for (i = 0; i < 16; i++) {
        *out++ = CBP(56, 5);
    }
    for (i = 0; i < 16; i++) {
        *out++ = CBP(52, 5);
    }
    for (i = 0; i < 16; i++) {
        *out++ = CBP(44, 5);
    }
    for (i = 0; i < 16; i++) {
        *out++ = CBP(28, 5);
    }
    for (i = 0; i < 16; i++) {
        *out++ = CBP(40, 5);
    }
    for (i = 0; i < 16; i++) {
        *out++ = CBP(20, 5);
    }
    for (i = 0; i < 16; i++) {
        *out++ = CBP(48, 5);
    }
    for (i = 0; i < 16; i++) {
        *out++ = CBP(12, 5);
    }
    for (i = 0; i < 32; i++) {
        *out++ = CBP(32, 4);
    }
    for (i = 0; i < 32; i++) {
        *out++ = CBP(16, 4);
    }
    for (i = 0; i < 32; i++) {
        *out++ = CBP(8, 4);
    }
    for (i = 0; i < 32; i++) {
        *out++ = CBP(4, 4);
    }
    for (i = 0; i < 64; i++) {
        *out++ = CBP(60, 3);
    }
    return out;
}

void mpvvlc_InitDcSiz(void)
{
    mpvvlc_InitDcSizY();
    mpvvlc_InitDcSizC();
}

void mpvvlc_InitDcSizY(void)
{
    Sint8 *out = mpvvlt_y_dcsiz;
    Sint32 i;

    /* @hack spelling: the counts tested with > 0, not with != 0 as the other
     * loops are */
    for (i = 32; i > 0; i--) {
        *out++ = DCSIZ(1, 2);
    }
    for (i = 32; i > 0; i--) {
        *out++ = DCSIZ(2, 2);
    }
    for (i = 16; i > 0; i--) {
        *out++ = DCSIZ(0, 3);
    }
    for (i = 16; i > 0; i--) {
        *out++ = DCSIZ(3, 3);
    }
    for (i = 16; i > 0; i--) {
        *out++ = DCSIZ(4, 3);
    }
    for (i = 8; i > 0; i--) {
        *out++ = DCSIZ(5, 4);
    }
    for (i = 4; i > 0; i--) {
        *out++ = DCSIZ(6, 5);
    }
    for (i = 2; i > 0; i--) {
        *out++ = DCSIZ(7, 6);
    }
    for (i = 2; i > 0; i--) {
        *out++ = DCSIZ(8, 7);
    }
}

void mpvvlc_InitDcSizC(void)
{
    Sint8 *out = mpvvlt_c_dcsiz;
    Sint32 i;
    /* @hack spelling: entries stored through value, not directly */
    Sint8 value;

    value = DCSIZ(0, 2);
    for (i = 32; i != 0; i--) {
        *out++ = value;
    }
    value = DCSIZ(1, 2);
    for (i = 32; i != 0; i--) {
        *out++ = value;
    }
    value = DCSIZ(2, 2);
    for (i = 32; i != 0; i--) {
        *out++ = value;
    }
    value = DCSIZ(3, 3);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
    value = DCSIZ(4, 4);
    for (i = 8; i != 0; i--) {
        *out++ = value;
    }
    value = DCSIZ(5, 5);
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    value = DCSIZ(6, 6);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = DCSIZ(7, 7);
    *out++ = value;
    value = DCSIZ(8, 8);
    *out++ = value;
}

void mpvvlc_InitIntRunLevel(void)
{
    Uint32 *out = mpvvlt_run_level_8;
    Sint32 i;
    /* @hack spelling: entries stored through value, not directly */
    Uint32 value;

    value = 0;
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(MPVVLC_RUN_LEVEL_ESCAPE, MPVVLC_RUN_LEVEL_ESCAPE, 6);
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(2, 2, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(9, 1, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(0, 4, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(8, 1, 8);
    for (i = 2; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(7, 1, 7);
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(6, 1, 7);
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(1, 2, 7);
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(5, 1, 7);
    for (i = 4; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(13, 1, 9);
    *out++ = value;
    value = RUN_LEVEL_BITS(0, 6, 9);
    *out++ = value;
    value = RUN_LEVEL_BITS(12, 1, 9);
    *out++ = value;
    value = RUN_LEVEL_BITS(11, 1, 9);
    *out++ = value;
    value = RUN_LEVEL_BITS(3, 2, 9);
    *out++ = value;
    value = RUN_LEVEL_BITS(1, 3, 9);
    *out++ = value;
    value = RUN_LEVEL_BITS(0, 5, 9);
    *out++ = value;
    value = RUN_LEVEL_BITS(10, 1, 9);
    *out++ = value;
    value = RUN_LEVEL_BITS(0, 3, 6);
    for (i = 8; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(4, 1, 6);
    for (i = 8; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(3, 1, 6);
    for (i = 8; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(0, 2, 5);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(2, 1, 5);
    for (i = 16; i != 0; i--) {
        *out++ = value;
    }
    value = RUN_LEVEL_BITS(1, 1, 4);
    for (i = 32; i != 0; i--) {
        *out++ = value;
    }
}

void mpvvlc_InitRunLevel(void)
{
    mpvvlc_InitIntRunLevel();
}

void mpvvlc_SetDflPtr(void)
{
    mpvvlc_mbai_i_0 = mpvvlt_mbai_i_0;
    mpvvlc_mbai_i_1 = mpvvlt_mbai_i_1;
    mpvvlc_mbai_p_0 = mpvvlt_mbai_p_0;
    mpvvlc_mbai_p_1 = mpvvlt_mbai_p_1;
    mpvvlc_mbai_b_0 = mpvvlt_mbai_b_0;
    mpvvlc_mbai_b_1 = mpvvlt_mbai_b_1;
    mpvvlc_p_mbtype = mpvvlt_p_mbtype;
    mpvvlc_b_mbtype = mpvvlt_b_mbtype;
    mpvvlc_motion_0 = mpvvlt_motion_0;
    mpvvlc_motion_1 = mpvvlt_motion_1;
    mpvvlc_cbp = mpvvlt_cbp;
    mpvvlc_y_dcsiz = (const Uint8 *)mpvvlt_y_dcsiz;
    mpvvlc_c_dcsiz = (const Uint8 *)mpvvlt_c_dcsiz;
    mpvvlc_run_level_0c = mpvvlt_run_level_0c;
    mpvvlc_run_level_0b = mpvvlt_run_level_0b;
    mpvvlc_run_level_0a = mpvvlt_run_level_0a;
    mpvvlc_run_level_1 = mpvvlt_run_level_1;
    mpvvlc_run_level_2 = mpvvlt_run_level_2;
    mpvvlc_run_level_4 = mpvvlt_run_level_4;
    mpvvlc_run_level_8 = mpvvlt_run_level_8;
}

void mpvvlc_SetupVlc(Uint8 *work)
{
    /* @hack spelling: end set to the work area's end before the first call,
     * where work + MPVVLC_WORK_SIZE as that call's argument would do */
    Uint8 *end = work + MPVVLC_WORK_SIZE;

    end = mpvvlc_SetVlcRunLevel(end);
    end = mpvvlc_SetVlcDcSiz(end);
    end = mpvvlc_SetVlcMotion(end);
    /* @hack redundant: a store of the result that nothing reads */
    end = mpvvlc_SetVlcMbType(end);
}

/* this and the three after it copy their family's tables into the bytes
 * below end. the tables' pointers then point at the copies. each returns a
 * pointer to the lowest byte it wrote */
Uint8 *mpvvlc_SetVlcRunLevel(Uint8 *end)
{
    /* @hack redundant: end's address, which nothing reads */
    Uint8 **end_address = &end;

    end -= sizeof(mpvvlt_run_level_8);
    mpvvlc_run_level_8 = (const Uint32 *)end;
    /* @hack spelling: each copy's destination read back from the mpvvlc_
     * pointer just set, where end would do */
    UTY_MemcpyDword((Uint32 *)mpvvlc_run_level_8, mpvvlt_run_level_8,
                    UTY_DWORDS(mpvvlt_run_level_8));
    end -= sizeof(mpvvlt_run_level_4);
    mpvvlc_run_level_4 = (const Uint16 *)end;
    UTY_MemcpyDword((Uint32 *)mpvvlc_run_level_4,
                    (const Uint32 *)mpvvlt_run_level_4,
                    UTY_DWORDS(mpvvlt_run_level_4));
    end -= sizeof(mpvvlt_run_level_2);
    mpvvlc_run_level_2 = (const Uint16 *)end;
    UTY_MemcpyDword((Uint32 *)mpvvlc_run_level_2,
                    (const Uint32 *)mpvvlt_run_level_2,
                    UTY_DWORDS(mpvvlt_run_level_2));
    end -= sizeof(mpvvlt_run_level_1);
    mpvvlc_run_level_1 = (const Uint16 *)end;
    UTY_MemcpyDword((Uint32 *)mpvvlc_run_level_1,
                    (const Uint32 *)mpvvlt_run_level_1,
                    UTY_DWORDS(mpvvlt_run_level_1));
    end -= sizeof(mpvvlt_run_level_0a);
    mpvvlc_run_level_0a = (const Uint16 *)end;
    UTY_MemcpyDword((Uint32 *)mpvvlc_run_level_0a,
                    (const Uint32 *)mpvvlt_run_level_0a,
                    UTY_DWORDS(mpvvlt_run_level_0a));
    end -= sizeof(mpvvlt_run_level_0b);
    mpvvlc_run_level_0b = (const Uint16 *)end;
    UTY_MemcpyDword((Uint32 *)mpvvlc_run_level_0b,
                    (const Uint32 *)mpvvlt_run_level_0b,
                    UTY_DWORDS(mpvvlt_run_level_0b));
    end -= sizeof(mpvvlt_run_level_0c);
    mpvvlc_run_level_0c = (const Uint16 *)end;
    UTY_MemcpyDword((Uint32 *)mpvvlc_run_level_0c,
                    (const Uint32 *)mpvvlt_run_level_0c,
                    UTY_DWORDS(mpvvlt_run_level_0c));
    return end;
}

Uint8 *mpvvlc_SetVlcDcSiz(Uint8 *end)
{
    /* @hack redundant: end's address, which nothing reads */
    Uint8 **end_address = &end;

    end -= sizeof(mpvvlt_y_dcsiz);
    mpvvlc_y_dcsiz = end;
    /* @hack spelling: each copy's destination read back from the mpvvlc_
     * pointer just set, where end would do */
    UTY_MemcpyDword((Uint32 *)mpvvlc_y_dcsiz, (const Uint32 *)mpvvlt_y_dcsiz,
                    UTY_DWORDS(mpvvlt_y_dcsiz));
    end -= sizeof(mpvvlt_c_dcsiz);
    mpvvlc_c_dcsiz = end;
    UTY_MemcpyDword((Uint32 *)mpvvlc_c_dcsiz, (const Uint32 *)mpvvlt_c_dcsiz,
                    UTY_DWORDS(mpvvlt_c_dcsiz));
    return end;
}

Uint8 *mpvvlc_SetVlcMotion(Uint8 *end)
{
    /* @hack redundant: end's address, which nothing reads */
    Uint8 **end_address = &end;

    end -= sizeof(mpvvlt_motion_0);
    mpvvlc_motion_0 = (const Sint16 *)end;
    /* @hack spelling: each copy's destination read back from the mpvvlc_
     * pointer just set, where end would do */
    UTY_MemcpyDword((Uint32 *)mpvvlc_motion_0, (const Uint32 *)mpvvlt_motion_0,
                    UTY_DWORDS(mpvvlt_motion_0));
    end -= sizeof(mpvvlt_motion_1);
    mpvvlc_motion_1 = (const Sint16 *)end;
    UTY_MemcpyDword((Uint32 *)mpvvlc_motion_1, (const Uint32 *)mpvvlt_motion_1,
                    UTY_DWORDS(mpvvlt_motion_1));
    return end;
}

Uint8 *mpvvlc_SetVlcMbType(Uint8 *end)
{
    /* @hack redundant: end's address, which nothing reads */
    Uint8 **end_address = &end;

    end -= sizeof(mpvvlt_p_mbtype);
    mpvvlc_p_mbtype = (const Sint16 *)end;
    /* @hack spelling: each copy's destination read back from the mpvvlc_
     * pointer just set, where end would do */
    UTY_MemcpyDword((Uint32 *)mpvvlc_p_mbtype, (const Uint32 *)mpvvlt_p_mbtype,
                    UTY_DWORDS(mpvvlt_p_mbtype));
    end -= sizeof(mpvvlt_b_mbtype);
    mpvvlc_b_mbtype = (const Sint16 *)end;
    UTY_MemcpyDword((Uint32 *)mpvvlc_b_mbtype, (const Uint32 *)mpvvlt_b_mbtype,
                    UTY_DWORDS(mpvvlt_b_mbtype));
    return end;
}
