/* atcom: the AT command interpreter, and the reports its commands send the
 * terminal */
#ifndef MODEM_ATCOM_H
#define MODEM_ATCOM_H
#include <sg_xpt.h>

#include "modemdef.h"

/* what a command leaves in at_com_result */
enum at_result {
    AT_CONTINUE, /* run the next command */
    AT_OK, /* end the line with OK */
    AT_ERROR, /* end the line with ERROR */
    AT_NO_CARRIER, /* end the line with NO CARRIER */
    AT_SILENT, /* end the line with no result */
    AT_WAIT_LINE, /* end the line, and leave its result to what it started */
    AT_WAIT_COMMAND /* keep the line, for a later at_commands to go on with */
};

/* whether digit is a DTMF digit: 0 to 9, A to D, * or # */
char isphonedigit(Uint8 digit);
/* copy source, up to its S3_THEN or nul, into destination and end it with
 * S3_THEN. return 0 for a string longer than MODEM_DIAL_LENGTH, which leaves
 * destination unterminated. destination needs MODEM_DIAL_LENGTH + 1 bytes */
char copy_dial_string(Uint8 *destination, Uint8 *source);
void hangup_voice_call(Uint8 mode);
void atcomResetBlackList(void);
void advance_ptr_and_eliminate_space(void);
/* read the decimal number at ptr_CMD_ALT and move ptr_CMD_ALT past it.
 * return the number mod 256, or 0 where no digit stands there */
Uint8 read_number(void);
/* the least and most value each S-register takes, and the factory profile
 * that &F0 and modem_reset load */
extern const Uint8 S_range_min[MODEM_S_REGISTERS];
extern const Uint8 S_range_max[MODEM_S_REGISTERS];
extern const Uint8 S_REG[MODEM_S_REGISTERS];

/* send the terminal each option's setting and each listed S-register's
 * value, from Sregisters. with saved set, the report adds the EEPROM's
 * power-up profile as &Y and the country as @C. its list of S-registers adds
 * S3, S41, S44 and S46 */
void send_configuration_to_dte(Uint8 saved);
void send_list_of_dial_string(Uint8 first, Uint8 last);
#define REPORT_LAST_COLUMN 74
/* send name and then value, in decimal of at least width digits, counting
 * the line's characters in line_length. the line ends first where line_length
 * is past REPORT_LAST_COLUMN, and a space follows the value where it isn't */
void send_conf_to_dte(char *name, Uint8 value, Uint8 width);
void send_dc_cr_lf(void);

/* run the command line the terminal has finished in CMD_BUF, or resume the
 * one that waits. the terminal gets the result of a line that ends */
void at_commands(void);
void answer_modem_on_pstn(void);
void do_modem_dial_on_pstn(void);
/* copy the dial string at *source into buffer, and end it with S3_THEN. Sn
 * or S=n in the string stands for stored number n. buffer needs limit + 2
 * bytes.
 *
 * the result is 1 where the string ends at S3_THEN, which buffer takes with
 * a nul after it. it is 2 for a string that stops after a ';'. it is 3 for
 * one that reaches limit bytes. after 1, 2 and 3, *source is at the S3_THEN or
 * past the last byte copied. the result is 0 for a slot past the directory,
 * or a stored number the call can't copy.
 *
 * with resume set, *source isn't read, and buffer gets stored number 0 under
 * &Q2, or else nothing. the result is then 1, with no nul after the
 * S3_THEN */
Uint8 get_dial_string(Uint8 **source, Uint8 *buffer, char resume, Uint16 limit);
void H0_common_section(void);
/* set_data_carrier_detect_option runs &C, set_data_terminal_option &D,
 * set_data_set_ready_options &S and test_command &T. each refuses a setting
 * it doesn't take with AT_ERROR in at_com_result */
void set_data_carrier_detect_option(Uint8 setting);
void set_data_terminal_option(Uint8 setting);
void set_data_set_ready_options(Uint8 setting);
void test_command(Uint8 test);
extern Uint8 second_dialing, use_keypad_ie, keypad_index, time_block_expired;
extern Uint32 IsdnRedialTimer;

#endif
