/* the expansion UART's registers, each a byte at the start of a four-byte
 * slot on the G2 bus. SERIAL_RESET alone takes a 32-bit word */
#ifndef NETWORK_UART_HARDWARE_H
#define NETWORK_UART_HARDWARE_H
#include <mdmAPI.h>
#include <sg_chain.h>
#include <sg_xpt.h>

#define SERIAL_LINE_CONTROL (*(volatile Uint8 *)0xa060040c)
#define SERIAL_MODEM_CONTROL (*(volatile Uint8 *)0xa0600410)
#define SERIAL_FIFO_CONTROL (*(volatile Uint8 *)0xa0600408)
/* the FIFO control register's address, which reads as the interrupt
 * identification register */
#define SERIAL_INTERRUPT_ID (*(volatile Uint8 *)0xa0600408)
#define SERIAL_INTERRUPT_ENABLE (*(volatile Uint8 *)0xa0600404)
#define SERIAL_SCRATCH (*(volatile Uint8 *)0xa060041c)
#define SERIAL_MODEM_STATUS (*(volatile Uint8 *)0xa0600418)
#define SERIAL_LINE_STATUS (*(volatile Uint8 *)0xa0600414)
#define SERIAL_PRESENT (*(volatile Sint8 *)0xa0600004)
#define SERIAL_RESET (*(volatile Uint32 *)0xa0600480)

#define SERIAL_DATA (*(volatile Uint8 *)0xa0600400)
/* the divisor latch's bytes, at SERIAL_DATA's and SERIAL_INTERRUPT_ENABLE's
 * addresses while LCR_DLAB is set */
#define SERIAL_DIVISOR_LOW (*(volatile Uint8 *)0xa0600400)
#define SERIAL_DIVISOR_HIGH (*(volatile Uint8 *)0xa0600404)
/* the bit of SERIAL_PRESENT that uart_modem_open needs set to open the UART */
#define PRESENT_UART 0x02

/* the registers' bits, where a 16550 holds them */
#define LCR_CHARLEN 0x03
#define LCR_STOPBIT 0x04
#define LCR_PARITY 0x38
#define LCR_BREAK 0x40
#define LCR_DLAB 0x80
#define MCR_DTR 0x01
#define MCR_RTS 0x02
/* a 16750's bit: automatic RTS and CTS flow control */
#define MCR_AUTO_FLOW 0x20
#define MSR_CTS_SHIFT 4
#define MSR_DSR_SHIFT 5
#define MSR_RI_SHIFT 6
#define MSR_DCD_SHIFT 7
#define MSR_CTS (1 << MSR_CTS_SHIFT)
#define MSR_DSR (1 << MSR_DSR_SHIFT)
#define MSR_RI (1 << MSR_RI_SHIFT)
#define MSR_DCD (1 << MSR_DCD_SHIFT)
#define LSR_DATA_READY 0x01
#define LSR_OVERRUN 0x02
#define LSR_PARITY_ERROR 0x04
#define LSR_FRAMING_ERROR 0x08
#define LSR_BREAK_SHIFT 4
#define LSR_BREAK (1 << LSR_BREAK_SHIFT)
#define LSR_TX_EMPTY 0x20
#define LSR_TX_IDLE 0x40
#define LSR_FIFO_ERROR 0x80
#define IER_RECEIVE 0x01
#define IER_TRANSMIT 0x02
#define IER_LINE 0x04
#define IER_MODEM 0x08
#define FCR_ENABLE 0x01
#define FCR_RX_RESET 0x02
#define FCR_TX_RESET 0x04
/* uart_fifo_trigger's shift into the FIFO control register */
#define FCR_TRIGGER_SHIFT 4
/* the interrupt identification register's cause, in its low four bits.
 * IIR_NONE_PENDING is set while no interrupt is pending */
#define IIR_CAUSE 0x0f
#define IIR_NONE_PENDING 0x01
#define IIR_MODEM 0x00
#define IIR_TRANSMIT 0x02
#define IIR_RECEIVE 0x04
#define IIR_LINE 0x06
#define IIR_TIMEOUT 0x0c
/* byte with the bits under mask replaced by bits */
#define REPLACE_BITS(byte, mask, bits) (((byte) & (0xff ^ (mask))) | (bits))

/* a read of the scratch register, then a write of it back. the driver makes
 * one after each access to a control register */
/* @hack redundant: a loop of one pass over a volatile counter, where the read
 * and write alone would do */
#define SERIAL_DELAY()                                                         \
    {                                                                          \
        volatile Uint8 value;                                                  \
        volatile int wait;                                                     \
        for (wait = 0; wait < 1; wait++) {                                     \
            value = SERIAL_SCRATCH;                                            \
            SERIAL_SCRATCH = value;                                            \
        }                                                                      \
    }

extern volatile Bool uart_tx_paused, uart_rx_paused;
extern Sint8 uart_fifo_trigger;
extern SYCHAIN uart_chain;
extern int uart_speed;
extern int uart_received_interrupts, uart_transmitted_interrupts;
void uart_interrupt(void *argument);
void uart_send_byte(Uint8 byte);
Uint8 uart_read_byte(void);
void uart_enable_interrupts(Uint8 mask);
void uart_disable_interrupts(Uint8 mask);
void uart_install(void);
void uart_remove(void);
void uart_set_baud(Float baud);
void uart_set_speed(int baud_code);
void uart_modem_service(void);
/* uart_modem_set_default in the SHC build, declared as the driver calls it,
 * with the state alone. the modem API calls the same entry with a port
 * first, which the entry never reads */
void uart_apply_default(MDS_DCB state);

#endif
