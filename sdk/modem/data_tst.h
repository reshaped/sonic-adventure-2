/* data_tst: the V.54 loop tests, which the AT&T commands start */
#ifndef MODEM_DATA_TST_H
#define MODEM_DATA_TST_H

/* the phases of test_phase */
enum modem_test_phase {
    TEST_RESET, /* end any test, and go to TEST_IDLE */
    TEST_IDLE, /* no test runs, but the far end can start one */
    TEST_REMOTE_REQUEST, /* the far end's remote digital loop runs */
    TEST_ACTIVE, /* a loop runs, for S18 seconds where S18 isn't 0 */
    TEST_LOCAL_ANALOG, /* start a local analog loop */
    TEST_REMOTE_DIGITAL, /* ask the far end for a remote digital loop */
    TEST_REPORT /* end the loop, and report its errors */
};

/* run test_phase's next step, unless line_monitor_phase is past
 * LINE_MONITORING */
void data_test_manager(void);
/* end the test, and go back to TEST_IDLE with S16 cleared */
void reset_test_vars(void);
/* end the test, go off line, report NO CARRIER and go back to command
 * mode */
void data_tst_error(void);

#endif
