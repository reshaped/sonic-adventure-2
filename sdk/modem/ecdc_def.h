/* ecdc_def: the state the modem's error control and data compression share */
#ifndef MODEM_ECDC_DEF_H
#define MODEM_ECDC_DEF_H
#include <sg_xpt.h>

struct EcdcStatistics {
    Uint32 compression_ratio;
    Uint16 frames_sent;
    Uint16 frames_received;
    Uint16 frames_resent;
    Uint16 rejects_sent;
    Uint16 timeouts;
    Uint16 rejects_received;
    Uint32 link_end;
};
/* why the link ended, as link_end holds it: its setup failed, N400 retries
 * went unanswered, V.42bis took a codeword it can't decode, or the peer sent
 * a DISC. a setup fails where it times out, or where it ends without LAPM
 * and the link can't fall back */
#define LINK_END_SETUP_FAILED 1
#define LINK_END_NO_ANSWER 3
#define LINK_END_BAD_CODEWORD 5
#define LINK_END_DISCONNECT 6

struct EcdcStatus {
    Uint32 error_corrected : 1;
    Uint32 crc32 : 1;
    Uint32 selective_reject : 1;
    Uint32 max_information_field : 1;
    Uint32 fixed_header : 1;
    Uint32 protocol : 3;
    Uint32 max_string_length : 7;
    Uint32 compress_tx : 1;
    Uint32 compress_rx : 1;
    Uint32 : 15;
    Uint16 frame_size;
    Uint16 dictionary_size;
    Uint8 receive_window;
    Uint8 transmit_window;
};

/* the controller's registers, each at its number */
extern Uint8 BMRregisters[33];
/* the interface options the controller negotiated, each at its number */
extern Uint8 OISregisters[29];
/* @hack type: the registers read and written through a volatile byte
 * pointer, where BMRregisters[number] would do */
#define BMR(number) (*(volatile Uint8 *)(BMRregisters + (number)))
#define OIS(number) (*(Uint8 *)(OISregisters + (number)))
/* OIS0_N400 holds LAPM's N400, the retries of an unanswered frame, where 0
 * retries without end. OIS1_N401 gives N401, the octets of an information
 * field: 64, 128, 192 or 256 */
#define OIS0_N400 0xf0
#define OIS1_N401 0x30
/* while the link's error control is detected, the line's characters wait in
 * a buffer under OIS1_FALLBACK_SEND or OIS1_FALLBACK_TERMINATOR. under
 * OIS1_FALLBACK_SEND the buffer ends past 512 bytes, not 4000, and goes to
 * the terminal as the link falls back. under OIS1_FALLBACK_TERMINATOR it
 * ends at OIS(2)'s 7-bit character */
#define OIS1_FALLBACK_SEND 0x40
#define OIS1_FALLBACK_TERMINATOR 0x80
/* set where V.42bis may run */
#define OIS2_V42BIS 0x80
/* OIS3_DETECTION holds the detection phase's time in DETECTION_UNIT ticks,
 * shifted up by 3, where 0 gives the phase no time. OIS3_LINE_IDLE holds the
 * minutes the line may idle before STATUS_INACTIVE, and OIS(4)'s low 7 bits
 * the terminal's, where 0 sets no limit. update_OIS leaves OIS3_LINE_IDLE 0 */
#define OIS3_DETECTION 0xf8
#define OIS3_LINE_IDLE 0x07
/* set where the modem originates the call */
#define OIS7_ORIGINATE 0x80
/* OIS8_T401 holds LAPM's T401 past 4 s, in tenths of a second. LAPM offers
 * a 32-bit FCS under OIS8_FCS32, and selective reject under OIS8_SREJ */
#define OIS8_T401 0x3f
#define OIS8_FCS32 0x40
#define OIS8_SREJ 0x80
/* OIS9_WINDOW holds LAPM's k, the frames it sends unacknowledged. an
 * answering modem runs the detection phase under OIS9_ANSWER_DETECT, whatever
 * REQ_LINK offers. an originator that asks for LAPM alone sends V.42's
 * detection pattern under OIS9_SEND_ODP, and goes straight to LAPM without
 * it. LAPM offers its loopback test under OIS9_LOOPBACK */
#define OIS9_WINDOW 0x1f
#define OIS9_ANSWER_DETECT 0x20
#define OIS9_SEND_ODP 0x40
#define OIS9_LOOPBACK 0x80
/* under OIS10_FALLBACK_RAW, a link that asks for LAPM and allows fallback
 * takes REQ_RAW in place of REQ_ALLOW_FALLBACK, and falls back without the
 * fallback buffer */
