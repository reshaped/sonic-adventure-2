#include <mdmAPI.h>
#include <sg_xpt.h>
#include "modem_backends.h"

void ModemMainCall(void *argument);

#define MODEM_PORTS 2
/* the port the G2 modem's and the UART's opens return, and the one
 * mdmSetCommState and mdmGetCommState act on. the SCIF's open returns 0 */
#define DEFAULT_PORT 1
#define INVALID_PORT ((MDM) - 1)
/* the API's version, then the library's version and build time */
const Uint16 mdgMdmAPIVer[2] = {5, 9};
const char *mdgMdmAPIBuild = "\nmdmAPI Ver. 5.10 Build:Jan 09 2001 15:05:20\n";
static MD_DEVICE devices[MODEM_PORTS];
#ifdef NONMATCHING
/* this side's PORT_DEVICE gives NO_DEVICE for a handle past the ports */
#define NO_DEVICE ((MD_DEVICE) - 1)
#define PORT_DEVICE(handle)                                                    \
    ((handle) < MODEM_PORTS ? devices[handle] : NO_DEVICE)
/* this side repairs the bug tagged below: result starts at -1, so a device no
 * driver takes returns -1 */
#define DECLARE_RESULT int result = -1
#else
#define PORT_DEVICE(handle) devices[handle]
/* @bug uninitialized: result is unset where the port's device is no
 * MDD_DEVICE_G2_MODEM, MDD_DEVICE_SCIF or MDD_DEVICE_UART, and mdmRxAvailSize
 * returns it (0x0c9e2dbe). the three other size calls, mdmRead and mdmWrite
 * return it alike */
#define DECLARE_RESULT int result
#endif

/* call operation of device's driver with arguments. CALL_DRIVER_INTO stores
 * what it returns in result. a device other than MDD_DEVICE_G2_MODEM,
 * MDD_DEVICE_SCIF and MDD_DEVICE_UART calls nothing */
#define CALL_DRIVER(device, operation, arguments)                              \
    switch (device) {                                                          \
    case MDD_DEVICE_G2_MODEM:                                                  \
        g2_modem_##operation arguments;                                        \
        break;                                                                 \
    case MDD_DEVICE_SCIF:                                                      \
        scif_modem_##operation arguments;                                      \
        break;                                                                 \
    case MDD_DEVICE_UART:                                                      \
        uart_modem_##operation arguments;                                      \
        break;                                                                 \
    }
#define CALL_DRIVER_INTO(result, device, operation, arguments)                 \
    switch (device) {                                                          \
    case MDD_DEVICE_G2_MODEM:                                                  \
        (result) = g2_modem_##operation arguments;                             \
        break;                                                                 \
    case MDD_DEVICE_SCIF:                                                      \
        (result) = scif_modem_##operation arguments;                           \
        break;                                                                 \
    case MDD_DEVICE_UART:                                                      \
        (result) = uart_modem_##operation arguments;                           \
        break;                                                                 \
    }

