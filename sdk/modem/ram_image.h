/* the data pump's RAM image, version 4729 of the Rockwell L8570A's: a
 * directory of modules, then their bytes. the first CONTROL_MODULES load into
 * the controller's RAM, and the last SIGNAL_MODULES into the signal
 * processor's. a module's size, and its offset in payload, count bytes */
#ifndef MODEM_RAM_IMAGE_H
#define MODEM_RAM_IMAGE_H

#include <sg_xpt.h>

#define CONTROL_MODULES 7
#define SIGNAL_MODULES 6

struct ram_module {
    Uint16 id;
    /* an overlay's id again, or 0xffff for any other module. no code reads
     * it */
    Uint16 tag;
    /* the checksum the pump returns once the module has loaded, which the
     * download compares */
    Uint8 checksum;
    Uint8 reserved[3];
    Uint32 address;
    Uint32 end_address;
    Uint32 size;
    Uint32 offset;
};

struct ram_database {
    struct ram_module modules[CONTROL_MODULES + SIGNAL_MODULES];
    Uint8 payload[28276];
};

#endif
