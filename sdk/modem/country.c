#include <sg_xpt.h>
#include "country.h"
#include "countrydb.h"
#include "modemdef.h"
#include "target.h"

struct country_st *cur_cnt;
struct Slimit *cur_slimits;
static Uint8 country_code;
static Uint8 dialtone_flags;
static Uint8 dialtone_state;
static Uint8 dialtone_count;
static Uint8 progress_flags;
static Uint8 busy_state;
static Uint8 ringback_state;
static Uint8 ringback_count;
static Uint8 congested_state;
static Uint8 congested_count;
static Uint8 unobtainable_state;
static Uint8 unobtainable_count;

static const Uint8 pulse_digits_shifted[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
static const Uint8 pulse_digits_reversed[10] = {0, 9, 8, 7, 6, 5, 4, 3, 2, 1};

/* the whole cycles of a busy, congested or unobtainable cadence that make
 * the tone */
#define TONE_REPEATS 3
#define WITHIN(ticks, interval)                                                \
    ((ticks) <= (interval).maximum && (ticks) >= (interval).minimum)

void country_InitCountry(Uint8 code)
{
    country_code = code;
    target_set_country(code);
    targetGetCountryInfo(code, &cur_cnt, &cur_slimits);
}

char country_CheckCountry(Uint8 code)
{
    if (!target_check_country(code)) {
        return 0;
    }
    return 1;
}

void country_CheckSregLimits(Uint8 reg, Uint8 *value)
{
    struct Slimit *limit;
    int current;
    int bound;

    for (limit = cur_slimits; limit->reg != END_LIMITS; limit++) {
        if (limit->reg == reg) {
            /* @hack type: the value and its bounds converted to Sint8 and
             * compared as Uint8, where Uint8 comparisons would do */
            bound = (Sint8)limit->minimum;
            current = (Sint8)*value;
            *value = (Uint8)current > (Uint8)bound ? current : bound;
            bound = (Sint8)limit->maximum;
            current = (Sint8)*value;
            /* @hack redundant: a dead test of limit */
            if (0) {
                if (limit) {
                }
            }
            *value = (Uint8)current < (Uint8)bound ? current : bound;
            return;
        }
    }
}

void country_CheckAllSregLimits(Uint8 *registers)
{
    struct Slimit *limit;
    Uint8 *reg;
    Uint8 value;
    Uint8 bound;

    for (limit = cur_slimits; limit->reg != END_LIMITS; limit++) {
        reg = registers + limit->reg;
        bound = limit->minimum;
        value = *reg;
        if (value <= bound) {
            value = bound;
        }
        /* @hack redundant: a dead test of bound */
        if (0) {
            if (bound) {
            }
        }
        /* @hack redundant: a dead test of limit->reg */
        if (0) {
            if (limit->reg == END_LIMITS) {
            }
        }
        /* @hack spelling: the assignment to bound inside the test, where a
         * statement of its own would do */
        if (value >= (bound = limit->maximum)) {
            value = bound;
        }
        *reg = value;
        /* @hack redundant: the continue that ends the loop's body */
        /* NOLINTNEXTLINE(readability-redundant-control-flow) */
        continue;
    }
}

void country_CheckSregDefault(Uint8 reg, Uint8 *value)
{
    struct Slimit *limit;
    for (limit = cur_slimits; limit->reg != END_LIMITS; limit++) {
        if (limit->reg == reg) {
            *value = limit->initial;
            return;
        }
    }
}

void country_CheckAllSregDefaults(Uint8 *registers)
{
    struct Slimit *limit;
    registers[35] = cur_cnt->s35_default;
    registers[57] = cur_cnt->s35_default;
    for (limit = cur_slimits; limit->reg != END_LIMITS; limit++) {
        registers[limit->reg] = limit->initial;
    }
}

void country_InitDialtoneDetection(void)
{
    dialtone_flags = 0;
    dialtone_state = 0;
    dialtone_count = 0;
}

void country_InitCallProgress(void)
{
    progress_flags = 0;
    busy_state = 0;
    ringback_state = 0;
    ringback_count = 0;
    congested_state = 0;
    congested_count = 0;
    unobtainable_state = 0;
    unobtainable_count = 0;
}

/* the bits of progress_flags and dialtone_flags, a pair for each test, that
 * mark the first and second tone of its cadence as seen */
#define BUSY_FIRST 0x01
#define BUSY_SECOND 0x02
#define CONGESTED_FIRST 0x04
#define CONGESTED_SECOND 0x08
#define RINGBACK_FIRST 0x10
#define RINGBACK_SECOND 0x20
#define UNOBTAINABLE_FIRST 0x40
#define UNOBTAINABLE_SECOND 0x80
#define DIALTONE_FIRST 0x01
#define DIALTONE_SECOND 0x02

/* step a call-progress test by the tone or silence that has just ended. tone
 * and length are as country.h's tests take them. state 0 waits for either
 * tone of cadence. state 1 and 3 follow its first and second tone, and 2 and
 * 4 the silence after each. where complete holds, a silence in its interval
 * counts a cycle in count and clears both bits of flags. the macro evaluates
 * complete only there. at any state but 0, a tone or silence outside the
 * interval its state waits for is bad. so is any tone that ends at state 1 or
 * 3. a bad one clears count and both bits. after a bad tone the test goes
 * back to state 0, and after a bad silence it waits for the same tone again */
/* @hack spelling: a macro, expanded in each test, where a function that
 * each test calls would do */
/* @hack spelling: the if bare, where a do-while around it would do */
#define CADENCE_STEP(tone, length, cadence, state, count, flags, first,        \
                     second, complete)                                         \
    if (!(tone)) {                                                             \
        switch (state) {                                                       \
        case 0:                                                                \
            if (WITHIN(length, (cadence).on1)) {                               \
                (flags) |= (first);                                            \
                (state) = 1;                                                   \
            } else if (WITHIN(length, (cadence).on2)) {                        \
                (flags) |= (second);                                           \
                (state) = 3;                                                   \
            }                                                                  \
            break;                                                             \
        case 2:                                                                \
            if (WITHIN(length, (cadence).on2)) {                               \
                (flags) |= (second);                                           \
                (state) = 3;                                                   \
            } else {                                                           \
                (flags) &= ~((first) | (second));                              \
                (count) = 0;                                                   \
                (state) = 0;                                                   \
            }                                                                  \
            break;                                                             \
        case 4:                                                                \
            if (WITHIN(length, (cadence).on1)) {                               \
                (flags) |= (first);                                            \
                (state) = 1;                                                   \
            } else {                                                           \
                (flags) &= ~((first) | (second));                              \
                (count) = 0;                                                   \
                (state) = 0;                                                   \
            }                                                                  \
            break;                                                             \
        default:                                                               \
            (flags) &= ~((first) | (second));                                  \
            (count) = 0;                                                       \
            (state) = 0;                                                       \
        }                                                                      \
    } else {                                                                   \
        switch (state) {                                                       \
        case 1:                                                                \
            if (WITHIN(length, (cadence).off1)) {                              \
                if (complete) {                                                \
                    (count)++;                                                 \
                    (flags) &= ~((first) | (second));                          \
                }                                                              \
                (state) = 2;                                                   \
            } else {                                                           \
                (flags) &= ~((first) | (second));                              \
                (count) = 0;                                                   \
                (state) = 4;                                                   \
            }                                                                  \
            break;                                                             \
        case 3:                                                                \
            if (WITHIN(length, (cadence).off2)) {                              \
                if (complete) {                                                \
                    (count)++;                                                 \
                    (flags) &= ~((first) | (second));                          \
                }                                                              \
                (state) = 4;                                                   \
            } else {                                                           \
                (flags) &= ~((first) | (second));                              \
                (count) = 0;                                                   \
                (state) = 2;                                                   \
            }                                                                  \
        }                                                                      \
    }

