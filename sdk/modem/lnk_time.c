#include <machine.h>
#include <sg_xpt.h>
#include <stdio.h>
#include <string.h>
#include "lnk_time.h"
#include "messages.h"

/* the firmware's build, as __DATE__ and __TIME__ give it */
static const char release_date[] = "Oct 02 2000";
#define MONTH_IS(name) (strncmp(release_date, (name), 3) == 0)
#define RELEASE_TIME "22:48:23"

/* write the identification and its nul to text, with day, year and month as
 * scratch. day and year must be char arrays of 3 and 5 */
/* @hack spelling: a macro, expanded in both entries, where a function that
 * both call would do */
#define FORMAT_IDENTIFICATION(text, day, year, month)                          \
    do {                                                                       \
        memset(day, 0, sizeof(day));                                           \
        memset(year, 0, sizeof(year));                                         \
        strncpy(day, release_date + 4, 2);                                     \
        strncpy(year, release_date + 7, 4);                                    \
        if ((Uint8)(day)[0] == ' ') {                                          \
            (day)[0] = '0';                                                    \
        }                                                                      \
        (year)[0]++;                                                           \
        if (MONTH_IS("Jan")) {                                                 \
            (month) = 1;                                                       \
        } else if (MONTH_IS("Feb")) {                                          \
            (month) = 2;                                                       \
        } else if (MONTH_IS("Mar")) {                                          \
            (month) = 3;                                                       \
        } else if (MONTH_IS("Apr")) {                                          \
            (month) = 4;                                                       \
        } else if (MONTH_IS("May")) {                                          \
            (month) = 5;                                                       \
        } else if (MONTH_IS("Jun")) {                                          \
            (month) = 6;                                                       \
        } else if (MONTH_IS("Jul")) {                                          \
            (month) = 7;                                                       \
        } else if (MONTH_IS("Aug")) {                                          \
            (month) = 8;                                                       \
        } else if (MONTH_IS("Sep")) {                                          \
            (month) = 9;                                                       \
        } else if (MONTH_IS("Oct")) {                                          \
            (month) = 10;                                                      \
        } else if (MONTH_IS("Nov")) {                                          \
            (month) = 11;                                                      \
            /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */     \
        } else if (MONTH_IS("Dec")) {                                          \
            (month) = 12;                                                      \
        }                                                                      \
        sprintf((char *)(text), "SD %s.%02d.%s.%c%c.%c%c.%c%c", year, month,   \
                day, RELEASE_TIME[0], RELEASE_TIME[1], RELEASE_TIME[3],        \
                RELEASE_TIME[4], RELEASE_TIME[6], RELEASE_TIME[7]);            \
    } while (0)

void lnk_timeDisplayCompiledTime(void)
{
    Uint8 text[32];
    char day[3];
    char year[5];
#ifdef NONMATCHING
    /* this side starts month at 0. GCC can't see that release_date, a constant,
     * names one of the twelve months FORMAT_IDENTIFICATION tests */
    Uint8 month = 0;
#else
    Uint8 month;
#endif

    FORMAT_IDENTIFICATION(text, day, year, month);
    send_cr_lf();
    send_message((char *)text);
    send_cr_lf();
}

void CompiledTime(Uint8 *destination)
{
    Uint8 text[32];
    char day[3];
    char year[5];
#ifdef NONMATCHING
    /* this side starts month at 0. GCC can't see that release_date, a constant,
     * names one of the twelve months FORMAT_IDENTIFICATION tests */
    Uint8 month = 0;
#else
    Uint8 month;
#endif

    FORMAT_IDENTIFICATION(text, day, year, month);
    _builtin_strcpy((char *)destination, (char *)text);
}

void DisplayTime(Uint8 *text)
{
    send_cr_lf();
    send_message((char *)text);
    send_cr_lf();
}
