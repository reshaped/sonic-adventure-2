/* command: the host's commands to the error control and data compression,
 * and their responses */
#ifndef MODEM_COMMAND_H
#define MODEM_COMMAND_H
#include <sg_xpt.h>

#define ECDC_RESPONSE_BAD_CHECKSUM 1
#define ECDC_RESPONSE_DIAG_OK 2
#define ECDC_RESPONSE_DIAG_ERROR 3
#define ECDC_RESPONSE_ID 4
#define ECDC_RESPONSE_CONNECTED 5
#define ECDC_RESPONSE_RESET 6
/* remote_err's and local_err's responses */
#define ECDC_RESPONSE_REMOTE_ERROR 7
#define ECDC_RESPONSE_LOCAL_ERROR 8
#define ECDC_RESPONSE_READY 9
/* test_good's response. test_bad's is ECDC_RESPONSE_TEST_BAD */
#define ECDC_RESPONSE_TEST_PASSED 11
#define ECDC_RESPONSE_TEST_BAD 12
#define ECDC_RESPONSE_ESCAPE 16
/* a break from the host that the interface options don't pass, and a break
 * from the line */
#define ECDC_RESPONSE_DTE_BREAK 17
#define ECDC_RESPONSE_DCE_BREAK 18
#define ECDC_RESPONSE_BAD_CODEWORD 19
/* no response, which no command sends. a caller writes it to
 * cmd_response_buffer[0] before a command whose response maintain waits for,
 * so that an earlier response doesn't pass for it */
#define ECDC_RESPONSE_NONE 0xff
/* send the host response as cmd_response_buffer[0], with the bytes the
 * caller has put after it. last_response keeps it for cmd_RESEND. for
 * ECDC_RESPONSE_RESET the call writes the next byte itself: 1 where
 * fhud_failed is 1, and 0 otherwise. it then clears fhud_failed */
void cmd_resp_to_host(Uint8 response);
void cmd_CLEAR_STAT(void);
void cmd_DIAG(void);
/* the requests cmd_HALT takes in cmd_parameter_buffer[0]. a halt ends the
 * link with TCI_sw_reset. HALT_AFTER_TERMINAL ends it once the terminal has
 * taken what the line sent. HALT_AFTER_LINE ends an error-corrected link once
 * the line has taken what the terminal sent. HALT_NOW ends the link at once. so
 * does any request where the line isn't connected, or carries raw data. the
 * two waits last only as long as OIS(25) allows, and HALT_END_WAIT ends
 * either */
#define HALT_AFTER_TERMINAL 0
#define HALT_AFTER_LINE 1
#define HALT_NOW 2
#define HALT_END_WAIT 3
void cmd_HALT(void);
void cmd_OIS(void);

struct EcdcStatistics;
struct EcdcStatus;

void cmd_ACC_LINK(void);
void cmd_GET_ID(void);
void cmd_GET_STAT(struct EcdcStatistics *statistics);
struct EcdcStatus *cmd_GET_STATUS(void);
void cmd_INIT_LINK(void);
void cmd_LOOP_TST(void);
void cmd_MAKE_LINK(void);
void cmd_NORM_LINK(void);
void cmd_RESEND(void);
void cmd_SND_BRK(void);
void cmd_START_DATA(void);
void cmd_STOP_DATA(void);
void cmd_STOP_LINK(void);
void do_strt_data(void);
void do_stop_data(void);
/* copy the host's interface options from cmd_parameter_buffer to
 * OISregisters, configure the link by them, and return 0. where the buffer's
 * bytes don't sum to 0, send ECDC_RESPONSE_BAD_CHECKSUM instead, and
 * return 1 */
char get_OIS(void);
/* the bits of OIS(0) that hold the link's mode */
#define LINK_MODE 0x0f
void configure_ECDC(void);

#endif
