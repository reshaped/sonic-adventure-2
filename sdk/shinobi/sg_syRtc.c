/* the real-time clock: a count of seconds in AICA's registers on the G2
 * bus. AICA is the sound chip */
#include <sg_chain.h>
#include <sg_syg2.h>
#include <sg_syrtc.h>
#include <sg_xpt.h>

/* the clock's registers: the count's high half, its low half and the write
 * enable. each half lies in the low RTC_HALF_BITS bits of its word */
#define RTC_HI ((void *)0xa0710000)
#define RTC_LO ((void *)0xa0710004)
#define RTC_WRITE_ENABLE ((void *)0xa0710008)
#define RTC_HALF_BITS 16
#define RTC_HALF_MASK 0xffff

/* the server's priority on its chain */
#define SERVER_PRI 92

/* a new moon, as a count, and the lunar month and the day in seconds */
#define NEW_MOON 0x5cb417d1
#define LUNAR_MONTH 0x26ee7f
#define DAY_SECONDS 86400
/* the years from 1948, the last leap year before SYD_RTC_FIRSTYEAR, to
 * SYD_RTC_FIRSTYEAR */
#define YEARS_PAST_LEAP 2
/* year / 400 less year / 100, the same for every year from 1900 to 2099 */
#define CENTURY_TERMS (-15)
/* syRtcGetCountForce first takes a reading, with up to FORCE_RETRIES more
 * tries. it then reads until FORCE_READINGS readings in a row equal the one
 * before them. it tries that run up to FORCE_RETRIES more times */
#define FORCE_RETRIES 50
#define FORCE_READINGS 3
/* the readings syRtcGetAndCheckCount compares */
#define CHECK_READINGS 2

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the library's version and build time */
const char *const sygRtcBuild = "\nsyRtc Ver 1.03 Build:Oct 18 2000 18:53:31\n";
/* the server's kept reading, and its mode, a set of SYD_RTC_STAT_ and
 * RTC_STAT_ bits. the vertical blank's handler writes the reading */
volatile Uint32 sygRtcCount;
volatile Sint32 sygRtcStat;
/* the server's handler on the vertical blank's chain */
SYCHAIN sygRtcChain;
/* where the debug log's next word goes, and the address it stops at */
Uint32 *sygRtcDbgLogAddr;
Uint32 *sygRtcDbgLogMax;
/* the count two readings agree on, or previous where they differ. a reading
 * that fails counts as previous */
Uint32 syRtcGetAndCheckCount(Uint32 previous);
/* the clock's count, once FORCE_READINGS readings in a row equal the one
 * before them. it returns -1 where every try at the first reading fails.
 * where the readings never steady, it returns the last one taken. where every
 * reading after the first fails, the bug tag on its return says what it
 * returns */
Uint32 syRtcGetCountForce(void);
/* store the clock's count in *count, and return 0. where the bus read
 * fails, it returns -1 and stores nothing */
Sint32 syRtcAicaGetCount(Uint32 *count);
/* syRtcAicaSetWriteFlag sets the write enable, and the other two write the
 * count's low and high halves. each returns 0, whatever syG2Write returns */
Sint32 syRtcAicaSetWriteFlag(void);
Sint32 syRtcAicaSetDateLo(Uint32 value);
Sint32 syRtcAicaSetDateHi(Uint32 value);
/* the day of the week of year, month and day, 0 for Sunday */
Uint8 syRtcMakeDayOfWeek(Uint16 year, Uint8 month, Uint8 day);
/* the whole days since the last new moon before date */
Uint8 syRtcMakeAgeOfMoon(SYS_RTC_DATE *date);
/* under RTC_STAT_LOG, syRtcDbgSetLogAddr gives the debug log the words from
 * begin up to limit. syRtcDbgWriteLog writes value to the log's next word,
 * while one is left */
void syRtcDbgSetLogAddr(Uint32 begin, Uint32 limit);
void syRtcDbgWriteLog(Uint32 value);
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 syRtcInit(void)
{
    sygRtcCount = syRtcGetCountForce();
    sygRtcStat = SYD_RTC_STAT_ACTIVE;
    syRtcExecServer();
    /* the chain calls its handler with an argument, here NULL.
     * syRtcExecServer takes none, and never reads the register that holds
     * it */
    sygRtcChain =
        syChainAddHandler(SYD_CHAIN_EVENT_IML6_VBLANK,
                          (void (*)(void *))syRtcExecServer, SERVER_PRI, NULL);
    return SYD_RTC_ERR_OK;
}

void syRtcFinish(void)
{
    sygRtcCount = 0;
    syChainDeleteHandler(sygRtcChain);
}

void syRtcExecServer(void)
{
    if (!(sygRtcStat & SYD_RTC_STAT_PASSIVE)) {
        sygRtcCount = syRtcGetAndCheckCount(sygRtcCount);
    }
    if (sygRtcStat & RTC_STAT_FORCE) {
        sygRtcCount = syRtcGetCountForce();
    }
}

