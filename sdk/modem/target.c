#include <sg_xpt.h>
#include "countrydb.h"
#include "dspdb.h"
#include "dte_vxd.h"
#include "dteapi.h"
#include "ecdc_def.h"
#include "messages.h"
#include "osif.h"
#include "roc_k56.h"
#include "targdef.h"
#include "target.h"
#include "util_mod.h"

/* the board's straps: the country code in MODEM_STRAPS_COUNTRY, and the DAA's
 * type in MODEM_STRAPS_DAA */
#define MODEM_STRAPS (*(Uint8 *)0xa0600000)
#define MODEM_STRAPS_COUNTRY 0x0f
#define MODEM_STRAPS_DAA 0x10
/* the data pump's control register, whose PUMP_RUN releases it from reset */
#define PUMP_CONTROL (*(volatile Uint8 *)0xa0600480)
#define PUMP_RUN 0x01
#define PUMP_REGISTERS ((volatile struct dp_registers *)0xa0600400)
#define SECONDS_PER_DAY 86400
/* the year Compute_YYYYMM_to_seconds counts from */
#define FIRST_YEAR 2000

#define DEFAULT_COUNTRY_NAME "Country Fail To Init"
/* a second copy of active_country's name, which nothing reads */
/* NOLINTNEXTLINE(clang-diagnostic-unused-const-variable) */
static const char default_country_name[] = DEFAULT_COUNTRY_NAME;

/* the record and S-register limits that targetGetCountryInfo hands out.
 * CountryDbGetData fills them from the country database, and they hold
 * these defaults until it does */
/* clang-format off */
static struct country_st active_country = {
    DEFAULT_COUNTRY_NAME, 181,
    9, 2, 0, 0,
    39, 33, 750, 900, 0,
    {{55, 20}, {55, 20}, {55, 20}, {55, 20}},
    {{250, 100}, {450, 380}, {250, 100}, {450, 380}},
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
    80, 300,
    43, 48, 10,
    67, 15,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    500, 25, 300, 0, 4, 75,
    0x188f, 0x0200, 0};
static struct Slimit active_limits[10] = {
    {6, 4, 4, 255},
    {7, 31, 10, 60},
    {11, 95, 95, 255},
    {END_LIMITS, 0, 0, 0}};
/* clang-format on */
static Uint8 stored_country = 255;
static Uint8 hardware_fault = 0;
static char irq_installed = 0;
/* the codes data_pump_error reports without marking the hardware failed. 0
 * ends the list */
/* clang-format off */
static Uint32 expected_errors[] = {
    DP_ERR_INTERRUPT,
    3120, 3140, 3160, 3180, 3200, 3220,
    5100, 5120, 5140, 5160,
    6100,
    7100, 7120, 7140, 7160,
    0};