void mdmDropDtr(MDM handle)
{
    CALL_DRIVER(PORT_DEVICE(handle), drop_dtr, (handle))
}
void mdmEnableDtr(MDM handle)
{
    CALL_DRIVER(PORT_DEVICE(handle), enable_dtr, (handle))
}
int mdmRxAvailSize(MDM handle)
{
    DECLARE_RESULT;
    CALL_DRIVER_INTO(result, PORT_DEVICE(handle), rx_available, (handle))
    return result;
}
int mdmRxFreeSize(MDM handle)
{
    DECLARE_RESULT;
    CALL_DRIVER_INTO(result, PORT_DEVICE(handle), rx_free, (handle))
    return result;
}
int mdmTxAvailSize(MDM handle)
{
    DECLARE_RESULT;
    CALL_DRIVER_INTO(result, PORT_DEVICE(handle), tx_available, (handle))
    return result;
}
int mdmTxFreeSzie(MDM handle)
{
    DECLARE_RESULT;
    CALL_DRIVER_INTO(result, PORT_DEVICE(handle), tx_free, (handle))
    return result;
}
void mdmInit(char *tx, char *rx, int tx_size, int rx_size)
{
    g2_modem_init(tx, rx, tx_size, rx_size);
    scif_modem_init(tx, rx, tx_size, rx_size);
    uart_modem_init(tx, rx, tx_size, rx_size);
}
MDM mdmOpen(MD_DEVICE device)
{
#ifdef NONMATCHING
    /* this side starts handle at INVALID_PORT, which stays for a device no
     * driver takes */
    MDM handle = INVALID_PORT;
#else
    MDM handle;
#endif
    CALL_DRIVER_INTO(handle, device, open, (device))
#ifdef NONMATCHING
    /* this side fails a handle past the ports, which indexes devices below */
    if (handle >= MODEM_PORTS) {
        return INVALID_PORT;
    }
#endif
    devices[handle] = device;
    return handle;
}
int mdmRead(MDM handle, char *buffer, int size)
{
    DECLARE_RESULT;
    CALL_DRIVER_INTO(result, PORT_DEVICE(handle), read, (handle, buffer, size))
    return result;
}
int mdmWrite(MDM handle, char *buffer, int size)
{
    DECLARE_RESULT;
    CALL_DRIVER_INTO(result, PORT_DEVICE(handle), write, (handle, buffer, size))
    return result;
}
void mdmSetCommState(MDS_DCB state)
{
    CALL_DRIVER(devices[DEFAULT_PORT], set_default, (DEFAULT_PORT, state))
}
void mdmSetCommState2(MDM handle, MDS_DCB state)
{
    switch (PORT_DEVICE(handle)) {
    case MDD_DEVICE_G2_MODEM:
        g2_modem_set_default(handle, state);
        break;
    case MDD_DEVICE_SCIF:
        scif_modem_set_state(handle, state);
        break;
    case MDD_DEVICE_UART:
        uart_modem_set_state(handle, state);
        break;
    }
}
void mdmGetCommState2(MDM handle, MDS_DCB *state)
{
    CALL_DRIVER(PORT_DEVICE(handle), get_state, (handle, state))
}
void mdmGetCommState(MDS_DCB *state)
{
    switch (devices[DEFAULT_PORT]) {
    case MDD_DEVICE_G2_MODEM:
        g2_modem_get_state(DEFAULT_PORT, state);
        break;
    case MDD_DEVICE_SCIF:
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: it passes the state's address
         * alone */
        scif_modem_get_default(state);
#else
        /* @bug mismatched-call: scif_modem_get_default writes the state at
         * address 1. it takes the state's address alone, and gets DEFAULT_PORT
         * for it (0x0c9e30ea) */
        /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
        ((void (*)(MDM, MDS_DCB *))scif_modem_get_default)(DEFAULT_PORT, state);
#endif
        break;
    case MDD_DEVICE_UART:
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: it passes the state's address
         * alone */
        uart_modem_get_default(state);
#else
        /* @bug mismatched-call: uart_modem_get_default writes the state at
         * address 1. it takes the state's address alone, and gets
         * DEFAULT_PORT for it (0x0c9e30f4) */
        /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
        ((void (*)(MDM, MDS_DCB *))uart_modem_get_default)(DEFAULT_PORT, state);
#endif
        return;
    }
}
void mdmPurgeTxBuffer(MDM handle)
{
    CALL_DRIVER(PORT_DEVICE(handle), purge_tx, (handle))
}
void mdmPurgeRxBuffer(MDM handle)
{
    CALL_DRIVER(PORT_DEVICE(handle), purge_rx, (handle))
}
void mdmClose(MDM handle)
{
    CALL_DRIVER(PORT_DEVICE(handle), close, (handle))
}
void mdmFinish(void)
{
    g2_modem_finish();
    scif_modem_finish();
    uart_modem_finish();
}
void mdmServicePort(MDM handle)
{
    switch (PORT_DEVICE(handle)) {
    case MDD_DEVICE_G2_MODEM:
        ModemMainCall(0);
        break;
    case MDD_DEVICE_SCIF:
    case MDD_DEVICE_UART:
        break;
    }
}
void mdmSelectDevice(MD_DEVICE device)
{
    switch (device) {
    case MDD_DEVICE_G2_MODEM:
        g2_modem_select(device);
        break;
    case MDD_DEVICE_UART:
        uart_modem_select(device);
        break;
    }
}