Sint32 syRtcGetDate(SYS_RTC_DATE *date)
{
    Uint32 count;

    if ((sygRtcStat & SYD_RTC_STAT_PASSIVE) == SYD_RTC_STAT_PASSIVE) {
        count = syRtcGetCountForce();
        syRtcCountToDate(count, date);
        sygRtcCount = count;
    } else {
        syRtcCountToDate(sygRtcCount, date);
    }
    date->dayofweek = syRtcMakeDayOfWeek(date->year, date->month, date->day);
    date->ageofmoon = syRtcMakeAgeOfMoon(date);
    return sygRtcStat;
}

Sint32 syRtcSetDate(const SYS_RTC_DATE *date)
{
    Uint32 count;
    Sint32 err;
    Uint32 read;

    syRtcDateToCount(date, &count);
    err = syRtcAicaSetWriteFlag();
    if (err == SYD_RTC_ERR_OK) {
        err = syRtcAicaSetDateLo(count & RTC_HALF_MASK);
        if (err == SYD_RTC_ERR_OK) {
            err = syRtcAicaSetDateHi((count >> RTC_HALF_BITS) & RTC_HALF_MASK);
            if (err == SYD_RTC_ERR_OK) {
                /* the count read back may be one past the count written,
                 * where the clock ticked since the write */
                read = syRtcGetCountForce();
                if (read == count || read == count + 1) {
                    sygRtcCount = read;
                    err = SYD_RTC_ERR_OK;
                } else {
                    err = SYD_RTC_ERR_FATAL;
                }
            }
        }
    }
    return err;
}

void syRtcSetServerMode(Sint32 mode)
{
    sygRtcStat = mode;
}

Sint32 syRtcGetStat(void)
{
    return sygRtcStat;
}

Sint32 syRtcCompareDate(const SYS_RTC_DATE *date1, const SYS_RTC_DATE *date2)
{
    Uint32 count1;
    Uint32 count2;
    Sint32 order;

    syRtcDateToCount(date1, &count1);
    syRtcDateToCount(date2, &count2);
    /* @hack spelling: order, set in each arm, where a return in each arm
     * would do */
    if (count1 == count2) {
        order = 0;
    } else if (count1 > count2) {
        order = 1;
    } else {
        order = -1;
    }
    return order;
}

void syRtcDateToCount(const SYS_RTC_DATE *date, Uint32 *count)
{
    Uint32 years;
    Uint32 days;
    Uint32 days_before[12] = {0,   31,  59,  90,  120, 151,
                              181, 212, 243, 273, 304, 334};

    years = date->year - SYD_RTC_FIRSTYEAR;
    days = (years * 365) + ((years + YEARS_PAST_LEAP) / 4);
    if ((years + YEARS_PAST_LEAP) % 4 == 0 && date->month <= 2) {
        days--;
    }
    days += days_before[date->month - 1];
    days += date->day - 1;
    *count = ((((((days * 24) + date->hour) * 60) + date->minute) * 60) +
              date->second);
}

/* @hack redundant: count, date, leap_days and month as register, where plain
 * declarations would do */
void syRtcCountToDate(register Uint32 count, register SYS_RTC_DATE *date)
{
    Uint32 years;
    register Uint32 leap_days;
    register Uint32 month;
    Uint32 days;
    Uint32 days_before[12] = {0,   31,  59,  90,  120, 151,
                              181, 212, 243, 273, 304, 334};

    date->second = count % 60;
    count /= 60;
    date->minute = count % 60;
    count /= 60;
    date->hour = count % 24;
    count /= 24;
    years = count / 365;
    days = count % 365;
    /* the leap days before the year */
    leap_days = (years + YEARS_PAST_LEAP - 1) / 4;
    if (leap_days <= days) {
        days -= leap_days;
        date->year = years + SYD_RTC_FIRSTYEAR;
    } else {
        date->year = years + SYD_RTC_FIRSTYEAR - 1;
        days += 365 - leap_days;
        if (date->year % 4 == 0) {
            days++;
        }
    }
    month = 1;
    if (date->year % 4 == 0) {
        if (days >= days_before[1]) {
            for (month = 2; month < 12; month++) {
                if (days < days_before[month] + 1) {
                    break;
                }
            }
        }
    } else {
        for (month = 1; month < 12; month++) {
            if (days < days_before[month]) {
                break;
            }
        }
    }
    date->month = month;
    date->day = days - days_before[month - 1] + 1;
    if (date->year % 4 == 0 && date->month > 2) {
        date->day--;
    }
    date->dayofweek = syRtcMakeDayOfWeek(date->year, date->month, date->day);
    date->ageofmoon = syRtcMakeAgeOfMoon(date);
}

Uint32 syRtcGetAndCheckCount(Uint32 previous)
{
    Uint32 readings[CHECK_READINGS];
    Uint32 count;
    Sint32 i;
    Sint32 changed;

    /* @hack spelling: count, which starts as previous and takes the first
     * reading where none changed, where returning one or the other would
     * do */
    count = previous;
    for (i = 0; i < CHECK_READINGS; i++) {
        if (syRtcAicaGetCount(&readings[i]) != 0) {
            readings[i] = previous;
        }
    }
    changed = 0;
    for (i = 0; i < CHECK_READINGS - 1; i++) {
        if (readings[i] != readings[i + 1]) {
            changed = 1;
            break;
        }
    }
    if (changed == 0) {
        count = readings[0];
    }
    return count;
}

