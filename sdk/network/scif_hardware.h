/* the SH-4's SCIF, its serial port with FIFOs, as scif_modem.c and
 * sh4scif.c drive it. SCIF_VECTOR names the slots, offsets from VBR, that
 * hold the handlers of its four interrupts */
#ifndef NETWORK_SCIF_HARDWARE_H
#define NETWORK_SCIF_HARDWARE_H

#include <machine.h>
#include <sg_xpt.h>

typedef void (*scif_handler)(void);
#define SCIF_VECTOR(offset) (*(scif_handler *)&((Uint8 *)get_vbr())[offset])
#define SCIF_ERROR_VECTOR 0x2a0
#define SCIF_RX_VECTOR 0x2a4
#define SCIF_BREAK_VECTOR 0x2a8
#define SCIF_TX_VECTOR 0x2ac

/* SCSMR2 as a signed halfword, as scif_modem.c reads it back */
#define SCIF_MODE (*(volatile Sint16 *)0xffe80000)
#define SCSMR2 (*(volatile Uint16 *)0xffe80000)
#define SCBRR2 (*(volatile Uint8 *)0xffe80004)
#define SCSCR2 (*(volatile Sint16 *)0xffe80008)
#define SCFTDR2 (*(volatile Uint8 *)0xffe8000c)
#define SCFSR2 (*(volatile Sint16 *)0xffe80010)
#define SCFRDR2 (*(volatile Uint8 *)0xffe80014)
#define SCFCR2 (*(volatile Uint16 *)0xffe80018)
/* SCFCR2's bits: a reset of both FIFOs, CTS and RTS flow control, and a
 * receive trigger at 14 bytes */
#define SCIF_FIFO_RESET 0x06
#define SCIF_FLOW_CONTROL 0x08
#define SCIF_RX_TRIGGER_14 0xc0
/* a receive trigger at 8 bytes */
#define SCIF_RX_TRIGGER_8 0x80
/* SCFCR2 as a signed halfword, as scif_install.c reads and clears it.
 * sh4scif.c takes it unsigned, as SCFCR2 */
#define SCIF_FIFO_CONTROL (*(volatile Sint16 *)&SCFCR2)
#define SCFDR2 (*(volatile Sint16 *)0xffe8001c)
/* the bytes the transmit FIFO and the receive FIFO hold, from SCFDR2's high
 * and low bytes */
#define SCIF_TX_COUNT ((SCFDR2 & 0xff00) >> 8)
#define SCIF_RX_COUNT ((Uint8)SCFDR2)
#define SCSPTR2 (*(volatile Uint16 *)0xffe80020)
#define SCLSR2 (*(volatile Sint16 *)0xffe80024)
#define SCBRR1 (*(volatile Uint8 *)0xffe00004)
#define ICR (*(volatile Uint16 *)0xffd00000)
#define IPRC (*(volatile Uint16 *)0xffd0000c)
/* IPRC's field for the SCIF's interrupt priority. with its four bits set, it
 * is 15, the highest */
#define IPRC_SCIF 0xf0

#define SCIF_FIFO_BYTES 16
#define SCIF_BAUD_SETTLE 0x80000
#define SCIF_RX_IRQ 0x40
#define SCIF_TX_IRQ 0x80
#define SCIF_TX_EMPTY 0x20
/* SCSCR2's enables of the transmitter and the receiver, beside SCIF_TX_IRQ
 * and SCIF_RX_IRQ */
#define SCIF_TX_ENABLE 0x20
#define SCIF_RX_ENABLE 0x10
/* SCFSR2's bits beside SCIF_TX_EMPTY. SCIF_RX_TRIGGERED says the receive
 * FIFO holds its trigger's count or more. SCIF_RX_READY says it holds some,
 * fewer than that, and none has arrived for 15 bit times */
#define SCIF_RX_ERROR 0x80
#define SCIF_TX_END 0x40
#define SCIF_BREAK 0x10
#define SCIF_FRAMING_ERROR 0x08
#define SCIF_PARITY_ERROR 0x04
#define SCIF_RX_TRIGGERED 0x02
#define SCIF_RX_READY 0x01
/* SCLSR2's one bit */
#define SCIF_OVERRUN 0x01
/* SCSMR2's bits: 7-bit characters, parity, odd parity and 2 stop bits */
#define SCIF_CHAR_7 0x40
#define SCIF_PARITY 0x20
#define SCIF_PARITY_ODD 0x10
#define SCIF_STOP_2 0x08

#endif
