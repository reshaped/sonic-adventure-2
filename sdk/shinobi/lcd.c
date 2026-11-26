/* each port's LCD number, and which way up an LCD in a controller's socket
 * faces */
#include <kaede.h>
#include <sg_lcd.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the member exports this name, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the library's version and build time */
const char pdgLcdBuild[] = "\npdLcd Ver 1.04 Build:Apr 26 1999 16:55:25\n";
/* NOLINTEND(misc-use-internal-linkage) */

/* pdLcdGetDirection's results, by the controller's connector_dir for the
 * socket, then by the LCD device's own connector_dir[0] */
/* @hack spelling: the table flat, indexed by KD_CONNECTOR_DIRS times the
 * socket's direction plus the device's, where a table of KD_CONNECTOR_DIRS rows
 * would do */
static const Uint8 lcd_directions[KD_CONNECTOR_DIRS * KD_CONNECTOR_DIRS] = {
    PDD_LCD_DIRECTION_FLIP,  PDD_LCD_DIRECTION_NORMAL, PDD_LCD_DIRECTION_LEFT,
    PDD_LCD_DIRECTION_RIGHT, PDD_LCD_DIRECTION_NORMAL, PDD_LCD_DIRECTION_FLIP,
    PDD_LCD_DIRECTION_RIGHT, PDD_LCD_DIRECTION_LEFT,   PDD_LCD_DIRECTION_RIGHT,
    PDD_LCD_DIRECTION_LEFT,  PDD_LCD_DIRECTION_FLIP,   PDD_LCD_DIRECTION_NORMAL,
    PDD_LCD_DIRECTION_LEFT,  PDD_LCD_DIRECTION_RIGHT,  PDD_LCD_DIRECTION_NORMAL,
    PDD_LCD_DIRECTION_FLIP,
};

const Uint8 pdgLcdNo[KDD_DEV_MAX] = {
    NO_LCD, 0, 1, NO_LCD, NO_LCD, NO_LCD, NO_LCD, 2, 3, NO_LCD, NO_LCD, NO_LCD,
    NO_LCD, 4, 5, NO_LCD, NO_LCD, NO_LCD, NO_LCD, 6, 7, NO_LCD, NO_LCD, NO_LCD,
};

/* each port's controller port: the first of its connector's six ports */
static const Uint8 lcd_root_ports[KDD_DEV_MAX] = {
    PDD_PORT_A0, PDD_PORT_A0, PDD_PORT_A0, PDD_PORT_A0, PDD_PORT_A0,
    PDD_PORT_A0, PDD_PORT_B0, PDD_PORT_B0, PDD_PORT_B0, PDD_PORT_B0,
    PDD_PORT_B0, PDD_PORT_B0, PDD_PORT_C0, PDD_PORT_C0, PDD_PORT_C0,
    PDD_PORT_C0, PDD_PORT_C0, PDD_PORT_C0, PDD_PORT_D0, PDD_PORT_D0,
    PDD_PORT_D0, PDD_PORT_D0, PDD_PORT_D0, PDD_PORT_D0,
};

void pdLcdInit(void)
{
}

Sint32 pdLcdGetDirection(Uint32 port)
{
    const KDS_DEVICEINFO *root;
    const KDS_DEVICEINFO *lcd;
    Sint32 socket;

    root = kdGetDeviceInfo(lcd_root_ports[port]);
    lcd = kdGetDeviceInfo(port);
    switch (port) {
    case PDD_PORT_A2:
    case PDD_PORT_B2:
    case PDD_PORT_C2:
    case PDD_PORT_D2:
        socket = 1;
        break;
    default:
        socket = 0;
        break;
    }
    return lcd_directions[(KD_CONNECTOR_DIRS * root->connector_dir[socket]) +
                          lcd->connector_dir[0]];
}

void pdLcdExit(void)
{
}
