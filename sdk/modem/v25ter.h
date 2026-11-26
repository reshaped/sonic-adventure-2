/* Extended AT commands and the modulation policy they read and update. */
#ifndef MODEM_V25TER_H
#define MODEM_V25TER_H
#include <sg_xpt.h>

/* These bits report the eight connection modes from S36. */
struct v25ter_config {
    Uint16 reserved : 8;
    Uint16 bit7 : 1;
    Uint16 bit6 : 1;
    Uint16 bit4 : 1;
    Uint16 bit5 : 1;
    Uint16 bit3 : 1;
    Uint16 bit2 : 1;
    Uint16 bit1 : 1;
    Uint16 bit0 : 1;
};

/* run the extended command after the + at *command, and move *command past
 * it. it returns V25TER_OK for a command it ran and V25TER_ERROR for one it
 * refuses, but for +FCLASS, whose result it never sets */
Uint8 v25terReadCommand(Uint8 **command);
/* the results as atcom's v25_result reads them. it also reads V25TER_SILENT,
 * V25TER_WAIT_COMMAND and V25TER_WAIT_LINE, which no command returns.
 * V25TER_SILENT ends the line with no result where nothing follows the
 * command */
#define V25TER_OK 1
#define V25TER_SILENT 2
#define V25TER_ERROR 3
#define V25TER_WAIT_COMMAND 4
#define V25TER_WAIT_LINE 5
void send_message_to_dte(char *text);
/* Intersect *modulations with the pump's supported modes and remove any
 * whose rate limits exclude the requested range. A maximum of zero
 * leaves that direction open. All rates are data-pump codes. */
void v25terRemoveModNotAuthorised(Uint32 *modulations, Uint8 min_tx,
                                  Uint8 max_tx, Uint8 min_rx, Uint8 max_rx);
void v25terUpdateV25terPar(void);
struct v25ter_config v25terReportConf(void);
/* Return the highest single bit of mask, or one for a zero mask. */
Uint32 v25terFindMaxBit(Uint32 mask);

#endif
