/* the peripheral server and the light gun calls. the server updates each
 * port's PDS_PERIPHERAL from what Kaede holds for its device */
#include <kaede.h>
#include <maple_driver.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

/* a controller's condition starts CONDITION_AT bytes into its response, after
 * the header and the function word. it starts with the buttons' low and high
 * bytes. a byte for each of r to y2 follows, in PDS_PERIPHERAL's order */
#define CONDITION_AT 8
/* the high half of on, the buttons a condition doesn't hold, all up */
#define HIGH_BUTTONS_UP 0xffff0000
/* an axis is its byte less AXIS_CENTER, -128 to 127 */
#define AXIS_CENTER 128
/* PDD_MODE_TYPE_A moves support's bits 18 to 31 down TYPE_A_SHIFT, into
 * TYPE_A_MOVED */
#define TYPE_A_SHIFT 10
#define TYPE_A_MOVED 0x3fff00

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the peripheral library's version and build time */
const char pdgBuild[] = "\npd Ver 1.51 Build:Feb 14 2000 14:28:38\n";
/* the Kaede handler slot that pdGunLeave empties */
const Uint32 pdgGunHandlerNo = 10;
/* pdExecPeripheralServer updates the ports only while this is set */
const Bool pdgAutoServer = TRUE;

/* TRUE once an init call has run */
Bool pdgEnableSupport;
/* TRUE when the last init call started Kaede. pdExitPeripheral stops Kaede
 * while it is set, and never clears it */
Bool pdgKaedeInitFlag;
/* the gun mode pdGunEnter last set, or 0 after an init call or pdGunLeave */
Uint32 pdgGunMode;
PDS_PERIPHERAL pdgPeripheral[KDD_DEV_MAX];
/* each port's PDD_ERR_ code */
Sint32 pdgPerError[KDD_DEV_MAX];
/* the logic pdInitPeripheral or pdInitPeripheralEx took: PDD_PLOGIC_ACTIVE,
 * or any other for negative logic */
Sint32 pdgLogic;
Uint8 pdgThOn;
Uint8 pdgThOff;
/* NOLINTEND(misc-use-internal-linkage) */

static const char pad_no_device[] = "(no device)";
/* each connector's controller port, by the connector kdGetGunLatchedPort
 * returns */
static const Sint32 pad_gun_ports[MPDRV_PORTS] = {PDD_PORT_A0, PDD_PORT_B0,
                                                  PDD_PORT_C0, PDD_PORT_D0};
/* whether each port's l trigger holds PDD_DGT_TL down, and its r trigger
 * PDD_DGT_TR. each changes when its trigger reaches a threshold, and an init
 * call clears it */
static Uint8 pad_l_pressed[KDD_DEV_MAX];
static Uint8 pad_r_pressed[KDD_DEV_MAX];

static void pad_clear(PDS_PERIPHERAL *per);

void pdInitPeripheral(Sint32 logic, void *recvbuf, void *sendbuf)
{
    Sint32 i;

    pdgLogic = logic;
    pdgKaedeInitFlag = FALSE;
    pdgEnableSupport = TRUE;
    pdgGunMode = 0;
    memset(pdgPeripheral, 0, sizeof(pdgPeripheral));
    if (sendbuf != NULL && recvbuf != NULL) {
        pdgKaedeInitFlag = TRUE;
        kdInitSystem(recvbuf, sendbuf);
    }
    for (i = 0; i < KDD_DEV_MAX; i++) {
        pdgPeripheral[i].info = (PDS_PERIPHERALINFO *)kdGetDeviceInfo(i);
        pad_clear(&pdgPeripheral[i]);
        pdgPerError[i] = PDD_ERR_OK;
        pad_l_pressed[i] = FALSE;
        pad_r_pressed[i] = FALSE;
    }
    pdgThOn = DEFAULT_THRESHOLD_ON;
    pdgThOff = DEFAULT_THRESHOLD_OFF;
}

