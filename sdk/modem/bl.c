#include <sg_xpt.h>
#include "atcom.h"
#include "bl.h"
#include "country.h"
#include "countrydb.h"
#include "messages.h"
#include "modemdef.h"
#include "target.h"
#include "util_mod.h"

#define ENTRY_COUNT 40
#define DIGITS 40
#define TIME_SLOTS 3
#define ENTRY_UNUSED '*'
#define ENTRY_DELAY 'D'
#define ENTRY_BAR 'B'
/* DELAY_MODE and BAR_MODE each restrict nothing at OFF. GLOBAL counts failed
 * calls over every number, and PER_NUMBER for each number in entries */
#define OFF 0
#define GLOBAL 1
#define PER_NUMBER 2
#define OPTIONS (cur_cnt->blacklist_options)
#define DELAY_MODE (OPTIONS & 3)
#define BAR_MODE ((OPTIONS & 0x0c) >> 2)
#define TIME_AT_FAILURE 0x10
#define RECORD_FAILURE_TIMES 0x20
#define KEEP_AT_LIMIT 0x40
#define SILENT_DELAY 0x80
#define EXPIRE_FROM_FIRST 0x100
#define DELAY_FROM_FIRST 0x200
/* targetGetRealTimeClock counts seconds */
#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_DAY 86400

/* a ring of the times of a number's calls, from at[first] to at[last] */
struct call_times {
    Uint32 at[TIME_SLOTS];
    Uint8 first, last, slots;
};
struct dial_entry {
    char state;
    /* text holds a number's digits and the ':' that ends them. failures
     * shares text[DIGITS], so for a number of DIGITS digits, add_number's 0
     * in failures replaces the ':'. find_number never finds such a number */
    union {
        char text[DIGITS + 1];
        struct {
            char digits[DIGITS];
            Uint8 failures;
        } record;
    } number;
    struct call_times calls;
};
struct call_count {
    Uint32 time;
    /* @hack type: the count held in a char, where a Uint8 would do */
    char count;
};
static struct dial_entry entries[ENTRY_COUNT];
static Uint32 remaining_delay;
static struct call_count calls;
static Uint8 current;
/* @hack type: the parts of a time held in chars, where Uint8s would do */
static char days, hours, minutes, seconds;
static char display_byte;
static Uint8 dial_number[DIGITS + 1];

static void reset_all(void);
static void reset_current(void);
static void clear_entry(Uint8 index);
static void discard_unused(void);
static char find_number(Uint8 *number, Uint8 *index);
static void add_number(Uint8 *number, Uint8 *index);
static char is_barred(Uint8 *number);
static char is_delayed(Uint8 *number);
static char calls_expired(void);
static char entry_expired(Uint8 index);
static void count_failure(void);
static void display_entries(void);
static char dial_digit(Uint8 value);
static void split_time(Uint32 value);
static Uint8 expire_times(Uint8 index, Uint32 now);

/* @hack spelling: SEND_DECIMAL's and SEND_TIME's statements bare, where a
 * do-while (0) around each would do */
#define SEND_DECIMAL(value, width, text)                                       \
    convert_value_to_ascii((value), 10, (width), (text));                      \
    send_message(text)
#define SEND_TIME(text)                                                        \
    SEND_DECIMAL(hours, 2, text);                                              \
    send_message(":");                                                         \
    SEND_DECIMAL(minutes, 2, text);                                            \
    send_message(":");                                                         \
    SEND_DECIMAL(seconds, 2, text)

void BL_Dial_Delay(void)
{
    char text[8];
    split_time(remaining_delay);
    if (!(OPTIONS & SILENT_DELAY)) {
        send_cr_lf();
        send_message("DELAYED ");
        SEND_TIME(text);
        send_cr_lf();
    }
}

void BL_Reset(void)
{
    reset_all();
}

