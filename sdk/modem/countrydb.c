#include <sg_xpt.h>
#include "countrydb.h"

/* the database holds its count of countries at COUNT_OFFSET, and each
 * country its code at CODE_OFFSET */
#define COUNT_OFFSET 4
#define CODE_OFFSET 2
#define LIMIT_BYTES 4

void CountryDbGetData(Uint8 code, Uint8 *database, struct country_st *country,
                      struct Slimit *limits)
{
    /* @hack type: countries, record_size and limit_size are volatile, where
     * plain locals would do */
    volatile Uint8 countries = 0;
    volatile Uint8 record_size = 0;
    Uint32 entry = 0;
    /* @hack redundant: default_code, which nothing uses, where a step past
     * its byte would do */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    volatile Uint8 default_code = 0xff;
    volatile Uint8 limit_size = 0;
    Uint16 i;
    Uint8 limit_count;
    Uint8 *source = database;
    struct Slimit *limit;

    if (source) {
        source += COUNT_OFFSET;
        countries = *source++;
        default_code = *source++;
        record_size = *source++;
        while (entry < countries && *source != code) {
            for (i = 0; i < record_size + 1; i++) {
                source++;
            }
            limit_size = *source;
            source++;
            source += limit_size;
            source += CODE_OFFSET;
            entry++;
        }
        /* @bug memory: a code the database doesn't hold takes the reader past
         * the database's end. it tests a byte there as a code, and copies
         * limits from the bytes after it (0x0caef8a0) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: a code the database doesn't
         * hold sets nothing */
        if (entry == countries) {
            return;
        }
#endif
        if (*source == code) {
            source++;
            getCountryCodeFromArrayAndConvert(country, source);
        }
        for (i = 0; i < record_size; i++) {
            source++;
        }
        limit_size = *source;
        source++;
        /* @hack spelling: limit_count set to limit_size and then divided,
         * where one assignment would do */
        limit_count = limit_size;
        limit_count = limit_count / LIMIT_BYTES;
        limit = limits;
        for (i = 0; i < limit_count; i++) {
            limit->reg = *source;
            source++;
            limit->initial = *source++;
            limit->minimum = *source++;
            limit->maximum = *source++;
            limit++;
        }
    }
}

void copyUINT8(Uint8 *dest, Uint8 *source, Uint32 length)
{
    Uint32 i;
    for (i = 0; i < length; i++) {
        *dest++ = *source++;
    }
}

void LeToBeCopyUINT16(Uint8 *dest, Uint8 *source)
{
    source++;
    *dest++ = *source--;
    *dest = *source;
}

void LeToBeCopyUINT32(Uint8 *dest, Uint8 *source)
{
    Uint32 i;
    source += 3;
    for (i = 0; i < 4; i++) {
        *dest++ = *source--;
    }
}

Uint8 *getCountryCodeFromArrayAndConvert(struct country_st *country,
                                         Uint8 *source)
{
#define READ_BYTE(field) (copyUINT8(&(field), source, 1), source += 1)
#define READ_WORD(field)                                                       \
    (LeToBeCopyUINT16((Uint8 *)&(field), source), source += 2)
#define READ_LONG(field)                                                       \
    (LeToBeCopyUINT32((Uint8 *)&(field), source), source += 4)
#define READ_INTERVAL(field)                                                   \
    (READ_WORD((field).maximum), READ_WORD((field).minimum))
#define READ_CADENCE(field)                                                    \
    (READ_INTERVAL((field).on1), READ_INTERVAL((field).off1),                  \
     READ_INTERVAL((field).on2), READ_INTERVAL((field).off2))
    copyUINT8(country->country_name, source, sizeof(country->country_name));
    source += sizeof(country->country_name);
    READ_WORD(country->unk_20);
    READ_BYTE(country->tone_level);
    READ_BYTE(country->tone_table);
    READ_BYTE(country->tone_on);
    READ_BYTE(country->tone_off);
    READ_WORD(country->pulse_break);
    READ_WORD(country->fast_pulse_break);
    READ_WORD(country->pulse_pause);
    READ_WORD(country->fast_pulse_pause);
    READ_BYTE(country->pulse_format);
    READ_CADENCE(country->busy);
    READ_CADENCE(country->ringback);
    READ_CADENCE(country->unobtainable);
    READ_CADENCE(country->congested);
    READ_CADENCE(country->dialtone);
    READ_LONG(country->initial_dialtone);
    READ_LONG(country->subsequent_dialtone);
    READ_BYTE(country->pump_option_1);
    READ_BYTE(country->pump_option_2);
    READ_BYTE(country->s35_default);
    READ_WORD(country->ring_off);
    READ_WORD(country->ring_on);
    READ_WORD(country->blacklist_options);
    READ_WORD(country->delay_after_failures);
    READ_WORD(country->delay_seconds);
    READ_WORD(country->bar_after_failures);
    READ_WORD(country->bar_after_minutes);
    READ_WORD(country->expire_after_minutes);
    READ_WORD(country->count_failure_events);
    READ_WORD(country->reserved_failure_events);
    READ_WORD(country->clear_number_events);
    READ_WORD(country->clear_all_events);
    READ_WORD(country->redial_delay);
    READ_WORD(country->answer_tone_time);
    READ_WORD(country->blind_dial_delay);
    READ_WORD(country->mute_delay);
    READ_WORD(country->unmute_delay);
    READ_WORD(country->hook_flash_time);
    READ_WORD(country->options);
    READ_WORD(country->reset_flags);
    READ_WORD(country->handshake_tones);
    return source;
#undef READ_BYTE
#undef READ_WORD
#undef READ_LONG
#undef READ_INTERVAL
#undef READ_CADENCE
}
