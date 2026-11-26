/* the console settings, which syBtCfgSet copies between a flash memory
 * packet and kbCfgGetId's record */
#include <sg_fm.h>
#include <sg_xpt.h>
#include <shinobi.h>

#define CFG_BANK 2
#define CFG_PACKET 5
/* where the settings lie in kbCfgGetId's record, and their size */
#define SETTINGS_OFFSET 16
#define SETTINGS_BYTES 8
/* permit's SYD_BT_CFGWRT_ bits run from AT's, bit 0, to TM's, bit 4. the
 * settings the bits name run down from the last byte. AT's is byte 7, SM's
 * 6, LG's 5 and GM's 4. TM's, the time, is bytes 0 to 3 */
#define PERMIT_BITS 5
#define TM_BIT 4

/* the member exports this name, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
char *syBtCfggBuild = "\nsyBtCfg Ver 1.03 Build:Apr 26 1999 14:36:52\n";
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 syBtCfgSet(Sint32 permit, void *work)
{
    char packet[FLASH_DATA_BYTES];
    char *settings;
    Sint32 result;
    Sint32 status;

    result = fmMngInit(CFG_BANK, work);
    if (result == FLASH_BAD_HEADER) {
        /* @hack spelling: the format's result in a local of its own, where
         * result would do */
        Sint32 error = fmMngFormat(FLASH_TYPE_LOG);
        if (error != FMD_OK) {
            return SYD_BT_CFG_ERR;
        }
    } else if (result != FMD_OK) {
        return SYD_BT_CFG_ERR;
    }
    result = fmMngRead(CFG_PACKET, packet);
    if (result != FMD_OK) {
        return SYD_BT_CFG_ERR;
    }
    while ((status = fmMngExecServer()) == FLASH_STATE_PENDING) {
    }
    if (status != FMD_STAT_COMPLETE) {
        return SYD_BT_CFG_ERR;
    }

    /* @hack redundant: a goto to the next statement, ahead of the record's
     * address */
    goto find_settings;
find_settings:
    settings = (char *)kbCfgGetId() + SETTINGS_OFFSET;
    /* @hack redundant: a goto to the next statement, ahead of the copy */
    goto copy;
copy:
    if (permit == SYD_BT_CFGINI) {
        char *from = packet;
        Sint32 count = SETTINGS_BYTES;
        /* @hack spelling: the copy counted down by a while, a step to each
         * statement, where a for loop of *settings++ = *from++ would do */
        while (count != 0) {
            --count;
            *settings = *from;
            ++from;
            ++settings;
        }
    } else {
        char *to = packet + (SETTINGS_BYTES - 1);
        Sint32 bit;
        settings += SETTINGS_BYTES - 1;
        for (bit = 0; bit < PERMIT_BITS; bit++) {
            if (permit & 1) {
                *to = *settings;
                if (bit == TM_BIT) {
                    *--to = *--settings;
                    *--to = *--settings;
                    *--to = *--settings;
                }
            }
            /* @hack spelling: permit = permit >> 1, where >>= would do */
            permit = permit >> 1;
            /* after TM's four bytes, the last step takes the pointer to one
             * byte before packet. C leaves such a pointer undefined, so the
             * step subtracts from the address as an integer */
            to = (char *)((Uint32)to - 1);
            --settings;
        }
        /* @hack redundant: a goto to the next statement, ahead of the write */
        goto write;
    write:
        result = fmMngWrite(CFG_PACKET, packet);
        if (result != FMD_OK) {
            return SYD_BT_CFG_ERR;
        }
        while ((status = fmMngExecServer()) == FLASH_STATE_PENDING) {
        }
        if (status != FMD_STAT_COMPLETE) {
            return SYD_BT_CFG_ERR;
        }
    }
    return SYD_BT_CFG_OK;
}