void pdInitPeripheralEx(Sint32 logic, Sint32 *permap, void **recvbuf,
                        void *sendbuf, Sint32 num)
{
    Sint32 i;

    pdgLogic = logic;
    pdgKaedeInitFlag = FALSE;
    pdgEnableSupport = TRUE;
    pdgGunMode = 0;
    memset(pdgPeripheral, 0, sizeof(pdgPeripheral));
    if (sendbuf != NULL && recvbuf != NULL) {
        pdgKaedeInitFlag = TRUE;
        kdInitSystemEx(permap, recvbuf, sendbuf, num);
    }
    for (i = 0; i < KDD_DEV_MAX; i++) {
        pdgPeripheral[i].info = (PDS_PERIPHERALINFO *)kdGetDeviceInfo(i);
        pad_clear(&pdgPeripheral[i]);
        pdgPerError[i] = PDD_ERR_OK;
        pad_l_pressed[i] = FALSE;
        pad_r_pressed[i] = FALSE;
    }
    pdgThOn = DEFAULT_THRESHOLD_ON;
    pdgThOff = DEFAULT_THRESHOLD_OFF;
}

/* set per to a port with no device, every button up in pdgLogic's sense.
 * info and old keep their values */
static void pad_clear(PDS_PERIPHERAL *per)
{
    per->id = 0;
    per->support = 0;
    if (pdgLogic == PDD_PLOGIC_ACTIVE) {
        per->on = 0;
        per->off = 0xffffffff;
        per->press = 0;
        per->release = 0;
    } else {
        per->on = 0xffffffff;
        per->off = 0;
        per->press = 0xffffffff;
        per->release = 0xffffffff;
    }
    /* @hack spelling: r to y2 cleared in one chained assignment, where a
     * statement for each, in field order, would do */
    per->x1 = per->y1 = per->x2 = per->y2 = per->l = per->r = 0;
    per->extend = NULL;
    per->name = (char *)pad_no_device;
}

/* read device's condition into per's on and r to y2. port indexes
 * pad_l_pressed and pad_r_pressed */
static void pad_read_controller(PDS_PERIPHERAL *per, const KDS_DEVICE *device,
                                Sint32 port)
{
    Uint32 on;
    Uint8 *data;

    /* @hack spelling: data set, then stepped to the condition by a second
     * assignment, where one assignment of (Uint8 *)device->recvdata +
     * CONDITION_AT would do */
    data = device->recvdata;
    data = data + CONDITION_AT;
    /* the button bytes fill on's low half, a bit clear for each button
     * pressed */
    on = data[0] | data[1] << 8 | HIGH_BUTTONS_UP;
    if (data[3] >= pdgThOn) {
        on &= ~PDD_DGT_TL;
        pad_l_pressed[port] = TRUE;
    } else if (data[3] <= pdgThOff) {
        /* @hack redundant: TL set again, though on starts with it set */
        on |= PDD_DGT_TL;
        pad_l_pressed[port] = FALSE;
    } else if (pad_l_pressed[port]) {
        on &= ~PDD_DGT_TL;
    }
    if (data[2] >= pdgThOn) {
        on &= ~PDD_DGT_TR;
        pad_r_pressed[port] = TRUE;
    } else if (data[2] <= pdgThOff) {
        /* @hack redundant: TR set again, though on starts with it set */
        on |= PDD_DGT_TR;
        pad_r_pressed[port] = FALSE;
    } else if (pad_r_pressed[port]) {
        on &= ~PDD_DGT_TR;
    }
    if (pdgLogic == PDD_PLOGIC_ACTIVE) {
        /* @hack spelling: on inverted by an exclusive or with H'FFFFFFFF,
         * where on = ~on would do */
        on ^= 0xffffffff;
    }
    per->on = on;
    per->r = (per->support & PDD_DEV_SUPPORT_AR) ? data[2] : 0;
    per->l = (per->support & PDD_DEV_SUPPORT_AL) ? data[3] : 0;
    per->x1 = (per->support & PDD_DEV_SUPPORT_AX1) ? data[4] - AXIS_CENTER : 0;
    per->y1 = (per->support & PDD_DEV_SUPPORT_AY1) ? data[5] - AXIS_CENTER : 0;
    per->x2 = (per->support & PDD_DEV_SUPPORT_AX2) ? data[6] - AXIS_CENTER : 0;
    per->y2 = (per->support & PDD_DEV_SUPPORT_AY2) ? data[7] - AXIS_CENTER : 0;
}