char country_BusyTest(Uint8 tone, Uint32 length)
{
    if (cur_cnt->options & DETECT_BUSY) {
        /* @hack spelling: the second tone's bit tested first, where the
         * first's would do */
        CADENCE_STEP(tone, length, cur_cnt->busy, busy_state, busy_cntr,
                     progress_flags, BUSY_FIRST, BUSY_SECOND,
                     (progress_flags & BUSY_SECOND) &&
                         (progress_flags & BUSY_FIRST));
        if (busy_cntr == TONE_REPEATS) {
            return 1;
        }
    }
    return 0;
}

char country_RingbackTest(Uint8 tone, Uint32 length)
{
    if (cur_cnt->options & DETECT_RINGBACK) {
        /* a cadence whose two silences share bounds counts as one ring
         * repeated: each silence within them ends a cycle, after either
         * tone */
        CADENCE_STEP(tone, length, cur_cnt->ringback, ringback_state,
                     ringback_count, progress_flags, RINGBACK_FIRST,
                     RINGBACK_SECOND,
                     ((progress_flags & RINGBACK_FIRST) &&
                      (progress_flags & RINGBACK_SECOND)) ||
                         (cur_cnt->ringback.off1.maximum ==
                              cur_cnt->ringback.off2.maximum &&
                          cur_cnt->ringback.off1.minimum ==
                              cur_cnt->ringback.off2.minimum));
        if (ringback_count >= 1) {
            ringback_count = 0;
            return 1;
        }
    }
    return 0;
}

char country_CongestedTest(Uint8 tone, Uint32 length)
{
    if (cur_cnt->options & DETECT_CONGESTED) {
        CADENCE_STEP(tone, length, cur_cnt->congested, congested_state,
                     congested_count, progress_flags, CONGESTED_FIRST,
                     CONGESTED_SECOND,
                     (progress_flags & CONGESTED_FIRST) &&
                         (progress_flags & CONGESTED_SECOND));
        if (congested_count == TONE_REPEATS) {
            return 1;
        }
    }
    return 0;
}

char country_UnobtainableTest(Uint8 tone, Uint32 length)
{
    if (cur_cnt->options & DETECT_UNOBTAINABLE) {
        /* @hack spelling: the second tone's bit tested first, where the
         * first's would do */
        CADENCE_STEP(tone, length, cur_cnt->unobtainable, unobtainable_state,
                     unobtainable_count, progress_flags, UNOBTAINABLE_FIRST,
                     UNOBTAINABLE_SECOND,
                     (progress_flags & UNOBTAINABLE_SECOND) &&
                         (progress_flags & UNOBTAINABLE_FIRST));
        if (unobtainable_count == TONE_REPEATS) {
            return 1;
        }
    }
    return 0;
}

char country_DialtoneTest(Uint8 tone, Uint32 length)
{
    if (cur_cnt->dialtone.on1.maximum > 0) {
        CADENCE_STEP(tone, length, cur_cnt->dialtone, dialtone_state,
                     dialtone_count, dialtone_flags, DIALTONE_FIRST,
                     DIALTONE_SECOND,
                     (dialtone_flags & DIALTONE_FIRST) &&
                         (dialtone_flags & DIALTONE_SECOND));
        if (dialtone_count == 1) {
            return 1;
        }
    }
    return 0;
}

Uint8 country_convert_pulse_format(Uint8 digit)
{
    switch (cur_cnt->pulse_format) {
    case 0:
        break;
    case 1:
        return pulse_digits_shifted[digit];
    case 2:
        return pulse_digits_reversed[digit];
    }
    return digit;
}
