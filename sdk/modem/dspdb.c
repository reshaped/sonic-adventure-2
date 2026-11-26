/* the data pump's database, opened through calls the modem is given */
#include <sg_xpt.h>

#include "dspdb.h"

static char dspdb_ram_header[] = "(c) Rockwell:23-Jul-1998:L8570A:CTL:4729 ";

/* the member exports these names, though no other member uses them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void init_dspdb_cc_f(void);

Uint16 RamFlexPcmVersion = 0x4729;
Uint16 V90CtlRamModuleTableSize = 7;
Uint16 V90SpxRamModuleTableSize = 6;
/* the api DspDbSetApi keeps */
struct SegaOptionalInterface *DspApiPtr;
/* NOLINTEND(misc-use-internal-linkage) */

/* the open database's handle, or null */
static void *dspdb_handle;

void *DspDbInitDataBase(void)
{
    if (dspdb_handle == NULL) {
        dspdb_handle = DspApiPtr->open_db();
        return dspdb_handle;
    }
    return NULL;
}

void DspDbRelease(void)
{
    DspApiPtr->close_db(dspdb_handle);
    dspdb_handle = NULL;
}

Uint16 DspDbGetRamFlexPcmVersion(void)
{
    return RamFlexPcmVersion;
}

char *DspDbGetdpRamHeaderInfo(void)
{
    return dspdb_ram_header;
}

void DspDbSetApi(struct SegaOptionalInterface *api)
{
    DspApiPtr = api;
}

void init_dspdb_cc_f(void)
{
    dspdb_handle = NULL;
}

/* the member's entry in DSG_INIT_, the table of functions the Chao area
 * calls as it starts */
#pragma section _INIT_
/* NOLINTBEGIN(misc-use-internal-linkage) */
void (*init_dspdbcc)(void) = init_dspdb_cc_f;
/* NOLINTEND(misc-use-internal-linkage) */
#pragma section
