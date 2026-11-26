#include <sg_xpt.h>
#include "../network/g2_io.h"
#include "../nexgen/rtl8139.h"
#include "../nexgen/rtl8139_eeprom.h"

/* the EEPROM word that holds the MAC address's first two bytes, low byte
 * first. the next two words hold the rest */
#define MAC_EEPROM_WORD 7

Sint32 DLD_SetEtdAddr(struct rtl_address *address)
{
    struct rtl_address readback;
    Bool wide;
    int i;
    int result;
    Uint16 value;
    /* @hack spelling: the call's result held in config, where the test could
     * take the call */
    Uint32 config = ETD_G2Read32(RTL_RCR);
    /* @hack spelling: wide set by an if and an else, where
     * wide = (config & RTL_RCR_EEPROM_WIDE) != 0 would do */
    if (config & RTL_RCR_EEPROM_WIDE) {
        wide = TRUE;
    } else {
        wide = FALSE;
    }
    ETD_G2Write8(RTL_CFG9346, RTL_EEPROM_PROGRAM);
    rtl8139_eeprom_enable_write(wide);
    for (i = 0; i < ETHERNET_ADDRESS_BYTES / 2; i++) {
        /* @hack spelling: the word held in value, where the write could take
         * its expression */
        value = (address->bytes[(i * 2) + 1] << 8) | address->bytes[i * 2];
        /* @bug logic: leaves the EEPROM's writes enabled and Cfg9346 at
         * RTL_EEPROM_PROGRAM when an erase or a write times out. the
         * branches to the -1 return at 0x0cae0ce4 and 0x0cae0cf2 skip the
         * cleanup at 0x0cae0cfe */
        if (rtl8139_eeprom_erase(wide, i + MAC_EEPROM_WORD) < 0) {
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it disables the EEPROM's
             * writes and sets Cfg9346 back to RTL_EEPROM_NORMAL */
            rtl8139_eeprom_disable_write(wide);
            ETD_G2Write8(RTL_CFG9346, RTL_EEPROM_NORMAL);
#endif
            return -1;
        }
        if (rtl8139_eeprom_write(wide, i + MAC_EEPROM_WORD, value) < 0) {
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it disables the EEPROM's
             * writes and sets Cfg9346 back to RTL_EEPROM_NORMAL */
            rtl8139_eeprom_disable_write(wide);
            ETD_G2Write8(RTL_CFG9346, RTL_EEPROM_NORMAL);
#endif
            return -1;
        }
    }
    rtl8139_eeprom_disable_write(wide);
    ETD_G2Write8(RTL_CFG9346, RTL_EEPROM_NORMAL);
    /* @hack spelling: the call's result held in result, where the test could
     * take the call */
    result = DLD_GetEtdAddr(&readback);
    if (result < 0) {
        return -1;
    }
    for (i = 0; i < ETHERNET_ADDRESS_BYTES; i++) {
        if (readback.bytes[i] != address->bytes[i]) {
            return -1;
        }
    }
    return 0;
}

Sint32 DLD_GetEtdAddr(struct rtl_address *address)
{
    Bool wide;
    int i;
    Uint16 value;
    /* @hack spelling: the call's result held in config, where the test could
     * take the call */
    Uint32 config = ETD_G2Read32(RTL_RCR);
    /* @hack spelling: wide set by an if and an else, where
     * wide = (config & RTL_RCR_EEPROM_WIDE) != 0 would do */
    if (config & RTL_RCR_EEPROM_WIDE) {
        wide = TRUE;
    } else {
        wide = FALSE;
    }
    ETD_G2Write8(RTL_CFG9346, RTL_EEPROM_PROGRAM);
    for (i = 0; i < ETHERNET_ADDRESS_BYTES / 2; i++) {
        value = rtl8139_eeprom_read(wide, i + MAC_EEPROM_WORD);
        address->bytes[i * 2] = value;
        address->bytes[(i * 2) + 1] = value >> 8;
    }
    ETD_G2Write8(RTL_CFG9346, RTL_EEPROM_NORMAL);
    return 0;
}
