#include "uart_ring.h"

#define SERIAL_RING(name) uart_##name
#include "serial_ring_impl.h"
