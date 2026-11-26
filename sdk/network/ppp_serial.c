#include <machine.h>
#include <mdmAPI.h>
#include <sg_xpt.h>
#include <string.h>
#include "ppp_link.h"
#include "ppp_modem.h"
#include "ppp_serial.h"
#include "ppp_system.h"

#pragma section ATA
static char build_version[44];
/* the library's version and build time */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
char *ntgPppBuild = build_version;
static char build_version[44] = "\nntPpp Ver 1.08 Build:Oct 25 2000 13:29:30\n";
#pragma section

MD_DEVICE ppp_modem_device = MDD_DEVICE_G2_MODEM;
short PP_ComOpen_Flag = 0;
static short read_paused = 0;
int modem_driver_opened = 0;
#ifdef DREAMCAST
static Uint8 reserved_data[4] = {0};
#endif
MDM ppp_modem_port[1];
char ppp_rx_storage[PPP_MODEM_BUFFER_BYTES];
char ppp_tx_storage[PPP_MODEM_BUFFER_BYTES];

int PP_OS_Disable(void)
{
    int saved_mask = get_imask();
    set_imask(IMASK_ALL);
    return saved_mask;
}
int PP_OS_Enable(void)
{
    int saved_mask = get_imask();
    set_imask(0);
    return saved_mask;
}
void PP_OS_Restore(short level)
{
    set_imask(level);
}
static int invalid_port(Uint16 port)
{
    if (port >= 1) {
        return 1;
    }
    return 0;
}
void PP_ComClose(void)
{
    if (PP_ComOpen_Flag) {
        PP_ComOpen_Flag = 0;
        PP_Com_ioctl(PPP_SERIAL_HANG_UP);
    }
}
int PP_ComOpen(const struct ppp_serial_config *config)
{
    short saved_mask;
    MDS_DCB state;
    /* @hack spelling: ppp_modem_port read through port, where
     * ppp_modem_port[0] would do */
    MDM *port;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it fails for a NULL config */
    if (!config) {
        return PPP_SERIAL_FAILED;
    }
#endif
    /* @bug memory: reads config before the test below, which allows it to
     * be NULL (0x0c9e0fde, 0x0c9e1004) */
    if (invalid_port(config->port) == 1) {
        return PPP_SERIAL_FAILED;
    }
    port = ppp_modem_port;
    saved_mask = PP_OS_Disable();
    *port = mdmOpen(ppp_modem_device);
    if (*port == (MDM)-1) {
        /* @bug logic: returns without PP_OS_Restore, so every interrupt
         * stays masked (0x0c9e0ff4) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it restores the interrupt
         * mask */
        PP_OS_Restore(saved_mask);
#endif
        return PPP_SERIAL_FAILED;
    }
    PP_Com_ioctl(PPP_SERIAL_CONNECT);
    if (config) {
        mdmGetCommState2(*port, &state);
        state.CharLen =
            config->data_bits == 1 ? MDD_LCR_CHARLEN_7 : MDD_LCR_CHARLEN_8;
#ifdef NONMATCHING
        /* this side tests each value once, the plain form the hack tag below
         * gives */
        if (config->parity == 0) {
            state.Parity = MDD_LCR_PARITY_EVEN;
        } else if (config->parity == 1) {
            state.Parity = MDD_LCR_PARITY_ODD;
        } else {
            state.Parity = MDD_LCR_PARITY_NONE;
        }
        state.StopBit =
            config->stop == 1 ? MDD_LCR_STOPBIT_2B : MDD_LCR_STOPBIT_1B;
#else
        /* @hack redundant: conditionals that test parity for 2 and stop for
         * 0, which give what any other value gives, where the tests above
         * would do */
        /* NOLINTBEGIN(readability-avoid-nested-conditional-operator) */
        state.Parity = config->parity == 2   ? MDD_LCR_PARITY_NONE
                       : config->parity == 1 ? MDD_LCR_PARITY_ODD
                       : config->parity == 0 ? MDD_LCR_PARITY_EVEN
                                             : MDD_LCR_PARITY_NONE;
        state.StopBit = config->stop == 0   ? MDD_LCR_STOPBIT_1B
                        : config->stop == 1 ? MDD_LCR_STOPBIT_2B
                                            : MDD_LCR_STOPBIT_1B;
        /* NOLINTEND(readability-avoid-nested-conditional-operator) */
#endif
        switch (config->speed) {
        case PPP_SPEED_300:
            state.LineSpeed = MDD_LINESPEED_300;
            break;
        case PPP_SPEED_1200:
            state.LineSpeed = MDD_LINESPEED_1200;
            break;
        case PPP_SPEED_2400:
            state.LineSpeed = MDD_LINESPEED_2400;
            break;
        case PPP_SPEED_4800:
            state.LineSpeed = MDD_LINESPEED_4800;
            break;
        case PPP_SPEED_9600:
            state.LineSpeed = MDD_LINESPEED_9600;
            break;
        case PPP_SPEED_14400:
            state.LineSpeed = MDD_LINESPEED_14400;
            break;
        case PPP_SPEED_19200:
            state.LineSpeed = MDD_LINESPEED_19200;
            break;
        case PPP_SPEED_38400:
            state.LineSpeed = MDD_LINESPEED_38400;
            break;
        case PPP_SPEED_57600:
            state.LineSpeed = MDD_LINESPEED_57600;
            break;
        case PPP_SPEED_115200:
            state.LineSpeed = MDD_LINESPEED_115200;
            break;
        case PPP_SPEED_230400:
            state.LineSpeed = MDD_LINESPEED_230400;
            break;
        case PPP_SPEED_460800:
            state.LineSpeed = MDD_LINESPEED_460800;
            break;
        default:
            state.LineSpeed = MDD_LINESPEED_115200;
            break;
        }
        mdmSetCommState2(*port, state);
    }
    PP_ComOpen_Flag = 1;
    PP_OS_Restore(saved_mask);
    return 0;
}
int PP_ComRead(Uint16 size, char *bytes, Uint16 *received)
{
    /* @hack spelling: ppp_modem_port read through port, where
     * ppp_modem_port[0] would do */
    MDM *port = ppp_modem_port;
    int count;
    if (read_paused == 1) {
        *received = 0;
    } else {
        memset(bytes, 0, size);
        count = mdmRead(*port, bytes, size);
#ifdef NONMATCHING
        /* this side fails a count mdmRead gives below 0 or past size */
        if (count < 0 || count > size) {
            *received = 0;
            return -1;
        }
#endif
        *received = count;
    }
    return 0;
}
int PP_ComRead_Direct(Uint16 size, char *bytes, Uint16 *received)
{
    /* @hack spelling: ppp_modem_port read through port, where
     * ppp_modem_port[0] would do */
    MDM *port = ppp_modem_port;
    int count;
    memset(bytes, 0, size);
    count = mdmRead(*port, bytes, size);
#ifdef NONMATCHING
    /* this side fails a count mdmRead gives below 0 or past size */
    if (count < 0 || count > size) {
        *received = 0;
        return -1;
    }
#endif
    *received = count;
    return 0;
}
int PP_ComWrite(Uint16 size, char *bytes)
{
    /* @hack spelling: ppp_modem_port read through port, where
     * ppp_modem_port[0] would do */
    MDM *port = ppp_modem_port;
    int sent;
    while (size) {
        if (!mdmTxFreeSzie(*port)) {
#ifdef NONMATCHING
            /* this side repairs the bug tagged below: a full transmit ring
             * fails the send with -1 */
            return -1;
#else
            /* @bug logic: a full transmit ring leaves the rest unsent, and
             * the result is still 0 (0x0c9e11b0) */
            break;
#endif
        }
        sent = mdmWrite(*port, bytes, size);
#ifdef NONMATCHING
        /* this side fails a count mdmWrite gives of 0 or less, or past size */
        if (sent <= 0 || sent > size) {
            return -1;
        }
#endif
        size -= sent;
        bytes += sent;
    }
    return 0;
}
void PP_Com_reflush(struct ppp_connection *config)
{
    (void)config;
}
int PP_ComReadedLen(Uint16 *length)
{
    /* @hack spelling: ppp_modem_port read through port, where
     * ppp_modem_port[0] would do */
    MDM *port = ppp_modem_port;
    short saved_mask;
    if (read_paused == 1) {
        *length = 0;
    } else {
        saved_mask = PP_OS_Disable();
        *length = mdmRxAvailSize(*port);
        PP_OS_Restore(saved_mask);
    }
    return 0;
}
int PP_ComWritableLen(Uint16 *length)
{
    /* @hack spelling: ppp_modem_port read through port, where
     * ppp_modem_port[0] would do */
    MDM *port = ppp_modem_port;
    short saved_mask = PP_OS_Disable();
    *length = mdmTxFreeSzie(*port);
    PP_OS_Restore(saved_mask);
    return 0;
}
int PP_Com_ioctl(Uint16 command)
{
    /* @hack spelling: ppp_modem_port read through port, where
     * ppp_modem_port[0] would do */
    MDM *port = ppp_modem_port;
    MDS_DCB state;
    int result;
    switch (command) {
    case PPP_SERIAL_HANG_UP:
        mdmGetCommState2(*port, &state);
        state.Dtr = MDD_MCR_DTR_OFF;
        mdmSetCommState2(*port, state);
        mdmGetCommState2(*port, &state);
        state.Rts = MDD_MCR_RTS_OFF;
        mdmSetCommState2(*port, state);
        result = 1;
        mdmClose(*port);
        break;
    case PPP_SERIAL_CONNECT:
        mdmGetCommState2(*port, &state);
        state.Dtr = MDD_MCR_DTR_ON;
        state.Rts = MDD_MCR_RTS_ON;
        mdmSetCommState2(*port, state);
        result = 1;
        break;
    default:
        result = 0;
        break;
    }
    return result;
}
Uint16 PP_ComLineSence(void)
{
    /* @hack spelling: ppp_modem_port read through port, where
     * ppp_modem_port[0] would do */
    MDM *port = ppp_modem_port;
    short saved_mask;
    MDS_DCB state;
    Uint16 carrier;
    saved_mask = PP_OS_Disable();
    mdmGetCommState2(*port, &state);
    if (state.Dcd == 0) {
        carrier = 0;
    } else {
        carrier = 1;
    }
    PP_OS_Restore(saved_mask);
    return carrier;
}
int PP_ComInitialize(void)
{
    return 0;
}
void PP_ComTerminate(void)
{
    modem_driver_opened = 0;
    mdmFinish();
}
