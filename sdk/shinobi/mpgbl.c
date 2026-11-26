/* Kaede's Maple driver state, and each peripheral's Maple address */
#include <kaede.h>
#include <maple_driver.h>
#include <sg_xpt.h>

/* a Maple address holds the port in bits 6 and 7, and one bit for the
 * device. bit 5 is the device in the port, and bits 0 to 4 its five
 * expansion sockets */
#define MAPLE_ADDRESS(port, device_bit) (((port) << 6) | (1U << (device_bit)))
#define PORT_ADDRESSES(port)                                                   \
    MAPLE_ADDRESS(port, 5), MAPLE_ADDRESS(port, 0), MAPLE_ADDRESS(port, 1),    \
        MAPLE_ADDRESS(port, 2), MAPLE_ADDRESS(port, 3), MAPLE_ADDRESS(port, 4)

const Uint8 _mp_Per2APTable[KDD_DEV_MAX] = {
    PORT_ADDRESSES(0), PORT_ADDRESSES(1), PORT_ADDRESSES(2), PORT_ADDRESSES(3)};
const Sint32 _mp_RootDeviceNo[MPDRV_PORTS] = {KDD_DEV_A0, KDD_DEV_B0,
                                              KDD_DEV_C0, KDD_DEV_D0};
const Sint32 _mp_MajorVersion = 1;
const Sint32 _mp_MinorVersion = 3;

struct mp_context _mp_Context;
struct mp_context *_mp;
