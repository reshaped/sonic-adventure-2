#ifndef NETWORK_MODEM_BAUD_H
#define NETWORK_MODEM_BAUD_H
#include <mdmAPI.h>

/* turn rate, an MDD_LINESPEED_ code, into bits a second, in place. a value
 * no case names stays as it is */
#define MODEM_BAUD_RATE(rate)                                                  \
    switch ((rate)) {                                                          \
    case MDD_LINESPEED_300:                                                    \
        (rate) = 300;                                                          \
        break;                                                                 \
    case MDD_LINESPEED_1200:                                                   \
        (rate) = 1200;                                                         \
        break;                                                                 \
    case MDD_LINESPEED_2400:                                                   \
        (rate) = 2400;                                                         \
        break;                                                                 \
    case MDD_LINESPEED_4800:                                                   \
        (rate) = 4800;                                                         \
        break;                                                                 \
    case MDD_LINESPEED_9600:                                                   \
        (rate) = 9600;                                                         \
        break;                                                                 \
    case MDD_LINESPEED_14400:                                                  \
        (rate) = 14400;                                                        \
        break;                                                                 \
    case MDD_LINESPEED_19200:                                                  \
        (rate) = 19200;                                                        \
        break;                                                                 \
    case MDD_LINESPEED_38400:                                                  \
        (rate) = 38400;                                                        \
        break;                                                                 \
    case MDD_LINESPEED_57600:                                                  \
        (rate) = 57600;                                                        \
        break;                                                                 \
    case MDD_LINESPEED_115200:                                                 \
        (rate) = 115200;                                                       \
        break;                                                                 \
    case MDD_LINESPEED_230400:                                                 \
        (rate) = 230400;                                                       \
        break;                                                                 \
    case MDD_LINESPEED_460800:                                                 \
        (rate) = 460800;                                                       \
        break;                                                                 \
    }
#endif