/* return support rearranged as PDD_MODE_TYPE_A lays it out */
static Uint32 pad_type_a_support(Uint32 support)
{
    Uint32 type_a = (support >> TYPE_A_SHIFT) & TYPE_A_MOVED;

    if (support & PDD_DEV_SUPPORT_AL) {
        type_a |= TYPE_A_L;
    }
    if (support & PDD_DEV_SUPPORT_AR) {
        type_a |= TYPE_A_R;
    }
    return type_a;
}

/* set per's support and id from info's controller fd word, and per's name to
 * info's product_name. under PDD_MODE_TYPE_A, support is rearranged first.
 * where kdDevGetControllerInfo fails, support and id become 0, and name is
 * left as it was */
static void pad_identify(PDS_PERIPHERAL *per, const KDS_DEVICEINFO *info)
{
    KDS_CONTROLLERINFO controller;

#ifdef NONMATCHING
    /* this side repairs the tag below: a device without KDD_FUNC_CONTROLLER
     * takes support and id 0, as where kdDevGetControllerInfo fails */
    controller.button = 0;
#endif
    /* @bug uninitialized: sets support and id from controller, which nothing
     * set, for a device with KDD_FUNC_ARGUN and not KDD_FUNC_CONTROLLER.
     * pdExecPeripheralServerUser passes such a device here. the test at
     * 0x8c10ef82 skips kdDevGetControllerInfo. the reads at 0x8c10efa8 and
     * 0x8c10efae then take controller.button from whatever the stack held */
    if (info->function & KDD_FUNC_CONTROLLER) {
        if (kdDevGetControllerInfo(&controller, info) < 0) {
            per->support = 0;
            per->id = 0;
            return;
        }
    }
    if (kdGetDriverMode(KDD_MODE_DEVENDIAN) == PDD_MODE_TYPE_A) {
        per->support = pad_type_a_support(controller.button);
    } else {
        per->support = controller.button;
    }
    per->id = per->support;
    per->name = (char *)info->product_name;
}

/* @hack redundant: per and info, parameters it never reads */
static Sint32 pad_response_error(PDS_PERIPHERAL *per,
                                 const KDS_DEVICEINFO *info,
                                 const KDS_DEVICE *device)
{
    switch (device->response) {
    case KDD_RES_TRANSMITAGAIN:
        return (Sint32)PDD_ERR_RETRY;
    case KDD_RES_NORESPONSE:
    case KDD_RES_DATATRANSFER:
    case KDD_RES_DEVICESTATUS:
    case KDD_RES_DEVICEALLSTATUS:
    case 0:
        return PDD_ERR_OK;
    default:
        return (Sint32)PDD_ERR_GENERIC;
    }
}

void pdExecPeripheralServer(void)
{
    if (pdgAutoServer) {
        pdExecPeripheralServerUser();
    }
}

void pdExecPeripheralServerUser(void)
{
    Sint32 i;
    Sint32 count;
    const KDS_DEVICE *device;
    const KDS_DEVICEINFO *info;
    PDS_PERIPHERAL *per;
    Uint32 changed;

    /* @hack redundant: count, stepped with i and never read */
    for (i = 0, count = 0; i < KDD_DEV_MAX; i++, count++) {
        device = kdGetDevice(i);
        info = kdGetDeviceInfo(i);
        per = &pdgPeripheral[i];
        per->info = (PDS_PERIPHERALINFO *)info;
        per->old = per->on;
        pdgPerError[i] = pad_response_error(per, info, device);
        if (i % MPDRV_PORT_DEVICES == 0) {
            if ((info->function & (KDD_FUNC_CONTROLLER | KDD_FUNC_ARGUN)) &&
                device->connect == 0) {
                pad_identify(per, info);
                /* @hack redundant: an empty else */
                if (device->response == KDD_RES_DATATRANSFER) {
                    pad_read_controller(per, device, i);
                } else {
                }
            } else {
                pad_clear(per);
            }
            per->off = ~per->on;
            if (pdgLogic == PDD_PLOGIC_ACTIVE) {
                changed = per->old ^ per->on;
                per->press = per->on & changed;
                per->release = changed & per->off;
            } else {
                /* @hack spelling: changed inverted by an exclusive or with
                 * H'FFFFFFFF, where changed = ~(per->old ^ per->on) would
                 * do */
                changed = per->old ^ per->on;
                changed ^= 0xffffffff;
                per->press = per->on | changed;
                per->release = changed | per->off;
            }
        } else {
            per->name = (char *)info->product_name;
        }
    }
}

