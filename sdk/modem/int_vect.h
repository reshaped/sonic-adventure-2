/* int_vect: the interrupts the modem's serial channel raises to the host */
#ifndef MODEM_INT_VECT_H
#define MODEM_INT_VECT_H
#include <sg_xpt.h>

/* four bytes the SDK's interrupt handler exports, which no member reads */
extern Uint8 WinQ[4];

/* serve the events the modem's side has raised to the host's side. it
 * acknowledges those pending on glbScc_Pc, and dispatches those the port has
 * unmasked. the serial channel must be open */
void Comm_Int(void);
#endif
