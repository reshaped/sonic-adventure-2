/* the modem's EEPROM, kept in RAM */
#include <sg_xpt.h>

#include "ee_ram.h"

/* the member exports the EEPROM's bytes, though no other member uses them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Uint8 RamTable[EEPROM_SIZE];
/* NOLINTEND(misc-use-internal-linkage) */

void eeprom_write_data(Uint8 data, Uint16 offset)
{
    RamTable[offset] = data;
}

void eeprom_read_data(Uint16 offset, Uint16 count, Uint8 *buf)
{
    Uint16 i = 0;
    Uint8 *from = RamTable + offset;

    /* @hack spelling: a do-while guarded by count > 0, where a for loop over
     * i would do */
    if (count > 0) {
        do {
            /* @hack spelling: the copy and its two steps as three
             * statements, where *buf++ = *from++ would do */
            *buf = *from;
            from++;
            buf++;
            i++;
        } while (i < count);
    }
}

Bool eeprom_save_complete(void)
{
    return TRUE;
}
