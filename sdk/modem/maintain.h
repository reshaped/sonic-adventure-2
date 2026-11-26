/* maintain: the modem controller's upkeep of the line, from a ring or a dial
 * to its end */
#ifndef MODEM_MAINTAIN_H
#define MODEM_MAINTAIN_H
#include <sg_xpt.h>

/* maintain_phase's values */
enum maintain_state {
    MAINTAIN_IDLE = 0,
    MAINTAIN_DIAL = 1,
    MAINTAIN_ESTABLISH = 2,
    MAINTAIN_WAIT_CONNECT = 3,
    MAINTAIN_ONLINE = 4,
    MAINTAIN_DRAIN = 6,
    MAINTAIN_CLEAR_DOWN = 8
};

/* follow one pass of maintain_phase. a run of 0 resets the controller's
 * phases instead */
void maintain(Uint8 run);
/* off_line_maintain runs one pass of the modem's upkeep off line, and
 * on_line_maintain one on line. on_line_maintain does nothing while
 * BMR6_ON_LINE is clear, and hangs up on STATUS_INACTIVE or BMR7_LINK_LOST */
void off_line_maintain(void);
void on_line_maintain(void);
/* 1 while the line rings, and 0 otherwise */
char get_ring_state(void);
/* count the line's rings in S1, and answer on the S0th. an S0 of 0 answers
 * no ring itself. under BMR32_ANSWER the modem answers once a ring is
 * counted. BMR31_NO_ANSWER keeps it from answering on S0, as does DTR off
 * under &D2 or S27's bit 1. it counts no ring off hook or on a leased line */
void ring_monitor(void);
/* while BMR11_LEASED_LINE is set, connect the leased line 3 s on. it calls
 * under S14_ORIGINATE, and otherwise answers */
void lease_line_monitor(void);
/* under S39_TEST_INPUTS, the terminal's test inputs start and end tests.
 * each is active low: one that falls starts its test, and one that rises ends
 * it. the analog loop's input starts a test off line, and the remote digital
 * loop's on line. at_com_result stays as it was */
void dte_tests_monitor(void);

#endif