const PDS_PERIPHERAL *pdGetPeripheral(Uint32 port)
{
    return &pdgPeripheral[port];
}

void pdExitPeripheral(void)
{
    Sint32 i;

    if (pdgKaedeInitFlag) {
        kdExitSystem();
    }
    memset(pdgPeripheral, 0, sizeof(pdgPeripheral));
    for (i = 0; i < KDD_DEV_MAX; i++) {
        pad_clear(&pdgPeripheral[i]);
        pdgPerError[i] = PDD_ERR_OK;
    }
}

Sint32 pdGetPeripheralError(Uint32 port)
{
    return pdgPerError[port];
}

const PDS_PERIPHERALINFO *pdGetPeripheralInfo(Uint32 port)
{
    return (const PDS_PERIPHERALINFO *)kdGetDeviceInfo(port);
}

void pdSetMode(Sint32 mode)
{
    kdSetDriverMode(KDD_MODE_DEVENDIAN, mode);
}

Sint32 pdGetMode(void)
{
    return kdGetDriverMode(KDD_MODE_DEVENDIAN);
}

void pdSetOptimize(Uint32 count)
{
    kdSetDriverMode(KDD_MODE_OPTIMIZE, count);
}

Uint32 pdGetOptimize(void)
{
    return kdGetDriverMode(KDD_MODE_OPTIMIZE);
}

void pdSetThreshold(Uint8 on, Uint8 off)
{
    pdgThOn = on;
    pdgThOff = off;
}

void pdGunEnter(Uint32 portbit)
{
    if (portbit) {
        kdSetDriverMode(KDD_MODE_GUNMODE, portbit);
        kdSetDriverMode(KDD_MODE_AUTOFLASH, -1);
        pdgGunMode = portbit;
    }
}

void pdGunLeave(void)
{
    kdSetDriverMode(KDD_MODE_GUNMODE, 0);
    kdSetDriverMode(KDD_MODE_AUTOFLASH, 0);
    kdSetHandler(pdgGunHandlerNo, NULL);
    pdgGunMode = 0;
}

Sint32 pdGunCheck(void)
{
    return kdGetDriverMode(KDD_MODE_GUNMODE);
}

void pdGunGetPosition(Sint32 *x, Sint32 *y)
{
    kdGetGunPosition(x, y);
}

void pdGunSetFlashColor(Uint32 color)
{
    kdSetDriverMode(KDD_MODE_AUTOFLASH, color);
}

Uint32 pdGunGetFlashColor(void)
{
    return kdGetDriverMode(KDD_MODE_AUTOFLASH);
}

Sint32 pdGunGetLatchedPort(void)
{
    Sint32 port = kdGetGunLatchedPort();

    if (port > 0) {
        port = pad_gun_ports[port];
    }
    return port;
}

void pdGunSetTrigger(Uint32 port)
{
    switch (port) {
    case PDD_PORT_A0:
        kdSetGunTrigger(0);
        break;
    case PDD_PORT_B0:
        kdSetGunTrigger(1);
        break;
    case PDD_PORT_C0:
        kdSetGunTrigger(2);
        break;
    case PDD_PORT_D0:
        kdSetGunTrigger(3);
        break;
    }
}

void pdGunSetCallback(Sint32 (*func)(void))
{
    kdSetGunCallback(func);
}