Uint8 BL_Checking(Uint8 *number)
{
    Uint8 read;
    Uint8 saved;
    Uint32 now;
    Uint8 slot;

    read = 0;
    saved = 0;
    /* @hack spelling: S3_THEN on the comparison's left, where number[read]
     * first would do */
    while (S3_THEN != number[read] && saved < DIGITS) {
        if (dial_digit(number[read])) {
            dial_number[saved++] = number[read];
        }
        read++;
    }
    dial_number[saved] = 0;
    if (is_barred(dial_number)) {
        return BL_BARRED;
    }
    if (is_delayed(dial_number)) {
        return BL_DELAYED;
    }
    now = targetGetRealTimeClock();
    if (!(OPTIONS & TIME_AT_FAILURE)) {
        calls.time = now;
    }
    if ((DELAY_MODE == PER_NUMBER || BAR_MODE == PER_NUMBER) &&
        !(OPTIONS & RECORD_FAILURE_TIMES)) {
        slot = (entries[current].calls.last + 1) % entries[current].calls.slots;
        if (slot != entries[current].calls.first) {
            entries[current].calls.last = slot;
            entries[current].calls.at[slot] = now;
        }
    }
    return BL_ALLOWED;
}

void BL_Condition(Uint8 event)
{
    if (cur_cnt->count_failure_events & event) {
        count_failure();
    }
    /* @hack redundant: the test of reserved_failure_events, whose arm is
     * empty */
    if (cur_cnt->reserved_failure_events & event) {
    }
    if (cur_cnt->clear_number_events & event) {
        reset_current();
    }
    if (event & cur_cnt->clear_all_events) {
        reset_all();
    }
    if (!event) {
        discard_unused();
    }
}

void BL_show_struct(Uint8 which)
{
    char text[8];
    split_time(targetGetRealTimeClock());
    if (which == BL_SHOW_ENTRIES) {
        display_entries();
    } else if (which == BL_SHOW_TIME) {
        send_cr_lf();
        SEND_DECIMAL(days, 3, text);
        send_message("  ");
        SEND_TIME(text);
        send_cr_lf();
    } else if (which == BL_SHOW_CLEAR) {
        reset_all();
    } else {
        at_com_result = AT_ERROR;
    }
}

Uint16 BL_GetBlackListBuffer(Uint8 **buffer)
{
    *buffer = (Uint8 *)entries;
    return sizeof(entries);
}

static void reset_all(void)
{
    Uint8 i;
    calls.count = 0;
    calls.time = 0;
    current = 0;
    for (i = 0; i < ENTRY_COUNT; i++) {
        clear_entry(i);
    }
}

static void reset_current(void)
{
    calls.count = 0;
    clear_entry(current);
}

static void clear_entry(Uint8 index)
{
    Uint8 digit;
    Uint8 slot;
    entries[index].state = ENTRY_UNUSED;
    digit = 0;
    do {
        entries[index].number.text[digit] = ' ';
    } while (++digit < DIGITS);
    entries[index].number.record.failures = 0;
    for (slot = 0; slot < TIME_SLOTS; slot++) {
        entries[index].calls.at[slot] = 0;
    }
    entries[index].calls.first = entries[index].calls.last = 0;
    entries[index].calls.slots = TIME_SLOTS;
}

static void discard_unused(void)
{
    if (!entries[current].number.record.failures) {
        clear_entry(current);
    }
}

static char find_number(Uint8 *number, Uint8 *index)
{
    Uint8 entry;
    Uint8 stored;
    Uint8 digits;
    Uint8 i;
    for (entry = 0; entry < ENTRY_COUNT; entry++) {
        if (entries[entry].state == ENTRY_BAR ||
            entries[entry].state == ENTRY_DELAY) {
            stored = 0;
            digits = 0;
            while (entries[entry].number.text[stored] != ':' &&
                   stored < DIGITS) {
                stored++;
            }
            while (dial_digit(number[digits]) && digits < DIGITS) {
                digits++;
            }
            if (stored == digits && stored < DIGITS) {
                for (i = 0; entries[entry].number.text[i] != ':' &&
                            entries[entry].number.text[i] == number[i];
                     i++) {
                }
                if (entries[entry].number.text[i] == ':') {
                    *index = entry;
                    return 1;
                }
            }
        }
    }
    return 0;
}

