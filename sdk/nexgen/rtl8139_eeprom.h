/* the RTL8139's serial EEPROM, addressed in 16-bit words, through Cfg9346's
 * pins. Cfg9346 must hold RTL_EEPROM_PROGRAM, and each call puts it back as
 * it found it. wide is TRUE for a 93C56, whose addresses take 8 bits. it is
 * FALSE for a 93C46, whose addresses take 6. RTL_RCR_EEPROM_WIDE says which
 * the card has */
#ifndef NEXGEN_RTL8139_EEPROM_H
#define NEXGEN_RTL8139_EEPROM_H
#include <sg_xpt.h>

Uint16 rtl8139_eeprom_read(Bool wide, Uint8 address);
/* a write or an erase takes effect only between rtl8139_eeprom_enable_write
 * and rtl8139_eeprom_disable_write. each returns 0 once the EEPROM is ready
 * again, or -1 if it stays busy */
Sint32 rtl8139_eeprom_write(Bool wide, Uint8 address, Uint16 value);
Sint32 rtl8139_eeprom_erase(Bool wide, Uint8 address);
Sint32 rtl8139_eeprom_erase_all(Bool wide);
void rtl8139_eeprom_enable_write(Bool wide);
void rtl8139_eeprom_disable_write(Bool wide);
#endif