#define OIS10_FALLBACK_RAW 0x08
/* OIS11_P0 holds V.42bis's P0, the directions it may compress */
#define OIS11_P0 0x0c
/* under OIS11_ENCRYPTION, LAPM negotiates encryption, and link_made hangs up
 * a link without LAPM, compression or encryption. under OIS11_BREAK_LENGTH,
 * LAPM takes a break from the host out of TxBreakLength and sends its length.
 * without it, LAPM sends the break with no length and leaves it queued */
#define OIS11_ENCRYPTION 0x10
#define OIS11_BREAK_LENGTH 0x20
/* under OIS11_XON_XOFF, XON and XOFF control the line's flow, and the
 * receiver drops them from the data. OIS11_KEEP_XON_XOFF keeps them there,
 * at the line and at the port */
#define OIS11_XON_XOFF 0x40
#define OIS11_KEEP_XON_XOFF 0x80
/* OIS19_DICTIONARY picks V.42bis's P1, the dictionary's size. the bits below
 * it hold P2, the longest string, less 6 */
#define OIS19_DICTIONARY 0xc0
/* under OIS25_FOLLOW_LINE, the port's data runs at the line's speed, OIS(28),
 * and otherwise at the terminal's, OIS(27) */
#define OIS25_FOLLOW_LINE 0x10
/* under OIS25_LONG_SPACE_DISCONNECT, on a link without LAPM or MNP, a break
 * of 1.6 s or more from the line hangs up */
#define OIS25_LONG_SPACE_DISCONNECT 0x40

/* set as DTR comes on to force a dial command, which clears it as it takes
 * the dial string */
#define BMR2_DTR_DIAL 0x04
/* set as a hook flash puts the line on hook. the flash ends once it finds
 * the line off hook again with the bit set */
#define BMR2_HOOK_FLASHED 0x40
/* BMR3_TEST is set while any test runs, BMR3_PATTERN while the test sends
 * its pattern, and BMR3_LOCAL_ANALOG or BMR3_REMOTE_DIGITAL while that loop
 * runs. BMR3_TESTS holds all four */
#define BMR3_TEST 0x08
#define BMR3_PATTERN 0x20
#define BMR3_LOCAL_ANALOG 0x40
#define BMR3_REMOTE_DIGITAL 0x80
#define BMR3_TESTS                                                             \
    (BMR3_TEST | BMR3_PATTERN | BMR3_LOCAL_ANALOG | BMR3_REMOTE_DIGITAL)
/* set where modem_reset loads the profile Z_COMMAND names, not the power-up
 * profile */
#define BMR4_Z_PROFILE 0x20
/* the connection's state in BMR6: BMR6_ON_LINE while it holds, with
 * BMR6_DATA in data mode or BMR6_COMMAND in its command mode */
#define BMR6_DATA 0x02
#define BMR6_COMMAND 0x04
#define BMR6_ON_LINE 0x08
/* set from the end of a command line until atcom has run it */
#define BMR6_RUNNING_COMMAND 0x40
/* set while a whole command line waits in CMD_BUF for atcom */
#define BMR7_COMMAND_LINE 0x01
/* set as the host sends a command byte other than S4's. a dial, a test and
 * HOMOLOGATION_PHASE each end on it */
#define BMR7_HOST_BYTE 0x08
/* set as the dial string's T, P or $ picks tone or pulse dialing, and as the
 * auto pulse step starts its dial tone test. each dial clears it once its
 * off-hook wait ends */
#define BMR7_DIALED 0x20
/* set when the link controller resets during a connection, or takes a bad
 * codeword. on_line_maintain then hangs up */
#define BMR7_LINK_LOST 0x80
/* set where the calling tone stays off, by the dial string's ^ or by S37's
 * V.23 */
#define BMR8_NO_CALLING_TONE 0x40
/* set while the modem holds the line off hook */
#define BMR9_OFF_HOOK 0x80
/* set before a result for a call the modem answers, which S14_QUIET_ANSWER
 * then holds back */
#define BMR10_ANSWERING 0x40
/* set where the link runs in direct mode: in a synchronous mode S27_Q picks,
 * or where S36 asks for it */
#define BMR10_DIRECT_MODE 0x10
/* set where a leased line is to connect, by a power-up reset or a handshake
 * under S27_LEASED_LINE. lease_line_monitor connects while it is set. a
 * hang-up clears it, and so does a command from the host */
#define BMR11_LEASED_LINE 0x10
/* set to run the rest of a command line once a dial's ; or a hang-up has
 * run. at_commands clears it as it resumes the line */
#define BMR16_RESUME_LINE 0x01
/* BMR16_DTR_ON is set while DTR is on, as dtr_monitor last saw it.
 * BMR16_DTR_DROPPED is set as DTR drops. dtr_monitor clears it and acts on &D
 * once dtr_timer runs out with DTR off */
