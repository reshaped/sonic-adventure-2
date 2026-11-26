#include <sd_work.h>
#include <sg_syg2.h>
#include <sg_xpt.h>

static Uint32 fill = SD_SND_MEM_FILL;

int sdSndMemClear(void)
{
    sdArm7ResetKeep();
    syG2Write(&fill, (void *)SD_SND_MEM_BASE, SYE_G2_DATA_WIDTH_32BIT,
              SD_SND_MEM_SIZE / sizeof(fill), SYE_G2_ADR_MODE_FIX,
              SYE_G2_ADR_MODE_ADD);
    return 0;
}
