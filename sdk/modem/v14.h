/* v14: V.14's asynchronous characters on the data pump's synchronous bit
 * stream */
#ifndef MODEM_V14_H
#define MODEM_V14_H
#include <sg_xpt.h>

/* the callbacks that open and close a frame each way. RxBufferLow is nonzero
 * once the receive ring can take data again. v14Initialize sets them to
 * dce_line.h's functions, but for a type of 22 to 24, which keeps the
 * caller's */
extern void (*OpenRxBuffer)(void);
extern void (*CloseRxBuffer)(void);
extern void (*OpenTxBuffer)(void);
extern void (*CloseTxBuffer)(void);
extern Uint8 (*RxBufferLow)(void);

/* v14_transmit returns the line's next bits bits, at most 32, at the top of
 * the word. the first to send is the lowest of them. v14_receive takes bits
 * bits of the line from input, the first in bit 0 */
Uint32 v14_transmit(Uint8 bits);
void v14_receive(Uint32 input, Uint8 bits);
/* v14InitializeTx and v14InitializeRx start one way afresh. v14Initialize
 * starts both for a line of that type, speed code, format and flow control */
void v14InitializeTx(void);
void v14InitializeRx(void);
void v14Initialize(Uint8 type, Uint8 speed, Uint8 format, Uint8 flow);
/* at speed codes 0 and 1 the data pump frames the characters itself.
 * async_receive then moves its bytes into the receive frame.
 * async_receive_idle_timeout closes a frame 100 ms after its last byte */
void async_receive(void);
void async_receive_idle_timeout(void);
/* send a break once the character going out ends, until ticks of 10 ms
 * after the call. it does nothing while a break goes out */
void sendv14_dce_break(Uint16 ticks);

#endif