#define BMR16_DTR_DROPPED 0x04
#define BMR16_DTR_ON 0x08
/* set while the dialer is in pulse mode: setup_in_pulse sets it, and
 * setup_in_dtmf clears it */
#define BMR18_PULSE 0x04
/* set once read_dial_and_logon_strings has taken a stored number's logon
 * string to send */
#define BMR20_LOGON 0x20
/* set as LAPM reports an error of the link to the host */
#define BMR23_LINK_ERROR 0x80
/* set as LAPM takes the link down: on a DISC, on a DM or a UA answering its
 * DISC, on a DM answering its SABME, and once N400 retries go unanswered */
#define BMR27_LINK_DOWN 0x02
/* set to ask LAPM for a loopback test, which it runs by a TEST frame. it
 * clears as the frame goes out, or as the test fails */
#define BMR27_TEST_REQUEST 0x04
/* set as LAPM takes a SABME or a UA */
#define BMR27_LINK_UP 0x10
/* set as an XID agrees on V.42bis */
#define BMR27_V42BIS 0x20
#define BMR27_LAPM 0x80
/* set as cmd_HALT starts HALT_AFTER_LINE. DRAIN_PHASE then goes on to
 * RESET_DELAY_PHASE */
#define BMR28_HALT_AFTER_LINE 0x20
/* set where ini_link makes a raw link: BMR28_RAW_SYNC where OIS(6) marks the
 * line synchronous, and BMR28_RAW_ASYNC where it doesn't */
#define BMR28_RAW_SYNC 0x40
#define BMR28_RAW_ASYNC 0x80
/* under BMR29_MNP, BMR29_V42BIS makes BMR29_COMPRESSION's V.42bis, not MNP
 * 5 */
#define BMR29_V42BIS 0x01
/* set where V.42bis compresses one direction only. BMR29_FROM_ORIGINATOR is
 * then set where that direction runs from the originator */
#define BMR29_ONE_WAY 0x02
#define BMR29_FROM_ORIGINATOR 0x04
/* set while error control's setup runs. link_made clears it */
#define BMR29_ESTABLISHING 0x10
#define BMR29_MNP 0x20
/* set as error control's setup puts dce's line in HDLC. link_made then
 * leaves dce's line as the setup left it */
#define BMR29_DCE_HDLC 0x40
#define BMR29_COMPRESSION 0x80
/* set to keep ring_monitor from answering on S0's ring. it clears once the
 * rings stop */
#define BMR31_NO_ANSWER 0x01
/* set to have ring_monitor answer once a ring is counted */
#define BMR32_ANSWER 0x20

/* the escape sequence's state. BMR14_ESC_ON is set while the modem watches for
 * the escape character. BMR14_ESC_FIRST and BMR14_ESC_SECOND count the first
 * two escape characters, and BMR21_ESC_THIRD marks the third. while
 * BMR21_LEAD_GUARD is set, an escape character counts only once the guard time
 * has passed since ESC_TOUT. BMR21_GUARD is set while the guard time after the
 * third runs. BMR21_TEXT is set while the text after it is tested, and
 * BMR21_TEXT_VALID once the text passes. BMR14_ESC_COUNT holds the count's
 * two bits, and BMR21_ESCAPE the escape's five bits of BMR21 */
#define BMR14_ESC_FIRST 0x01
#define BMR14_ESC_SECOND 0x02
#define BMR14_ESC_COUNT (BMR14_ESC_FIRST | BMR14_ESC_SECOND)
#define BMR14_ESC_ON 0x20
#define BMR21_LEAD_GUARD 0x01
#define BMR21_TEXT_VALID 0x02
#define BMR21_GUARD 0x04
#define BMR21_ESC_THIRD 0x08
#define BMR21_TEXT 0x10
#define BMR21_ESCAPE                                                           \
    (BMR21_LEAD_GUARD | BMR21_TEXT_VALID | BMR21_GUARD | BMR21_ESC_THIRD |     \
     BMR21_TEXT)
/* set while a break from the line waits in RxBreakLength, with its place in
 * RR in RxBreakPointer */
#define BMR23_LINE_BREAK 0x01
/* set while a break from the host waits in TxBreakLength for the line */
#define BMR25_BREAK_PENDING 0x08
/* the field of OISregisters[1] that says how a break from the host goes to
 * the line. a value of OIS1_BREAK_IN_SEQUENCE sends it after the data that
 * came before it */
#define OIS1_BREAK_TYPE 0x06
#define OIS1_BREAK_IN_SEQUENCE 0x04
/* OIS1_BREAK_EXPEDITED sends the break ahead of that data. a field of 0
 * sends it destructive: the data waiting both ways is dropped */
