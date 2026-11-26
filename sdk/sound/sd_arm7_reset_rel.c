#include <machine.h>
#include <sd_work.h>
#include <sg_syg2.h>
#include <sg_xpt.h>

int sdArm7ResetRel(void)
{
    /* @hack redundant: control set to 0 where it is declared, though the
     * read replaces it */
    Uint32 control = 0;
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    syG2Read(SD_ARM_RESET_REG, &control, SYE_G2_DATA_WIDTH_32BIT, 1,
             SYE_G2_ADR_MODE_ADD, SYE_G2_ADR_MODE_ADD);
    control &= ~SD_ARM_RESET;
    syG2Write(&control, SD_ARM_RESET_REG, SYE_G2_DATA_WIDTH_32BIT, 1,
              SYE_G2_ADR_MODE_ADD, SYE_G2_ADR_MODE_ADD);
    set_imask(mask);
    return 0;
}
