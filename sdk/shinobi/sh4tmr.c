/* SH-4 timer unit: a free-running count, a one-shot interrupt and a
 * general-purpose timer */
#include <sg_xpt.h>
#include <shinobi.h>

/* the timer unit's registers, by their SH-4 names: the output control and
 * start registers, then each timer's constant, count and control registers */
/* @hack type: TSTR, TCR1 and TCR2, the 8- and 16-bit registers the code
 * reads, signed, where unsigned would do */
#define TOCR (*(volatile Uint8 *)0xffd80000)
#define TSTR (*(volatile Sint8 *)0xffd80004)
#define TCOR0 (*(volatile Uint32 *)0xffd80008)
#define TCNT0 (*(volatile Uint32 *)0xffd8000c)
#define TCR0 (*(volatile Uint16 *)0xffd80010)
#define TCOR1 (*(volatile Uint32 *)0xffd80014)
#define TCNT1 (*(volatile Uint32 *)0xffd80018)
#define TCR1 (*(volatile Sint16 *)0xffd8001c)
#define TCOR2 (*(volatile Uint32 *)0xffd80020)
#define TCNT2 (*(volatile Uint32 *)0xffd80024)
#define TCR2 (*(volatile Sint16 *)0xffd80028)

/* each timer's start bit, in TSTR */
#define STR0 0x1
#define STR1 0x2
#define STR2 0x4
/* the bits of TCR0 to TCR2 */
#define UNF 0x100 /* set at an underflow, until cleared */
#define UNIE 0x20 /* lets an underflow raise the timer's interrupt */
#define TPSC 0x7 /* the timer's clock, a SY_TMR_CLOCK */

/* IPRA, the interrupt priority register, and its fields for timers 1 and 2.
 * syTmrInt1SetEx and syTmrGenSetInt set each to 15, the highest priority */
/* @hack type: IPRA signed, as TSTR, TCR1 and TCR2 are, where unsigned would
 * do */
#define IPRA (*(volatile Sint16 *)0xffd00004)
#define IPRA_TMU1 0x0f00
#define IPRA_TMU2 0x00f0

/* timers 0 and 2 reload from COUNT_MAX, and a reading is COUNT_MAX less the
 * timer's count */
#define COUNT_MAX 0xffffffff
/* a cycle of the 50 MHz peripheral clock, in hundredths of a microsecond */
#define CYCLE_LENGTH 2
/* the hundredths in a microsecond */
#define HUNDREDTHS 100
/* a count's length at each clock: the clock's divisor in cycles */
#define COUNT_LENGTH_P4 (4 * CYCLE_LENGTH)
#define COUNT_LENGTH_P16 (16 * CYCLE_LENGTH)
#define COUNT_LENGTH_P64 (64 * CYCLE_LENGTH)
#define COUNT_LENGTH_P256 (256 * CYCLE_LENGTH)
#define COUNT_LENGTH_P1024 (1024 * CYCLE_LENGTH)

typedef void (*timer_func)(void *arg);

/* the member exports this name, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the library's version and build time */
const char *const sygTmrBuild = "\nsyTmr Ver 1.04 Build:Apr 26 1999 14:43:51\n";
/* NOLINTEND(misc-use-internal-linkage) */

static timer_func timer_one_shot_func;
static void *timer_one_shot_arg;
static timer_func timer_gen_func;
static void *timer_gen_arg;

static void timer_one_shot_handler(void);
static void timer_gen_handler(void);

void syTmrInit(void)
{
    TOCR = 0;
    TSTR &= ~STR0;
    TCR0 = SYD_TMR_CLOCK_P64;
    TCOR0 = COUNT_MAX;
    TCNT0 = COUNT_MAX;
    TSTR |= STR0;
}

Uint32 syTmrGetCount(void)
{
    /* @hack type: TCNT0 cast to its own type */
    return COUNT_MAX - (Uint32)TCNT0;
}

Uint32 syTmrDiffCount(Uint32 count1, Uint32 count2)
{
    return count2 - count1;
}

/* count * COUNT_LENGTH_P64 / HUNDREDTHS, with count / HUNDREDTHS and
 * count % HUNDREDTHS scaled apart. neither product overflows unless the
 * result does */
Uint32 syTmrCountToMicro(Uint32 count)
{
    return ((count / HUNDREDTHS) * COUNT_LENGTH_P64) +
           ((count % HUNDREDTHS) * COUNT_LENGTH_P64 / HUNDREDTHS);
}

Uint32 syTmrMicroToCount(Uint32 micro)
{
    return ((micro / COUNT_LENGTH_P64) * HUNDREDTHS) +
           ((micro % COUNT_LENGTH_P64) * HUNDREDTHS / COUNT_LENGTH_P64);
}

Sint32 syTmrInt1Set(Uint32 count, void (*func)(void *), void *arg)
{
    return syTmrInt1SetEx(count, func, arg, SYD_TMR_CLOCK_P64);
}

