/* Shinobi's keyboard library. pdKbdExecServer reads the device at the root
 * of each controller port. a port number is a Kaede device number, below
 * KDD_DEV_MAX */
#ifndef SG_PDKBD_H
#define SG_PDKBD_H
#include <sg_xpt.h>

typedef struct {
    Uint8 lang;
    Uint8 type;
    Uint8 led;
    Uint8 led_ctrl;
} PDS_KEYBOARDINFO;

typedef struct {
    Uint8 ctrl;
    Uint8 led;
    Uint8 key[6];
    PDS_KEYBOARDINFO *info;
} PDS_KEYBOARD;

void pdKbdInit(void);
void pdKbdExit(void);
void pdKbdExecServer(void);
/* the keyboard state of port, which the server rewrites each time it runs. it
 * is NULL where the port holds no keyboard. pdKbdGetInfo returns the port's
 * layout record, keyboard or not */
const PDS_KEYBOARD *pdKbdGetData(Uint32 port);
const PDS_KEYBOARDINFO *pdKbdGetInfo(Uint32 port);
#endif
