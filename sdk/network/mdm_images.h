/* the interface through which the G2 modem opens its images: the data pump's
 * database and the country table. mdmDSPCodeClose and mdmCountryCodeClose do
 * nothing */
#ifndef NETWORK_MDM_IMAGES_H
#define NETWORK_MDM_IMAGES_H
#include <sg_xpt.h>

struct SegaOptionalInterface;

struct SegaOptionalInterface *mdmOptInit(void);
void mdmOptFinish(void);
void *mdmDSPCodeOpen(void);
void mdmDSPCodeClose(void *image);
Uint8 *mdmCountryCodeOpen(void);
void mdmCountryCodeClose(void *image);

#endif
