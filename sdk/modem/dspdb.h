/* dspdb: the data pump's database, opened through calls the modem is given */
#ifndef MODEM_DSPDB_H
#define MODEM_DSPDB_H
#include <sg_xpt.h>

/* the host's optional calls, which it passes to Open. open_db opens the data
 * pump's database and returns its handle. close_db closes the database whose
 * handle it takes */
struct SegaOptionalInterface {
    void *(*open_db)(void);
    void *reserved_db;
    void (*close_db)(void *db);
    Uint8 *(*open_country_db)(void);
    void *reserved_country;
    void (*close_country_db)(void *db);
};

/* DspDbInitDataBase and DspDbRelease call through the api DspDbSetApi
 * keeps, and need one kept first */

/* open the database, and return its handle. while one is open, it returns
 * null and leaves that one open */
void *DspDbInitDataBase(void);
/* close the database, and mark none open. with none open, it passes
 * close_db null */
void DspDbRelease(void);
/* return the data pump RAM's version, 0x4729 */
Uint16 DspDbGetRamFlexPcmVersion(void);
/* return the data pump RAM's header text */
char *DspDbGetdpRamHeaderInfo(void);
/* keep api, not a copy of it, for DspDbInitDataBase and DspDbRelease */
void DspDbSetApi(struct SegaOptionalInterface *api);

#endif
