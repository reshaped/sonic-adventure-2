/* messages: the AT results the modem sends the terminal */
#ifndef MODEM_MESSAGES_H
#define MODEM_MESSAGES_H
#include <sg_xpt.h>

/* the default text of each AT result, by its number. a text given to
 * send_coded_message that isn't null replaces it in verbose mode */
extern const char *const CodedMessages[134];
#define RESULT_OK 0
#define RESULT_CONNECT 1
#define RESULT_RING 2
#define RESULT_NO_CARRIER 3
#define RESULT_ERROR 4
#define RESULT_NO_DIALTONE 6
#define RESULT_BUSY 7
#define RESULT_NO_ANSWER 8
#define RESULT_CONNECT_38400 15
#define RESULT_CONNECT_57600 16
#define RESULT_CONNECT_115200 17
#define RESULT_RINGING 18
#define RESULT_CONNECT_230400 19
#define RESULT_CONNECT_64000 21
#define RESULT_CONNECT_460800 24
#define RESULT_CONNECT_691200 25
#define RESULT_CONNECT_921600 26
#define RESULT_CARRIER_300 40
#define RESULT_CARRIER_2400 47
#define RESULT_CONNECT_75TX_1200RX 62
#define RESULT_CONNECT_1200TX_75RX 63
#define RESULT_CARRIER_75TX_1200RX 64
#define RESULT_CARRIER_1200TX_75RX 65
#define RESULT_PROTOCOL_NONE 70
#define RESULT_PROTOCOL_V42 77
#define RESULT_PROTOCOL_MNP_REL 79
#define RESULT_CONNECT_128000 96
#define RESULT_TEST_FAILED 97
#define RESULT_TEST_PASSED 98
#define RESULT_CIRCUIT_BUSY 99
void send_coded_message(Uint8 code, const char *text);
void send_connect_message(void);
void send_carrier_message(void);
Uint8 get_dte_spd_mess(void);
void add_bis_secur(char **cursor);

/* send_message selects the terminal's interface first.
 * send_message_in_current_mode sends where the last selection left it. each
 * sends a string up to its nul */
void send_message(const char *text);
void send_message_in_current_mode(const char *text);
void message_byte_to_dte(Uint8 value);
void send_cr_lf(void);
void parity_adjust(Uint8 *byte);

void send_bps_message(Uint8 speed);
/* return the bits per second of speed, a TCI speed code from 0 to 59. a code
 * of 0xf0 or more gives 115200 */
Uint32 ConvertTCISpeedToBps(Uint8 speed);
#define TCI_SPEED_300 1
#define TCI_SPEED_1200 2
#define TCI_SPEED_2400 3
#define TCI_SPEED_4800 4
#define TCI_SPEED_7200 5
#define TCI_SPEED_9600 6
#define TCI_SPEED_12000 7
#define TCI_SPEED_14400 8
#define TCI_SPEED_38400 27
#define TCI_SPEED_115200 51
#define TCI_SPEED_230400 53
#define TCI_SPEED_460800 54

#endif
