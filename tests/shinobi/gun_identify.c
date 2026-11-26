/* The peripheral server with a light gun that has no controller function on
 * port A. The build fills each local that nothing sets with a pattern, so a
 * support read unset isn't 0. */
#include <assert.h>

#include "../../sdk/shinobi/pdmain.c"

static KDS_DEVICE devices[KDD_DEV_MAX];
static KDS_DEVICEINFO infos[KDD_DEV_MAX];

const KDS_DEVICE *kdGetDevice(Sint32 device)
{
    return &devices[device];
}

const KDS_DEVICEINFO *kdGetDeviceInfo(Sint32 device)
{
    return &infos[device];
}

/* the server asks no device without KDD_FUNC_CONTROLLER for its controller
 * word */
Sint32 kdDevGetControllerInfo(KDS_CONTROLLERINFO *sinfo,
                              const KDS_DEVICEINFO *info)
{
    (void)sinfo;
    (void)info;
    assert(!"called");
    return -1;
}

static Sint32 mode;

Sint32 kdGetDriverMode(Sint32 type)
{
    assert(type == KDD_MODE_DEVENDIAN);
    return mode;
}

/* run the server with Kaede's driver mode at driver_mode. the gun's port
 * takes support and id 0, and the gun's name */
static void serve(Sint32 driver_mode)
{
    mode = driver_mode;
    pdgPeripheral[PDD_PORT_A0].support = 1;
    pdgPeripheral[PDD_PORT_A0].id = 1;
    pdExecPeripheralServerUser();
    assert(pdgPeripheral[PDD_PORT_A0].support == 0);
    assert(pdgPeripheral[PDD_PORT_A0].id == 0);
    assert(pdgPeripheral[PDD_PORT_A0].name == infos[PDD_PORT_A0].product_name);
}

int main(void)
{
    Sint32 i;

    for (i = 0; i < KDD_DEV_MAX; i++) {
        devices[i].response = KDD_RES_NORESPONSE;
    }
    infos[PDD_PORT_A0].function = KDD_FUNC_ARGUN;
    strcpy(infos[PDD_PORT_A0].product_name, "Gun");
    serve(0);
    serve(PDD_MODE_TYPE_A);
    return 0;
}
