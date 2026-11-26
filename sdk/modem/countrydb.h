/* countrydb: a country's rules for dialing, call-progress tones and dial
 * restrictions, and the database of them */
#ifndef MODEM_COUNTRYDB_H
#define MODEM_COUNTRYDB_H
#include <sg_xpt.h>

/* an S-register's initial value and bounds. a list of them ends at a reg of
 * END_LIMITS */
struct Slimit {
    Uint8 reg;
    Uint8 initial;
    Uint8 minimum;
    Uint8 maximum;
};
#define END_LIMITS 0xff

/* the bounds of a tone's or a silence's length, in 10 ms ticks */
struct tone_interval {
    Uint16 maximum;
    Uint16 minimum;
};

/* two tones, each with the silence after it */
struct tone_cadence {
    struct tone_interval on1;
    struct tone_interval off1;
    struct tone_interval on2;
    struct tone_interval off2;
};

/* the bits of a country's options */
#define ALLOW_TONE_DIALING 0x0001 /* without it, the modem dials by pulses */
#define ALLOW_PULSE_DIALING 0x0002 /* without it, by tones */
#define ALLOW_BLIND_DIALING 0x0004 /* without it, it waits for a dial tone */
#define ALLOW_AUTO_PULSE 0x0008
#define CHECK_LINE_IN_USE 0x0010 /* ATA and ATD test the line first */
#define SEND_CALLING_TONE 0x0020
#define KEEP_CALLING_TONE 0x0040 /* the dial string's ^ can't turn it off */
#define ALLOW_S35 0x0080 /* ATS35= takes a value */
#define MUTE_PULSE_DIALING 0x0100
#define COUNTRY_GUARD_TONE 0x0200 /* the country, not S23, picks the tone */
#define SECOND_GUARD_TONE 0x0400 /* the country's pick: the second tone */
#define DETECT_BUSY 0x0800 /* the DETECT_ bits turn call-progress tests on */
#define DETECT_RINGBACK 0x1000
#define DETECT_CONGESTED 0x2000
#define DETECT_UNOBTAINABLE 0x4000
#define OFF_HOOK_IS_H9 0x8000 /* ATH takes 0 and 9, and 9 goes off hook */

/* the bits of a country's reset_flags */
#define RAISE_S0_1 0x0001 /* an S0 of 1 becomes 2 */
#define EARTH_RELAY_DIALING                                                    \
    0x0002 /* > grounds the earth relay for S59 ticks                          \
            */
#define BLIND_DIAL_DELAY 0x0004 /* a blind dial waits blind_dial_delay */
#define W_WAITS_S6 0x0008 /* W waits S6 seconds for a tone, not S7 */
#define FORCE_DTR_HANGUP 0x0010 /* &D0 and &D1 become &D2 */
#define TAKE_OVER_LINE 0x0020 /* a bare ATA or ATD needs a line in use */
#define NO_STORED_LENGTH_CHECK                                                 \
    0x0040 /* ATDS doesn't check a number's length                             \
            */
#define SHORT_RING_PULSES 0x0200 /* a short pulse can count as a ring */
#define REFUSE_PTT_TESTS 0x0400 /* AT%H fails */
#define IGNORE_HOOK_FLASH 0x0800 /* the dial string's ! does nothing */
#define STRETCH_PULSES 0x1000 /* each break and make grows by 6% */
#define RAISE_S0_2 0x2000 /* an S0 of 2 becomes 3 */

struct country_st {
    Uint8 country_name[32];
    Uint16 unk_20;
    Uint8 tone_level;
    Uint8 tone_table;
    Uint8 tone_on;
    Uint8 tone_off;
    Uint16 pulse_break;
    Uint16 fast_pulse_break;
    Uint16 pulse_pause;
    Uint16 fast_pulse_pause;
    Uint8 pulse_format;
    struct tone_cadence busy;
    struct tone_cadence ringback;
    struct tone_cadence unobtainable;
    struct tone_cadence congested;
    struct tone_cadence dialtone;
    Uint32 initial_dialtone;
    Uint32 subsequent_dialtone;
    Uint8 pump_option_1;
    Uint8 pump_option_2;
    Uint8 s35_default;
    Uint16 ring_off;
    Uint16 ring_on;
    Uint16 blacklist_options;
    Uint16 delay_after_failures;
    Uint16 delay_seconds;
    Uint16 bar_after_failures;
    Uint16 bar_after_minutes;
    Uint16 expire_after_minutes;
    Uint16 count_failure_events;
    Uint16 reserved_failure_events;
    Uint16 clear_number_events;
    Uint16 clear_all_events;
    Uint16 redial_delay;
    Uint16 answer_tone_time;
    Uint16 blind_dial_delay;
    Uint16 mute_delay;
    Uint16 unmute_delay;
    Uint16 hook_flash_time;
    Uint16 options;
    Uint16 reset_flags;
    Uint16 handshake_tones;
};

/* the database's bytes, with each word and long big-endian:
 *
 *   4      bytes the reader skips
 *   1      the count of countries
 *
 * and then for each country:
 *
 *   1      a byte nothing uses
 *   1      record_size, which the reader takes from the first country alone
 *   1      the country's code
 *   ...    record_size bytes of fields, in struct country_st's order
 *   1      the size of the limits, in bytes
 *   ...    the limits, 4 bytes each, in struct Slimit's order
 */

/* decode country code's fields from database into *country, and its limits
 * into limits. limits needs room for them all. code must be one the
 * database holds */
void CountryDbGetData(Uint8 code, Uint8 *database, struct country_st *country,
                      struct Slimit *limits);
/* decode a country's fields at source into *country, and return the address
 * past them */
Uint8 *getCountryCodeFromArrayAndConvert(struct country_st *country,
                                         Uint8 *source);
void copyUINT8(Uint8 *dest, Uint8 *source, Uint32 length);
/* copy the 2 or 4 bytes at source to dest in reverse order. a byte swap
 * undoes itself, so the copy also turns the database's big-endian fields into
 * the SH-4's order */
void LeToBeCopyUINT16(Uint8 *dest, Uint8 *source);
void LeToBeCopyUINT32(Uint8 *dest, Uint8 *source);
#endif
