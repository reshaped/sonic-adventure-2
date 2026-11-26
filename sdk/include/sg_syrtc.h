/* Shinobi real-time clock library, as far as the sources use it */
#ifndef SG_SYRTC_H
#define SG_SYRTC_H
#include <sg_xpt.h>

/* syRtcInit reads the clock, puts the clock's server on the vertical
 * blank's chain, and returns 0. at each vertical blank, the server reads the
 * clock twice, and keeps the reading where the two agree. syRtcFinish takes
 * the server off the chain, and sets the kept reading to 0.
 *
 * the server's mode is a set of bits that change how the server and
 * syRtcGetDate read the clock. syRtcInit sets the mode to 0, and nothing in
 * the program changes it */
Sint32 syRtcInit(void);
void syRtcFinish(void);
/* the clock's results: SYD_RTC_ERR_FATAL is syRtcSetDate's failure */
#define SYD_RTC_ERR_OK 0
#define SYD_RTC_ERR_FATAL (-1)
/* the server's mode bits. under SYD_RTC_STAT_PASSIVE, the server skips its two
 * readings, and syRtcGetDate reads the clock itself. SYD_RTC_STAT_ACTIVE is no
 * bit set. RTC_STAT_FORCE adds a forced read in either mode. that read waits
 * for three readings in a row that equal the one before them. it gives up
 * after 51 tries. RTC_STAT_LOG lets the library's debug log take words */
#define SYD_RTC_STAT_ACTIVE 0
#define SYD_RTC_STAT_PASSIVE 1
#define RTC_STAT_FORCE 2
#define RTC_STAT_LOG 8
/* syRtcExecServer is the server's step, which the vertical blank runs.
 * syRtcSetServerMode sets the mode, and syRtcGetStat returns it */
void syRtcExecServer(void);
void syRtcSetServerMode(Sint32 mode);
Sint32 syRtcGetStat(void);

/* a date and time. dayofweek counts from Sunday, and ageofmoon is the
 * moon's age in days */
typedef struct {
    Uint16 year;
    Uint8 month;
    Uint8 day;
    Uint8 hour;
    Uint8 minute;
    Uint8 second;
    Uint8 dayofweek;
    Uint8 ageofmoon;
} SYS_RTC_DATE;

/* store in *date the date and time of the server's kept reading. it fills in
 * the day of the week and the moon's age too. it returns the server's mode,
 * which syRtcInit leaves 0 */
Sint32 syRtcGetDate(SYS_RTC_DATE *date);
/* set the clock to date, then read it back. a reading of date's count, or of
 * the second after it, becomes the server's kept reading. the call then returns
 * SYD_RTC_ERR_OK. any other reading returns SYD_RTC_ERR_FATAL, and leaves the
 * clock as written */
Sint32 syRtcSetDate(const SYS_RTC_DATE *date);
/* 0 where date1 and date2 fall in the same second. 1 where date1 is later,
 * and -1 where it is earlier. each date's dayofweek and ageofmoon are
 * ignored */
Sint32 syRtcCompareDate(const SYS_RTC_DATE *date1, const SYS_RTC_DATE *date2);
/* the clock's count is the seconds since the start of SYD_RTC_FIRSTYEAR. every
 * year of the count divisible by 4 is a leap year.
 *
 * syRtcDateToCount stores date's count in *count. date's month must be 1 to 12,
 * which the call doesn't check. its dayofweek and ageofmoon are ignored.
 * syRtcCountToDate stores count's date in *date, with its day of the week and
 * the moon's age */
#define SYD_RTC_FIRSTYEAR 1950
void syRtcDateToCount(const SYS_RTC_DATE *date, Uint32 *count);
void syRtcCountToDate(Uint32 count, SYS_RTC_DATE *date);

#endif
