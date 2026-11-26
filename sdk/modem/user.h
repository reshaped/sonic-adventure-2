/* user: LAPM's reports to the host, and its reset for a new link */
#ifndef MODEM_USER_H
#define MODEM_USER_H
#include <sg_xpt.h>

/* test_bad's reasons, which ECDC_RESPONSE_TEST_BAD gives in the top two
 * bits of cmd_response_buffer[1] */
#define TEST_NO_LOOPBACK 1 /* the link agreed on no loopback test */
#define TEST_TIMED_OUT 2 /* no answer came within T401 */
#define TEST_ANSWER_DIFFERS 3 /* the answer differs from the frame sent */
#define TEST_REASON_SHIFT 6

/* each sends the host its response: ECDC_RESPONSE_REMOTE_ERROR,
 * ECDC_RESPONSE_LOCAL_ERROR, ECDC_RESPONSE_TEST_PASSED or
 * ECDC_RESPONSE_TEST_BAD */
void remote_err(void);
void local_err(void);
void test_good(void);
void test_bad(Uint8 reason);
/* clear LAPM's state for a new link. the rings' cursors are left for
 * init_v42_hand_shake to set */
void clear_EC_ram(void);
/* divide host_t401 by bps. a bps of 0 makes host_t401 0 under SHC. the GCC
 * build leaves it as it was */
void divide_t401(void);

#endif
