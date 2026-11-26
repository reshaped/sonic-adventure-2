/* the PPP link's serial line: the modem port ppp_modem_device names, and the
 * interrupt mask around its calls */
#ifndef NETWORK_PPP_SERIAL_H
#define NETWORK_PPP_SERIAL_H
#include <sg_xpt.h>
struct ppp_connection;
/* the line PP_ComOpen opens, which reads neither enabled nor flags. port
 * must be 0.
 *
 *   parity     0 even, 1 odd, any other none
 *   stop       1 two stop bits, any other one
 *   data_bits  1 seven bits, any other eight
 *   speed      a PPP_SPEED_, any other 115200 bps
 */
struct ppp_serial_config {
    Sint32 port, parity, stop, enabled, speed, data_bits;
    Sint16 flags;
};
enum {
    PPP_SPEED_300,
    PPP_SPEED_1200,
    PPP_SPEED_2400,
    PPP_SPEED_4800,
    PPP_SPEED_9600,
    PPP_SPEED_14400,
    PPP_SPEED_19200,
    PPP_SPEED_38400,
    PPP_SPEED_57600,
    PPP_SPEED_115200,
    PPP_SPEED_230400,
    PPP_SPEED_460800
};
/* PP_ComOpen's failure */
enum { PPP_SERIAL_FAILED = 2 };
/* the line's events, which ppp_iostatus takes, and the commands PP_Com_ioctl
 * takes for them. a hang-up drops DTR and RTS and closes the port, and a
 * connect raises both */
enum { PPP_SERIAL_HANG_UP = 129, PPP_SERIAL_CONNECT };
/* 1 while the port is open. PP_ComClose hangs up only then */
extern short PP_ComOpen_Flag;
extern int modem_driver_opened;
/* mask every interrupt, or unmask every one, and return the mask level it
 * replaced, which PP_OS_Restore puts back */
int PP_OS_Disable(void);
int PP_OS_Enable(void);
void PP_OS_Restore(short level);
/* open the port, raise DTR and RTS, and set the line config gives. the
 * result is 0, or PPP_SERIAL_FAILED where config's port isn't 0 or the port
 * won't open. in the SHC build a port that won't open leaves every interrupt
 * masked */
int PP_ComOpen(const struct ppp_serial_config *config);
/* zero size bytes at bytes, read up to size into them, and set *received to
 * the count read. the result is 0. in the GCC build it is -1, with *received
 * 0, where mdmRead's count is negative or past size */
int PP_ComRead(Uint16 size, char *bytes, Uint16 *received);
int PP_ComRead_Direct(Uint16 size, char *bytes, Uint16 *received);
/* write size bytes from bytes. the SHC build writes as many as the transmit
 * ring has room for, and returns 0 either way. the GCC build returns 0 once
 * all are written. it returns -1 for a full ring or a write that fails */
int PP_ComWrite(Uint16 size, char *bytes);
void PP_Com_reflush(struct ppp_connection *config);
/* PP_ComReadedLen sets *length to the bytes waiting to be read, and
 * PP_ComWritableLen to the room left to write. each returns 0 */
int PP_ComReadedLen(Uint16 *length);
int PP_ComWritableLen(Uint16 *length);
/* the result is 1 for a PPP_SERIAL_ command, and 0 for any other, which does
 * nothing */
int PP_Com_ioctl(Uint16 command);
int PP_ComInitialize(void);
#endif
