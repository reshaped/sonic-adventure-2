/* the body of scif_ring.c and uart_ring.c. each defines SERIAL_RING(name) to
 * give every name here its driver's prefix. scif_ring.h and uart_ring.h give
 * the contract */
#include <machine.h>
#include <sg_xpt.h>

Uint32 SERIAL_RING(tx_capacity);
Uint32 SERIAL_RING(rx_capacity);
char *SERIAL_RING(tx_buffer);
char *SERIAL_RING(tx_reader);
char *SERIAL_RING(tx_writer);
int SERIAL_RING(tx_space);
char *SERIAL_RING(rx_buffer);
char *SERIAL_RING(rx_reader);
char *SERIAL_RING(rx_writer);
int SERIAL_RING(rx_space);

int SERIAL_RING(take_tx)(char *buffer, int size, int *pending)
{
    Uint16 saved_mask;
    char *cursor;
    int count;
    int transferred;
    Uint32 available;
    char *destination;

    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    cursor = SERIAL_RING(tx_reader);
    destination = buffer;
    available = SERIAL_RING(tx_capacity) - SERIAL_RING(tx_space);
    if (available > (Uint32)size) {
        count = size;
        transferred = size;
        *pending = available - count;
    } else {
        count = available;
        transferred = available;
        *pending = 0;
    }
    SERIAL_RING(tx_space) += count;
    while (count > 0) {
        *destination++ = *cursor++;
        if (cursor == SERIAL_RING(tx_buffer) + SERIAL_RING(tx_capacity)) {
            cursor = SERIAL_RING(tx_buffer);
        }
        /* @hack spelling: count = count - 1, where count-- would do */
        count = count - 1;
    }
    SERIAL_RING(tx_reader) = cursor;
    set_imask(saved_mask);
    return transferred;
}

int SERIAL_RING(put_rx)(char *buffer, int size, volatile int *rejected)
{
    Uint16 saved_mask;
    char *cursor;
    int count;
    int transferred;
    char *source;

    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    cursor = SERIAL_RING(rx_writer);
    source = buffer;
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((count = SERIAL_RING(rx_space)) > size) {
        transferred = count = size;
        *rejected = 0;
    } else {
        transferred = count;
        *rejected = size - count;
    }
    SERIAL_RING(rx_space) -= count;
    while (count > 0) {
        *cursor++ = *source++;
        if (cursor == SERIAL_RING(rx_buffer) + SERIAL_RING(rx_capacity)) {
            cursor = SERIAL_RING(rx_buffer);
        }
        count--;
    }
    SERIAL_RING(rx_writer) = cursor;
    set_imask(saved_mask);
    return transferred;
}

void SERIAL_RING(tx_complete)(void)
{
}
void SERIAL_RING(rx_complete)(void)
{
}