Sint32 syTmrInt1SetEx(Uint32 count, void (*func)(void *), void *arg,
                      SY_TMR_CLOCK clock)
{
    if (TSTR & STR1) {
        return SYD_TMR_ERR_EXIST;
    }
    TOCR = 0;
    TSTR &= ~STR1;
    TCR1 = clock;
    if (func != NULL) {
        USER_VECT(SYD_INT_EVENT_TUNI1) = (Uint32)timer_one_shot_handler;
        timer_one_shot_func = func;
        timer_one_shot_arg = arg;
        TCR1 |= UNIE;
        IPRA = (IPRA & ~IPRA_TMU1) | IPRA_TMU1;
        TCR1 &= ~UNF;
    }
    TCOR1 = count;
    TCNT1 = count;
    TSTR |= STR1;
    if (func == NULL) {
        while (!(TCR1 & UNF)) {
        }
        TSTR &= ~STR1;
        TCR1 &= ~UNF;
    }
    return SYD_TMR_ERR_OK;
}

void syTmrInt1Cancel(void)
{
    TSTR &= ~STR1;
    TCR1 &= ~UNIE;
}

static void timer_one_shot_handler(void)
{
    TSTR &= ~STR1;
    TCR1 &= ~UNIE;
    TCR1 &= ~UNF;
    timer_one_shot_func(timer_one_shot_arg);
}

static void timer_gen_handler(void)
{
    TSTR &= ~STR2;
    TCR2 &= ~UNIE;
    TCR2 &= ~UNF;
    timer_gen_func(timer_gen_arg);
}

void syTmrGenStart(void)
{
    TOCR = 0;
    TSTR |= STR2;
}

void syTmrGenStop(void)
{
    TSTR &= ~STR2;
}

void syTmrGenSetInt(void (*func)(void *), void *arg)
{
    Sint8 started;

    started = TSTR;
    TSTR &= ~STR2;
    if (func != NULL) {
        USER_VECT(SYD_INT_EVENT_TUNI2) = (Uint32)timer_gen_handler;
        timer_gen_func = func;
        timer_gen_arg = arg;
        TCR2 |= UNIE;
        IPRA = (IPRA & ~IPRA_TMU2) | IPRA_TMU2;
        TCR2 &= ~UNF;
    }
    TSTR = started;
    if (func == NULL) {
        TOCR = 0;
        TSTR |= STR2;
        while (!(TCR2 & UNF)) {
        }
        TSTR &= ~STR2;
        TCR2 &= ~UNF;
    }
}

void syTmrGenCancelInt(void)
{
    Sint8 started;

    started = TSTR;
    TSTR &= ~STR2;
    TCR2 &= ~UNIE;
    TSTR = started;
}

void syTmrGenSetClock(SY_TMR_CLOCK clock)
{
    Sint8 started;

    if (clock != SYD_TMR_CLOCK_P4 && clock != SYD_TMR_CLOCK_P16 &&
        clock != SYD_TMR_CLOCK_P64 && clock != SYD_TMR_CLOCK_P256 &&
        clock != SYD_TMR_CLOCK_P1024) {
        clock = SYD_TMR_CLOCK_P64;
    }
    started = TSTR;
    TSTR &= ~STR2;
    TCR2 = (TCR2 & ~TPSC) | clock;
    TSTR = started;
}

void syTmrGenSetCount(Uint32 count)
{
    Sint8 started;

    started = TSTR;
    TSTR &= ~STR2;
    TCOR2 = COUNT_MAX;
    TCNT2 = -count;
    TSTR = started;
}

Uint32 syTmrGenGetCount(void)
{
    /* @hack type: TCNT2 cast to its own type */
    return COUNT_MAX - (Uint32)TCNT2;
}

Uint32 syTmrGenCountToMicro(Uint32 count, SY_TMR_CLOCK clock)
{
    Uint32 length;

    switch (clock) {
    case SYD_TMR_CLOCK_P4:
        length = COUNT_LENGTH_P4;
        break;
    case SYD_TMR_CLOCK_P16:
        length = COUNT_LENGTH_P16;
        break;
    case SYD_TMR_CLOCK_P256:
        length = COUNT_LENGTH_P256;
        break;
    case SYD_TMR_CLOCK_P1024:
        length = COUNT_LENGTH_P1024;
        break;
    default:
        length = COUNT_LENGTH_P64;
        break;
    }
    return ((count / HUNDREDTHS) * length) +
           ((count % HUNDREDTHS) * length / HUNDREDTHS);
}

Uint32 syTmrGenMicroToCount(Uint32 micro, SY_TMR_CLOCK clock)
{
    Uint32 length;

    switch (clock) {
    case SYD_TMR_CLOCK_P4:
        length = COUNT_LENGTH_P4;
        break;
    case SYD_TMR_CLOCK_P16:
        length = COUNT_LENGTH_P16;
        break;
    case SYD_TMR_CLOCK_P256:
        length = COUNT_LENGTH_P256;
        break;
    case SYD_TMR_CLOCK_P1024:
        length = COUNT_LENGTH_P1024;
        break;
    default:
        length = COUNT_LENGTH_P64;
        break;
    }
    return ((micro / length) * HUNDREDTHS) +
           ((micro % length) * HUNDREDTHS / length);
}

Uint32 syTmrGenDiffCount(Uint32 count1, Uint32 count2)
{
    return syTmrDiffCount(count1, count2);
}
