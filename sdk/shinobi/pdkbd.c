/* the keyboard server. it updates each root port's PDS_KEYBOARD and
 * PDS_KEYBOARDINFO from what Kaede holds for the port */
#include <kaede.h>
#include <sg_pdkbd.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

/* a keyboard's condition starts CONDITION_AT bytes into its response, after
 * the header and the function word */
#define CONDITION_AT 8

/* the member exports these names, the static aside, though no header of the
 * SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const char pdgKbdBuild[] = "\npdKbd Ver 1.51 Build:Feb 14 2000 14:28:41\n";
static const Uint8 root_ports[] = {PDD_PORT_A0, PDD_PORT_B0, PDD_PORT_C0,
                                   PDD_PORT_D0};
/* the Kaede handler slot that pdKbdInit fills and pdKbdExit empties */
Uint32 pdgKbdHandlerNo = 15;
PDS_KEYBOARD pdgKbdData[KDD_DEV_MAX];
PDS_KEYBOARDINFO pdgKbdInfo[KDD_DEV_MAX];
/* NOLINTEND(misc-use-internal-linkage) */

void pdKbdInit(void)
{
    Sint32 i;
    memset(pdgKbdData, 0, sizeof(pdgKbdData));
    memset(pdgKbdInfo, 0, sizeof(pdgKbdInfo));
    for (i = 0; i < KDD_DEV_MAX; i++) {
        pdgKbdData[i].info = &pdgKbdInfo[i];
    }
    kdSetHandler(pdgKbdHandlerNo, pdKbdExecServer);
}

/* @hack redundant: info, a parameter it never reads */
static void pdSetKbdData(Sint32 port, const KDS_DEVICE *device,
                         const KDS_DEVICEINFO *info, const Uint8 *function_data)
{
    PDS_KEYBOARD *report;
    PDS_KEYBOARDINFO *layout;
    const Uint8 *input;
    Uint8 *keys;
    Uint32 i;
    /* @hack spelling: input set, then stepped to the condition by a second
     * assignment, where one assignment of (const Uint8 *)device->recvdata +
     * CONDITION_AT would do */
    input = device->recvdata;
    input += CONDITION_AT;
    report = &pdgKbdData[port];
    layout = report->info;
    report->ctrl = *input++;
    report->led = *input++;
    keys = report->key;
    for (i = 0; i < sizeof(report->key); i++) {
        *keys++ = *input++;
    }
    if (function_data) {
        layout->lang = function_data[0];
        layout->type = function_data[1];
        layout->led = function_data[2];
        layout->led_ctrl = function_data[3];
    }
}

static void pdSetKbdDataNull(Sint32 port)
{
    PDS_KEYBOARD *report;
    PDS_KEYBOARDINFO *layout;
    Uint8 *keys;
    Uint32 i;
    report = &pdgKbdData[port];
    layout = report->info;
    report->ctrl = 0;
    report->led = 0;
    keys = report->key;
    for (i = 0; i < sizeof(report->key); i++) {
        *keys++ = 0;
    }
    layout->lang = 0;
    layout->type = 0;
    layout->led = 0;
    layout->led_ctrl = 0;
}

const PDS_KEYBOARDINFO *pdKbdGetInfo(Uint32 port)
{
    return &pdgKbdInfo[port];
}

const PDS_KEYBOARD *pdKbdGetData(Uint32 port)
{
    const PDS_PERIPHERAL *peripheral = pdGetPeripheral(port);
    if (peripheral->info->type & PDD_DEVTYPE_KEYBOARD) {
        return &pdgKbdData[port];
    }
    return NULL;
}

void pdKbdExecServer(void)
{
    Sint32 port;
    const KDS_DEVICE *device;
    const KDS_DEVICEINFO *info;
    const Uint8 *function_data;
    Uint32 i;

    for (i = 0; i < sizeof(root_ports); i++) {
        port = root_ports[i];
        device = kdGetDevice(port);
        info = kdGetDeviceInfo(port);
        function_data = kdDevGetFD(info, KDD_FUNC_KEYBOARD);
        if ((info->function & KDD_FUNC_KEYBOARD) && !device->connect &&
            device->response == KDD_RES_DATATRANSFER) {
            pdSetKbdData(port, device, info, function_data);
        } else {
            pdSetKbdDataNull(port);
        }
    }
}

void pdKbdExit(void)
{
    Sint32 i;
    kdSetHandler(pdgKbdHandlerNo, NULL);
    memset(pdgKbdData, 0, sizeof(pdgKbdData));
    memset(pdgKbdInfo, 0, sizeof(pdgKbdInfo));
    for (i = 0; i < KDD_DEV_MAX; i++) {
        pdgKbdData[i].info = &pdgKbdInfo[i];
    }
}
