/* the UART's transmit and receive rings. the modem API fills the transmit
 * ring and drains the receive ring. the UART's interrupts drain the transmit
 * ring and fill the receive ring */
#ifndef NETWORK_UART_RING_H
#define NETWORK_UART_RING_H

#include <sg_xpt.h>

extern Uint32 uart_tx_capacity, uart_rx_capacity;
extern char *uart_tx_buffer, *uart_tx_reader, *uart_tx_writer;
extern int uart_tx_space;
extern char *uart_rx_buffer, *uart_rx_reader, *uart_rx_writer;
extern int uart_rx_space;

/* uart_take_tx copies up to size queued bytes into buffer, and sets *pending
 * to the bytes still queued. uart_put_rx copies up to size bytes of buffer
 * into the receive ring, and sets *rejected to the rest. each masks every
 * interrupt while it copies, and returns how many bytes it copied.
 * uart_tx_complete and uart_rx_complete do nothing */
int uart_take_tx(char *buffer, int size, int *pending);
int uart_put_rx(char *buffer, int size, volatile int *rejected);
void uart_tx_complete(void);
void uart_rx_complete(void);

#endif
