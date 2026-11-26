/* ee_ram: the modem's EEPROM, kept in RAM */
#ifndef MODEM_EE_RAM_H
#define MODEM_EE_RAM_H
#include <sg_xpt.h>

/* the bytes the EEPROM holds */
#define EEPROM_SIZE 512

/* store data at offset, which must be below EEPROM_SIZE. the call doesn't check
 * it */
void eeprom_write_data(Uint8 data, Uint16 offset);
/* copy count bytes from offset on into buf. offset + count must be at most
 * EEPROM_SIZE, which the call doesn't check */
void eeprom_read_data(Uint16 offset, Uint16 count, Uint8 *buf);
/* return TRUE. no write is ever left to save, since eeprom_write_data
 * stores its byte at once */
Bool eeprom_save_complete(void);

#endif
