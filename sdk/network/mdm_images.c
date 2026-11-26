#include <sg_xpt.h>
#include "../modem/dspdb.h"
#include "../modem/ram_image.h"
#include "country_image.h"
#include "mdm_images.h"

/* NOLINTNEXTLINE(misc-use-internal-linkage) */
struct ram_database DspImage = {
#include "network/mdm_images/DspImage.inc"
};

/* NOLINTNEXTLINE(misc-use-internal-linkage) */
struct modem_country_image CountryImage = {
#include "network/mdm_images/CountryImage.inc"
};

/* NOLINTNEXTLINE(misc-use-internal-linkage) */
struct SegaOptionalInterface mdmgOptInterface;
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
Uint8 mdmgCtryObj[12];

struct SegaOptionalInterface *mdmOptInit(void)
{
    mdmgOptInterface.open_db = mdmDSPCodeOpen;
    mdmgOptInterface.reserved_db = NULL;
    mdmgOptInterface.close_db = mdmDSPCodeClose;
    mdmgOptInterface.open_country_db = mdmCountryCodeOpen;
    mdmgOptInterface.reserved_country = NULL;
    mdmgOptInterface.close_country_db = mdmCountryCodeClose;
    return &mdmgOptInterface;
}

void mdmOptFinish(void)
{
}
void *mdmDSPCodeOpen(void)
{
    return &DspImage;
}
void mdmDSPCodeClose(void *image)
{
}
Uint8 *mdmCountryCodeOpen(void)
{
    return (Uint8 *)&CountryImage;
}
void mdmCountryCodeClose(void *image)
{
}
