/* line establishment, which takes a call from dialing or answering to a
 * connection with the remote modem */
#ifndef MODEM_LINE_ESTAB_H
#define MODEM_LINE_ESTAB_H
#include <sg_xpt.h>

/* the phases of line_estab_phase, the call's setup as line_estab runs it */
#define ESTAB_BEGIN 0
#define ESTAB_ANSWER_DELAY 2
#define ESTAB_DIAL_DELAY 3
#define ESTAB_CALLING_TONE 4
#define ESTAB_HANDSHAKE 5
#define ESTAB_CARRIER 6
#define ESTAB_DATA_DELAY 7
#define ESTAB_CLEAR_DOWN 9
void line_estab(void);
void set_fax_is_on(void);
void select_dce_speed(char update);
/* return the TCI speed code of rate, a pump rate code. a code the call
 * doesn't convert gives 0, TCI's code for 75 bps. RATE_75 is such a code */
Uint8 lestConvertDpRate(Uint8 rate);

/* the least and most rates the data pump may connect at, sending and then
 * receiving. each is a data-pump rate code from the configuration */
Uint8 lestGetMinRate(void);
Uint8 lestGetMaxRate(void);
Uint8 lestGetMinRxRate(void);
Uint8 lestGetMaxRxRate(void);
Uint32 lestGetModulationMask(void);
void lestUpdateModRates(void);
void update_dce_speed(void);
void init_busy_ringback_detector(void);
/* what detect_busy_ringback returns: the call-progress tone it has heard, or
 * PROGRESS_NONE */
#define PROGRESS_NONE 0
#define PROGRESS_RINGBACK 1
#define PROGRESS_BUSY 2
#define PROGRESS_CONGESTED 3
#define PROGRESS_UNOBTAINABLE 4
Uint8 detect_busy_ringback(void);
void init_answer_tone_detector(void);
char detect_answer_tone(void);
void prepare_link(void);

#endif