static void add_number(Uint8 *number, Uint8 *index)
{
    Uint8 entry;
    Uint8 i;
    Uint32 now = targetGetRealTimeClock();
    for (entry = 0; entry < ENTRY_COUNT; entry++) {
        if (entries[entry].state != ENTRY_BAR &&
            entries[entry].state != ENTRY_DELAY) {
            entries[entry].state = ENTRY_DELAY;
            for (i = 0; dial_digit(number[i]) && i < DIGITS; i++) {
                entries[entry].number.text[i] = number[i];
            }
            entries[entry].number.text[i] = ':';
            entries[entry].number.record.failures = 0;
            entries[entry].calls.first = entries[entry].calls.last = 0;
            entries[entry].calls.slots = TIME_SLOTS;
            entries[entry].calls.at[0] = now;
            *index = entry;
            break;
        }
    }
}

static char is_barred(Uint8 *number)
{
    Uint32 now = targetGetRealTimeClock();
    Uint32 age;
    Uint8 slot;
    switch (BAR_MODE) {
    case OFF:
        return 0;
    case GLOBAL:
        if (calls_expired()) {
            calls.count = 0;
            reset_all();
        }
        if (calls.count >= cur_cnt->bar_after_failures) {
            return 1;
        }
        return 0;
    case PER_NUMBER:
        if (find_number(number, &current)) {
            if (entry_expired(current)) {
                entries[current].number.record.failures = 0;
                entries[current].state = ENTRY_DELAY;
                expire_times(current, now);
            }
            if (entries[current].state == ENTRY_BAR) {
                return 1;
            } else if (entries[current].number.record.failures >=
                       cur_cnt->bar_after_failures) {
                entries[current].state = ENTRY_BAR;
                return 1;
            } else {
                slot = entries[current].calls.first;
                age = (now - entries[current].calls.at[slot]) /
                      SECONDS_PER_MINUTE;
                if (age >= cur_cnt->bar_after_minutes &&
                    cur_cnt->bar_after_minutes > 0) {
                    entries[current].state = ENTRY_BAR;
                    return 1;
                }
            }
        } else {
            add_number(number, &current);
        }
        break;
    }
    return 0;
}

static char is_delayed(Uint8 *number)
{
    char delayed = 0;
    Uint32 now;
    Uint32 age;
    Uint8 slot;
    now = targetGetRealTimeClock();
    switch (DELAY_MODE) {
    case OFF:
        break;
    case GLOBAL:
        if (calls_expired()) {
            calls.count = 0;
        }
        if (calls.count >= cur_cnt->delay_after_failures) {
            age = now - calls.time;
            if (age < cur_cnt->delay_seconds) {
                delayed = 1;
            }
        }
        break;
    case PER_NUMBER:
        if (find_number(number, &current)) {
            if (entry_expired(current)) {
                entries[current].number.record.failures = 0;
                expire_times(current, now);
            } else if (entries[current].number.record.failures >=
                       cur_cnt->delay_after_failures) {
                age = now;
                if (OPTIONS & DELAY_FROM_FIRST) {
                    slot = entries[current].calls.first;
                } else {
                    slot = entries[current].calls.last;
                }
                age -= entries[current].calls.at[slot];
                if (age < cur_cnt->delay_seconds) {
                    delayed = 1;
                }
            }
        } else {
            add_number(number, &current);
        }
        break;
    }
    if (delayed == 1) {
        remaining_delay = cur_cnt->delay_seconds - age;
    }
    return delayed;
}

static char calls_expired(void)
{
    Uint32 now;
    Uint32 age;
    now = targetGetRealTimeClock();
    age = (now - calls.time) / SECONDS_PER_MINUTE;
    if ((OPTIONS & KEEP_AT_LIMIT) &&
        calls.count >= cur_cnt->bar_after_failures) {
        return 0;
    }
    if (age >= cur_cnt->expire_after_minutes && cur_cnt->expire_after_minutes) {
        return 1;
    }
    return 0;
}

