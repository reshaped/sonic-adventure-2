/* The line and terminal interfaces of the software modem. */
#ifndef MODEM_UTIL_MOD_H
#define MODEM_UTIL_MOD_H
#include <sg_xpt.h>

extern const Uint8 invert_byte_table[256];
extern const Uint8 S23_to_fmt[4];
/* Write at least width digits in base 10 or 16, then a NUL. The caller
 * supplies room for the larger of width and the digit count, plus one. */
void convert_value_to_ascii(Uint32 value, Uint8 base, Uint8 width,
                            char *buffer);
Uint8 is_hex_number(Uint8 character);
void update_SS22(void);
void update_S44(void);
void update_OIS(void);
/* modem_reset's modes past ATZ's profiles, which are below EEPROM_PROFILES.
 * RESET_KEEP_PUMP leaves the data pump as it is */
#define RESET_KEEP_PUMP 0xfd
#define RESET_DTR 0xfe
#define RESET_POWER_UP 0xff
void modem_reset(Uint8 mode);

void init_on_line(void);
void umCarrierDetected(void);
void modem_connect(void);
void carrier_lost(void);
void go_off_line(void);

/* Queue a command through the same AT buffer as a request from the terminal. */
void force_h0_cmd(void);
void force_a_cmd(void);
void force_d_cmd(void);
void force_z_cmd(void);
void force_DS_cmd(Uint8 number);
void force_o_cmd(void);
void enter_escape_mode(void);
void set_on_line_cmd(void);
void set_command_mode(void);
void set_dte_for_message(void);
void adj_spk_level(void);
void redial_monitor(void);
void GetXonXoffValues(Uint8 *xon, Uint8 *xoff);
Uint8 dtr_monitor(void);
void escape_seq_monitor(void);
void rts_monitor(void);

#endif
