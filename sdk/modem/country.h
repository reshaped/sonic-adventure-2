/* country: the active country, its limits on the S-registers and its
 * call-progress tests */
#ifndef MODEM_COUNTRY_H
#define MODEM_COUNTRY_H
#include <sg_xpt.h>
#include "countrydb.h"

/* the active country's record and S-register limits, which
 * country_InitCountry sets */
extern struct country_st *cur_cnt;
extern struct Slimit *cur_slimits;
/* make code the active country. code must be one the country database
 * holds */
void country_InitCountry(Uint8 code);
char country_CheckCountry(Uint8 code);
/* bring *value within S-register reg's bounds, where cur_slimits has any */
void country_CheckSregLimits(Uint8 reg, Uint8 *value);
/* bring each S-register cur_slimits lists within its bounds. registers holds
 * the S-registers at their numbers */
void country_CheckAllSregLimits(Uint8 *registers);
/* set *value to S-register reg's initial value, where cur_slimits has one */
void country_CheckSregDefault(Uint8 reg, Uint8 *value);
/* set S35 and S57 to s35_default, and each S-register cur_slimits lists to
 * its initial value */
void country_CheckAllSregDefaults(Uint8 *registers);

void country_InitDialtoneDetection(void);
void country_InitCallProgress(void);
/* the call-progress tests, each called where a tone starts or ends. tone is
 * nonzero where one starts, and 0 where one ends. length is how long the
 * state before lasted, in 10 ms ticks. busy, congested and unobtainable
 * return 1 as their cadence comes round the third time. ringback returns 1
 * each time, and dial tone the first time. a test the country turns off
 * returns 0 */
char country_BusyTest(Uint8 tone, Uint32 length);
char country_RingbackTest(Uint8 tone, Uint32 length);
char country_CongestedTest(Uint8 tone, Uint32 length);
char country_UnobtainableTest(Uint8 tone, Uint32 length);
char country_DialtoneTest(Uint8 tone, Uint32 length);
/* the count of pulses that dials digit, 0 to 9, by cur_cnt's pulse_format,
 * with 0 for 10 */
Uint8 country_convert_pulse_format(Uint8 digit);
#endif
