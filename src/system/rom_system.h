/* the system code a boot ROM keeps, copied over the system's RAM */
#ifndef SYSTEM_ROM_SYSTEM_H
#define SYSTEM_ROM_SYSTEM_H

/* copy the 12 KB the boot ROM keeps at 0x1000 over the system's RAM at
 * 0x8c001000. it copies only where the ROM's bytes at 0x7c1 read "KAB" and its
 * byte at 0x7ce reads "0". the copy goes through the operand cache, and each of
 * its lines is then written back to RAM. sbInitSystem calls it first, with
 * every interrupt masked */
void rom_system_reload(void);

#endif