static char entry_expired(Uint8 index)
{
    Uint32 now = targetGetRealTimeClock();
    Uint32 elapsed;
    Uint32 age;
    Uint8 slot;
    if (entries[index].state == ENTRY_BAR ||
        entries[index].state == ENTRY_DELAY) {
        elapsed = now;
        if (OPTIONS & EXPIRE_FROM_FIRST) {
            slot = entries[current].calls.first;
        } else {
            slot = entries[current].calls.last;
        }
        elapsed -= entries[current].calls.at[slot];
        elapsed /= SECONDS_PER_MINUTE;
        if (elapsed >= cur_cnt->expire_after_minutes &&
            cur_cnt->expire_after_minutes && !(OPTIONS & KEEP_AT_LIMIT)) {
            return 1;
        }
        return 0;
    }
    slot = expire_times(index, now);
    age = (now - entries[index].calls.at[slot]) / SECONDS_PER_MINUTE;
    if (age >= cur_cnt->expire_after_minutes && cur_cnt->expire_after_minutes) {
        return 1;
    }
    return 0;
}

static void count_failure(void)
{
    Uint32 now;
    Uint8 slot;
    now = targetGetRealTimeClock();
    calls.count++;
    if (OPTIONS & TIME_AT_FAILURE) {
        calls.time = now;
    }
    if (DELAY_MODE == PER_NUMBER || BAR_MODE == PER_NUMBER) {
        entries[current].number.record.failures++;
        if (OPTIONS & RECORD_FAILURE_TIMES) {
            slot = (entries[current].calls.last + 1) %
                   entries[current].calls.slots;
            if (slot != entries[current].calls.first) {
                entries[current].calls.last = slot;
                entries[current].calls.at[slot] = now;
            }
        }
    }
}

static void display_entries(void)
{
    char i;
    char digit;
    char text[12];
    struct dial_entry *entry;
    Uint8 slot;
    for (i = 0; i < ENTRY_COUNT / 2; i++) {
        display_byte = i;
        send_cr_lf();
        SEND_DECIMAL(display_byte, 2, text);
        send_message("  ");
        display_byte = entries[(int)i].state;
        message_byte_to_dte(display_byte);
        send_message(" ");
        digit = 0;
        do {
            display_byte = entries[(int)i].number.text[(int)digit];
            message_byte_to_dte(display_byte);
        } while (++digit < DIGITS);
        send_message(" ");
        entry = &entries[(int)i];
        display_byte = entry->number.record.failures;
        SEND_DECIMAL(display_byte, 2, text);
        send_message(" ");
        slot = entry->calls.last;
        split_time(entry->calls.at[slot]);
        SEND_TIME(text);
        send_message("  ");
        slot = entry->calls.first;
        split_time(entry->calls.at[slot]);
        SEND_TIME(text);
        send_message("  ");
        SEND_DECIMAL(calls.count, 2, text);
    }
}

static char dial_digit(Uint8 value)
{
    if ((value >= '0' && value <= '9') || value == 'A' || value == 'B' ||
        value == 'C' || value == 'D' || value == '#' || value == '*') {
        return 1;
    }
    return 0;
}

static void split_time(Uint32 value)
{
    days = value / SECONDS_PER_DAY;
    hours = (value % SECONDS_PER_DAY) / SECONDS_PER_HOUR;
    minutes =
        ((value % SECONDS_PER_DAY) % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
    seconds =
        ((value % SECONDS_PER_DAY) % SECONDS_PER_HOUR) % SECONDS_PER_MINUTE;
}

static Uint8 expire_times(Uint8 index, Uint32 now)
{
    Uint8 first = entries[index].calls.first;
    Uint8 last = entries[index].calls.last;
    Uint8 slots = entries[index].calls.slots;
    while (first != entries[index].calls.last) {
        if (now - entries[index].calls.at[first] <
            SECONDS_PER_MINUTE * cur_cnt->expire_after_minutes) {
            break;
        }
        first = entries[index].calls.first = (first + 1) % slots;
    }
    if (last < first) {
        entries[index].number.record.failures = last - first + slots + 1;
    } else {
        entries[index].number.record.failures = last - first + 1;
    }
    return first;
}