#define OIS1_BREAK_EXPEDITED 0x02
/* under OIS1_SEND_BREAK a break from the host goes to the line. without it,
 * the controller only reports the break by ECDC_RESPONSE_DTE_BREAK */
#define OIS1_SEND_BREAK 0x01
/* under OIS1_KEEP_LENGTH a break keeps its length. without it, the break
 * takes BREAK_DEFAULT_TICKS */
#define OIS1_KEEP_LENGTH 0x08
/* OIS(OIS_BREAK_LENGTH) holds the default length of a break LAPM takes from
 * the line. update_OIS stores BREAK_DEFAULT_TICKS there */
#define OIS_BREAK_LENGTH 24
/* a break's length where none is kept, in 10 ms ticks */
#define BREAK_DEFAULT_TICKS 30

/* scm_main_core establishes the link's error control in ESTABLISH_PHASE. it
 * passes data in DATA_PHASE. in DRAIN_PHASE it waits for dce's transmit
 * buffer to empty, until data_pass_timer runs out */
#define ESTABLISH_PHASE 2
#define DATA_PHASE 4
#define DRAIN_PHASE 6
/* in RESET_PHASE it resets both interfaces with TCI_sw_reset */
#define RESET_PHASE 0
/* cmd_HALT's waits, which end at fhud_timer. HALT_AFTER_LINE_PHASE waits for
 * the line to take what the terminal sent. HALT_AFTER_TERMINAL_PHASE waits
 * for the terminal to take what the line sent */
#define HALT_AFTER_LINE_PHASE 3
#define HALT_AFTER_TERMINAL_PHASE 5
/* IDLE_PHASE runs ini_link once STATUS_LINKING is set. RESET_DELAY_PHASE
 * resets once data_pass_timer runs out. HOMOLOGATION_PHASE goes off line on
 * BMR7_HOST_BYTE. either halt's wait sets fhud_failed where fhud_timer runs
 * out first */
#define IDLE_PHASE 1
#define RESET_DELAY_PHASE 7
#define HOMOLOGATION_PHASE 8
/* the phase scm_main_core runs the controller in */
extern Uint8 scm_main_phase;
extern Uint16 REQ_LINK;
/* bits of REQ_LINK, the link the host's command asks for. REQ_RAW asks for a
 * raw link: ini_link sets it up in BMR(28)'s raw modes, and cmd_GET_STATUS
 * reports it as PROTOCOL_RAW */
#define REQ_ALLOW_FALLBACK 0x01
#define REQ_ALTERNATE 0x02
#define REQ_LAPM 0x04
#define REQ_IGNORE_TIMEOUT 0x08
#define REQ_RAW 0x10
#define REQ_ORIGINATE 0x40
#define REQ_OFFER_LAPM 0x80
/* bits of the controller's state */
extern Uint8 STATUS_BITS;
/* set while a link is asked for and not yet made. the host's link commands
 * set it, and DRAIN_PHASE sets it to link again after an MNP link. link_made
 * clears it. TCI_sw_reset clears it too, and ini_link calls TCI_sw_reset
 * where it makes no link */
#define STATUS_LINKING 0x01
#define STATUS_CONNECTED 0x02
#define STATUS_COMMAND_MODE 0x04
/* set once the terminal or the line has been idle for the minutes the
 * interface options allow. the controller then hangs up */
#define STATUS_INACTIVE 0x08
/* set as N400 retries of an XID, a SABME or a DISC go unanswered */
#define STATUS_LAPM_FAILED 0x10
/* set to take the link down. LAPM then sends a DISC */
#define STATUS_HANGUP 0x20

/* osifGet10msCount's reading, as timer_expired last took it */
extern Uint32 TIM_10MS;
/* the seconds the terminal and the line have each been idle */
extern Uint16 DTE_INAC_TIM;
extern Uint16 DCE_INAC_TIM;
extern Uint8 fhud_failed;
extern Uint32 fhud_timer;
/* the reading of osifGet10msCount at which DRAIN_PHASE stops waiting */
extern Uint32 data_pass_timer;
extern Uint8 prsc_1;
extern Uint8 Last_BMR27;
extern Uint8 Last_BMR29;
extern Uint8 cmd_parameter_buffer[30];
extern Uint8 cmd_response_buffer[8];
extern Uint8 last_response;
extern Uint8 ecdc_self_test;

/* V.42bis counts the bytes it puts out while it takes RATIO_INPUT bytes in.
 * a count closes once that many or more have gone in. last_nb_out holds the
 * last count */
extern Uint16 last_nb_out;
#define RATIO_INPUT 1024
extern Uint16 n_of_bytes_in;
extern Uint16 n_of_bytes_out;
extern struct EcdcStatistics StatisticInfo;
extern struct EcdcStatus StatusInfo;
#endif
