/* dialing: the dial string's interpreter, and the dial tone's detector */
#ifndef MODEM_DIALING_H
#define MODEM_DIALING_H
#include <sg_xpt.h>

/* dialing's results. DIAL_PENDING says the dial string isn't done, and
 * DIAL_DONE that the number is dialed. DIAL_COMMAND is for a dial string
 * that ends in ;, which returns to command mode. each other result ends the
 * call, and dialing_error reports it. dialing never returns
 * DIAL_CIRCUIT_BUSY */
#define DIAL_PENDING 0
#define DIAL_DONE 1
#define DIAL_COMMAND 2
#define DIAL_ABORTED 3
#define DIAL_NO_CARRIER 4
#define DIAL_NO_DIALTONE 5
#define DIAL_BUSY 6
#define DIAL_NO_ANSWER 7
#define DIAL_CIRCUIT_BUSY 8
/* take the dial string's next step. maintain calls it while the modem
 * dials */
Uint8 dialing(void);
/* run the dial string's character at dial_string_ptr, and step past it. S3's
 * character ends the string with DIAL_DONE, and ; with DIAL_COMMAND. any other
 * gives DIAL_PENDING */
Uint8 execute_dial_string(void);
/* dial number, 0 to 9 by pulses or 0 to 15 by tones, as dp_dial_number takes
 * it. the library's name for it is dial_number, which Chao Kindergarten's
 * code defines too */
void dialing_send_digit(Uint8 number);
/* return 1 where the call is to end at once, and 0 otherwise */
char dial_line_est_abort(void);
/* send the terminal the AT result for a dialing result that ends the call */
void dialing_error(Uint8 result);
/* set the pump up to dial by pulses, or by tones, with the country's
 * timings */
void setup_in_pulse(void);
void setup_in_dtmf(void);
void stop_mute(void);
/* start waiting at most seconds for a dial tone. a tone counts once it sounds
 * duration ticks without a break, where duration isn't 0. one that matches the
 * country's cadence counts too */
void init_dial_tone_detector(Uint8 seconds, Uint32 duration);
/* detect_dialtone's results. dialing blind, the tone counts as found once
 * the wait has run out */
#define DIALTONE_WAITING 0
#define DIALTONE_FOUND 1
#define DIALTONE_NONE 2
Uint8 detect_dialtone(void);

#endif