/* clang-format on */
struct month_table {
    Uint8 days[13];
};
static struct month_table normal_months = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
static struct month_table leap_months = {
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

/* NOLINTBEGIN(misc-use-internal-linkage) */
struct SegaOptionalInterface *CountryApiPtr;
void init_target_cc_f(void);
/* NOLINTEND(misc-use-internal-linkage) */

char li_get_lcs(void)
{
    return 0;
}

char li_get_ring(void)
{
    return dp_test_ring();
}

void li_set_hook(Uint8 on)
{
}
void target_dpNeedsHookControl(Uint8 on)
{
}
void li_set_mute(char on)
{
}
void li_set_pulse(Uint8 on)
{
    /* @hack redundant: an empty if */
    if (on) {
    }
}
void li_set_rj11(char on)
{
}
void target_control(Uint8 phase)
{
    /* @hack redundant: an empty if */
    if (phase) {
    }
    if (next_dte_type != DTE_NONE && vxd_message_buff_empty()) {
        vxd_dte_set_interface(next_dte_type, speed_bak, format_bak, flwctl_bak,
                              S2_bak, S12_bak, XON_bak, XOFF_bak,
                              rx_break_type_bak);
    }
}
void target_dip_switch(Uint8 *registers)
{
}
void target_dp_reset(char reset)
{
    if (reset == 1) {
        PUMP_CONTROL &= ~PUMP_RUN;
    } else {
        PUMP_CONTROL |= PUMP_RUN;
    }
}
void target_test_pin(Uint8 pin, Uint8 state)
{
}
char target_get_TD(void)
{
    return 1;
}
void target_local_digital_loop(char on)
{
    if (on == 1) {
        dp_enter_local_digital_loop();
    } else {
        dp_stop_local_digital_loop();
    }
}
void target_set_TCI_timer(Uint16 ticks)
{
}
void target_set_volume(Uint8 level)
{
    dp_set_volume(level);
}
char target_set_analog(Uint8 on)
{
    return 0;
}
void target_apply_extclk(Uint8 transmit, Uint8 receive)
{
}
void target_remove_extclk(void)
{
}
void led_control(void)
{
}

void break_check(void)
{
    if (BREAK_FLAGS & BREAK_TO_HOST) {
        if (--DTE_BK_TIME == 0) {
            BREAK_FLAGS &= ~BREAK_TO_HOST;
            vxd_send_next_dte_break();
        }
    }
    if (BREAK_FLAGS & BREAK_TO_LINE) {
        if (--DCE_BK_TIME == 0) {
            BREAK_FLAGS &= ~BREAK_TO_LINE;
        }
    }
    if (BREAK_FLAGS & BREAK_FROM_HOST) {
        if (DTE_BRK_LEN < 0xffff) {
            DTE_BRK_LEN++;
        }
        if ((Buffer_memory.lcr & COM_LCR_BREAK) != COM_LCR_BREAK ||
            (BREAK_FLAGS & BREAK_END)) {
            /* @bug logic: the test at 0x0cae9ae2 compares lcr's break bit
             * with 0xffff, which it never equals. a break from the host keeps
             * the ticks counted while it ran, never BREAK_DEFAULT_TICKS */
#ifdef NONMATCHING
            /* this side keeps the result of the bug tagged above: the break
             * keeps the ticks counted. it reads lcr, as the test that never
             * holds does */
            (void)Buffer_memory.lcr;
#else
            /* NOLINTBEGIN(clang-diagnostic-*,misc-redundant-expression) */
            if ((Buffer_memory.lcr & COM_LCR_BREAK) == 0xffff) {
                DTE_BRK_LEN = BREAK_DEFAULT_TICKS;
            }
            /* NOLINTEND(clang-diagnostic-*,misc-redundant-expression) */
#endif
            BREAK_FLAGS &= ~BREAK_FROM_HOST;
            vxd_end_of_dte_brk_rx(DTE_BRK_LEN);
        }
    }
    if (BREAK_FLAGS & BREAK_FROM_LINE) {
        if (DCE_BRK_LEN < 0xffff) {
            DCE_BRK_LEN++;
        }
        DCE_BRK_LEN = BREAK_DEFAULT_TICKS;
        BREAK_FLAGS &= ~BREAK_FROM_LINE;
    }
}

void set_direct_sync_interface(void)
{
}
void set_direct_async_interface(void)
{
}
void set_normal_interface(void)
{
}
void set_v25bis_sync_interface(void)
{
}
void disc_dce_clocks(void)
{
}
void connect_dce_clocks(void)
{
}
void disc_dce_tx_rx(void)
{
}
void connect_dce_tx_rx(void)
{
}
void SET_75BPS_TX(Uint8 on)
{
}
void SET_75BPS_RX(void)
{
}
void RESTART_SYSTEM(void)
{
}
char *target_id(void)
{
    return "";
}
char *target_send_dp_version(void)
{
    return DspDbGetdpRamHeaderInfo();
}

void data_pump_error(Uint32 code)
{
    char text[20];
    Uint8 i = 0;
    char expected = 0;
    convert_value_to_ascii(code, 16, 8, text);
    send_message("Data Pump Error Code =   ");
    send_message(text);
    send_cr_lf();
    for (; expected_errors[i]; i++) {
        if (expected_errors[i] == code) {
            expected = 1;
            break;
        }
    }
    if (!expected) {
        hardware_fault = 1;
    }
}
void target_message(char *text)
{
}
char target_test_dp(void)
{
    return dp_test();
}
volatile struct dp_registers *targetGetComMemInterfaceAddr(void)
{
    return PUMP_REGISTERS;
}
void targetSetComMemInterfaceAddr(Uint32 address)
{
}
void targetReadCurrentSregistersFile(void)
{
}
void targetSaveCurrentSregistersFile(void)
{
}
void targetReadCurrentFaxFrameFile(void)
{
}
void targetSaveCurrentFaxFrameFile(void)
{
}

Uint32 targetGetRealTimeClock(void)
{
    struct Sys_Time now;
    Uint32 seconds;
    osifGetSystemTime(&now);
    /* @hack type: the fields read as char, where their Uint8 would do */
    seconds = (char)now.second;
    seconds += (char)now.minute * 60;
    seconds += (char)now.hour * 3600;
    seconds += (char)now.day * SECONDS_PER_DAY;
    /* @hack spelling: now copied a field at a time, where passing now would
     * do */
    {
        struct Sys_Time *from = &now;
        struct Sys_Time copy;
        copy.year = from->year;
        copy.month = from->month;
        copy.day = from->day;
        copy.hour = from->hour;
        copy.minute = from->minute;
        copy.second = from->second;
        seconds += Compute_YYYYMM_to_seconds(copy);
    }
    return seconds;
}

Uint32 Compute_YYYYMM_to_seconds(volatile struct Sys_Time date)
{
    struct month_table normal;
    struct month_table leap;
    int month;
    int year;
    Uint32 days;
    Uint8 *lengths;
    /* @hack spelling: normal_months and leap_months copied to locals, where
     * reading the tables would do */
    normal = normal_months;
    leap = leap_months;
    days = 0;
    /* @hack type: the month read as a char, where its Uint8 would do */
    month = (char)date.month - 1;
    for (year = date.year; year >= FIRST_YEAR; year--) {
        lengths = IsBissextile(year) ? leap.days : normal.days;
        for (; month > 0; month--) {
            days += lengths[month];
        }
        month = 12;
    }
    days *= SECONDS_PER_DAY;
    return days;
}

char IsBissextile(char year)
{
    if (year % 4 == 0) {
        if (year % 100 == 0) {
            /* @hack spelling: the comparison's 1 and 0 spelled out, where
             * returning the comparison would do */
            return year % 400 == 0 ? 1 : 0;
        }
        return 1;
    }
    return 0;
}

char targetIsDpIrqInstalled(void)
{
    return irq_installed;
}

void target_InstallDpIrq(void (*handler)(void))
{
    if (handler) {
        irq_installed = 1;
        osifHookDataPumpIntr(1, ModemIntrCall);
    } else {
        irq_installed = 0;
        osifHookDataPumpIntr(0, ModemIntrCall);
    }
}
Uint8 target_HwFault(void)
{
    return hardware_fault;
}
void targetClose(void)
{
    hardware_fault = 0;
    irq_installed = 0;
    target_dp_reset(1);
}
void targetOpen(void)
{
}
void target_set_country(Uint8 code)
{
    /* @hack redundant: a store of code, which nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    volatile Uint8 selected = code;
}
void target_set_earth_relay(Uint8 on)
{
}
char target_check_country(Uint8 code)
{
    switch (code) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 21:
    case 24:
    case 26:
    case 30:
    case 31:
    case 34:
    case 42:
    case 43:
    case 44:
        return 1;
    }
    return 0;
}
Uint8 targetGetCountryCode(void)
{
    /* @hack spelling: the straps masked in a local, where returning the mask
     * would do */
    Uint8 code = MODEM_STRAPS;
    code &= MODEM_STRAPS_COUNTRY;
    return code;
}
Uint8 targetGetDAA_Type(void)
{
    /* @hack spelling: the straps masked in a local, where returning the mask
     * would do */
    Uint8 type = MODEM_STRAPS;
    type &= MODEM_STRAPS_DAA;
    return type;
}
void targetGetCountryInfo(Uint8 code, struct country_st **record,
                          struct Slimit **limits)
{
    Uint8 *database;
    struct Slimit *limit;
    database = CountryApiPtr->open_country_db();
    /* @hack spelling: active_limits kept in limit, where naming it again
     * would do */
    CountryDbGetData(code, database, &active_country, limit = active_limits);
    *record = &active_country;
    *limits = limit;
}
void targetSetOptApi(struct SegaOptionalInterface *api)
{
    CountryApiPtr = api;
}
void init_target_cc_f(void)
{
    CountryApiPtr = 0;
}
/* the member's entry in DSG_INIT_, the table of functions the Chao area
 * calls as it starts */
#pragma section _INIT_
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void (*init_targetcc)(void) = init_target_cc_f;
#pragma section