Uint32 syRtcGetCountForce(void)
{
    Uint32 first;
    Uint32 next;
    Sint32 tries;
    Sint32 ready;
    Sint32 result;
    Sint32 stable;
    Sint32 i;

    ready = 0;
    tries = 0;
    do {
        /* @hack spelling: result, where the call in the test would do */
        result = syRtcAicaGetCount(&first);
        if (result == 0) {
            ready = 1;
        }
    } while (ready == 0 && tries++ < FORCE_RETRIES);
    if (ready == 0) {
        return -1;
    }
    stable = 0;
    tries = 0;
    do {
        for (i = 0; i < FORCE_READINGS; i++) {
            if (syRtcAicaGetCount(&next) != 0) {
                break;
            }
            if (first != next) {
                first = next;
                break;
            }
        }
        if (i == FORCE_READINGS) {
            stable = 1;
        }
    } while (stable == 0 && tries++ < FORCE_RETRIES);
    /* @bug uninitialized: returns next unset where every read into next
     * fails. syRtcAicaGetCount's failed read returns at 0x8c110a32, before
     * its store to *count at 0x8c110a48. MOV.L @R15,R0 at 0x8c1109f6 then
     * loads next's stack word as it was */
    return next;
}

Sint32 syRtcAicaGetCount(Uint32 *count)
{
    Uint32 halves[2];

    if (syG2Read(RTC_HI, halves, SYE_G2_DATA_WIDTH_32BIT, 2,
                 SYE_G2_ADR_MODE_ADD, SYE_G2_ADR_MODE_ADD) != 0) {
        return -1;
    }
    *count = ((halves[0] << RTC_HALF_BITS) & ~RTC_HALF_MASK) |
             (halves[1] & RTC_HALF_MASK);
    return 0;
}

Sint32 syRtcAicaSetWriteFlag(void)
{
    Uint16 enable;

    enable = 1;
    /* @bug uninitialized: the write enable takes enable's high half, which no
     * path writes. MOV.W R3,@R15 at 0x8c110a5e stores two bytes, and
     * syG2Write's MOV.L @R3,R2 at 0x8c135bf8 sends four */
    syG2Write(&enable, RTC_WRITE_ENABLE, SYE_G2_DATA_WIDTH_32BIT, 1,
              SYE_G2_ADR_MODE_FIX, SYE_G2_ADR_MODE_FIX);
    return 0;
}

Sint32 syRtcAicaSetDateLo(Uint32 value)
{
    syG2Write(&value, RTC_LO, SYE_G2_DATA_WIDTH_32BIT, 1, SYE_G2_ADR_MODE_FIX,
              SYE_G2_ADR_MODE_FIX);
    return 0;
}

Sint32 syRtcAicaSetDateHi(Uint32 value)
{
    syG2Write(&value, RTC_HI, SYE_G2_DATA_WIDTH_32BIT, 1, SYE_G2_ADR_MODE_FIX,
              SYE_G2_ADR_MODE_FIX);
    return 0;
}

Uint8 syRtcMakeDayOfWeek(Uint16 year, Uint8 month, Uint8 day)
{
    /* a form of Zeller's congruence. it counts January and February as
     * months 13 and 14 of the year before. CENTURY_TERMS stands for its
     * terms in year / 100 and year / 400 */
    if (month <= 2) {
        year--;
        month += 12;
    }
    return ((year / 4) + (((month * 13) + 8) / 5) + year + day +
            CENTURY_TERMS) %
           7;
}

Uint8 syRtcMakeAgeOfMoon(SYS_RTC_DATE *date)
{
    Uint32 count;
    Uint32 phase;

    syRtcDateToCount(date, &count);
    /* @hack spelling: each arm's difference and remainder in statements of
     * their own, where one expression an arm would do */
    if (count >= NEW_MOON) {
        phase = count - NEW_MOON;
        phase %= LUNAR_MONTH;
    } else {
        phase = NEW_MOON - count;
        phase %= LUNAR_MONTH;
        phase = LUNAR_MONTH - phase;
    }
    return phase / DAY_SECONDS;
}

void syRtcDbgSetLogAddr(Uint32 begin, Uint32 limit)
{
    if (sygRtcStat & RTC_STAT_LOG) {
        sygRtcDbgLogAddr = (Uint32 *)begin;
        sygRtcDbgLogMax = (Uint32 *)limit;
    }
}

void syRtcDbgWriteLog(Uint32 value)
{
    if (sygRtcStat & RTC_STAT_LOG) {
        if (sygRtcDbgLogAddr < sygRtcDbgLogMax) {
            *sygRtcDbgLogAddr = value;
            sygRtcDbgLogAddr++;
        }
    }
}
